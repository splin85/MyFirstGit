
#ifndef __RPC_CLIENT_PORT_H__
#define __RPC_CLIENT_PORT_H__


#include <sai/types.h>

#include <rpc/common/msg.h>


typedef int (*rpc_port_link_notify_cb)(sai_port_t port, int status);
typedef int (*rpc_port_duplex_notify_cb)(sai_port_t port, int duplex);
typedef int (*rpc_port_speed_notify_cb)(sai_port_t port, int speed);
typedef int (*rpc_port_dhcp_notify_cb)(sai_port_t port, int dhcp);

/**
 * RPC port init
 * @return FHOS_E_XXXX
 */
int rpc_port_init(const int unit);

/**
 * Physically enable or disable the MAC/PHY on this port asynchronous
 * @param[in] port      Logical port
 * @param[in] enable    1 port is enabled, 0 port is disabled
 * @param[in] cb        Callback function
 * @param[in] user_data Callback user data
 * @return FHOS_E_XXXX
 */
int rpc_port_enable_set_async(const sai_port_t port, const int enable,
                              fhos_rpc_request_cb cb, void *user_data);

/**
 * Physically enable or disable the MAC/PHY on this port
 * @see sai_port_enable_set
 * @see rpc_port_enable_set_async
 * @param[in] port      Logical port
 * @param[in] enable    1 port is enabled, 0 port is disabled
 * @return FHOS_E_XXXX
 */
int rpc_port_enable_set(const sai_port_t port, const int enable);

/**
 * Get enable state on this port
 * @param[in] port      Logical port
 * @param[in] enable    1 port is enabled, 0 port is disabled
 * @param[in] cb        Callback function
 * @param[in] user_data Callback user data
 * @return FHOS_E_XXXX
 */
int rpc_port_enable_get_async(const sai_port_t port, fhos_rpc_request_cb cb, void *user_data);

/**
 * Get enable state on this port
 * @see sai_port_enable_get
 * @see rpc_port_enable_get_async
 * @param[in] port      Logical port
 * @param[in] enable    1 port is enabled, 0 port is disabled
 * @param[in] cb        Callback function
 * @param[in] user_data Callback user data
 * @return FHOS_E_XXXX
 */
int rpc_port_enable_get(const sai_port_t port, int *enable);

/**
 * Set the STP state of the given port
 * @see sai_stp_t
 * @param[in] unit  Device unit
 * @param[in] port  Logical port
 * @param[in] state STP state
 * @return FHOS_E_XXXX
 */
int rpc_port_stp_set_async(const sai_port_t port, const int state, fhos_rpc_request_cb cb, void *user_data);

/**
 * Get the STP state of the port
 * @see hal_bcm_port_stp_set
 * @param[in]  unit  Device unit
 * @param[in]  port  Logical port
 * @param[out] state STP state
 * @return FHOS_E_XXXX
 */
int rpc_port_stp_get(const sai_port_t port, int *state);

/**
 * Register a handler to process port link up/down event
 * @see sai_port_link_register
 * @see SAI_PORT_LINK_STATUS_DOWN and SAI_PORT_LINK_STATUS_UP
 * @param[in] handler Function to process link event
 * @return FHOS_E_XXXX
 */
int rpc_port_link_register(rpc_port_link_notify_cb cb);

/**
 * Register a handler to process port duplex change
 * @see sai_port_link_register
 * @see SAI_PORT_LINK_STATUS_DOWN and SAI_PORT_LINK_STATUS_UP
 * @param[in] handler Function to process link event
 * @return FHOS_E_XXXX
 */
int rpc_port_duplex_register(rpc_port_duplex_notify_cb cb);

/**
 * Register a handler to process port speed change
 * @see sai_port_link_register
 * @see SAI_PORT_LINK_STATUS_DOWN and SAI_PORT_LINK_STATUS_UP
 * @param[in] handler Function to process link event
 * @return FHOS_E_XXXX
 */
int rpc_port_speed_register(rpc_port_speed_notify_cb cb);

/**
 * Register a handler to process port dhcp enable change
 * @see sai_port_link_register
 * @see FHOS_ENABLE or FHOS_DISABLE
 * @param[in] handler Function to process link event
 * @return FHOS_E_XXXX
 */
int rpc_port_dhcp_register(rpc_port_speed_notify_cb cb);

/**
 * Get the link status of the port
 * @param[in]  port   Logical port
 * @param[out] status Port link state
 * @return FHOS_E_XXXX
 */
int rpc_port_link_status_get(const sai_port_t port, int *status);

/**
 * Get the link status of the port
 * @param[in]  port   Logical port
 * @param[out] status Port STP state
 * @return FHOS_E_XXXX
 */
int rpc_port_stp_status_get(const sai_port_t port, int *enable);

/**
 * Get the link speed of the port
 * @param[in]  port   Logical port
 * @param[out] rpc_three_int_t port speed duplex
 * @return FHOS_E_XXXX
 */
int rpc_port_speed_and_duplex_get_async(const sai_port_t port, fhos_rpc_request_cb cb, void *user_data);

/**
 * Get the port info of the port include link status duplex speed
 * @param[in]  port             Logical port
 * @param[out] rpc_port_info_t  port info
 * @return FHOS_E_XXXX
 */
int rpc_port_info_get(rpc_port_info_t *port_info);


int rpc_port_global_dhcp_set(const int enable);

#endif

