
#include <stdio.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <frr/network.h>

#include <fhos/list.h>

#include <rpc/core/socket.h>



#define FHOS_SOCKET_CLIENT_CONNECT_TIMEOUT      1
#define FHOS_SOCKET_ALIVE_CHECK_TIMEOUT         1





typedef enum fhos_socket_event_e
{
    FHOS_SOCKET_SERVER_READ,
    FHOS_SOCKET_SERVER_WRITE,
    FHOS_SOCKET_SERVER_ACCEPT,
    FHOS_SOCKET_CLIENT_READ,
    FHOS_SOCKET_CLIENT_CONNECT,
    FHOS_SOCKET_CLIENT_ALIVE_CHECK
} fhos_socket_event_t;



static void fhos_socket_event(struct thread_master *master,
                              fhos_socket_event_t   event,
                              int                   fd,
                              fhos_socket_t        *socket);



int fhos_socket_close(fhos_socket_t *socket)
{
    if (!socket)
    {
        return FHOS_E_PARAM;
    }

    if (socket->fd >= 0)
    {
        close(socket->fd);
        socket->fd = -1;
    }

    THREAD_READ_OFF(socket->thr_read);
    THREAD_WRITE_OFF(socket->thr_write);
    THREAD_TIMER_OFF(socket->thr_timer);

    return FHOS_E_NONE;
}


static
int fhos_socket_is_error(const int fd)
{
    int         rc;
	int         sock_err;
	socklen_t   sock_len = sizeof(sock_err);


    rc = getsockopt(fd, SOL_SOCKET,
                    SO_ERROR, (char *)&sock_err, &sock_len);
	if (rc < 0 ||
        ((sock_len == sizeof(sock_err)) && sock_err))
    {
		return 1;
	}

    return 0;
}


static
int fhos_socket_is_writable(const int fd)
{
    int     rc;


    if (fd < 0)
    {
        return 0;
    }

    /**
     * This is just tricks to fast check the socket is alive or not!
     */
    while (1)
    {
        rc = write(fd, NULL, 0);
        if (rc < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else if (!FHOS_SOCKET_IS_RETRY(errno))
            {
                return 0;
            }
        }

        break;
    }

    return 1;
}


int fhos_server_socket_init(const char           *path,
                            struct thread_master *master,
                            fhos_server_socket_t *server_socket)
{
    int                 rc;
    int                 fd;
    socklen_t           len;
    struct sockaddr_un  addr;


    if (!path   ||
        !master ||
        !server_socket)
    {
        return FHOS_E_NULL_PTR;
    }

    memset(server_socket, 0, sizeof(*server_socket));
    server_socket->state = FHOS_SOCKET_STATE_IDLE;

    unlink(path);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return FHOS_E_INTERNAL;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path, sizeof(addr.sun_path));
    len = sizeof(addr.sun_family) + strlen(addr.sun_path);
    rc = bind(fd, (struct sockaddr *)&addr, len);
    if (rc < 0)
    {
        close(fd);
        return FHOS_E_INTERNAL;
    }

    set_nonblocking(fd);
    set_cloexec(fd);

    rc = listen(fd, 5);
    if (rc < 0)
    {
        close(fd);
        return FHOS_E_INTERNAL;
    }

    server_socket->state = FHOS_SOCKET_STATE_LISTEN;
    server_socket->socket.fd = fd;
    server_socket->master = master;
    fhos_socket_event(master, FHOS_SOCKET_SERVER_ACCEPT, fd, &server_socket->socket);

    return fd;
}


int fhos_server_socket_recv_start(fhos_socket_t *socket)
{
    fhos_server_socket_t   *server_socket;


    if (!socket && !socket->cookie)
    {
        return FHOS_E_NULL_PTR;
    }

    server_socket = (fhos_server_socket_t *)socket->cookie;

    fhos_socket_event(server_socket->master, FHOS_SOCKET_SERVER_READ, socket->fd, socket);

    return FHOS_E_NONE;
}


static
int fhos_socket_server_accept(struct thread *thread)
{
    int                     fd;
    socklen_t               addr_len;
    struct sockaddr_un      addr;
    fhos_socket_t          *socket;
    fhos_server_socket_t   *server_socket;


    socket = THREAD_ARG(thread);
    server_socket = container_of(socket, fhos_server_socket_t, socket);

    fhos_socket_event(thread->master, FHOS_SOCKET_SERVER_ACCEPT, socket->fd, &server_socket->socket);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr_len = sizeof(struct sockaddr_un);

    fd = accept(socket->fd, (struct sockaddr *)&addr, (socklen_t *)&addr_len);
    if (fd < 0)
    {
        return FHOS_E_INTERNAL;
    }

    if (set_nonblocking(fd) < 0)
    {
        close(fd);
        return FHOS_E_INTERNAL;
    }

    set_cloexec(fd);

    if (!server_socket->accept ||
        server_socket->accept(server_socket, fd, (struct sockaddr *)&addr) < 0)
    {
        close(fd);
        return FHOS_E_INTERNAL;
    }

    return FHOS_E_NONE;
}


static
int fhos_socket_server_recv_service(struct thread *thread)
{
    int             len;
    int             fd;
    fhos_socket_t  *socket;


    fd = THREAD_FD(thread);
    socket = THREAD_ARG(thread);

    if (socket->recv)
    {
        len = socket->recv(fd, socket);
    }
    else
    {
        char  buf[1024];

        len = 1;
        while (len > 0)
        {
            len = read(fd, buf, sizeof buf); /* Discard packets */
            if (len < 0 && errno == EINTR)
            {
                len = 1;
                continue;
            }
        }
    }

    if (len <= 0)
    {
        if (len == 0 || !FHOS_SOCKET_IS_RETRY(errno))
        {
            fhos_server_socket_t  *server_socket = (fhos_server_socket_t *)socket->cookie;

            fhos_socket_close(socket);
            if (server_socket && server_socket->socket.close)
            {
                server_socket->socket.close(socket);
            }

            return FHOS_E_NONE;
        }
    }

    fhos_socket_event(thread->master, FHOS_SOCKET_SERVER_READ, fd, socket);

    return FHOS_E_NONE;
}


static inline
int fhos_socket_client_down(fhos_client_socket_t *client_socket)
{
    fhos_socket_close(&client_socket->socket);

    if (client_socket->state == FHOS_SOCKET_STATE_ESTABLISHED &&
        client_socket->socket.close)
    {
        client_socket->socket.close(&client_socket->socket);
    }

    client_socket->state = FHOS_SOCKET_STATE_IDLE;

    return FHOS_E_NONE;
}


static
int fhos_socket_client_connect_check(struct thread *thread)
{
    fhos_socket_t          *socket = THREAD_ARG(thread);
    fhos_client_socket_t   *client_socket = container_of(socket, fhos_client_socket_t, socket);


    if (!fhos_socket_is_error(socket->fd))
    {
	    client_socket->state = FHOS_SOCKET_STATE_ESTABLISHED;
    }

    fhos_socket_event(client_socket->master, FHOS_SOCKET_CLIENT_ALIVE_CHECK, socket->fd, socket);

    return FHOS_E_NONE;
}


static
int fhos_socket_client_alive_check(struct thread *thread)
{
    fhos_socket_t          *socket = THREAD_ARG(thread);
    fhos_client_socket_t   *client_socket = container_of(socket, fhos_client_socket_t, socket);


    if (!fhos_socket_is_writable(socket->fd))
    {
        fhos_socket_client_down(client_socket);
        fhos_socket_client_connect(client_socket);
        return FHOS_E_NONE;
    }

    client_socket->state = FHOS_SOCKET_STATE_ESTABLISHED;
    fhos_socket_event(client_socket->master, FHOS_SOCKET_CLIENT_ALIVE_CHECK, socket->fd, socket);

	return FHOS_E_NONE;
}


int fhos_socket_server_destroy(fhos_server_socket_t *server_socket)
{
    if (!server_socket)
    {
        return FHOS_E_NULL_PTR;
    }

    fhos_socket_close(&server_socket->socket);

    return FHOS_E_NONE;
}


int fhos_socket_client_destroy(fhos_client_socket_t *client_socket)
{
    if (!client_socket)
    {
        return FHOS_E_NULL_PTR;
    }

    fhos_socket_close(&client_socket->socket);

    return FHOS_E_NONE;
}


int fhos_socket_client_connect(fhos_client_socket_t *client_socket)
{
     int                     rc;
     int                     fd;
     struct sockaddr_un      addr;
     fhos_socket_t          *sock;


    if (!client_socket)
    {
        return FHOS_E_NULL_PTR;
    }

    if (client_socket->state == FHOS_SOCKET_STATE_ESTABLISHED)
    {
        return FHOS_E_NONE;
    }

    fhos_socket_client_down(client_socket);

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return FHOS_E_INTERNAL;
    }

    rc = set_nonblocking(fd);
    if (rc < 0)
    {
        close(fd);
        return FHOS_E_INTERNAL;
    }

    set_cloexec(fd);

    sock = &client_socket->socket;
    sock->fd = fd;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock->path, sizeof(addr.sun_path) - 1);
    rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        if (!FHOS_SOCKET_IS_RETRY(errno))
        {
            fhos_socket_client_down(client_socket);
            fhos_socket_event(client_socket->master, FHOS_SOCKET_CLIENT_ALIVE_CHECK, fd, sock);
            return FHOS_E_NONE;
        }

        client_socket->state = FHOS_SOCKET_STATE_CONNECTING;
        fhos_socket_event(client_socket->master, FHOS_SOCKET_CLIENT_CONNECT, fd, sock);
        return FHOS_E_NONE;
    }

    client_socket->state = FHOS_SOCKET_STATE_ESTABLISHED;
    client_socket->socket.fd = fd;

    if (client_socket->connect)
    {
        client_socket->connect(client_socket);
    }

    fhos_socket_event(client_socket->master, FHOS_SOCKET_CLIENT_READ, fd, sock);
    fhos_socket_event(client_socket->master, FHOS_SOCKET_CLIENT_ALIVE_CHECK, fd, sock);

    return FHOS_E_NONE;
}


int fhos_client_socket_init(const char                   *path,
                            struct thread_master         *master,
                            fhos_client_socket_t         *client_socket,
                            fhos_socket_connect_handler_t handler)
{
    int     rc;


    if (!path   ||
        !master ||
        !client_socket)
    {
        return FHOS_E_NULL_PTR;
    }

    memset(client_socket, 0, sizeof(*client_socket));

    client_socket->state = FHOS_SOCKET_STATE_IDLE;
    client_socket->master = master;
    client_socket->socket.fd = -1;
    client_socket->connect = handler;
    strncpy(client_socket->socket.path, path, sizeof(client_socket->socket.path) - 1);
    rc = fhos_socket_client_connect(client_socket);

    return rc;
}


static
int fhos_socket_client_recv_service(struct thread *thread)
{
    int                     len;
    int                     fd;
    fhos_socket_t          *socket;
    fhos_client_socket_t   *client_socket;


    socket = THREAD_ARG(thread);
    fd = socket->fd;
    client_socket = container_of(socket, fhos_client_socket_t, socket);

    if (socket->recv)
    {
        len = socket->recv(fd, client_socket);
    }
    else
    {
        char  buf[1024];

        len = 1;
        while (len > 0)
        {
            len = read(fd, buf, sizeof buf); /* Discard packets */
            if (len < 0 && errno == EINTR)
            {
                len = 1;
                continue;
            }
        }
    }

    if (len <= 0)
    {
        if (len == 0 || !FHOS_SOCKET_IS_RETRY(errno))
        {
            fhos_socket_client_down(client_socket);
            fhos_socket_event(client_socket->master,
                              FHOS_SOCKET_CLIENT_ALIVE_CHECK,
                              fd, &client_socket->socket);
            return FHOS_E_NONE;
        }
    }

    fhos_socket_event(thread->master, FHOS_SOCKET_CLIENT_READ, fd, socket);

    return FHOS_E_NONE;
}


void fhos_socket_event(struct thread_master *master,
                       fhos_socket_event_t   event,
                       int                   fd,
                       fhos_socket_t        *socket)
{
    switch (event)
    {
        case FHOS_SOCKET_SERVER_READ:
            socket->thr_read = NULL;
		    thread_add_read(master, fhos_socket_server_recv_service,
		                    socket, fd, &socket->thr_read);
            break;

        case FHOS_SOCKET_SERVER_WRITE:
            break;

        case FHOS_SOCKET_SERVER_ACCEPT:
            thread_add_read(master, fhos_socket_server_accept, socket, fd, NULL);
            break;

        case FHOS_SOCKET_CLIENT_READ:
            socket->thr_read = NULL;
		    thread_add_read(master, fhos_socket_client_recv_service,
		                    socket, fd, &socket->thr_read);
            break;

        case FHOS_SOCKET_CLIENT_CONNECT:
            socket->thr_write = NULL;
            thread_add_write(master, fhos_socket_client_connect_check,
                             socket, fd, &socket->thr_write);
            break;

        case FHOS_SOCKET_CLIENT_ALIVE_CHECK:
            socket->thr_timer = NULL;
            thread_add_timer(master, fhos_socket_client_alive_check, socket,
                             FHOS_SOCKET_ALIVE_CHECK_TIMEOUT, &socket->thr_timer);
            break;

        default:
            ;
    }
}


int fhos_socket_readn(int fd, char *buf, int len)
{
    int     left = len;
    int     read_bytes;
    char   *p = buf;


    while (left > 0)
    {
        read_bytes = read(fd, p, left);
        if (read_bytes < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return read_bytes;
        }
        else if (read_bytes == 0)
        {
            break;
        }

        left -= read_bytes;
        p += read_bytes;
    }

    return len - left;
}


int fhos_socket_writen(int fd, const char *buf, int len)
{
    int         left = len;
    int         write_bytes;
    const char *p = buf;


    while (left > 0)
    {
        write_bytes = write(fd, p, left);
        if (write_bytes < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else if (FHOS_SOCKET_IS_RETRY(errno))
            {
                write_bytes = 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            if (write_bytes == 0)
            {
                return len - left;
            }
        }

        left -= write_bytes;
        p += write_bytes;
    }

    return len - left;
}


