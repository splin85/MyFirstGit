/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/26/18
 */

#ifndef __RPC_COMMON_COMMON_H__
#define __RPC_COMMON_COMMON_H__


#include <rpc/core/rpc.h>


#define RPC_API_PATH                "/tmp/rpc_api"


#define RPC_API_HDR_CAST(buf)       (rpc_api_header_t *)(buf)

/* RPC Header + API Header + Data Payload */

#define RPC_API_HDR_LEN             (int)(FHOS_RPC_MSG_ALIGN(sizeof(rpc_api_header_t)))
#define RPC_API_SPACE(len)          (int)FHOS_RPC_SPACE(RPC_API_HDR_LEN + (len))
#define RPC_API_HDR(rpc_hdr)        RPC_API_HDR_CAST(FHOS_RPC_DATA(rpc_hdr))
#define RPC_API_DATA(api_hdr)       ((void *)(((char *)api_hdr) + RPC_API_HDR_LEN))

#define RPC_API_HDR_INIT(hdr)       memset(hdr, 0, RPC_API_HDR_LEN)
#define RPC_API_DATA_LEN(rpc_hdr)   (int)((len) - FHOS_RPC_HDR_LEN - RPC_API_HDR_LEN)

#define RPC_API_MIN_LEN             RPC_API_SPACE(0)


#define RPC_API_ID_IS_VALID(id)     (((id) > RPC_API_ID_START) && ((id) < RPC_API_ID_COUNT))


typedef enum rpc_api_id_e
{
    RPC_API_ID_START,

    /* PORT API */
    RPC_PORT_ENABLE_SET,
    RPC_PORT_ENABLE_GET,
    RPC_PORT_SPEED_SET,
    RPC_PORT_SPEED_GET,
    RPC_PORT_DUPLEX_SET,
    RPC_PORT_DUPLEX_GET,
    RPC_PORT_STP_SET,
    RPC_PORT_STP_GET,
    RPC_PORT_LINK_GET,
    RPC_PORT_LINK_NOTIFY,
    RPC_PORT_DUPLEX_NOTIFY,
    RPC_PORT_SPEED_NOTIFY,
    RPC_PORT_INFO_GET,

    /* PKT API */
    RPC_PKT_RX,
    RPC_PKT_TX,

    /* NTP API */
    RPC_NTP_CONFIG_CHANGE,
    RPC_NTP_IS_ENABLE,

    /* VLAN API */
    RPC_VLAN_MEMBER_GET,
    RPC_VLAN_IS_EXISTS,
    RPC_VLAN_DEFAULT_GET,
    RPC_VLAN_ALL_GET,
    RPC_VLAN_NOTIFY,

    /* IGMP API */
    RPC_IGMP_SNOOPING_ENABLE_SET,
    RPC_MCAST_ADDR_ADD,
    RPC_MCAST_ADDR_REMOVE,
    RPC_MCAST_PORT_ADD,
    RPC_MCAST_PORT_REMOVE,

    /* SYS API*/
    RPC_SYS_DEVICE_ID_GET,
    RPC_SYS_DEVICE_ID_NOTIFY,

    /* FDB API*/
    RPC_FDB_MAC_EXIST,

    /* TRUNK API */
    RPC_TRUNK_GRP_GET,
    RPC_TRUNK_GRP_NOTIFY,

    /* DHCP API*/
    RPC_PORT_DHCP_NOTIFY,
    RPC_PORT_GLOBAL_DHCP_SET,

    /* DOT1X API */
    RPC_DOT1X_AUTH_MODE_SET,
    RPC_DOT1X_AUTH_EGRESS_SET,

    /* MSTP API */
    RPC_STG_DEFAULT_GET,
    RPC_STG_CREATE,
    RPC_STG_DESTROY,
    RPC_STG_VLAN_ADD,
    RPC_STG_VLAN_REMOVE,
    RPC_STG_PORT_STATE_SET,
    RPC_STG_PORT_STATE_GET,


    /* L2 CACHE ADDR */
    RPC_L2_CACHE_ADDR_ADD,
    RPC_L2_CACHE_ADDR_DEL,
    RPC_L2_CACHE_ADDR_GET,

    RPC_API_ID_COUNT
} rpc_api_id_t;


typedef struct rpc_api_header_s
{
    uint32_t    id;    /* @see rpc_api_t */
    int         code;  /* Call API return code, @see FHOS_E_XXXX */
} rpc_api_header_t;


#endif
