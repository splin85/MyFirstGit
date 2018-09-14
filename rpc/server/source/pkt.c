/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 03/02/18
 */
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#include <fhos/errno.h>
#include <fhos/utils.h>
#include <fhos/pkt.h>

#include <sai/types.h>
#include <sai/common.h>

#include <rpc/server/server.h>
#include <rpc/server/pkt.h>



#define RPC_RX_MAX_LENGTH           2048




static inline
int rpc_mac_cmp(const uint8_t *lhs, const sai_mac_t rhs)
{
    const short   *l = (const short *)lhs;
    const short   *r = (const short *)rhs;


    return ((l[0] ^ r[0]) | (l[1] ^ r[1]) | (l[2] ^ r[2])) != 0;
}


static
int rpc_pkt_is_dhcp(const uint8_t *buf, const uint32_t len)
{
    const uint16_t       server_port = htons(67);
    const uint16_t       client_port = htons(68);
    const struct ip     *ip;
    const struct udphdr *udp;


    if (len < (UDP_HDR_OFFSET + UDP_HDR_LEN))
    {
        return 0;
    }

    ip = (struct ip *)(buf + IP_HDR_OFFSET);
    if (ip->ip_p != IPPROTO_UDP)
    {
        return 0;
    }

    udp = (struct udphdr *)(buf + UDP_HDR_OFFSET);

    return (udp->source == server_port && udp->dest == client_port) ||
           (udp->source == client_port && udp->dest == server_port);
}


static inline
int rpc_pkt_is_dot1x(const uint8_t *buf, const uint32_t len)
{
    const uint16_t    ether_type = *(uint16_t *)(buf + ETH_NORMAL_TAG_PAYLOAD_OFFSET);
    const sai_mac_t   mac = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x03 };


    return (len > (ETH_NORMAL_TAG_PAYLOAD_OFFSET + ETHER_TYPE_LEN)) &&
           (rpc_mac_cmp(buf, mac) == 0)                             &&
           (htons(ether_type) == 0x888e);
}


static
int rpc_rx_pkt_proto(const uint8_t *buf, const uint32_t len)
{
    const sai_mac_t   stp_mac = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };


    if (rpc_mac_cmp(buf, stp_mac) == 0)
    {
        return PKT_PROTO_STP;
    }

    if (buf[0] == 0x01 && buf[1] == 0x00 && buf[2] == 0x5e)
    {
        return PKT_PROTO_IGMP;
    }

    if (rpc_pkt_is_dhcp(buf, len))
    {
        return PKT_PROTO_DHCP;
    }

    if (rpc_pkt_is_dot1x(buf, len))
    {
        return PKT_PROTO_DOT1X;
    }

    return PKT_PROTO_NONE;
}


/**
 * rpc_sai_rx receive packet from SAI
 */
int rpc_sai_rx(int unit, sai_pkt_t *pkt, void *cookie)
{
    pkt_proto_t     proto;
    fhos_rpc_pid_t  pid = RPC_PID_COUNT;
    char            buf[RPC_API_MIN_LEN + RPC_RX_MAX_LENGTH];
    rpc_pkt_t      *rpc_pkt = RPC_PKT_CAST(buf);


    FHOS_UNUSED(unit);
    FHOS_UNUSED(cookie);

    if (!pkt)
    {
        return SAI_RX_INVALID;
    }

    if (pkt->len > RPC_RX_MAX_LENGTH)
    {
        return SAI_RX_NOT_HANDLED;
    }

    proto = rpc_rx_pkt_proto(pkt->data, pkt->len);
    switch (proto)
    {
        case PKT_PROTO_STP:
            pid = RPC_PID_STP;
            break;

        case PKT_PROTO_IGMP:
            pid = RPC_PID_IGMP;
            break;

        case PKT_PROTO_DHCP:
            pid = RPC_PID_DHCP;
            break;

        case PKT_PROTO_DOT1X:
            pid = RPC_PID_DOT1X;
            break;

        default:
            return SAI_RX_NOT_HANDLED;
    }

    memset(rpc_pkt, 0, sizeof(*rpc_pkt));
    rpc_pkt->unit       = pkt->unit;
    rpc_pkt->flags      = pkt->flags;
    rpc_pkt->rx_port    = pkt->rx_port;
    rpc_pkt->vlan       = pkt->vlan;
    rpc_pkt->inner_vlan = pkt->inner_vlan;
    rpc_pkt->len        = pkt->len;
    memcpy(rpc_pkt->data, pkt->data, pkt->len);

    return rpc_pkt_tx(pid, rpc_pkt);
}


static
int rpc_server_pkt_tx_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    sai_pkt_t       sai_pkt;
    rpc_pkt_t      *rpc_pkt = (rpc_pkt_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    memset(&sai_pkt, 0, sizeof(sai_pkt));
    sai_pkt.unit       = rpc_pkt->unit;
    sai_pkt.flags      = rpc_pkt->flags;
    sai_pkt.rx_port    = rpc_pkt->rx_port;
    sai_pkt.vlan       = rpc_pkt->vlan;
    sai_pkt.inner_vlan = rpc_pkt->inner_vlan;
    sai_pkt.tx_pbmp    = rpc_pkt->tx_pbmp;
    sai_pkt.tx_ubmp    = rpc_pkt->tx_ubmp;
    sai_pkt.len        = rpc_pkt->len;
    sai_pkt.data       = rpc_pkt->data;

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_tx(rpc_pkt->unit, &sai_pkt, NULL);
#endif

    return RPC_API_MIN_LEN;
}


int rpc_server_pkt_init()
{
    rpc_server_api_register(RPC_PKT_TX, rpc_server_pkt_tx_ack);

#if SAI_FUNCTION_ENABLE
    sai_rx_register(0, 100, rpc_sai_rx, NULL);
#endif

    return FHOS_E_NONE;
}


static
int rpc_pkt_tx_impl(fhos_rpc_pid_t       pid,
                    rpc_pkt_t           *pkt,
                    fhos_rpc_request_cb  cb,
                    void                *user_data,
                    int                  flags)
{
    char               *base = RPC_PKT_BASE(pkt);
    rpc_api_header_t   *api_hdr = RPC_API_HDR(base);


    if (!FHOS_RPC_PID_IS_VALID(pid) || !pkt)
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PKT_RX;
    api_hdr->code = 0;

    return rpc_server_write(pid, base, RPC_PKT_LENGTH(pkt->len), cb, user_data, flags);
}


int rpc_pkt_tx_async(const fhos_rpc_pid_t pid,
                     rpc_pkt_t           *pkt,
                     fhos_rpc_request_cb  cb,
                     void                *user_data)
{
    return rpc_pkt_tx_impl(pid, pkt, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_pkt_tx(const fhos_rpc_pid_t pid, rpc_pkt_t *pkt)
{
    int     rc;
    int     code;


    rc = rpc_pkt_tx_impl(pid, pkt, rpc_server_default_request_ack_cb, &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


