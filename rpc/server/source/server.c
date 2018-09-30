
#include <fhos/errno.h>
#include <fhos/memory.h>
#include <fhos/utils.h>

#include <rpc/server/server.h>
#include <rpc/server/port.h>
#include <rpc/server/pkt.h>
#include <rpc/server/igmp.h>
#include <rpc/server/vlan.h>
#include <rpc/server/fdb.h>
#include <rpc/server/trunk.h>
#include <rpc/server/ntp.h>
#include <rpc/server/dot1x.h>
#include <rpc/server/stg.h>
#include <rpc/server/sys.h>
#include <rpc/server/l2.h>


static fhos_rpc_server_t    *rpc_server = NULL;

static rpc_server_api_cb    *rpc_server_api_map = NULL;




/**
 * Only process request message
 *
 * @attention
 * The len is the buf capacity!
 */
static
int rpc_server_recv_handler(char *buf, int len, void *cookie)
{
    int                 write_bytes;
    int                 fd = *(int *)cookie;
    fhos_rpc_header_t  *rpc_hdr = FHOS_RPC_HDR(buf);
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);


    if (rpc_hdr->flags & FHOS_RPC_HDR_FLAGS_ACK)
    {
        return 0;
    }

    if (!buf || rpc_hdr->len < RPC_API_HDR_LEN)
    {
        return FHOS_E_INTERNAL;
    }

    if (!RPC_API_ID_IS_VALID(api_hdr->id))
    {
        return FHOS_E_INTERNAL;
    }

    if (rpc_server_api_map && rpc_server_api_map[api_hdr->id])
    {
        rpc_hdr->len = rpc_server_api_map[api_hdr->id](api_hdr, &len);
    }
    else
    {
        rpc_hdr->len = RPC_API_MIN_LEN;
        api_hdr->code = FHOS_E_UNAVAIL;
    }

    rpc_hdr->flags |= FHOS_RPC_HDR_FLAGS_ACK;
    write_bytes = fhos_socket_writen(fd, buf, rpc_hdr->len);
    if (write_bytes != rpc_hdr->len)
    {
    }

    return 0;
}


static
int rpc_server_api_map_init(void)
{
    rpc_server_port_init();
    rpc_server_pkt_init();
    rpc_server_igmp_init();
    rpc_server_vlan_init();
    rpc_server_fdb_init();
    rpc_server_trunk_init();
    rpc_server_ntp_init();
    rpc_server_dot1x_init();
    rpc_server_stg_init();
    rpc_server_sys_init();
    rpc_server_l2_init();

    return 0;
}


int rpc_server_init(struct thread_master *master)
{
    int     rc;


    if (rpc_server)
    {
        return FHOS_E_EXISTS;
    }

    if (!rpc_server_api_map)
    {
        rpc_server_api_map = FHOS_MALLOC(sizeof(*rpc_server_api_map) * RPC_API_ID_COUNT);
        if (!rpc_server_api_map)
        {
            return FHOS_E_MEMORY;
        }
        memset(rpc_server_api_map, 0, sizeof(rpc_server_api_map) * RPC_API_ID_COUNT);
    }

    rpc_server_api_map_init();

    rpc_server = (fhos_rpc_server_t *)FHOS_MALLOC(sizeof(*rpc_server));
    if (!rpc_server)
    {
        FHOS_FREE(rpc_server_api_map);
        rpc_server_api_map = NULL;
        return FHOS_E_MEMORY;
    }

    rc = fhos_rpc_server_init(RPC_API_PATH, master, rpc_server);
    if (rc < 0)
    {
        FHOS_FREE(rpc_server_api_map);
        rpc_server_api_map = NULL;

        FHOS_FREE(rpc_server);
        rpc_server = NULL;
        return rc;
    }

    rc = fhos_rpc_server_recv_install(rpc_server, rpc_server_recv_handler);

    return rc;
}


int rpc_server_destroy()
{
    return 0;
}


int rpc_server_api_register(const rpc_api_id_t id, rpc_server_api_cb cb)
{
    if (!rpc_server_api_map)
    {
        return FHOS_E_UNAVAIL;
    }

    if (!RPC_API_ID_IS_VALID(id))
    {
        return FHOS_E_PARAM;
    }

    rpc_server_api_map[id] = cb;

    return FHOS_E_NONE;
}


int rpc_server_write(fhos_rpc_pid_t      pid,
                     char               *buf,
                     int                 len,
                     fhos_rpc_request_cb cb,
                     void               *user_data,
                     int                 flags)

{
    int     write_bytes;


    if (!rpc_server)
    {
        return FHOS_E_UNAVAIL;
    }

    if (!buf || len < RPC_API_MIN_LEN)
    {
        return FHOS_E_PARAM;
    }

    write_bytes = fhos_rpc_server_write(pid, rpc_server, buf, len, cb, user_data, flags);
    if (write_bytes != len)
    {
        return FHOS_E_INTERNAL;
    }

    return FHOS_E_NONE;
}


int rpc_server_default_request_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);


    if (user_data)
    {
        if (!buf || len < RPC_API_HDR_LEN)
        {
            *(int *)user_data = FHOS_E_RPC;
        }
        else
        {
            *(int *)user_data = api_hdr->code;
        }
    }

    return FHOS_E_NONE;
}


