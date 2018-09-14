/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file dot1x.c
 * @author Ou Fan
 * @date 2018/7/20
 */


#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/auth.h>
#include <rpc/common/msg.h>
#include <rpc/server/server.h>
#include <rpc/server/dot1x.h>


static int rpc_dot1x_port_auth_mode_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int len = RPC_API_MIN_LEN;
    rpc_two_int_t *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);
    int port = two->val1;
    int mode = two->val2;


    FHOS_UNUSED(cookie);
#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_auth_mode_set(port, mode);
#else
    FHOS_UNUSED(api_hdr);
    FHOS_UNUSED(port);
    FHOS_UNUSED(mode);
#endif

    return len;
}

static int rpc_dot1x_port_auth_egress_set_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int len = RPC_API_MIN_LEN;
    rpc_two_int_t *two = (rpc_two_int_t *)RPC_API_DATA(api_hdr);
    int port = two->val1;
    int enable = two->val2;


    FHOS_UNUSED(cookie);
#if SAI_FUNCTION_ENABLE
    api_hdr->code = sai_auth_egress_set(port, enable);
#else
    FHOS_UNUSED(api_hdr);
    FHOS_UNUSED(port);
    FHOS_UNUSED(enable);
#endif

    return len;
}

int rpc_server_dot1x_init(void)
{
    rpc_server_api_register(RPC_DOT1X_AUTH_MODE_SET, rpc_dot1x_port_auth_mode_set_ack);
    rpc_server_api_register(RPC_DOT1X_AUTH_EGRESS_SET, rpc_dot1x_port_auth_egress_set_ack);

    return FHOS_E_NONE;
}

