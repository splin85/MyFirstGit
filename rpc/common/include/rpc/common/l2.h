
#ifndef __RPC_COMMON_L2_H__
#define __RPC_COMMON_L2_H__


typedef struct rpc_l2_cache_addr_s
{
    int                 code;
    int                 unit;
    int                 index;
    sai_l2_cache_addr_t addr;
    int                 index_used;
} rpc_l2_cache_addr_t;


#endif
