

#include <fhos/errno.h>
#include <fhos/utils.h>
#include <fhos/config.h>

#include <sai/types.h>

#include <rpc/server/server.h>
#include <rpc/server/trunk.h>
#include <rpc/common/msg.h>

#include <trunk/trunk_proc.h>



static
int rpc_server_trunk_group_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int                 tid;
    int                 len   = RPC_API_MIN_LEN;
    rpc_trunk_group_t  *group = (rpc_trunk_group_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    for (tid = TRUNK_NUM_MIN; tid <= TRUNK_NUM_MAX; tid++)
    {
        group->tid = tid;
        group->flag = TRUNK_GROUP_MEMBER_NONE;
        l2d_trunk_conf_member_get(tid, &group->conf);

        group++;
    }

    return len + sizeof(rpc_trunk_group_t) * TRUNK_NUM_MAX;
}


int rpc_server_trunk_init()
{
    rpc_server_api_register(RPC_TRUNK_GRP_GET,  rpc_server_trunk_group_get_ack);

    return FHOS_E_NONE;
}


static
int rpc_server_trunk_group_notify_impl(fhos_rpc_pid_t      pid,
                                       rpc_trunk_group_t  *group,
                                       fhos_rpc_request_cb cb,
                                       void               *user_data,
                                       int                 flags)
{
    char                buf[RPC_API_SPACE(sizeof(rpc_trunk_group_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_trunk_group_t  *bgroup  = (rpc_trunk_group_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id   = RPC_TRUNK_GRP_NOTIFY;
    api_hdr->code = 0;

    *bgroup = *group;

    return rpc_server_write(pid, buf, sizeof(buf), cb, user_data, flags);
}


int rpc_server_trunk_group_notify_async(fhos_rpc_pid_t      pid,
                                        rpc_trunk_group_t  *group,
                                        fhos_rpc_request_cb cb,
                                        void               *user_data)
{
    return rpc_server_trunk_group_notify_impl(pid, group, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}

