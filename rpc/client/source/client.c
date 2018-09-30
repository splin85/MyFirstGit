
#include <fhos/errno.h>
#include <fhos/memory.h>


#include <rpc/client/client.h>
#include <rpc/client/port.h>
#include <rpc/client/pkt.h>
#include <rpc/client/ntp.h>
#include <rpc/client/trunk.h>
#include <rpc/client/vlan.h>



static fhos_rpc_client_t   *rpc_client = NULL;

static rpc_client_api_cb   *rpc_client_api_map = NULL;



/**
 * Only process request message
 *
 * @attention
 * The len is the buf capacity!
 */
static
int rpc_client_recv_handler(char *buf, int len, void *cookie)
{
    int                 write_bytes;
    int                 fd = *(int *)cookie;
    fhos_rpc_header_t  *rpc_hdr = FHOS_RPC_HDR(buf);
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);


    if (rpc_hdr->flags & FHOS_RPC_HDR_FLAGS_ACK)
    {
        return FHOS_E_NONE;
    }

    if (!buf || rpc_hdr->len < RPC_API_MIN_LEN)
    {
        return FHOS_E_INTERNAL;
    }

    if (!RPC_API_ID_IS_VALID(api_hdr->id))
    {
        return FHOS_E_INTERNAL;
    }

    if (rpc_client_api_map && rpc_client_api_map[api_hdr->id])
    {
        rpc_hdr->len = rpc_client_api_map[api_hdr->id](rpc_hdr, &len);
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
int rpc_api_map_init(const int unit)
{
    rpc_port_init(unit);
    rpc_vlan_init(unit);
    rpc_pkt_init(unit);
    rpc_ntp_init(unit);
    rpc_trunk_init(unit);

    return 0;
}


int rpc_client_init(const fhos_rpc_pid_t       pid,
                    struct thread_master      *master,
                    fhos_rpc_connect_handler_t handler)
{
    int     rc;


    if (rpc_client)
    {
        return FHOS_E_EXISTS;
    }

    if (!rpc_client_api_map)
    {
        rpc_client_api_map = FHOS_MALLOC(sizeof(*rpc_client_api_map) * RPC_API_ID_COUNT);
        if (!rpc_client_api_map)
        {
            return FHOS_E_MEMORY;
        }
        memset(rpc_client_api_map, 0, sizeof(rpc_client_api_map) * RPC_API_ID_COUNT);
    }

    rpc_client = (fhos_rpc_client_t *)FHOS_MALLOC(sizeof(*rpc_client));
    if (!rpc_client)
    {
        FHOS_FREE(rpc_client_api_map);
        rpc_client_api_map = NULL;
        return FHOS_E_MEMORY;
    }

    memset(rpc_client, 0, sizeof(*rpc_client));

    fhos_rpc_client_connect_install(rpc_client, handler);

    rc = fhos_rpc_client_init(pid, RPC_API_PATH, master, rpc_client);
    if (rc < 0)
    {
        FHOS_FREE(rpc_client_api_map);
        rpc_client_api_map = NULL;

        FHOS_FREE(rpc_client);
        rpc_client = NULL;
        return rc;
    }

    rpc_api_map_init(0);

    rc = fhos_rpc_client_recv_install(rpc_client, rpc_client_recv_handler);

    return rc;
}


/**
 * @see fhos_rpc_client_destory
 *
 * Do not call this funtion, it may be crash!
 */
int rpc_client_destroy(const fhos_rpc_pid_t pid)
{
    if (rpc_client_api_map)
    {
        FHOS_FREE(rpc_client_api_map);
        rpc_client_api_map = NULL;
    }

    if (!rpc_client)
    {
        return FHOS_E_NOT_FOUND;
    }

    if (rpc_client->pid != pid)
    {
        return FHOS_E_BADID;
    }

    fhos_rpc_client_destory(rpc_client);

    FHOS_FREE(rpc_client);
    rpc_client = NULL;

    return FHOS_E_NONE;
}


int rpc_client_is_alive()
{
    if (!rpc_client)
    {
        return 0;
    }

    return rpc_client->status;
}



int rpc_client_connect_register(const fhos_rpc_pid_t pid, fhos_rpc_connect_handler_t handler)
{
    if (!rpc_client)
    {
        return FHOS_E_UNAVAIL;
    }

    if (rpc_client->pid != pid)
    {
        return FHOS_E_BADID;
    }

    return fhos_rpc_client_connect_install(rpc_client, handler);
}


int rpc_client_close_register(const fhos_rpc_pid_t pid, fhos_rpc_close_handler_t handler)
{
    if (!rpc_client)
    {
        return FHOS_E_UNAVAIL;
    }

    if (rpc_client->pid != pid)
    {
        return FHOS_E_BADID;
    }

    return fhos_rpc_client_close_install(rpc_client, handler);
}


int rpc_client_api_register(const rpc_api_id_t id, rpc_client_api_cb cb)
{
    if (!RPC_API_ID_IS_VALID(id))
    {
        return FHOS_E_PARAM;
    }

    rpc_client_api_map[id] = cb;

    return FHOS_E_NONE;
}


int rpc_client_write(char *buf, int len, fhos_rpc_request_cb cb, void *user_data, int flags)
{
    int     write_bytes;


    if (!rpc_client)
    {
        return FHOS_E_UNAVAIL;
    }

    if (!buf || len < RPC_API_MIN_LEN)
    {
        return FHOS_E_PARAM;
    }

    write_bytes = fhos_rpc_client_write(rpc_client, buf, len, cb, user_data, flags);
    if (write_bytes != len)
    {
        return FHOS_E_INTERNAL;
    }

    return 0;
}


int rpc_client_default_set_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            if (user_data)
            {
                *(int *)user_data = FHOS_E_TIMEOUT;
            }

            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return FHOS_E_PARAM;
        }
    }

    if (user_data)
    {
        *(int *)user_data = api_hdr->code;
    }

    return FHOS_E_NONE;
}


int rpc_client_default_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_one_int_t       *one = (rpc_one_int_t *)RPC_API_DATA(api_hdr);
    rpc_two_int_t       *two = (rpc_two_int_t *)user_data;


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            two->val1 = FHOS_E_TIMEOUT;

            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return -1;
        }
    }

    if (user_data)
    {
        two->val1 = api_hdr->code;
        two->val2 = one->val1;
    }

    return 0;
}


int rpc_client_port_info_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t      *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_port_info_t       *src = (rpc_port_info_t *)RPC_API_DATA(api_hdr);
    rpc_port_info_t       *dst = (rpc_port_info_t *)user_data;


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            dst->code = FHOS_E_TIMEOUT;

            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return -1;
        }
    }

    if (user_data)
    {
        *dst = *src;
    }

    return 0;
}

