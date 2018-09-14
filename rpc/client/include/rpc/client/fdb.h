/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author long qiang
 * @date   4/11/2018
 */

#ifndef __RPC_CLIENT_FDB_H__
#define __RPC_CLIENT_FDB_H__

int rpc_fdb_mac_is_exist_async(const fhos_mac_t    mac,
                               const fhos_port_t   port,
                               fhos_rpc_request_cb cb,
                               void               *user_data);

int rpc_fdb_mac_is_exist(const fhos_mac_t mac, const fhos_port_t port);

#endif

