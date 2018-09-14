/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 03/01/18
 */

#ifndef __RPC_COMMON_MSG_H__
#define __RPC_COMMON_MSG_H__

#include <fhos/types.h>

#include <sai/mcast.h>


typedef struct rpc_one_int_s
{
    int     val1;
} rpc_one_int_t;


typedef struct rpc_two_int_s
{
    int     val1;
    int     val2;
} rpc_two_int_t;


typedef struct rpc_three_int_s
{
    int     val1;
    int     val2;
    int     val3;
} rpc_three_int_t;


typedef struct rpc_mac_int_s
{
    fhos_mac_t  mac;
    int         val;
} rpc_mac_int_t;


typedef enum rpc_port_speed_e
{
    RPC_PORT_SPEED_10M = 0,
    RPC_PORT_SPEED_100M,
    RPC_PORT_SPEED_1G,
    RPC_PORT_SPEED_2500M,
    RPC_PORT_SPEED_UNKOWN,
} rpc_port_speed_t;


typedef struct rpc_port_info_s
{
    int         code;
    sai_pbmp_t  link_bmp;   //port link status bitmap
    sai_pbmp_t  duplex_bmp; //port duplex bitmap
    uint8_t     speed[SAI_MAX_PORT_PER_SLOT];
    sai_pbmp_t  dhcp_bmp;
} rpc_port_info_t;


typedef enum rpc_trunk_group_notify_e
{
    TRUNK_GROUP_MEMBER_NONE,
    TRUNK_GROUP_MEMBER_ADD,
    TRUNK_GROUP_MEMBER_REMOVE,
} rpc_trunk_group_notify_t;


typedef struct rpc_trunk_group_s
{
    int         tid;
    int         flag; //see TRUNK_GRUOP_MEMBER_XXX
    sai_pbmp_t  conf;
} rpc_trunk_group_t;


typedef enum
{
    IGMP_GROUP_CREATE,
    IGMP_GROUP_DELETE,
    IGMP_PORT_ADD,
    IGMP_PORT_DEL,
} rpc_igmp_group_action_t;


typedef enum pkt_proto_e
{
    PKT_PROTO_NONE,
    PKT_PROTO_STP,
    PKT_PROTO_IGMP,
    PKT_PROTO_RAW,
    PKT_PROTO_DHCP,
    PKT_PROTO_DOT1X,
    PKT_PROTO_COUNT
} pkt_proto_t;


typedef struct rpc_pkt_s
{
    int             unit;       /* Device unit */
    int             flags;      /* @see SAI_PKT_FLAG_XXXX_ */
    sai_port_t      rx_port;    /* Rx packet port */
    sai_vlan_t      vlan;       /* Outer vlan */
    sai_vlan_t      inner_vlan; /* Innser vlan */
    sai_pbmp_t      tx_pbmp;    /* Tx target ports */
    sai_pbmp_t      tx_ubmp;    /* Tx untagged target ports */
    uint32_t        len;        /* Data length */
    uint8_t         data[0];    /* Packet start from L2 header */
} __attribute__ ((packed)) rpc_pkt_t;


/**
 * Use this macro to calculate the minimum length to Rx/Tx pkt
 */
#define RPC_PKT_LENGTH(len)     RPC_API_SPACE(sizeof(rpc_pkt_t) + len)

#define RPC_PKT_CAST(buf)       RPC_API_DATA(FHOS_RPC_DATA(buf))
#define RPC_PKT_BASE(pkt)       (char *)((char *)(pkt) - RPC_API_SPACE(0))


#endif
