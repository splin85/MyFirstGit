
#include <fhos/errno.h>
#include <fhos/utils.h>

#include <sai/types.h>
#include <sai/port.h>

#include <fdb/fdb.h>

#include <rpc/server/server.h>
#include <rpc/server/fdb.h>
#include <rpc/common/msg.h>


int rpc_server_fdb_mac_exist_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    rpc_mac_int_t  *data = (rpc_mac_int_t *)RPC_API_DATA(api_hdr);


    FHOS_UNUSED(cookie);

    api_hdr->code = l2d_fdb_mac_in_port_exist(data->mac, data->val);

    return RPC_API_MIN_LEN;
}


int rpc_server_fdb_init()
{
    rpc_server_api_register(RPC_FDB_MAC_EXIST,   rpc_server_fdb_mac_exist_ack);

    return FHOS_E_NONE;
}