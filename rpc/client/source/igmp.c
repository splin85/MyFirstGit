/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author Xie Peng
 * @date 03/15/18
 */


#include <errno.h>
#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/mcast.h>

#include <rpc/client/client.h>
#include <rpc/client/port.h>
#include <rpc/client/igmp.h>
#include <rpc/common/msg.h>

#include <igmpd/igmp_rpc.h>


static int rpc_igmp_mcast_snooping_enable_set_impl(const int            unit,
                                                   const int            enable,
                                                   fhos_rpc_request_cb  cb,
                                                   void                *user_data,
                                                   int                  flags);


int rpc_igmp_mcast_snooping_enable_set_async(const int unit, const int enable, fhos_rpc_request_cb cb, void *user_data)
{
    int ret;

    ret = rpc_igmp_mcast_snooping_enable_set_impl(unit, enable,
                                                  cb,
                                                  user_data, FHOS_RPC_REQUEST_ASYNC);

    return ret;
}


static int rpc_igmp_mcast_snooping_enable_set_impl(const int            unit,
                                                   const int            enable,
                                                   fhos_rpc_request_cb  cb,
                                                   void                *user_data,
                                                   int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t    *my_arg  = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_IGMP_SNOOPING_ENABLE_SET;
    api_hdr->code = 0;

    my_arg->val1 = unit;
    my_arg->val2 = enable;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);


    return rc;
}


static int rpc_mcast_action_impl(sai_mcast_addr_t    *maddr,
                                 uint8_t              action,
                                 fhos_rpc_request_cb  cb,
                                 void                *user_data,
                                 int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(sizeof(sai_mcast_addr_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    sai_mcast_addr_t *data    = (sai_mcast_addr_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);

    switch (action)
    {
    case RPC_MCAST_ADDR_ADD:
        api_hdr->id = RPC_MCAST_ADDR_ADD;
        break;

    case RPC_MCAST_ADDR_REMOVE:
        api_hdr->id = RPC_MCAST_ADDR_REMOVE;
        break;

    case RPC_MCAST_PORT_ADD:
        api_hdr->id = RPC_MCAST_PORT_ADD;
        break;

    case RPC_MCAST_PORT_REMOVE:
        api_hdr->id = RPC_MCAST_PORT_REMOVE;
        break;

    default:
        break;
    }

    memcpy(data, maddr, sizeof(*maddr));

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_mcast_port_remove_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data)
{
    return rpc_mcast_action_impl(maddr, RPC_MCAST_PORT_REMOVE,
                                 cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_mcast_addr_remove_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data)
{
    return rpc_mcast_action_impl(maddr, RPC_MCAST_ADDR_REMOVE,
                                 cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_mcast_port_add_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data)
{
    return rpc_mcast_action_impl(maddr, RPC_MCAST_PORT_ADD,
                                 cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_mcast_addr_add_async(sai_mcast_addr_t *maddr, fhos_rpc_request_cb cb, void *user_data)
{
    return rpc_mcast_action_impl(maddr, RPC_MCAST_ADDR_ADD,
                                 cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_mcast_port_add(sai_mcast_addr_t *maddr)
{
    int ret;
    int code;

    ret = rpc_mcast_action_impl(maddr, RPC_MCAST_PORT_ADD,
                                rpc_mcast_port_add_ack_cb,
                                &code, FHOS_RPC_REQUEST_SYNC);

    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    return code;
}


int rpc_mcast_addr_add(sai_mcast_addr_t *maddr)
{
    int code;
    int ret;


    ret = rpc_mcast_action_impl(maddr, RPC_MCAST_ADDR_ADD,
                                rpc_mcast_addr_add_ack_cb,
                                &code, FHOS_RPC_REQUEST_SYNC);
    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    return code;
}


int rpc_mcast_port_remove(sai_mcast_addr_t *maddr)
{
    int code;
    int ret;


    ret = rpc_mcast_action_impl(maddr, RPC_MCAST_PORT_REMOVE, rpc_mcast_port_remove_ack_cb,
                                &code, FHOS_RPC_REQUEST_SYNC);
    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    return code;
}


int rpc_mcast_addr_remove(sai_mcast_addr_t *maddr)
{
    int code;
    int ret;


    ret = rpc_mcast_action_impl(maddr, RPC_MCAST_ADDR_REMOVE, rpc_mcast_addr_remove_ack_cb,
                                &code, FHOS_RPC_REQUEST_SYNC);
    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    return code;
}
