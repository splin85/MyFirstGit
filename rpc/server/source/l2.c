/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author wang shun qiang
 * @date   9/11/2018
 */

#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/l2.h>

#include <rpc/common/l2.h>
#include <rpc/common/msg.h>
#include <rpc/server/server.h>
#include <rpc/server/l2.h>



int rpc_server_l2_cache_add_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int                 len = RPC_API_MIN_LEN;
    rpc_l2_cache_addr_t *cache = (rpc_l2_cache_addr_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    cache->code = sai_l2_cache_addr_add(cache->unit, cache->index, &cache->addr, &cache->index_used);
    len += sizeof(rpc_l2_cache_addr_t);
#else
    FHOS_UNUSED(cache);
#endif

    return len;
}


int rpc_server_l2_cache_addr_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int                   len = RPC_API_MIN_LEN;
    rpc_l2_cache_addr_t  *cache = (rpc_l2_cache_addr_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    cache->code = sai_l2_cache_addr_get(cache->unit, cache->index, &cache->addr);
    len += sizeof(rpc_l2_cache_addr_t);
#else
    FHOS_UNUSED(cache);
#endif

    return len;
}


int rpc_server_l2_cache_addr_del_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_l2_cache_addr_delete(two->val1, two->val2);
#else
    FHOS_UNUSED(two);
#endif

    return RPC_API_MIN_LEN;
}


int rpc_server_l2_init()
{
    rpc_server_api_register(RPC_L2_CACHE_ADDR_ADD, rpc_server_l2_cache_add_ack);
    rpc_server_api_register(RPC_L2_CACHE_ADDR_GET, rpc_server_l2_cache_addr_get_ack);
    rpc_server_api_register(RPC_L2_CACHE_ADDR_DEL, rpc_server_l2_cache_addr_del_ack);

    return FHOS_E_NONE;
}



