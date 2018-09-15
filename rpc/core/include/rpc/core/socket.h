

#ifndef __RPC_CORE_SOCKET_H__
#define __RPC_CORE_SOCKET_H__


#include <frr/thread.h>

#include <fhos/errno.h>



#define FHOS_SOCKET_IS_RETRY(err) \
            (((err) == EAGAIN) || ((err) == EWOULDBLOCK))


typedef struct fhos_server_socket_s fhos_server_socket_t;

typedef struct fhos_socket_s fhos_socket_t;

typedef int (*fhos_socket_accept_handler_t)(fhos_server_socket_t *server_socket, int fd, struct sockaddr *addr);

typedef int (*fhos_socket_close_handler_t)(fhos_socket_t *socket);

typedef int (*fhos_socket_recv_handler_t)(int fd, void *cookie);

typedef int (*fhos_socket_connect_handler_t)(void *cookie);


struct fhos_socket_s
{
    int                            fd;
    char                           path[64];
    fhos_socket_recv_handler_t     recv;
    fhos_socket_close_handler_t    close;
    void                          *cookie;
    struct thread                 *thr_read;
    struct thread                 *thr_write;
    struct thread                 *thr_timer;
};


typedef enum fhos_socket_state_e
{
    FHOS_SOCKET_STATE_IDLE,
    FHOS_SOCKET_STATE_CONNECTING,
    FHOS_SOCKET_STATE_ESTABLISHED,
    FHOS_SOCKET_STATE_LISTEN
} fhos_socket_state_t;


struct fhos_server_socket_s
{
    fhos_socket_state_t             state;
    fhos_socket_accept_handler_t    accept;
    fhos_socket_t                   socket;
    struct thread_master           *master;
};


typedef struct fhos_client_socket_s
{
    fhos_socket_state_t             state;
    fhos_socket_t                   socket;
    fhos_socket_connect_handler_t   connect;
    struct thread_master           *master;
} fhos_client_socket_t;



/**
 * Create unix domain server
 * @param[in] path           socket path
 * @param[in] master         Thread master
 * @param[in] server_socket  FHOS server socket structure
 * @return FHOS_E_XXXX
 */
int fhos_server_socket_init(const char           *path,
                            struct thread_master *master,
                            fhos_server_socket_t *server_socket);

/**
 * Install accept handler to server
 * @param[in] server_socket  Server socket
 * @param[in] accept         Accept handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_server_socket_accept_install(fhos_server_socket_t        *server_socket,
                                      fhos_socket_accept_handler_t accept)
{
    if (server_socket)
    {
        server_socket->accept = accept;
        return FHOS_E_NONE;
    }

    return FHOS_E_PARAM;
}


/**
 * Install client close handler to server
 * @param[in] server_socket  Server socket
 * @param[in] accept         Accept handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_server_socket_close_install(fhos_server_socket_t       *server_socket,
                                     fhos_socket_close_handler_t close)
{
  if (server_socket)
  {
      server_socket->socket.close = close;
      return FHOS_E_NONE;
  }

  return FHOS_E_PARAM;
}


/**
 * Server socket read message from client start
 * @param[in] socket FHOS socket
 * @return FHOS_E_XXXX
 */
int fhos_server_socket_recv_start(fhos_socket_t *socket);

/**
 * Create unix domain client
 * @param[in] path           socket path
 * @param[in] master         Thread master
 * @param[in] client_socket  FHOS client socket structure
 * @param[in] handler        Socket connect callback handler
 * @return FHOS_E_XXXX
 */
int fhos_client_socket_init(const char                   *path,
                            struct thread_master         *master,
                            fhos_client_socket_t         *client_socket,
                            fhos_socket_connect_handler_t handler);

/**
 * Destroy FHOS server socket
 * @param[in] server_socket  FHOS socket structure
 * @return FHOS_E_XXXX
 */
int fhos_socket_server_destroy(fhos_server_socket_t *server_socket);

/**
 * Destroy FHOS client socket
 * @param[in] client_socket  FHOS socket structure
 * @return FHOS_E_XXXX
 */
int fhos_socket_client_destroy(fhos_client_socket_t *client_socket);

/**
 * Client connect server
 * @param[in] client_socket  Client socket
 * @return FHOS_E_XXXX
 */
int fhos_socket_client_connect(fhos_client_socket_t *client_socket);

/**
 * Read n bytes to buffer
 * @param[in] fd  File descriptor
 * @param[in] buf Buffer address
 * @param[in] len Buffer length
 * @return FHOS_E_XXXX
 */
int fhos_socket_readn(int fd, char *buf, int len);

/**
 * Write n bytes to buffer
 * @param[in] fd  File descriptor
 * @param[in] buf Buffer address
 * @param[in] len Buffer length
 * @return FHOS_E_XXXX
 */
int fhos_socket_writen(int fd, const char *buf, int len);

/**
 * Install client close handler
 * @param[in] server_socket  Server socket
 * @param[in] accept         Accept handler
 * @return FHOS_E_XXXX
 */
static inline
int fhos_client_socket_close_install(fhos_client_socket_t       *client_socket,
                                     fhos_socket_close_handler_t close)
{
  if (client_socket)
  {
      client_socket->socket.close = close;
      return FHOS_E_NONE;
  }

  return FHOS_E_PARAM;
}

#endif
