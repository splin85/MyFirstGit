

#ifndef __RPC_CLIENT_IGMP_H__
#define __RPC_CLIENT_IGMP_H__


#include <sai/types.h>
#include <rpc/common/msg.h>
#include <rpc/core/rpc.h>


int rpc_igmp_mcast_snooping_enable_set_async(const int unit, const int enable, fhos_rpc_request_cb cb, void *user_data);

int rpc_mcast_port_remove_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data);
int rpc_mcast_addr_remove_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data);
int rpc_mcast_port_add_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data);
int rpc_mcast_addr_add_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data);
int rpc_mcast_port_add(sai_mcast_addr_t *maddr);
int rpc_mcast_addr_add(sai_mcast_addr_t *maddr);
int rpc_mcast_port_remove(sai_mcast_addr_t *maddr);
int rpc_mcast_addr_remove(sai_mcast_addr_t *maddr);


#endif
