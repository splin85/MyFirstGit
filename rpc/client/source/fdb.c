
#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>

#include <rpc/client/client.h>
#include <rpc/common/msg.h>
#include <rpc/client/fdb.h>

static
int rpc_fdb_mac_is_exist_impl(const fhos_mac_t    mac,
                              const fhos_port_t   port,
                              fhos_rpc_request_cb cb,
                              void               *user_data,
                              int                 flags)
{
    char                buf[RPC_API_SPACE(sizeof(rpc_mac_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_mac_int_t      *data = (rpc_mac_int_t *)RPC_API_DATA(api_hdr);


    if (!SAI_IS_VALID_PORT(port) &&
        -1 != port)
    {
        return FHOS_E_PORT;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_FDB_MAC_EXIST;
    api_hdr->code = 0;

    memcpy(data->mac, mac, sizeof(fhos_mac_t));
    data->val = port;

    return rpc_client_write(buf, sizeof(buf), cb, user_data, flags);
}


int rpc_fdb_mac_is_exist_async(const fhos_mac_t    mac,
                               const fhos_port_t   port,
                               fhos_rpc_request_cb cb,
                               void               *user_data)
{
    return rpc_fdb_mac_is_exist_impl(mac, port, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


int rpc_fdb_mac_is_exist(const fhos_mac_t mac, const fhos_port_t port)
{
    int     rc;
    int     code;


    rc = rpc_fdb_mac_is_exist_impl(mac,
                                   port,
                                   rpc_client_default_set_ack_cb,
                                   &code,
                                   FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}

