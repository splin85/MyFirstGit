/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/27/18
 */

#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/port.h>

#include <rpc/server/server.h>
#include <rpc/server/port.h>
#include <rpc/common/msg.h>


#define IF_ERROR_BREAK(ret)             \
            if (FHOS_E_NONE != ret)     \
            {                           \
                break;                  \
            }


int rpc_server_port_enable_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_port_enable_set(two->val1, two->val2);
#else
    FHOS_UNUSED(two);
#endif

    return RPC_API_MIN_LEN;
}


int rpc_server_port_enable_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_port_enable_get(two->val1, &two->val1);
    len += sizeof(int);
#else
    FHOS_UNUSED(two);
#endif

    return len;
}


int rpc_server_port_info_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int               len = RPC_API_MIN_LEN;
    int               port;
    int               ret = FHOS_E_NONE;
    int               result = FHOS_E_NONE;
    uint8_t           speed;
    rpc_port_info_t  *pinfo = (rpc_port_info_t *)RPC_API_DATA(api_hdr);

    extern int l2d_port_dhcp_trap_get(const int port, int *enable);


    FHOS_UNUSED(cookie);

    SAI_PBMP_CLEAR(pinfo->duplex_bmp);
    SAI_PBMP_CLEAR(pinfo->link_bmp);
    SAI_PBMP_CLEAR(pinfo->dhcp_bmp);
    memset(pinfo->speed, 0, sizeof(pinfo->speed));

    FHOS_LPORT_ITER(port)
    {
#if SAI_FUNCTION_ENABLE
        ret = sai_port_link_status_get(port, &result);
#endif
        IF_ERROR_BREAK(ret);
        if (result)
        {
            SAI_PBMP_PORT_ADD(pinfo->link_bmp, port);
        }
#if SAI_FUNCTION_ENABLE
        ret = sai_port_duplex_get(port, &result);
#endif
        IF_ERROR_BREAK(ret);
        if (SAI_PORT_DUPLEX_HALF == result)
        {
            SAI_PBMP_PORT_ADD(pinfo->duplex_bmp, port);
        }

        l2d_port_dhcp_trap_get(port, &result);
        if (FHOS_ENABLE == result)
        {
            SAI_PBMP_PORT_ADD(pinfo->dhcp_bmp, port);
        }

#if SAI_FUNCTION_ENABLE
        ret = sai_port_speed_get(port, &result);
#endif
        IF_ERROR_BREAK(ret);
        speed = SAI_PORT_SPEED_1G == result ? RPC_PORT_SPEED_1G :
                SAI_PORT_SPEED_10M == result ? RPC_PORT_SPEED_10M :
                SAI_PORT_SPEED_100M == result ? RPC_PORT_SPEED_100M :
                SAI_PORT_SPEED_2500M == result ? RPC_PORT_SPEED_2500M :
                RPC_PORT_SPEED_UNKOWN;

        pinfo->speed[port - 1] = speed;

    }

    pinfo->code = ret;
    len += sizeof(rpc_port_info_t);

    return len;
}


int rpc_server_port_link_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_one_int_t  *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_port_link_status_get(one->val1, &one->val1);
    len += sizeof(int);
#else
    FHOS_UNUSED(one);
#endif

    return len;
}


int rpc_server_port_speed_and_duplex_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int               rc = FHOS_E_NONE;
    int               len = RPC_API_MIN_LEN;
    rpc_three_int_t  *three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    rc = sai_port_speed_get(three->val1, &three->val2);
    api_hdr->code = rc;
    if (FHOS_E_NONE != rc)
    {
        return len;
    }
    rc = sai_port_duplex_get(three->val1, &three->val3);
    api_hdr->code = rc;
    if (FHOS_E_NONE != rc)
    {
        return len;
    }
#else
    FHOS_UNUSED(three);
    FHOS_UNUSED(rc);
#endif

    len += sizeof(rpc_three_int_t);
    return len;
}


int rpc_server_port_stp_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);
    int             len = RPC_API_MIN_LEN;


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_port_stp_set(two->val1, two->val2);
    len += sizeof(rpc_two_int_t);
#else
    FHOS_UNUSED(two);
#endif

    return len;
}


int rpc_server_port_stp_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int             len = RPC_API_MIN_LEN;
    rpc_two_int_t  *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_port_stp_get(two->val1, &two->val1);
    len += sizeof(int);
#else
    FHOS_UNUSED(two);
#endif

    return len;
}

extern int port_dhcp_global_enable(int enable);
int rpc_server_port_global_dhcp_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    rpc_one_int_t  *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    api_hdr->code = port_dhcp_global_enable(one->val1);

    return RPC_API_MIN_LEN;

}


int rpc_server_port_init()
{
    rpc_server_api_register(RPC_PORT_ENABLE_SET, rpc_server_port_enable_set_ack);
    rpc_server_api_register(RPC_PORT_ENABLE_GET, rpc_server_port_enable_get_ack);
    rpc_server_api_register(RPC_PORT_STP_SET,    rpc_server_port_stp_set_ack);
    rpc_server_api_register(RPC_PORT_STP_GET,    rpc_server_port_stp_get_ack);
    rpc_server_api_register(RPC_PORT_LINK_GET,   rpc_server_port_link_get_ack);
    rpc_server_api_register(RPC_PORT_INFO_GET,   rpc_server_port_info_get_ack);
    rpc_server_api_register(RPC_PORT_SPEED_GET,  rpc_server_port_speed_and_duplex_get_ack);
    rpc_server_api_register(RPC_PORT_GLOBAL_DHCP_SET, rpc_server_port_global_dhcp_set_ack);

    return FHOS_E_NONE;
}


static
int rpc_server_port_link_notify_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);


    if (!buf || len < RPC_API_HDR_LEN)
    {
        return FHOS_E_PARAM;
    }

    if (user_data)
    {
        *(int *)user_data = api_hdr->code;
    }

    return FHOS_E_NONE;
}


static
int rpc_port_link_notify_impl(fhos_rpc_pid_t       pid,
                              const int            port,
                              const int            status,
                              fhos_rpc_request_cb  cb,
                              void                *user_data,
                              int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *port_status = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!FHOS_RPC_PID_IS_VALID(pid) &&
        !SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_LINK_NOTIFY;
    api_hdr->code = 0;

    port_status->val1 = port;
    port_status->val2 = status;

    rc = rpc_server_write(pid, buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_port_dhcp_notify_impl(fhos_rpc_pid_t       pid,
                              const int            port,
                              const int            status,
                              fhos_rpc_request_cb  cb,
                              void                *user_data,
                              int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *port_status = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!FHOS_RPC_PID_IS_VALID(pid) &&
        !SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_DHCP_NOTIFY;
    api_hdr->code = 0;

    port_status->val1 = port;
    port_status->val2 = status;

    rc = rpc_server_write(pid, buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_port_duplex_notify_impl(fhos_rpc_pid_t       pid,
                                const int            port,
                                const int            duplex,
                                fhos_rpc_request_cb  cb,
                                void                *user_data,
                                int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *port_duplex = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!FHOS_RPC_PID_IS_VALID(pid) &&
        !SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_LINK_NOTIFY;
    api_hdr->code = 0;

    port_duplex->val1 = port;
    port_duplex->val2 = duplex;

    rc = rpc_server_write(pid, buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


static
int rpc_port_speed_notify_impl(fhos_rpc_pid_t       pid,
                               const int            port,
                               const int            duplex,
                               fhos_rpc_request_cb  cb,
                               void                *user_data,
                               int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *port_speed = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!FHOS_RPC_PID_IS_VALID(pid) &&
        !SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_PORT_SPEED_NOTIFY;
    api_hdr->code = 0;

    port_speed->val1 = port;
    port_speed->val2 = duplex;

    rc = rpc_server_write(pid, buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_port_link_notify(const fhos_rpc_pid_t pid, const sai_port_t port, const int status)
{
    int     rc;
    int     code;


    rc = rpc_port_link_notify_impl(pid, port, status, rpc_server_port_link_notify_cb,
                                   &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    return code;
}


int rpc_port_link_notify_async(const fhos_rpc_pid_t pid,
                               const sai_port_t     port,
                               const int            status,
                               fhos_rpc_request_cb  cb,
                               void                *user_data)
{
    return rpc_port_link_notify_impl(pid, port, status, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_port_dhcp_notify_async(const fhos_rpc_pid_t pid,
                               const sai_port_t     port,
                               const int            status,
                               fhos_rpc_request_cb  cb,
                               void                *user_data)
{
    return rpc_port_dhcp_notify_impl(pid, port, status, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_port_duplex_notify_async(const fhos_rpc_pid_t pid,
                                 const sai_port_t     port,
                                 const int            duplex,
                                 fhos_rpc_request_cb  cb,
                                 void                *user_data)
{
    return rpc_port_duplex_notify_impl(pid, port, duplex, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_port_speed_notify_async(const fhos_rpc_pid_t pid,
                                const sai_port_t     port,
                                const int            speed,
                                fhos_rpc_request_cb  cb,
                                void                *user_data)
{
    return rpc_port_speed_notify_impl(pid, port, speed, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


