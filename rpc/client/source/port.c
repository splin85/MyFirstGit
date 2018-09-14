/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/27/18
 */

#include <fhos/errno.h>
#include <fhos/utils.h>

#include <rpc/client/client.h>
#include <rpc/client/port.h>
#include <rpc/common/msg.h>




static rpc_port_link_notify_cb    port_link_notify_cb = NULL;
static rpc_port_duplex_notify_cb  port_duplex_notify_cb = NULL;
static rpc_port_speed_notify_cb   port_speed_notify_cb = NULL;
static rpc_port_dhcp_notify_cb    port_dhcp_notify_cb = NULL;



/**
 * @see rpc_port_link_notify
 * @see rpc_port_link_notify_async
 */
static
int rpc_port_link_notify_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_two_int_t      *port_status = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (port_link_notify_cb)
    {
        port_link_notify_cb(port_status->val1, port_status->val2);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;
}


static
int rpc_port_duplex_notify_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_two_int_t      *port_duplex = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (port_duplex_notify_cb)
    {
        port_duplex_notify_cb(port_duplex->val1, port_duplex->val2);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;
}


static
int rpc_port_speed_notify_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_two_int_t      *port_speed = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (port_speed_notify_cb)
    {
        port_speed_notify_cb(port_speed->val1, port_speed->val2);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;
}


static
int rpc_port_dhcp_notify_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_two_int_t      *port_speed = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (port_dhcp_notify_cb)
    {
        port_dhcp_notify_cb(port_speed->val1, port_speed->val2);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;
}


int rpc_port_init(const int unit)
{
    FHOS_UNUSED(unit);

    rpc_client_api_register(RPC_PORT_LINK_NOTIFY,   rpc_port_link_notify_ack);
    rpc_client_api_register(RPC_PORT_DUPLEX_NOTIFY, rpc_port_duplex_notify_ack);
    rpc_client_api_register(RPC_PORT_SPEED_NOTIFY,  rpc_port_speed_notify_ack);
    rpc_client_api_register(RPC_PORT_DHCP_NOTIFY,   rpc_port_dhcp_notify_ack);


    return FHOS_E_NONE;
}


static
int rpc_port_enable_set_impl(const int            port,
                             const int            enable,
                             fhos_rpc_request_cb  cb,
                             void                *user_data,
                             int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *port_enable = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_ENABLE_SET;
    api_hdr->code = 0;

    port_enable->val1 = port;
    port_enable->val2 = enable;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_port_global_dhcp_enable_set_impl(const int            enable,
                                         fhos_rpc_request_cb  cb,
                                         void                *user_data,
                                         int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *port_enable = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_GLOBAL_DHCP_SET;
    api_hdr->code = 0;

    port_enable->val1 = enable;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}



int rpc_port_enable_set_async(const sai_port_t    port,
                              const int           enable,
                              fhos_rpc_request_cb cb,
                              void               *user_data)
{
    return rpc_port_enable_set_impl(port, enable, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_port_enable_set(const sai_port_t port, const int enable)
{
    int     rc;
    int     code;


    rc = rpc_port_enable_set_impl(port, enable, rpc_client_default_set_ack_cb,
                                  &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


int rpc_port_global_dhcp_set(const int enable)
{
    int     rc;
    int     code;


    rc = rpc_port_global_dhcp_enable_set_impl(enable, rpc_client_default_set_ack_cb,
                                              &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


static
int rpc_port_link_status_get_impl(const int            port,
                                  fhos_rpc_request_cb  cb,
                                  void                *user_data,
                                  int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_LINK_GET;
    api_hdr->code = 0;

    one->val1 = port;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_port_info_get_impl(fhos_rpc_request_cb  cb,
                           void                *user_data,
                           int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_port_info_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_INFO_GET;
    api_hdr->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_port_enable_get_impl(const int            port,
                             fhos_rpc_request_cb  cb,
                             void                *user_data,
                             int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_ENABLE_GET;
    api_hdr->code = 0;

    one->val1 = port;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_port_enable_get_async(const sai_port_t port, fhos_rpc_request_cb cb, void *user_data)
{
    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    return rpc_port_enable_get_impl(port, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_port_enable_get(const sai_port_t port, int *enable)
{
    int             rc;
    rpc_two_int_t   two;


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    if (!enable)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_port_enable_get_impl(port, rpc_client_default_get_ack_cb,
                                  &two, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    *enable = two.val2;

    return two.val1;
}


static
int rpc_port_stp_set_impl(const int            port,
                          const int            state,
                          fhos_rpc_request_cb  cb,
                          void                *user_data,
                          int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *port_enable = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_STP_SET;
    api_hdr->code = 0;

    port_enable->val1 = port;
    port_enable->val2 = state;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_port_stp_set_async(const sai_port_t    port,
                           const int           state,
                           fhos_rpc_request_cb cb,
                           void               *user_data)
{
    return rpc_port_stp_set_impl(port, state, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_port_link_status_get(const sai_port_t port, int *status)
{
    int             rc;
    rpc_two_int_t   two;


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    if (!status)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_port_link_status_get_impl(port, rpc_client_default_get_ack_cb,
                                       &two, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    *status = two.val2;

    return two.val1;
}


int rpc_port_info_get(rpc_port_info_t *port_info)
{
    int             rc;


    if (!port_info)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_port_info_get_impl(rpc_client_port_info_get_ack_cb,
                                port_info, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }


    return port_info->code;
}



int rpc_port_link_register(rpc_port_link_notify_cb cb)
{
    port_link_notify_cb = cb;

    return FHOS_E_NONE;
}


int rpc_port_duplex_register(rpc_port_duplex_notify_cb cb)
{
    port_duplex_notify_cb = cb;

    return FHOS_E_NONE;
}


int rpc_port_speed_register(rpc_port_speed_notify_cb cb)
{
    port_speed_notify_cb = cb;

    return FHOS_E_NONE;
}

int rpc_port_dhcp_register(rpc_port_dhcp_notify_cb cb)
{
    port_dhcp_notify_cb = cb;

    return FHOS_E_NONE;
}


static
int rpc_port_stp_get_impl(const int            port,
                          fhos_rpc_request_cb  cb,
                          void                *user_data,
                          int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_STP_GET;
    api_hdr->code = 0;

    one->val1 = port;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_port_stp_status_get(const sai_port_t port, int *enable)
{
    int             rc;
    rpc_two_int_t   two;


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    if (!enable)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_port_stp_get_impl(port, rpc_client_default_get_ack_cb,
                               &two, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    *enable = two.val2;

    return two.val1;
}


static
int rpc_port_speed_and_duplex_get_impl(const int            port,
                                       fhos_rpc_request_cb  cb,
                                       void                *user_data,
                                       int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_three_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_three_int_t    *three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_SPEED_GET;
    api_hdr->code = 0;

    three->val1 = port;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_port_speed_and_duplex_get_async(const sai_port_t port, fhos_rpc_request_cb cb, void *user_data)
{
    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    return rpc_port_speed_and_duplex_get_impl(port, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}
