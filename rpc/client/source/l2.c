/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author wang shun qiang
 * @date   9/11/2018
 */
#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/l2.h>

#include <rpc/common/l2.h>
#include <rpc/common/msg.h>
#include <rpc/client/client.h>
#include <rpc/client/l2.h>


static
int rpc_l2_cache_addr_add_impl(rpc_l2_cache_addr_t  *cache,
                               fhos_rpc_request_cb  cb,
                               void                *user_data,
                               int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_l2_cache_addr_t))];
    rpc_api_header_t    *api_hdr = RPC_API_HDR(buf);
    rpc_l2_cache_addr_t *cache2 = (rpc_l2_cache_addr_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_L2_CACHE_ADDR_ADD;
    api_hdr->code = 0;

    memcpy(cache2, cache, sizeof(rpc_l2_cache_addr_t));

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_client_l2_cache_addr_add_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t      *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_l2_cache_addr_t   *src = (rpc_l2_cache_addr_t *)RPC_API_DATA(api_hdr);
    rpc_l2_cache_addr_t   *dst = (rpc_l2_cache_addr_t *)user_data;


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            dst->code = FHOS_E_TIMEOUT;

            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return -1;
        }
    }

    if (user_data)
    {
        *dst = *src;
    }

    return 0;
}


int rpc_l2_cache_addr_add(rpc_l2_cache_addr_t *cache)
{
    int             rc;


    if (!cache)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_l2_cache_addr_add_impl(cache, rpc_client_l2_cache_addr_add_ack_cb,
                                    cache, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        rc = cache->code;
    }

    return rc;
}


static
int rpc_l2_cache_addr_get_impl(const int            unit,
                               const int            index,
                               fhos_rpc_request_cb  cb,
                               void                *user_data,
                               int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_l2_cache_addr_t))];
    rpc_api_header_t    *api_hdr = RPC_API_HDR(buf);
    rpc_l2_cache_addr_t *cache = (rpc_l2_cache_addr_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_L2_CACHE_ADDR_GET;
    api_hdr->code = 0;

    cache->unit = unit;
    cache->index = index;
    cache->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_client_l2_cache_addr_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t      *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_l2_cache_addr_t   *src = (rpc_l2_cache_addr_t *)RPC_API_DATA(api_hdr);
    rpc_l2_cache_addr_t   *dst = (rpc_l2_cache_addr_t *)user_data;


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            dst->code = FHOS_E_TIMEOUT;

            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return -1;
        }
    }

    if (user_data)
    {
        *dst = *src;
    }

    return 0;
}


int rpc_l2_cache_addr_get(const int unit, const int index, sai_l2_cache_addr_t *addr)
{
    int                 rc;
    rpc_l2_cache_addr_t cache;


    if (!addr)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_l2_cache_addr_get_impl(unit, index, rpc_client_l2_cache_addr_get_ack_cb,
                                    &cache, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    *addr = cache.addr;

    return cache.code;
}


static
int rpc_l2_cache_addr_del_impl(const int            unit,
                               const int            index,
                               fhos_rpc_request_cb  cb,
                               void                *user_data,
                               int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *addr = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_L2_CACHE_ADDR_DEL;
    api_hdr->code = 0;

    addr->val1 = unit;
    addr->val2 = index;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_l2_cache_addr_del(const int unit, const int index)
{
    int     rc;
    int     code;


    rc = rpc_l2_cache_addr_del_impl(unit, index, rpc_client_default_set_ack_cb,
                                    &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}
