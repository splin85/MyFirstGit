

#include <sai/types.h>
#include <rpc/common/common.h>
#include <rpc/common/msg.h>
#include <rpc/client/client.h>


int rpc_dot1x_port_auth_mode_set(int port, int mode)
{
    int rt, code;
    char buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t    *two     = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_DOT1X_AUTH_MODE_SET;
    api_hdr->code = 0;

    two->val1 = port;
    two->val2 = mode;

    rt = rpc_client_write(buf, sizeof(buf), rpc_client_default_set_ack_cb, &code, FHOS_RPC_REQUEST_SYNC);

    if (rt != FHOS_E_NONE)
    {
        return rt;
    }

    return code;
}

int rpc_dot1x_port_auth_egress_set(int port, int enable)
{
    int rt, code;
    char buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t    *two     = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_DOT1X_AUTH_EGRESS_SET;
    api_hdr->code = 0;

    two->val1 = port;
    two->val2 = enable;

    rt = rpc_client_write(buf, sizeof(buf), rpc_client_default_set_ack_cb, &code, FHOS_RPC_REQUEST_SYNC);

    if (rt != FHOS_E_NONE)
    {
        return rt;
    }

    return code;
}

