/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author long qiang
 * @date   4/17/2018
 */

#ifndef __RPC_TRUNK_H__
#define __RPC_TRUNK_H__

#include <rpc/core/rpc.h>
#include <rpc/common/msg.h>

int rpc_server_trunk_group_notify_async(fhos_rpc_pid_t      pid,
                                        rpc_trunk_group_t  *group,
                                        fhos_rpc_request_cb cb,
                                        void               *user_data);

int rpc_server_trunk_init();

#endif


