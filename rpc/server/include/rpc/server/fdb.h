/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author long qiang
 * @date 4/11/2018
 */
#ifndef __RPC_SERVER_FDB_H__
#define __RPC_SERVER_FDB_H__


int rpc_server_fdb_mac_exist_ack(rpc_api_header_t *api_hdr, void *cookie);

int rpc_server_fdb_init();

#endif

