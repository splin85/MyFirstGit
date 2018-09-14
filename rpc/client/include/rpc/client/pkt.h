/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 03/02/18
 */

#ifndef __RPC_CLIENT_PKT_H__
#define __RPC_CLIENT_PKT_H__


#include <sai/types.h>
#include <rpc/common/msg.h>


typedef int (*rpc_pkt_rx_cb)(rpc_pkt_t *pkt, void *cookie);



/**
 * Rx packet init
 * @param[in] unit  Device unit
 * @return FHOS_E_XXXX
 */
int rpc_pkt_init(const int unit);

/**
 * Register to receive callbacks for receive packets
 * @param[in] unit    Device unit
 * @param[in] cb      Function pointer to be called when rx packets
 * @param[in] cookie  Application data passed on the callback
 * @return FHOS_E_XXXX
 */
int rpc_pkt_rx_register(const int unit, rpc_pkt_rx_cb cb, void *cookie);

/**
 * Transmit packet to switch asynchronous
 * @Attention
 * pkt MUST allocate minimum RPC_PKT_LENGTH(len),
 * And, pkt point to RPC_API_DATA
 * eg.
 * Assume the pkt data length is len
 *
 * char   buf[RPC_PKT_LENGTH(len)]
 * rpc_pkt_t  *pkt = RPC_PKT_CAST(buf);
 *
 * @param[in] unit      Device unit
 * @param[in] pkt       Transmited Packet
 * @param[in] cb        Callback function
 * @param[in] user_data Callback user data
 * @return FHOS_E_XXXX
 */
int rpc_pkt_tx_async(const int unit, rpc_pkt_t *pkt, fhos_rpc_request_cb cb, void *user_data);

/**
 * Transmit packet to switch
 * @param[in] unit   Device unit
 * @param[in] pkt    Transmited Packet
 * @return FHOS_E_XXXX
 */
int rpc_pkt_tx(const int unit, rpc_pkt_t *pkt);


#endif
