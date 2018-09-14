/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author Xie Peng
 * @date 05/31/18
 */


#include <fhos/errno.h>
#include <fhos/utils.h>
#include <fhos/common.h>

#include <sai/types.h>
#include <sai/port.h>
#include <sai/mcast.h>

#include <rpc/server/server.h>
#include <rpc/server/port.h>
#include <rpc/server/igmp.h>
#include <rpc/common/msg.h>

#include <vlan/vlan.h>

static int rpc_vlan_member_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int              len   = RPC_API_MIN_LEN;
    rpc_three_int_t *three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);
    sai_vlan_t       vid;
    l2_vlan_t        vlan;
    sai_pbmp_t       untag;


    FHOS_UNUSED(cookie);

    vid = three->val1;

    three->val1 = l2_vlan_detail_get(vid, &vlan);

    if (three->val1 != FHOS_E_NONE)
    {
        three->val2 = 0;
        three->val3 = 0;
    }
    else
    {
        SAI_PBMP_ASSIGN(three->val2, vlan.member);
        SAI_PBMP_NEGATE(untag, vlan.taged);
        SAI_PBMP_ASSIGN(three->val3, SAI_PBMP_AND(untag, vlan.member));
    }

    len += sizeof(int) * 3;

    return len;
}


static int rpc_vlan_is_exists_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int            len = RPC_API_MIN_LEN;
    rpc_one_int_t *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);
    sai_vlan_t     vid;


    FHOS_UNUSED(cookie);

    vid = one->val1;

    one->val1 = l2_vlan_is_exists(vid);

    len += sizeof(int) * 1;

    return len;
}

static int rpc_vlan_default_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int            len = RPC_API_MIN_LEN;
    rpc_two_int_t *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);
    sai_port_t     port;


    FHOS_UNUSED(cookie);

    port = two->val1;

    two->val1 = l2_vlan_default_get(port, (uint16_t *)&two->val2);

    len += sizeof(int) * 2;

    return len;
}


static int rpc_vlan_all_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int      len  = RPC_API_MIN_LEN;
    uint8_t *vlan = (uint8_t *)RPC_API_DATA(api_hdr);

    FHOS_UNUSED(cookie);

    api_hdr->code = l2_vlan_all_get(vlan);

    len += L2_VLAN_MAP_BYTE;

    return len;
}


static int rpc_vlan_notify_async_impl(fhos_rpc_pid_t       pid,
                                      const sai_vlan_t     vid,
                                      const int            action,
                                      fhos_rpc_request_cb  cb,
                                      void                *user_data,
                                      int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t *api_hdr     = RPC_API_HDR(buf);
    rpc_two_int_t    *vlan_status = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!FHOS_RPC_PID_IS_VALID(pid) &&
        !L2D_IS_VALID_VID(vid))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_VLAN_NOTIFY;
    api_hdr->code = 0;

    vlan_status->val1 = vid;
    vlan_status->val2 = action;

    rc = rpc_server_write(pid, buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_vlan_notify_async(const fhos_rpc_pid_t      pid,
                               const sai_vlan_t     vid,
                               const int            action,
                               fhos_rpc_request_cb  cb,
                               void                *user_data)
{
    return rpc_vlan_notify_async_impl(pid, vid, action, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_server_vlan_init()
{
    rpc_server_api_register(RPC_VLAN_MEMBER_GET, rpc_vlan_member_get_ack);
    rpc_server_api_register(RPC_VLAN_IS_EXISTS, rpc_vlan_is_exists_ack);
    rpc_server_api_register(RPC_VLAN_DEFAULT_GET, rpc_vlan_default_get_ack);
    rpc_server_api_register(RPC_VLAN_ALL_GET, rpc_vlan_all_get_ack);

    return FHOS_E_NONE;
}
