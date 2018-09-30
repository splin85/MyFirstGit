
#include <errno.h>
#include <fhos/errno.h>
#include <fhos/utils.h>
#include <fhos/common.h>

#include <sai/types.h>
#include <sai/mcast.h>

#include <rpc/client/client.h>
#include <rpc/client/port.h>
#include <rpc/client/vlan.h>
#include <rpc/common/msg.h>

static rpc_vlan_notify_cb vlan_notify_cb = NULL;

static int rpc_vlan_notify_ack(fhos_rpc_header_t *rpc_hdr, void *cookie)
{
    rpc_api_header_t   *api_hdr = RPC_API_HDR(rpc_hdr);
    rpc_two_int_t      *vlan_status = (rpc_two_int_t *)RPC_API_DATA(api_hdr);

    FHOS_UNUSED(cookie);

    if (vlan_notify_cb)
    {
        vlan_notify_cb(vlan_status->val1, vlan_status->val2);
    }

    api_hdr->code = 0;

    return RPC_API_MIN_LEN;
}

int rpc_vlan_notify_register(rpc_vlan_notify_cb cb)
{
    vlan_notify_cb = cb;

    return FHOS_E_NONE;
}

static int rpc_vlan_member_get_impl(sai_vlan_t           vid,
                                    fhos_rpc_request_cb  cb,
                                    void                *user_data,
                                    int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(sizeof(rpc_three_int_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    rpc_three_int_t  *my_arg  = (rpc_three_int_t *)RPC_API_DATA(api_hdr);

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_VLAN_MEMBER_GET;
    api_hdr->code = 0;

    my_arg->val1 = vid;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}

static int rpc_vlan_member_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_three_int_t  *r_three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);
    rpc_three_int_t  *three   = (rpc_three_int_t *)user_data;

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
        three->val1 = api_hdr->code;
        three->val2 = r_three->val2;
        three->val3 = r_three->val3;
    }

    return FHOS_E_NONE;
}

int rpc_vlan_member_get(sai_vlan_t vid, sai_pbmp_t *member, sai_pbmp_t *untag)
{
    int             ret;
    rpc_three_int_t three;

    ret = rpc_vlan_member_get_impl(vid, rpc_vlan_member_get_ack_cb,
                                   &three, FHOS_RPC_REQUEST_SYNC);

    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    ret = three.val1;

    if (ret != FHOS_E_NONE)
    {
        return ret;
    }
    else
    {
        *member = three.val2;
        *untag  = three.val3;

        return FHOS_E_NONE;
    }
}

static int rpc_vlan_is_exists_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_one_int_t    *r_one   = (rpc_one_int_t *)RPC_API_DATA(api_hdr);
    rpc_two_int_t    *two     = (rpc_two_int_t *)user_data;

    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            if (user_data)
            {
                *(int *)user_data = FHOS_E_TIMEOUT;
            }

            return FHOS_E_TIMEOUT;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            *(int *)user_data = FHOS_E_PARAM;

            return FHOS_E_PARAM;
        }
    }

    if (user_data)
    {
        two->val1 = FHOS_E_NONE;
        two->val2 = r_one->val1;
    }

    return FHOS_E_NONE;
}

static int rpc_vlan_is_exists_impl(sai_vlan_t           vid,
                                   fhos_rpc_request_cb  cb,
                                   void                *user_data,
                                   int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t    *my_arg  = (rpc_one_int_t *)RPC_API_DATA(api_hdr);

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_VLAN_IS_EXISTS;
    api_hdr->code = 0;

    my_arg->val1 = vid;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}

int rpc_vlan_is_exists(sai_vlan_t vid)
{
    int           ret;
    rpc_two_int_t two;

    ret = rpc_vlan_is_exists_impl(vid, rpc_vlan_is_exists_ack_cb,
                                  &two, FHOS_RPC_REQUEST_SYNC);

    if (ret != FHOS_E_NONE)
    {
        return FHOS_FALSE;
    }

    if (two.val1 != FHOS_E_NONE)
    {
        return FHOS_FALSE;
    }
    else
    {
        return two.val2;
    }
}

static int rpc_vlan_default_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_two_int_t    *r_two   = (rpc_two_int_t *)RPC_API_DATA(api_hdr);
    rpc_two_int_t    *two     = (rpc_two_int_t *)user_data;

    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            if (user_data)
            {
                *(int *)user_data = FHOS_E_TIMEOUT;
            }

            return FHOS_E_TIMEOUT;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            *(int *)user_data = FHOS_E_PARAM;

            return FHOS_E_PARAM;
        }
    }

    if (user_data)
    {
        two->val1 = r_two->val1;
        two->val2 = r_two->val2;
    }

    return FHOS_E_NONE;
}

static int rpc_vlan_default_get_impl(sai_port_t           port,
                                     fhos_rpc_request_cb  cb,
                                     void                *user_data,
                                     int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t    *my_arg  = (rpc_one_int_t *)RPC_API_DATA(api_hdr);

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_VLAN_DEFAULT_GET;
    api_hdr->code = 0;

    my_arg->val1 = port;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}

int rpc_vlan_default_get(sai_port_t port, int *vlan)
{
    int           ret;
    rpc_two_int_t two;

    ret = rpc_vlan_default_get_impl(port, rpc_vlan_default_get_ack_cb,
                                    &two, FHOS_RPC_REQUEST_SYNC);

    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    if (two.val1 != FHOS_E_NONE)
    {
        *vlan = -1;
        return two.val1;
    }
    else
    {
        *vlan = two.val2;
        return FHOS_E_NONE;
    }
}

struct rpc_vlan_data_s
{
    int      ret;
    uint8_t *vlan;
};

static int rpc_vlan_all_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t       *api_hdr = RPC_API_HDR_CAST(buf);
    uint8_t                *r_data  = (uint8_t *)RPC_API_DATA(api_hdr);
    struct rpc_vlan_data_s *data    = (struct rpc_vlan_data_s *)user_data;

    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            if (user_data)
            {
                data->ret = FHOS_E_TIMEOUT;
            }

            return FHOS_E_TIMEOUT;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            if (user_data)
            {
                data->ret = FHOS_E_PARAM;
            }
            return FHOS_E_PARAM;
        }
    }

    if (user_data)
    {
        data->ret = api_hdr->code;
        memcpy(data->vlan, r_data, L2_VLAN_MAP_BYTE);
    }

    return FHOS_E_NONE;
}

static int rpc_vlan_all_get_impl(fhos_rpc_request_cb  cb,
                                 void                *user_data,
                                 int                  flags)
{
    int               rc;
    char              buf[RPC_API_SPACE(0)];
    rpc_api_header_t *api_hdr = RPC_API_HDR(buf);

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id   = RPC_VLAN_ALL_GET;
    api_hdr->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}

int rpc_vlan_all_get(uint8_t *vlan)
{
    int          ret;

    struct rpc_vlan_data_s user_data;

    user_data.ret  = FHOS_E_NONE;
    user_data.vlan = vlan;

    ret = rpc_vlan_all_get_impl(rpc_vlan_all_get_ack_cb, &user_data, FHOS_RPC_REQUEST_SYNC);

    if (ret != FHOS_E_NONE)
    {
        return ret;
    }

    if (user_data.ret != FHOS_E_NONE)
    {
        return user_data.ret;
    }

    return FHOS_E_NONE;
}

int rpc_vlan_init(const int unit)
{
    FHOS_UNUSED(unit);

    rpc_client_api_register(RPC_VLAN_NOTIFY, rpc_vlan_notify_ack);

    return FHOS_E_NONE;
}
