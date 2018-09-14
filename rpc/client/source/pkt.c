/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 03/02/18
 */

#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/sai.h>

#include <rpc/common/msg.h>

#include <rpc/client/client.h>
#include <rpc/client/pkt.h>



static rpc_pkt_rx_cb  pkt_rx_cb = NULL;
static void          *pkt_rx_cookie = NULL;



/**
 * @see rpc_pkt_tx
 * @see rpc_pkt_tx_async
 */
static
int rpc_pkt_rx_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_pkt_t          *pkt = (rpc_pkt_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (pkt_rx_cb)
    {
        pkt_rx_cb(pkt, pkt_rx_cookie);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;
}


int rpc_pkt_init(const int unit)
{
    FHOS_UNUSED(unit);

    pkt_rx_cb = NULL;
    pkt_rx_cookie = NULL;

    rpc_client_api_register(RPC_PKT_RX, rpc_pkt_rx_ack);

    return FHOS_E_NONE;
}


int rpc_pkt_rx_register(const int unit, rpc_pkt_rx_cb cb, void *cookie)
{
    FHOS_UNUSED(unit);

    pkt_rx_cb = cb;
    pkt_rx_cookie = cookie;

    return FHOS_E_NONE;
}


static
int rpc_pkt_tx_impl(rpc_pkt_t           *pkt,
                    fhos_rpc_request_cb  cb,
                    void                *user_data,
                    int                  flags)
{
    int                 rc;
    rpc_api_header_t   *api_hdr = RPC_API_HDR(RPC_PKT_BASE(pkt));


    if (!pkt)
    {
        return FHOS_E_NULL_PTR;
    }

    if (!SAI_IS_VALID_UNIT(pkt->unit))
    {
        return FHOS_E_UNIT;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PKT_TX;
    api_hdr->code = 0;

    rc = rpc_client_write(RPC_PKT_BASE(pkt), RPC_PKT_LENGTH(pkt->len), cb, user_data, flags);

    return rc;
}


int rpc_pkt_tx_async(const int           unit,
                     rpc_pkt_t          *pkt,
                     fhos_rpc_request_cb cb,
                     void               *user_data)
{
    if (!SAI_IS_VALID_UNIT(unit))
    {
        return FHOS_E_UNIT;
    }

    return rpc_pkt_tx_impl(pkt, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_pkt_tx(const int unit, rpc_pkt_t *pkt)
{
    int     rc;
    int     code;


    if (!SAI_IS_VALID_UNIT(unit))
    {
        return FHOS_E_UNIT;
    }

    rc = rpc_pkt_tx_impl(pkt, rpc_client_default_set_ack_cb, &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    return code;
}


