
/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/18/18
 */

#include <fhos/common.h>
#include <fhos/memory.h>

#include <rpc/core/socket.h>
#include <rpc/core/rpc.h>



#define RPC_CONNECT_ID              0x19170924

#define RPC_SYNC_TIMEOUT            (2 * MSEC_PER_SEC)
#define RPC_RECV_TIMEOUT            ((RPC_SYNC_TIMEOUT * 3) / 2) /* Must great than RPC_SYN_TIMEOUT */

#define RPC_SERVER_FREE_REQUEST_MAX 8
#define RPC_CLIENT_FREE_REQUEST_MAX 5

#define RPC_SPIN_LOCK(lock)         pthread_spin_lock(lock)
#define RPC_SPIN_UNLOCK(lock)       pthread_spin_unlock(lock)

#define RPC_MUTEX_LOCK(lock)        pthread_mutex_lock(lock)
#define RPC_MUTEX_UNLOCK(lock)      pthread_mutex_unlock(lock)


static int  pid_to_fd_map[RPC_PID_COUNT];



static
fhos_rpc_request_t* fhos_rpc_server_request_alloc(fhos_rpc_server_t *rpc_server)
{
    uint32_t            id;
    fhos_rpc_request_t *request = NULL;


    RPC_SPIN_LOCK(&rpc_server->lock);

    id = rpc_server->request_id++;
    if (!list_empty(&rpc_server->free_requests))
    {
        request = list_first_entry(&rpc_server->free_requests, fhos_rpc_request_t, node);
        list_move_tail(&request->node, &rpc_server->requests);
        rpc_server->free_count--;
        RPC_SPIN_UNLOCK(&rpc_server->lock);
    }
    else
    {
        pthread_condattr_t   attr;


        RPC_SPIN_UNLOCK(&rpc_server->lock);

        request = (fhos_rpc_request_t *)FHOS_MALLOC(sizeof(*request));
        if (!request)
        {
            return NULL;
        }

        memset(request, 0, sizeof(*request));

        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_cond_init(&request->cv, &attr);
        pthread_mutex_init(&request->lock, NULL);

        INIT_LIST_HEAD(&request->node);

        RPC_SPIN_LOCK(&rpc_server->lock);
        list_add_tail(&request->node, &rpc_server->requests);
        RPC_SPIN_UNLOCK(&rpc_server->lock);
    }

    request->id = id;

    return request;
}


static
void fhos_rpc_server_request_free(fhos_rpc_server_t *rpc_server, fhos_rpc_request_t *request)
{
    if (!rpc_server || !request)
    {
        return;
    }

    RPC_MUTEX_LOCK(&request->lock);
    pthread_cond_signal(&request->cv);
    RPC_MUTEX_UNLOCK(&request->lock);

    RPC_SPIN_LOCK(&rpc_server->lock);

    if (rpc_server->free_count < RPC_SERVER_FREE_REQUEST_MAX)
    {
        rpc_server->free_count++;
        list_move(&request->node, &rpc_server->free_requests);

        RPC_SPIN_UNLOCK(&rpc_server->lock);
    }
    else
    {
        list_del(&request->node);
        RPC_SPIN_UNLOCK(&rpc_server->lock);

        if (!request->handler.cb)
        {
            pthread_cond_destroy(&request->cv);
            pthread_mutex_destroy(&request->lock);
        }

        FHOS_FREE(request);
    }
}


static
int fhos_rpc_server_recv_timeout(struct thread *thread)
{
    fhos_rpc_request_t  *request;
    fhos_rpc_server_t   *rpc_server;


    request = THREAD_ARG(thread);
    rpc_server = (fhos_rpc_server_t *)request->cookie;
    THREAD_TIMER_OFF(request->thr_timer);

    if (request->handler.cb)
    {
        request->handler.cb(NULL, -1, request->cookie);
    }

    fhos_rpc_server_request_free(rpc_server, request);

    return 0;
}


static
int fhos_rpc_server_read(int fd, void *cookie)
{
    int                         len;
    int                         found = 0;
    char                        buf[FHOS_RPC_READ_BUF_LEN];
    fhos_rpc_header_t          *header = FHOS_RPC_HDR(buf);
    fhos_socket_t              *socket = (fhos_socket_t *)cookie;
    fhos_rpc_client_socket_t   *client_socket;
    fhos_rpc_server_t          *rpc_server;
    fhos_rpc_request_t         *request;
    fhos_rpc_request_t         *n;


    FHOS_RPC_HDR_INIT(header);
    len = fhos_socket_readn(fd, (char *)header, FHOS_RPC_HDR_LEN);
    if (len != FHOS_RPC_HDR_LEN)
    {
        return len;
    }

    len = fhos_socket_readn(fd, FHOS_RPC_DATA(header), FHOS_RPC_DATA_LEN(header));
    if (len != FHOS_RPC_DATA_LEN(header))
    {
        return len;
    }

    client_socket = container_of(socket, fhos_rpc_client_socket_t, socket);
    rpc_server = (fhos_rpc_server_t *)client_socket->cookie;

    RPC_SPIN_LOCK(&rpc_server->lock);

    /**
     * @attention
     *
     * The client first message
     */
    if (header->len   == FHOS_RPC_HDR_LEN &&
        header->id    == RPC_CONNECT_ID &&
        header->flags == FHOS_RPC_HDR_FLAGS_CONNECT)
    {
        fhos_rpc_pid_t   pid = header->pid;

        if (FHOS_RPC_PID_IS_VALID(pid) &&
            pid_to_fd_map[pid] != fd)
        {
            pid_to_fd_map[pid] = fd;
            client_socket->pid = pid;
        }
        len = 1;
    }
    else
    {
        list_for_each_entry_safe(request, n, &rpc_server->requests, node)
        {
            if ((header->flags & FHOS_RPC_HDR_FLAGS_ACK) &&
                request->id == header->id)
            {
                RPC_SPIN_UNLOCK(&rpc_server->lock);
                THREAD_TIMER_OFF(request->thr_timer);
                found = 1;
                if (request->handler.cb)
                {
                    request->handler.cb(FHOS_RPC_DATA(header), len, request->handler.user_data);
                }
                fhos_rpc_server_request_free(rpc_server, request);
                break;
            }
        }
    }

    if (!found)
    {
        RPC_SPIN_UNLOCK(&rpc_server->lock);

        if (rpc_server->recv)
        {
            rpc_server->recv(buf, header->len, &fd);
        }
    }

    return len;
}


static
int fhos_rpc_server_client_fd_find(fhos_rpc_server_t *rpc_server, fhos_rpc_pid_t pid)
{
    int     fd = -1;


    if (FHOS_RPC_PID_IS_VALID(pid))
    {
        RPC_SPIN_LOCK(&rpc_server->lock);

        fd = pid_to_fd_map[pid];

        RPC_SPIN_UNLOCK(&rpc_server->lock);
    }

    return fd;
}


int fhos_rpc_server_write(fhos_rpc_pid_t      pid,
                          fhos_rpc_server_t  *rpc_server,
                          const char         *buf,
                          size_t              len,
                          fhos_rpc_request_cb callback,
                          void               *user_data,
                          int                 flags)
{
    int                     write_bytes;
    int                     fd;
    fhos_rpc_request_t     *request;
    fhos_rpc_header_t      *header;


    if (!rpc_server            ||
        !buf                   ||
        len < FHOS_RPC_HDR_LEN ||
        !((flags == FHOS_RPC_REQUEST_SYNC) || (flags == FHOS_RPC_REQUEST_ASYNC)))
    {
        return FHOS_E_PARAM;
    }

    fd = fhos_rpc_server_client_fd_find(rpc_server, pid);
    if (fd < 0)
    {
        return FHOS_E_NOT_FOUND;
    }

    request = fhos_rpc_server_request_alloc(rpc_server);
    if (!request)
    {
        return FHOS_E_MEMORY;
    }

    request->handler.cb = callback;
    request->handler.user_data = user_data;
    request->cookie = rpc_server;
    request->thr_timer = NULL;

    header = FHOS_RPC_HDR(buf);
    FHOS_RPC_HDR_INIT(header);
    header->ver = FHOS_RPC_HDR_VERSION;
    header->pid = RPC_PID_SERVER;
    header->id  = request->id;
    header->len = len;

    request->thr_timer = NULL;
    thread_add_timer_msec(rpc_server->socket.master, fhos_rpc_server_recv_timeout,
                          request, RPC_RECV_TIMEOUT, &request->thr_timer);

    RPC_MUTEX_LOCK(&request->lock);

    write_bytes = fhos_socket_writen(fd, buf, len);
    if (write_bytes == (int)len) /* sync */
    {
        if (flags == FHOS_RPC_REQUEST_SYNC)
        {
            int              rc;
            struct timespec  ts;

            clock_gettime(CLOCK_MONOTONIC, &ts);
            ts.tv_sec += RPC_SYNC_TIMEOUT;
            rc = pthread_cond_timedwait(&request->cv, &request->lock, &ts);
            if (rc)
            {
                /* Caller should check ETIMEDOUT or not */
                errno = rc;
                write_bytes = FHOS_E_INTERNAL;
            }
        }
    }
    else
    {
        /**
         * Should cancel timer, but FRR THREAD_TIMER_CANCEL have some limits,
         * May be try thread async cancel method.
         */
        write_bytes = FHOS_E_INTERNAL;
    }

    RPC_MUTEX_UNLOCK(&request->lock);

    return write_bytes;
}


static
int fhos_rpc_server_accept(fhos_server_socket_t *server_socket,
                           int                   fd,
                           struct sockaddr      *addr)
{
    fhos_rpc_client_socket_t    *client_socket;
    fhos_rpc_server_t           *rpc_server;


    (void)addr;

    if (!server_socket)
    {
        return FHOS_E_NULL_PTR;
    }

    client_socket = FHOS_MALLOC(sizeof(*client_socket));
    if (!client_socket)
    {
        return FHOS_E_MEMORY;
    }

    memset(client_socket, 0, sizeof(*client_socket));

    rpc_server = container_of(server_socket, fhos_rpc_server_t, socket);
    client_socket->pid           = -1;
    client_socket->socket.fd     = fd;
    client_socket->socket.recv   = fhos_rpc_server_read;
    client_socket->socket.cookie = server_socket;
    client_socket->cookie        = rpc_server;

    RPC_SPIN_LOCK(&rpc_server->lock);
    list_add(&client_socket->node, &rpc_server->client_list);
    RPC_SPIN_UNLOCK(&rpc_server->lock);
    fhos_server_socket_recv_start(&client_socket->socket);

    return FHOS_E_NONE;
}


int fhos_rpc_server_client_close(fhos_socket_t *socket)
{
    fhos_rpc_server_t           *rpc_server;
    fhos_rpc_client_socket_t    *client_socket;


    if (!socket)
    {
        return FHOS_E_NULL_PTR;
    }

    client_socket = container_of(socket, fhos_rpc_client_socket_t, socket);
    rpc_server = (fhos_rpc_server_t *)client_socket->cookie;

    RPC_SPIN_LOCK(&rpc_server->lock);
    list_del(&client_socket->node);
    if (FHOS_RPC_PID_IS_VALID(client_socket->pid))
    {
        pid_to_fd_map[client_socket->pid] = -1;
    }
    RPC_SPIN_UNLOCK(&rpc_server->lock);

    FHOS_FREE(client_socket);

    return FHOS_E_NONE;
}


int fhos_rpc_server_init(const char           *path,
                         struct thread_master *master,
                         fhos_rpc_server_t    *rpc_server)
{
    int     rc;
    int     pid;


    if (!rpc_server)
    {
        return FHOS_E_NULL_PTR;
    }

    memset(rpc_server, 0, sizeof(*rpc_server));

    rc = fhos_server_socket_init(path, master, &rpc_server->socket);
    if (rc < 0)
    {
        return rc;
    }

    for (pid = 0; pid < RPC_PID_COUNT; pid++)
    {
        pid_to_fd_map[pid] = -1;
    }

    INIT_LIST_HEAD(&rpc_server->requests);
    INIT_LIST_HEAD(&rpc_server->free_requests);
    INIT_LIST_HEAD(&rpc_server->client_list);
    pthread_spin_init(&rpc_server->lock, PTHREAD_PROCESS_PRIVATE);

    fhos_server_socket_accept_install(&rpc_server->socket, fhos_rpc_server_accept);
    fhos_server_socket_close_install(&rpc_server->socket, fhos_rpc_server_client_close);

    return FHOS_E_NONE;
}


static
int fhos_rpc_client_connect_handler(void *cookie)
{
    char                    buf[FHOS_RPC_HDR_LEN];
    fhos_rpc_header_t      *header = FHOS_RPC_HDR(buf);
    fhos_client_socket_t   *client_socket = (fhos_client_socket_t *)cookie;
    fhos_rpc_client_t      *rpc_client = container_of(client_socket, fhos_rpc_client_t, socket);


    FHOS_RPC_HDR_INIT(header);
    header->pid   = rpc_client->pid;
    header->len   = FHOS_RPC_HDR_LEN;
    header->id    = RPC_CONNECT_ID;
    header->flags = FHOS_RPC_HDR_FLAGS_CONNECT;
    fhos_socket_writen(rpc_client->socket.socket.fd, buf, FHOS_RPC_HDR_LEN);

    RPC_SPIN_LOCK(&rpc_client->lock);
    rpc_client->status = RPC_CLIENT_CONNECTED;
    RPC_SPIN_UNLOCK(&rpc_client->lock);

    if (rpc_client->connect)
    {
        rpc_client->connect();
    }

    return 0;
}


static
int fhos_rpc_client_close_handler(fhos_socket_t *socket)
{
    fhos_client_socket_t    *client_socket;
    fhos_rpc_client_t       *rpc_client;


    if (!socket)
    {
        return FHOS_E_NULL_PTR;
    }

    client_socket = container_of(socket, fhos_client_socket_t, socket);
    rpc_client = container_of(client_socket, fhos_rpc_client_t, socket);

    RPC_SPIN_LOCK(&rpc_client->lock);
    rpc_client->status = RPC_CLIENT_CONNECTED;
    RPC_SPIN_UNLOCK(&rpc_client->lock);

    if (rpc_client && rpc_client->close)
    {
        rpc_client->close();
    }

    return FHOS_E_NONE;
}


int fhos_rpc_client_init(const fhos_rpc_pid_t  pid,
                         const char           *path,
                         struct thread_master *master,
                         fhos_rpc_client_t    *rpc_client)
{
    int     rc;


    if (!rpc_client ||
        !path       ||
        !master     ||
        !FHOS_RPC_PID_IS_VALID(pid))
    {
        return FHOS_E_PARAM;
    }

    rpc_client->pid = pid;
    INIT_LIST_HEAD(&rpc_client->requests);
    INIT_LIST_HEAD(&rpc_client->free_requests);
    pthread_spin_init(&rpc_client->lock, PTHREAD_PROCESS_PRIVATE);

    rc = fhos_client_socket_init(path, master, &rpc_client->socket,
                                 fhos_rpc_client_connect_handler);
    if (rc < 0)
    {
        pthread_spin_destroy(&rpc_client->lock);
        return rc;
    }

    rpc_client->socket.socket.recv  = fhos_rpc_client_read;
    rpc_client->socket.socket.close = fhos_rpc_client_close_handler;

    return FHOS_E_NONE;
}


static
fhos_rpc_request_t* fhos_rpc_client_request_alloc(fhos_rpc_client_t *rpc_client)
{
    uint32_t            id;
    fhos_rpc_request_t *request = NULL;


    RPC_SPIN_LOCK(&rpc_client->lock);

    id = rpc_client->request_id++;
    if (!list_empty(&rpc_client->free_requests))
    {
        request = list_first_entry(&rpc_client->free_requests, fhos_rpc_request_t, node);
        list_move_tail(&request->node, &rpc_client->requests);
        rpc_client->free_count--;
        RPC_SPIN_UNLOCK(&rpc_client->lock);
    }
    else
    {
        pthread_condattr_t   attr;


        RPC_SPIN_UNLOCK(&rpc_client->lock);

        request = (fhos_rpc_request_t *)FHOS_MALLOC(sizeof(*request));
        if (!request)
        {
            return NULL;
        }

        memset(request, 0, sizeof(*request));

        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_cond_init(&request->cv, &attr);
        pthread_mutex_init(&request->lock, NULL);

        INIT_LIST_HEAD(&request->node);

        RPC_SPIN_LOCK(&rpc_client->lock);
        list_add_tail(&request->node, &rpc_client->requests);
        RPC_SPIN_UNLOCK(&rpc_client->lock);
    }

    request->id = id;

    return request;
}


static
void fhos_rpc_client_request_free(fhos_rpc_client_t *rpc_client, fhos_rpc_request_t *request)
{
    if (!rpc_client || !request)
    {
        return;
    }

    RPC_MUTEX_LOCK(&request->lock);
    pthread_cond_signal(&request->cv);
    RPC_MUTEX_UNLOCK(&request->lock);

    RPC_SPIN_LOCK(&rpc_client->lock);

    if (rpc_client->free_count < RPC_CLIENT_FREE_REQUEST_MAX)
    {
        rpc_client->free_count++;
        list_move(&request->node, &rpc_client->free_requests);

        RPC_SPIN_UNLOCK(&rpc_client->lock);
    }
    else
    {
        list_del(&request->node);
        RPC_SPIN_UNLOCK(&rpc_client->lock);

        if (!request->handler.cb)
        {
            pthread_cond_destroy(&request->cv);
            pthread_mutex_destroy(&request->lock);
        }

        FHOS_FREE(request);
    }
}


static
int fhos_rpc_client_recv_timeout(struct thread *thread)
{
    fhos_rpc_request_t  *request;
    fhos_rpc_client_t   *rpc_client;


    request = THREAD_ARG(thread);
    rpc_client = (fhos_rpc_client_t *)request->cookie;
    THREAD_TIMER_OFF(request->thr_timer);

    if (request->handler.cb)
    {
        request->handler.cb(NULL, -1, request->cookie);
    }

    fhos_rpc_client_request_free(rpc_client, request);

    return 0;
}


/**
 * @Attention
 *
 * Current DO NOT implements!!!
 */
int fhos_rpc_client_destory(fhos_rpc_client_t *rpc_client)
{
    if (!rpc_client)
    {
        return FHOS_E_NULL_PTR;
    }

    close(rpc_client->socket.socket.fd);
    rpc_client->socket.socket.fd = -1;

    return FHOS_E_NONE;
}


int fhos_rpc_client_write(fhos_rpc_client_t  *rpc_client,
                          const char         *buf,
                          size_t              len,
                          fhos_rpc_request_cb callback,
                          void               *user_data,
                          int                 flags)
{
    int                     write_bytes;
    fhos_socket_t          *socket;
    fhos_rpc_request_t     *request;
    fhos_rpc_header_t      *header;


    if (!rpc_client            ||
        !buf                   ||
        len < FHOS_RPC_HDR_LEN ||
        !((flags == FHOS_RPC_REQUEST_SYNC) || (flags == FHOS_RPC_REQUEST_ASYNC)))
    {
        return FHOS_E_PARAM;
    }

    socket = &rpc_client->socket.socket;
    if (socket->fd < 0)
    {
        return FHOS_E_INTERNAL;
    }

    request = fhos_rpc_client_request_alloc(rpc_client);
    if (!request)
    {
        return FHOS_E_MEMORY;
    }

    request->handler.cb = callback;
    request->handler.user_data = user_data;
    request->cookie = rpc_client;
    request->thr_timer = NULL;

    header = FHOS_RPC_HDR(buf);
    FHOS_RPC_HDR_INIT(header);
    header->ver = FHOS_RPC_HDR_VERSION;
    header->pid = rpc_client->pid;
    header->id  = request->id;
    header->len = len;

    request->thr_timer = NULL;
    thread_add_timer_msec(rpc_client->socket.master, fhos_rpc_client_recv_timeout,
                          request, RPC_RECV_TIMEOUT, &request->thr_timer);

    RPC_MUTEX_LOCK(&request->lock);

    write_bytes = fhos_socket_writen(socket->fd, buf, len);
    if (write_bytes == (int)len) /* sync */
    {
        if (flags == FHOS_RPC_REQUEST_SYNC)
        {
            int              rc;
            struct timespec  ts;

            clock_gettime(CLOCK_MONOTONIC, &ts);
            ts.tv_sec  += RPC_SYNC_TIMEOUT / MSEC_PER_SEC;
            ts.tv_nsec += (RPC_SYNC_TIMEOUT % MSEC_PER_SEC) * NSEC_PER_MSEC;
            if (ts.tv_nsec >= NSEC_PER_SEC)
            {
                ts.tv_sec += 1;
                ts.tv_nsec = NSEC_PER_SEC - ts.tv_nsec;
            }

            rc = pthread_cond_timedwait(&request->cv, &request->lock, &ts);
            if (rc)
            {
                /* Caller should check ETIMEDOUT or not */
                errno = rc;
                write_bytes = FHOS_E_INTERNAL;
            }
        }
    }
    else
    {
        /**
         * Should cancel timer, but FRR THREAD_TIMER_CANCEL have some limits,
         * May be try thread async cancel method.
         */
        write_bytes = FHOS_E_INTERNAL;
    }

    RPC_MUTEX_UNLOCK(&request->lock);

    return write_bytes;
}


int fhos_rpc_client_read(int fd, void *cookie)
{
    int                     found = 0;
    int                     len;
    char                    buf[FHOS_RPC_READ_BUF_LEN];
    fhos_rpc_request_t     *request;
    fhos_rpc_request_t     *n;
    fhos_rpc_header_t      *header;
    fhos_client_socket_t   *socket = (fhos_client_socket_t *)cookie;
    fhos_rpc_client_t      *rpc_client;


    rpc_client = container_of(socket, fhos_rpc_client_t, socket);

    header = FHOS_RPC_HDR(buf);
    FHOS_RPC_HDR_INIT(header);

    len = fhos_socket_readn(fd, (char *)header, FHOS_RPC_HDR_LEN);
    if (len != FHOS_RPC_HDR_LEN)
    {
        return len;
    }

    len = fhos_socket_readn(fd, FHOS_RPC_DATA(header), header->len - FHOS_RPC_HDR_LEN);
    if (len != FHOS_RPC_DATA_LEN(header))
    {
        return len;
    }

    RPC_SPIN_LOCK(&rpc_client->lock);

    list_for_each_entry_safe(request, n, &rpc_client->requests, node)
    {
        if (request->id == header->id)
        {
            RPC_SPIN_UNLOCK(&rpc_client->lock);
            THREAD_TIMER_OFF(request->thr_timer);
            found = 1;
            if (request->handler.cb)
            {
                request->handler.cb(FHOS_RPC_DATA(header), len, request->handler.user_data);
            }
            fhos_rpc_client_request_free(rpc_client, request);
            break;
        }
    }

    if (!found)
    {
        RPC_SPIN_UNLOCK(&rpc_client->lock);

        if (rpc_client->recv)
        {
            rpc_client->recv(buf, sizeof(buf), &fd);
        }
    }

    return len;
}


