
#ifndef __RPC_CLIENT_VLAN_H__
#define __RPC_CLIENT_VLAN_H__

#include <sai/types.h>
#include <rpc/common/msg.h>
#include <rpc/core/rpc.h>

typedef int (*rpc_vlan_notify_cb)(sai_vlan_t vid, int action);

int rpc_vlan_member_get(sai_vlan_t vid, sai_pbmp_t *member, sai_pbmp_t *tag);
int rpc_vlan_is_exists(sai_vlan_t vid);
int rpc_vlan_default_get(sai_port_t port, int *vlan);
int rpc_vlan_all_get(uint8_t *vlan);
int rpc_vlan_notify_register(rpc_vlan_notify_cb cb);
int rpc_vlan_init(const int unit);

#endif
