/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file ntp.c
 * @author Ou Fan
 * @date 03/13/18
 */


#include <stdlib.h>
#include <fhos/errno.h>
#include <fhos/utils.h>
#include <sai/types.h>
#include <rpc/client/client.h>
#include <rpc/client/ntp.h>
#include <rpc/common/msg.h>


static rpc_ntp_config_change_cb g_ntp_config_change_cb = NULL;


int rpc_ntp_config_change_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_one_int_t      *ntp_msg = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    if (g_ntp_config_change_cb)
    {
        g_ntp_config_change_cb(ntp_msg->val1);
    }

    return RPC_API_MIN_LEN;
}

int rpc_ntp_init(const int unit)
{
    FHOS_UNUSED(unit);

    rpc_client_api_register(RPC_NTP_CONFIG_CHANGE, rpc_ntp_config_change_ack);

    return FHOS_E_NONE;
}

void rpc_ntp_config_change_register(rpc_ntp_config_change_cb cb)
{
    g_ntp_config_change_cb = cb;
}

static int rpc_ntp_check_enable_send_msg(fhos_rpc_request_cb cb, void *data, int flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id   = RPC_NTP_IS_ENABLE;
    api_hdr->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), cb, data, flags);

    return rc;
}

static int rpc_ntp_check_enable_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_one_int_t       *data    = RPC_API_DATA(api_hdr);


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return FHOS_E_PARAM;
        }
    }

    if (user_data)
    {
        memcpy(user_data, data, sizeof(rpc_one_int_t));
    }

    return FHOS_E_NONE;
}

int rpc_ntp_check_enable(void)
{
    rpc_one_int_t data;
    int ret;

    data.val1 = 0;

    ret = rpc_ntp_check_enable_send_msg(rpc_ntp_check_enable_ack_cb, &data, FHOS_RPC_REQUEST_SYNC);
    if (ret < FHOS_E_NONE)
    {
        return 0;
    }

    return data.val1;
}

