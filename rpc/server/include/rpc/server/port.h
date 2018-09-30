
#ifndef __RPC_SERVER_PORT_H__
#define __RPC_SERVER_PORT_H__


#include <sai/types.h>


/**
 * RPC port init
 * @return FHOS_E_XXXX
 */
int rpc_server_port_init();

/**
 * Call SAI to enable port or not
 * @see rpc_port_enable_set
 * @see rpc_port_enable_set_async
 * @param[in] api_hdr  API header
 * @param[in] cookie   Server cookie
 * @return Response length
 */
int rpc_server_port_enable_set_ack(rpc_api_header_t *api_hdr, void *cookie);

/**
 * Notify RPC client port link up/down asynchronous
 * @param[in] pid    Process ID
 * @param[in] port   Logical port
 * @param[in] status Link status
 * @return FHOS_E_XXXX
 */
int rpc_port_link_notify_async(const fhos_rpc_pid_t pid,
                               const sai_port_t     port,
                               const int            status,
                               fhos_rpc_request_cb  cb,
                               void                *user_data);

/**
 * Notify RPC client port duplex asynchronous
 * @param[in] pid    Process ID
 * @param[in] port   Logical port
 * @param[in] duplex Port duplex
 * @return FHOS_E_XXXX
 */
int rpc_port_duplex_notify_async(const fhos_rpc_pid_t pid,
                                 const sai_port_t     port,
                                 const int            duplex,
                                 fhos_rpc_request_cb  cb,
                                 void                *user_data);

/**
 * Notify RPC client port speed asynchronous
 * @param[in] pid    Process ID
 * @param[in] port   Logical port
 * @param[in] speed  Port Speed
 * @return FHOS_E_XXXX
 */
int rpc_port_speed_notify_async(const fhos_rpc_pid_t pid,
                                const sai_port_t     port,
                                const int            speed,
                                fhos_rpc_request_cb  cb,
                                void                *user_data);

/**
 * Notify RPC client port link up/down
 * @see rpc_port_link_notify_async
 * @param[in] pid    Process ID
 * @param[in] port   Logical port
 * @param[in] status Link status
 * @return FHOS_E_XXXX
 */
int rpc_port_link_notify(const fhos_rpc_pid_t pid, const sai_port_t port, const int status);

/**
 * Call SAI to get port link status
 * @param[in] api_hdr  API header
 * @param[in] cookie   Server cookie
 * @return Response length
 */
int rpc_server_port_link_get_ack(rpc_api_header_t * api_hdr, void * cookie);


/**
 * Notify RPC client port dhcp enable/disable
 * @see rpc_port_dhcp_notify_async
 * @param[in] pid    Process ID
 * @param[in] port   Logical port
 * @param[in] enable enable status
 * @return FHOS_E_XXXX
 */
int rpc_port_dhcp_notify_async(const fhos_rpc_pid_t pid,
                               const sai_port_t     port,
                               const int            enable,
                               fhos_rpc_request_cb  cb,
                               void                *user_data);

#endif

