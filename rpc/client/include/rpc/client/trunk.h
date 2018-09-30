

#ifndef __RPC_CLIENT_TRUNK_H__
#define __RPC_CLIENT_TRUNK_H__

#include <rpc/common/msg.h>



typedef void (*rpc_trunk_group_notify_handler_t)(rpc_trunk_group_t *);

void rpc_trunk_group_register(rpc_trunk_group_notify_handler_t cb);

int rpc_trunk_group_get(rpc_trunk_group_t *group, size_t size);

int rpc_trunk_init(const int unit);

#endif

