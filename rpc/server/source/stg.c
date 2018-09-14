/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author wang shun qiang
 * @date 07/31/2018
 */

#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/stg.h>

#include <rpc/server/server.h>
#include <rpc/server/stg.h>
#include <rpc/common/msg.h>




int rpc_server_stg_default_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_one_int_t  *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_default_get(FHOS_UNIT_DEFAULT, &one->val1);
    len += sizeof(int);
#else
    FHOS_UNUSED(one);
#endif

    return len;
}


int rpc_server_stg_create_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_one_int_t  *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_create(FHOS_UNIT_DEFAULT, &one->val1);
    len += sizeof(rpc_one_int_t);
#else
    FHOS_UNUSED(one);
#endif

    return len;
}


int rpc_server_stg_destroy_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_one_int_t  *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_destroy(FHOS_UNIT_DEFAULT, one->val1);
    len += sizeof(rpc_one_int_t);
#else
    FHOS_UNUSED(one);
#endif

    return len;
}


int rpc_server_stg_vlan_add_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_vlan_add(FHOS_UNIT_DEFAULT, two->val1, two->val2);
    len += sizeof(rpc_two_int_t);
#else
    FHOS_UNUSED(two);
#endif

    return len;
}


int rpc_server_stg_vlan_remove_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_vlan_remove(FHOS_UNIT_DEFAULT, two->val1, two->val2);
    len += sizeof(rpc_two_int_t);
#else
    FHOS_UNUSED(two);
#endif

    return len;
}


int rpc_server_stg_port_state_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int              len = RPC_API_MIN_LEN;
    rpc_three_int_t *three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_stp_set(FHOS_UNIT_DEFAULT, three->val2, three->val1, three->val3);
    len += sizeof(rpc_three_int_t);
#else
    FHOS_UNUSED(three);
#endif

    return len;
}


int rpc_server_stg_port_state_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int              len = RPC_API_MIN_LEN;
    rpc_three_int_t *three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_stg_stp_get(FHOS_UNIT_DEFAULT, three->val2, three->val1, &three->val3);
    len += sizeof(rpc_three_int_t);
#else
    FHOS_UNUSED(three);
#endif

    return len;
}


int rpc_server_stg_init()
{
    rpc_server_api_register(RPC_STG_DEFAULT_GET,    rpc_server_stg_default_get_ack);
    rpc_server_api_register(RPC_STG_CREATE,         rpc_server_stg_create_ack);
    rpc_server_api_register(RPC_STG_DESTROY,        rpc_server_stg_destroy_ack);
    rpc_server_api_register(RPC_STG_VLAN_ADD,       rpc_server_stg_vlan_add_ack);
    rpc_server_api_register(RPC_STG_VLAN_REMOVE,    rpc_server_stg_vlan_remove_ack);
    rpc_server_api_register(RPC_STG_PORT_STATE_SET, rpc_server_stg_port_state_set_ack);
    rpc_server_api_register(RPC_STG_PORT_STATE_GET, rpc_server_stg_port_state_get_ack);

    return FHOS_E_NONE;
}
