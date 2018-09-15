
 
#include <string.h>
#include <fhos/memory.h>
#include <fhos/log.h>
#include <fhos/utils.h>
#include <sai/types.h>
#include <rpc/common/common.h>
#include <rpc/common/msg.h>
#include <rpc/server/server.h>
#include <ntp/ntp.h>

int rpc_server_ntp_nofify_change_config(int status)
{
    int buf_len = sizeof(rpc_one_int_t);
    int ret;
    char buf[RPC_API_SPACE(buf_len)];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *ntp_msg = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_NTP_CONFIG_CHANGE;
    api_hdr->code = 0;

    ntp_msg->val1 = status;

    ret = rpc_server_write(RPC_PID_NTP, (char *)buf, sizeof(buf), NULL, NULL, FHOS_RPC_REQUEST_ASYNC);
    return ret;
}

static int rpc_server_ntp_enable_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    int                 len   = RPC_API_MIN_LEN;
    rpc_one_int_t  *ready = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    ready->val1 = l2d_ntp_client_status_get();

    return len + sizeof(rpc_one_int_t);
}

int rpc_server_ntp_init(void)
{
    rpc_server_api_register(RPC_NTP_IS_ENABLE,  rpc_server_ntp_enable_ack);

    return FHOS_E_NONE;
}

