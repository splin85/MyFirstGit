/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author long qiang
 * @date   4/17/2018
 */

#include <fhos/errno.h>
#include <fhos/utils.h>
#include <fhos/config.h>

#include <sai/types.h>

#include <rpc/client/client.h>
#include <rpc/client/trunk.h>
#include <rpc/common/msg.h>



static rpc_trunk_group_notify_handler_t trunk_group_notify_cb = NULL;


static
int rpc_trunk_group_get_impl(fhos_rpc_request_cb  cb,
                             void                *user_data,
                             int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_trunk_group_t) * TRUNK_NUM_MAX)];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id   = RPC_TRUNK_GRP_GET;
    api_hdr->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_trunk_group_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_trunk_group_t   *group   = RPC_API_DATA(api_hdr);


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return FHOS_E_PARAM;
        }
    }

    if (user_data)
    {
        memcpy(user_data, group, sizeof(*group) * TRUNK_NUM_MAX);
    }

    return FHOS_E_NONE;
}


int rpc_trunk_group_get(rpc_trunk_group_t *group, size_t size)
{
    int             rc;


    if (!group || sizeof(*group) * TRUNK_NUM_MAX != size)
    {
        return FHOS_E_PARAM;
    }

    group->tid = -1;

    rc = rpc_trunk_group_get_impl(rpc_trunk_group_get_ack_cb, group, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    return (-1 != group->tid) ? FHOS_E_NONE : FHOS_E_TIMEOUT;
}


void rpc_trunk_group_register(rpc_trunk_group_notify_handler_t cb)
{
    trunk_group_notify_cb = cb;
}


static
int rpc_client_trunk_group_notify_ack(fhos_rpc_header_t *hdr, void * cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(hdr);
    rpc_trunk_group_t  *group   = (rpc_trunk_group_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (trunk_group_notify_cb)
    {
        trunk_group_notify_cb(group);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;

}


int rpc_trunk_init(const int unit)
{
    FHOS_UNUSED(unit);

    rpc_client_api_register(RPC_TRUNK_GRP_NOTIFY, rpc_client_trunk_group_notify_ack);

    return FHOS_E_NONE;
}
