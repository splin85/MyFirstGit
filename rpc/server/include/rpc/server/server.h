
#ifndef __RPC_SERVER_SERVER_H__
#define __RPC_SERVER_SERVER_H__


#include <rpc/core/rpc.h>

#include <rpc/common/common.h>



typedef int (*rpc_server_api_cb)(rpc_api_header_t *hdr, void *cookie);


/**
 * RPC API server init
 * @param[in] master Thread master
 * @return FHOS_E_XXXX
 */
int rpc_server_init(struct thread_master *master);

/**
 * RPC api client destory
 * @return FHOS_E_XXXX
 */
int rpc_server_destroy();

/**
 * RPC API callback register
 * @param[in] id  API ID
 * @param[in] cb  API handler
 * @return FHOS_E_XXXX
 */
int rpc_server_api_register(const rpc_api_id_t id, rpc_server_api_cb cb);

/**
 * RPC server write to RPC client
 * @param[in] buf        API request buffer, start from fhos_rpc_header_t
 * @param[in] len        Request length
 * @param[in] cb         Write callback function
 * @param[in] user_data  User data used by callback function
 * @param[in] flags      Write flags, sync or async, etc
 * @return FHOS_E_XXXX
 */
int rpc_server_write(fhos_rpc_pid_t      pid,
                     char               *buf,
                     int                 len,
                     fhos_rpc_request_cb cb,
                     void               *user_data,
                     int                 flags);

/**
 * RPC server request client reponse callback,
 * Always used in synchronous to get client return code
 * @param[in] buf        API request buffer, start from fhos_rpc_header_t
 * @param[in] len        Request length
 * @param[in] user_data  User data used by callback function
 * @return FHOS_E_XXXX
 */
int rpc_server_default_request_ack_cb(char *buf, int len, void *user_data);


#endif
