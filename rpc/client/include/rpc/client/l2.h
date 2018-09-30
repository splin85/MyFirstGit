
#ifndef __RPC_CLIENT_L2_H__
#define __RPC_CLIENT_L2_H__


/* For l2_cache_addr index*/
typedef enum {
    L2_CACHE_ADDR_INDEX_BPDU        =  0,
    L2_CACHE_ADDR_INDEX_LLDP,
    L2_CACHE_ADDR_INDEX_DOT1X,
    L2_CACHE_ADDR_INDEX_LOOPBACK,
    /* add index below */

    L2_CACHE_ADDR_INDEX_END, /* don't change */
} fhos_l2_cache_index_t;



int rpc_l2_cache_addr_add(rpc_l2_cache_addr_t *cache); /* demo see mstp_l2_cache_addr_add() in mstp.c */
int rpc_l2_cache_addr_get(const int unit, const int index, sai_l2_cache_addr_t *addr);
int rpc_l2_cache_addr_del(const int unit, const int index);

#endif


