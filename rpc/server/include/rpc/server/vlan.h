/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author Xie Peng
 * @date 05/31/18
 */

#ifndef __RPC_SERVER_VLAN_H__
#define __RPC_SERVER_VLAN_H__

/**
 * Notify RPC client port link up/down
 * @see rpc_vlan_notify_async
 * @param[in] pid    Process ID
 * @param[in] vid    VLAN id
 * @param[in] status VLAN status, new or del
 * @return FHOS_E_XXXX
 */
int rpc_vlan_notify_async(const fhos_rpc_pid_t      pid,
                               const sai_vlan_t     vid,
                               const int            action,
                               fhos_rpc_request_cb  cb,
                               void                *user_data);

int rpc_server_vlan_init();

#endif
