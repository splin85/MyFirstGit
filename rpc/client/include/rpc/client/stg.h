/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author wang shun qiang
 * @date 07/31/18
 */

#ifndef __RPC_CLIENT_STG_H__
#define __RPC_CLIENT_STG_H__


#include <sai/types.h>

#include <rpc/common/msg.h>


/**
 * Get the current default STG ID
 * @param[in] stg  STG ID
 * @return FHOS_E_XXXX
 */
int rpc_stg_default_get(int *stg);

/**
 * Create a new STG
 * @param[in] stg  STG ID
 * @return FHOS_E_XXXX
 */
int rpc_stg_create(int *stg);

/**
 * Destroy the specified STG
 * @param[in] stg  STG ID
 * @return FHOS_E_XXXX
 */
int rpc_stg_destroy(const int stg);

/**
 * Add a VLAN to a specified STG
 * @param[in] stg  STG ID
 * @param[in] vid  VLAN ID
 * @return FHOS_E_XXXX
 */
int rpc_stg_vlan_add(const int stg, const sai_vlan_t vid);

/**
 * Remove a VLAN from a specified STG
 * @param[in] stg  STG ID
 * @param[in] vid  VLAN ID
 * @return FHOS_E_XXXX
 */
int rpc_stg_vlan_remove(const int stg, const sai_vlan_t vid);


/**
 * Set the STP state of the given port
 * @param[in] port  Logical port
 * @param[in] stg   STG ID
 * @param[in] state STP state
 * @return FHOS_E_XXXX
 */
int rpc_stg_port_state_set(const sai_port_t port, const int stg, const int state);

/**
 * Get the STP state of the port
 * @param[in]  port  Logical port
 * @param[in]  stg   STG ID
 * @param[out] state STP state
 * @return FHOS_E_XXXX
 */
int rpc_stg_port_state_get(const sai_port_t port, const int stg, int *state);


int rpc_stg_vlan_add_async(const int            stg,
                           const sai_vlan_t     vid,
                           fhos_rpc_request_cb  cb,
                           void                *user_data);


int rpc_stg_vlan_remove_async(const int            stg,
                              const sai_vlan_t     vid,
                              fhos_rpc_request_cb  cb,
                              void                *user_data);


int rpc_stg_port_state_set_async(const sai_port_t     port,
                                 const int            stg,
                                 const int            state,
                                 fhos_rpc_request_cb  cb,
                                 void                *user_data);
#endif

