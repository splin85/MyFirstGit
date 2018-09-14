/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/17/18
 */

#ifndef __RPC_CORE_RPC_H__
#define __RPC_CORE_RPC_H__


#include <pthread.h>

#include <fhos/list.h>

#include <rpc/core/socket.h>


/**
 * The length must hold on Rx/Tx packet
 */
#define FHOS_RPC_READ_BUF_LEN        4096


#define FHOS_RPC_MSG_ALIGNTO         4U
#define FHOS_RPC_MSG_ALIGN(len)      (((len) + FHOS_RPC_MSG_ALIGNTO - 1) & ~(FHOS_RPC_MSG_ALIGNTO - 1))

#define FHOS_RPC_HDR_LEN             (int)(FHOS_RPC_MSG_ALIGN(sizeof(fhos_rpc_header_t)))
#define FHOS_RPC_MSG_LENGTH(len)     (int)(FHOS_RPC_HDR_LEN + (len))
#define FHOS_RPC_HDR(buf)            (fhos_rpc_header_t *)buf
#define FHOS_RPC_SPACE(len)          (int)FHOS_RPC_MSG_ALIGN(FHOS_RPC_MSG_LENGTH(len))
#define FHOS_RPC_DATA(hdr)           ((void *)(((char *)hdr) + FHOS_RPC_HDR_LEN))

#define FHOS_RPC_HDR_INIT(hdr)       memset(hdr, 0, FHOS_RPC_HDR_LEN)
#define FHOS_RPC_DATA_LEN(hdr)       (int)((hdr)->len - FHOS_RPC_HDR_LEN)


#define FHOS_RPC_REQUEST_ASYNC       0x0
#define FHOS_RPC_REQUEST_SYNC        0x1

#define RPC_WRITE_FLAGS(is_sync)    (is_sync) ? FHOS_RPC_REQUEST_SYNC : FHOS_RPC_REQUEST_ASYNC


#define FHOS_RPC_HDR_VERSION         1

#define FHOS_RPC_HDR_FLAGS_ACK       0x0001  /* Acknowlege message */
#define FHOS_RPC_HDR_FLAGS_BROADCAST 0x0002  /* Broadcast message */
#define FHOS_RPC_HDR_FLAGS_NO_ACK    0x4000  /* Do acknowlege the request */
#define FHOS_RPC_HDR_FLAGS_CONNECT   0x8000  /* Connect message */


#define FHOS_RPC_PID_IS_VALID(pid)  \
            (((pid) >= RPC_PID_START) && ((pid) < RPC_PID_COUNT))


typedef enum fhos_rpc_pid_e
{
    RPC_PID_START,
    RPC_PID_STP = RPC_PID_START,
    RPC_PID_IGMP,
    RPC_PID_SERVER,
    RPC_PID_BROADCAST,
    RPC_PID_NTP,
    RPC_PID_DHCP,
    RPC_PID_DOT1X,
    RPC_PID_COUNT
} fhos_rpc_pid_t;


typedef struct fhos_rpc_header_s
{
    uint8_t     ver;
    uint8_t     pid;   /* RPC_PID_XXXX */
    uint16_t    flags; /* RPC_HDR_FLAGS_XXXX */
    uint32_t    id;    /* Identifier, monotonic increament */
    int         len;   /* RPC message length include header */
} fhos_rpc_header_t;


#define FHOS_RPC_REQUEST_IS_TIMOUT(len)     ((len) == -1)

/* If buf == NULL and len == -1 mean timeout */
typedef int (*fhos_rpc_request_cb)(char *buf, int len, void *user_data);


typedef struct fhos_rpc_request_handler_s
{
    fhos_rpc_request_cb     cb;
    void                   *user_data;
} fhos_rpc_request_handler_t;


typedef struct fhos_rpc_request_s
{
    uint32_t                    id;
    fhos_rpc_request_handler_t  handler;
    void                       *cookie;
    pthread_mutex_t             lock;
    pthread_cond_t              cv;
    struct thread              *thr_timer;
    struct list_head            node;
} fhos_rpc_request_t;


typedef int (*fhos_rpc_recv_handler_t)(char *buf, int len, void *cookie);

typedef void (*fhos_rpc_connect_handler_t)(void);

typedef void (*fhos_rpc_close_handler_t)(void);


#define RPC_CLIENT_CLOSED           0
#define RPC_CLIENT_CONNECTED        1

typedef struct fhos_rpc_client_s
{
    int                         status; /* @see RPC_CLIENT_XXXX */
    fhos_rpc_pid_t              pid;
    uint32_t                    request_id;
    struct list_head            requests;
    struct list_head            free_requests;
    uint32_t                    free_count; /* free requests count */
    fhos_rpc_recv_handler_t     recv;       /* If request ID do not find */
    fhos_rpc_connect_handler_t  connect;    /* If connect server */
    fhos_rpc_close_handler_t    close;      /* If disconnect to server */
    fhos_client_socket_t        socket;
    pthread_spinlock_t          lock;
} fhos_rpc_client_t;


typedef struct fhos_rpc_client_socket_s
{
    fhos_rpc_pid_t          pid;
    fhos_socket_t           socket;
    void                   *cookie; /* fhos_rpc_server_t */
    struct list_head        node;
} fhos_rpc_client_socket_t;


typedef struct fhos_rpc_server_s
{
    uint32_t                request_id;
    struct list_head        requests;
    struct list_head        free_requests;
    uint32_t                free_count;      /* free requests count */
    struct list_head        client_list;
    fhos_rpc_recv_handler_t recv;
    fhos_server_socket_t    socket;
    pthread_spinlock_t      lock;
} fhos_rpc_server_t;



/**
 * RPC(Remote Procedure Call) client init
 * @param[in] path        UNIX domain path
 * @param[in] master      FRR thread master
 * @param[in] rpc_client  RPC client
 * @return FHOS_E_XXXX
 */
int fhos_rpc_server_init(const char           *path,
                         struct thread_master *master,
                         fhos_rpc_server_t    *rpc_server);

/**
 * Install recv handler to RPC server
 * @param[in] rpc_server  Server socket
 * @param[in] accept      Accept handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_rpc_server_recv_install(fhos_rpc_server_t      *rpc_server,
                                 fhos_rpc_recv_handler_t recv)
{
    if (rpc_server)
    {
        rpc_server->recv = recv;
        return FHOS_E_NONE;
    }

    return FHOS_E_NULL_PTR;
}

/**
 * RPC client synchronous write
 * @param[in] rpc_client  RPC client
 * @param[in] buf         Buffer
 * @param[in] len         Write buffer length
 * @param[in] callback    Callback when write complete, If it is NULL, write is sync, or async
 * @param[in] user_data   Callback user data
 * @return FHOS_E_XXXX
 */
int fhos_rpc_server_write(fhos_rpc_pid_t      pid,
                          fhos_rpc_server_t  *rpc_server,
                          const char         *buf,
                          size_t              len,
                          fhos_rpc_request_cb callback,
                          void               *user_data,
                          int                 flags);

/**
 * RPC(Remote Procedure Call) client init
 * @param[in] path        UNIX domain path
 * @param[in] master      FRR thread master
 * @param[in] rpc_client  RPC client
 * @return FHOS_E_XXXX
 */
int fhos_rpc_client_init(const fhos_rpc_pid_t  pid,
                         const char           *path,
                         struct thread_master *master,
                         fhos_rpc_client_t    *rpc_client);

/**
 * RPC client destory
 * @param[in] rpc_client  RPC client
 * @return FHOS_E_XXX
 */
int fhos_rpc_client_destory(fhos_rpc_client_t *rpc_client);

/**
 * Install default request handler to rpc client.
 * @param[in] rpc_client RPC client
 * @param[in] handler    Request handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_rpc_client_recv_install(fhos_rpc_client_t      *rpc_client,
                                 fhos_rpc_recv_handler_t handler)
{
    if (rpc_client)
    {
        rpc_client->recv = handler;
        return FHOS_E_NONE;
    }

    return FHOS_E_NULL_PTR;
}

/**
 * Install default connect handler to rpc client.
 * @param[in] rpc_client RPC client
 * @param[in] handler    Request handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_rpc_client_connect_install(fhos_rpc_client_t         *rpc_client,
                                    fhos_rpc_connect_handler_t handler)
{
    if (rpc_client)
    {
        rpc_client->connect = handler;
        return FHOS_E_NONE;
    }

    return FHOS_E_NULL_PTR;
}

/**
 * Install default close handler to rpc client.
 * @param[in] rpc_client RPC client
 * @param[in] handler    Request handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_rpc_client_close_install(fhos_rpc_client_t         *rpc_client,
                                  fhos_rpc_connect_handler_t handler)
{
    if (rpc_client)
    {
        rpc_client->close = handler;
        return FHOS_E_NONE;
    }

    return FHOS_E_NULL_PTR;
}

/**
 * RPC client synchronous write
 * @param[in] rpc_client  RPC client
 * @param[in] buf         Buffer
 * @param[in] len         Write buffer length
 * @param[in] callback    Callback when write complete, If it is NULL, write is sync, or async
 * @param[in] user_data   Callback user data
 * @return FHOS_E_XXXX
 */
int fhos_rpc_client_write(fhos_rpc_client_t  *rpc_client,
                          const char         *buf,
                          size_t              len,
                          fhos_rpc_request_cb callback,
                          void               *user_data,
                          int                 flags);

/**
 * RPC client read
 * @param[in] fd      File descriptor
 * @param[in] cookie  Cookie data
 * @return FHOS_E_XXXX
 */
int fhos_rpc_client_read(int fd, void *cookie);


#endif
