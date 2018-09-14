/**
 * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file
 * @author long qiang
 * @date   8/6/2018
 */
#include <fhos/errno.h>
#include <fhos/utils.h>

#include <rpc/client/client.h>


static
int rpc_sys_device_id_get_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);
    int                 *id      = user_data;


    if (buf && id)
    {
        *id = api_hdr->code;
        return 0;
    }

    if (!buf &&
        FHOS_RPC_REQUEST_IS_TIMOUT(len) &&
        id)
    {
        *id = FHOS_E_TIMEOUT;
    }

    return FHOS_E_INTERNAL;
}



int rpc_sys_device_id_get()
{
    char                buf[RPC_API_SPACE(0)];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    int                 rc;
    int                 id;


    api_hdr->id   = RPC_SYS_DEVICE_ID_GET;
    api_hdr->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), rpc_sys_device_id_get_cb, &id, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    return id;
}
