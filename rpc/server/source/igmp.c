/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author Xie Peng
 * @date 03/15/18
 */


#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/port.h>
#include <sai/mcast.h>

#include <rpc/server/server.h>
#include <rpc/server/port.h>
#include <rpc/server/igmp.h>
#include <rpc/common/msg.h>


static int rpc_igmp_mcast_snooping_enable_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int            len    = RPC_API_MIN_LEN;
    rpc_two_int_t *two    = (rpc_two_int_t *)RPC_API_DATA(api_hdr);
    int            unit   = two->val1;
    int            enable = two->val2;


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_mcast_igmp_snooping_enable_set(unit, enable);
#else
    api_hdr->code = FHOS_E_NONE;
    FHOS_UNUSED(unit);
    FHOS_UNUSED(enable);
#endif

    /* return the value to igmpd to update the igmp status. */
    two->val1 = enable;

    len += sizeof(int);

    return len;
}


static int rpc_mcast_addr_add_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int               len   = RPC_API_MIN_LEN;
    sai_mcast_addr_t *maddr = (sai_mcast_addr_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_mcast_addr_add(FHOS_UNIT_DEFAULT, maddr);
#else
    FHOS_UNUSED(maddr);
    api_hdr->code = FHOS_E_NONE;
#endif

    len += sizeof(*maddr);

    return len;
}

static int rpc_mcast_addr_remove_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int               len   = RPC_API_MIN_LEN;
    sai_mcast_addr_t *maddr = (sai_mcast_addr_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_mcast_addr_remove(FHOS_UNIT_DEFAULT, maddr->mac, maddr->vid);
#else
    api_hdr->code = FHOS_E_NONE;
#endif

    len += sizeof(*maddr);

    return len;
}

static int rpc_mcast_port_add_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int               len   = RPC_API_MIN_LEN;
    sai_mcast_addr_t *maddr = (sai_mcast_addr_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_mcast_port_add(FHOS_UNIT_DEFAULT, maddr);
#else
    FHOS_UNUSED(maddr);
    api_hdr->code = FHOS_E_NONE;
#endif

    len += sizeof(*maddr);

    return len;
}


static int rpc_mcast_port_remove_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int               len   = RPC_API_MIN_LEN;
    sai_mcast_addr_t *maddr = (sai_mcast_addr_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_mcast_port_remove(FHOS_UNIT_DEFAULT, maddr);
#else
    api_hdr->code = FHOS_E_NONE;
#endif

    len += sizeof(*maddr);

    return len;
}


int rpc_server_igmp_init()
{
    rpc_server_api_register(RPC_IGMP_SNOOPING_ENABLE_SET ,   rpc_igmp_mcast_snooping_enable_set_ack);
    rpc_server_api_register(RPC_MCAST_ADDR_ADD           ,   rpc_mcast_addr_add_ack);
    rpc_server_api_register(RPC_MCAST_ADDR_REMOVE        ,   rpc_mcast_addr_remove_ack);
    rpc_server_api_register(RPC_MCAST_PORT_ADD           ,   rpc_mcast_port_add_ack);
    rpc_server_api_register(RPC_MCAST_PORT_REMOVE        ,   rpc_mcast_port_remove_ack);

    return FHOS_E_NONE;
}
