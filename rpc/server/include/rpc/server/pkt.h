/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 03/02/18
 */

#ifndef __RPC_SERVER_PKT_H__
#define __RPC_SERVER_PKT_H__


#include <sai/types.h>
#include <sai/pkt.h>

#include <rpc/common/msg.h>



/**
 * RPC pkt init
 * @return FHOS_E_XXXX
 */
int rpc_server_pkt_init();

/**
 * Transmit packet to client asynchronous
 * @Attention
 * pkt MUST allocate minimum RPC_PKT_LENGTH(len),
 * And, pkt point to RPC_API_DATA
 * eg.
 * Assume the pkt data length is len
 *
 * char   buf[RPC_PKT_LENGTH(len)]
 * rpc_pkt_t  *pkt = RPC_PKT_CAST(buf);
 *
 * @param[in] pid       Process ID
 * @param[in] pkt       Transmited Packet
 * @param[in] cb        Callback function
 * @param[in] user_data Callback user data
 * @return FHOS_E_XXXX
 */
int rpc_pkt_tx_async(const fhos_rpc_pid_t pid,
                     rpc_pkt_t           *pkt,
                     fhos_rpc_request_cb  cb,
                     void                *user_data);

/**
 * Transmit packet to client
 * @param[in] pid  Process ID
 * @param[in] pkt  Transmited Packet
 * @return FHOS_E_XXXX
 */
int rpc_pkt_tx(const fhos_rpc_pid_t pid, rpc_pkt_t *pkt);


#endif
