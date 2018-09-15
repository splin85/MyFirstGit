
#include <fhos/utils.h>

#include <id/id.h>

#include <rpc/server/server.h>
#include <rpc/server/sys.h>


static
int rpc_server_sys_device_get_ack(rpc_api_header_t *api_hdr, void *cookie)
{
    FHOS_UNUSED(cookie);
#if defined(GSWC_CONFIG)
    api_hdr->code = device_id_get();
#else
    api_hdr->code = 0;
#endif
    return RPC_API_MIN_LEN;
}


int rpc_server_sys_init()
{
    rpc_server_api_register(RPC_SYS_DEVICE_ID_GET, rpc_server_sys_device_get_ack);

    return FHOS_E_NONE;
}


