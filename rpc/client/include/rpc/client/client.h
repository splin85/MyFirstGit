/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/26/18
 */

#ifndef __RPC_CLIENT_CLIENT_H__
#define __RPC_CLIENT_CLIENT_H__


#include <rpc/core/rpc.h>

#include <rpc/common/common.h>




typedef int (*rpc_client_api_cb)(fhos_rpc_header_t *hdr, void * cookie);



/**
 * RPC API client init
 * @param[in] pid     Process ID
 * @param[in] master  Thread master
 * @param[in] handler Connect callback when connect to server
 * @return FHOS_E_XXXX
 */
int rpc_client_init(const fhos_rpc_pid_t       pid,
                    struct thread_master      *master,
                    fhos_rpc_connect_handler_t handler);

/**
 * RPC api client destory
 * @param[in] pid  Process ID
 * @return FHOS_E_XXXX
 */
int rpc_client_destroy(const fhos_rpc_pid_t pid);

/**
 * Check the rpc client connect to server or not
 * @return 1 connected 0 disconnected
 */
int rpc_client_is_alive();

/**
 * Register connect handler, It will be called when client connect to server
 *
 * @attention
 * Currently, this function have bug!
 * Because if server is start before client, It may be never called!
 * More please @see rpc_client_init
 *
 * @param[in] pid      Process ID
 * @param[in] handler  Connect callback when connect to server
 * @return FHOS_E_XXXX
 */
int rpc_client_connect_register(const fhos_rpc_pid_t pid, fhos_rpc_connect_handler_t handler);

/**
 * Register close handler, It will be called when client disconnect to server
 * @param[in] pid      Process ID
 * @param[in] handler  Close callback when disconnect to server
 * @return FHOS_E_XXXX
 */
int rpc_client_close_register(const fhos_rpc_pid_t pid, fhos_rpc_close_handler_t handler);

/**
 * RPC API callback register
 * @param[in] id  API ID
 * @param[in] cb  API handler
 * @return FHOS_E_XXXX
 */
int rpc_client_api_register(const rpc_api_id_t id, rpc_client_api_cb cb);

/**
 * RPC client write to RPC server
 * @param[in] buf        API request buffer, start from fhos_rpc_header_t
 * @param[in] len        Request length
 * @param[in] cb         Write callback function
 * @param[in] user_data  User data used by callback function
 * @param[in] flags      Write flags, sync or async, etc
 * @return FHOS_E_XXXX
 */
int rpc_client_write(char *buf, int len, fhos_rpc_request_cb cb, void *user_data, int flags);


/**
 * RPC client set request server reponse callback,
 * Always used in synchronous to get server return code
 * @attention
 * The return code store in api_header_t code
 *
 * More, this is set template, you can follow it to add new set function.
 *
 * @param[in] buf        API request buffer, start from fhos_rpc_header_t
 * @param[in] len        Request length
 * @param[in] user_data  User data used by callback function
 * @return FHOS_E_XXXX
 */
int rpc_client_default_set_ack_cb(char *buf, int len, void *user_data);

/**
 * RPC client get request server reponse callback,
 * Always used in synchronous to get server return code
 * @attention
 * The return code store in api_header_t code,
 * and the get value store API_RPC_DATA which only one int.
 * So, user_data must large than rpc_two_int_t
 * two->val1 store the return code
 * two->val2 store the get int value
 *
 * More, this is get template, you can follow it to add new get function
 * which value can not store in ONE int.
 * @see rpc_pkt_rx_ack
 *
 * @param[in] buf        API request buffer, start from fhos_rpc_header_t
 * @param[in] len        Request length
 * @param[in] user_data  User data used by callback function
 * @return FHOS_E_XXXX
 */
int rpc_client_default_get_ack_cb(char *buf, int len, void *user_data);


/**
 * RPC client get port info server reponse callback,
 *
 * @param[in] buf        API request buffer, start from fhos_rpc_header_t
 * @param[in] len        Request length
 * @param[in] user_data  User data used by callback function
 * @return FHOS_E_XXXX
 */

int rpc_client_port_info_get_ack_cb(char *buf, int len, void *user_data);

#endif

