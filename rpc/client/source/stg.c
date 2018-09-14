/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author wang shun qiang
 * @date 07/31/18
 */

#include <fhos/errno.h>
#include <fhos/utils.h>

#include <rpc/client/client.h>
#include <rpc/client/stg.h>
#include <rpc/common/msg.h>




static
int rpc_stg_default_get_impl(fhos_rpc_request_cb  cb,
                             void                *user_data,
                             int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_DEFAULT_GET;
    api_hdr->code = 0;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_stg_default_get(int *stg)
{
    int               rc;
    rpc_two_int_t     two;


    if (!stg)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_stg_default_get_impl(rpc_client_default_get_ack_cb,
                                  &two, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    *stg = two.val2;

    return two.val1;
}


static
int rpc_stg_create_impl(const int           *stg,
                        fhos_rpc_request_cb  cb,
                        void                *user_data,
                        int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *stg_create = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_CREATE;
    api_hdr->code = 0;

    stg_create->val1 = *stg;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_stg_create(int *stg)
{
    int     rc;
    int     code;


    rc = rpc_stg_create_impl(stg, rpc_client_default_set_ack_cb,
                             &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


static
int rpc_stg_destroy_impl(const int            stg,
                         fhos_rpc_request_cb  cb,
                         void                *user_data,
                         int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_one_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_one_int_t      *stg_destroy = (rpc_one_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_DESTROY;
    api_hdr->code = 0;

    stg_destroy->val1 = stg;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_stg_destroy(int stg)
{
    int     rc;
    int     code;


    rc = rpc_stg_destroy_impl(stg, rpc_client_default_set_ack_cb,
                              &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


static
int rpc_stg_vlan_add_impl(const int            stg,
                          const sai_vlan_t     vid,
                          fhos_rpc_request_cb  cb,
                          void                *user_data,
                          int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *vlan_add = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!SAI_IS_VALID_VLAN(vid))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_VLAN_ADD;
    api_hdr->code = 0;

    vlan_add->val1 = stg;
    vlan_add->val2 = vid;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_stg_vlan_add(const int stg, const sai_vlan_t vid)
{
    int     rc;
    int     code;


    rc = rpc_stg_vlan_add_impl(stg, vid, rpc_client_default_set_ack_cb,
                               &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


int rpc_stg_vlan_add_async(const int            stg,
                           const sai_vlan_t     vid,
                           fhos_rpc_request_cb  cb,
                           void                *user_data)
{
    return rpc_stg_vlan_add_impl(stg, vid, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


static
int rpc_stg_vlan_remove_impl(const int            stg,
                             const sai_vlan_t     vid,
                             fhos_rpc_request_cb  cb,
                             void                *user_data,
                             int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_two_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_two_int_t      *vlan_remove = (rpc_two_int_t *)RPC_API_DATA(api_hdr);


    if (!SAI_IS_VALID_VLAN(vid))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_VLAN_REMOVE;
    api_hdr->code = 0;

    vlan_remove->val1 = stg;
    vlan_remove->val2 = vid;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_stg_vlan_remove(const int stg, const sai_vlan_t vid)
{
    int     rc;
    int     code;


    rc = rpc_stg_vlan_remove_impl(stg, vid, rpc_client_default_set_ack_cb,
                                  &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


int rpc_stg_vlan_remove_async(const int            stg,
                              const sai_vlan_t     vid,
                              fhos_rpc_request_cb  cb,
                              void                *user_data)
{
    return rpc_stg_vlan_remove_impl(stg, vid, cb, user_data, FHOS_RPC_REQUEST_ASYNC);
}


static
int rpc_stg_port_state_set_impl(const sai_port_t     port,
                                const int            stg,
                                const int            state,
                                fhos_rpc_request_cb  cb,
                                void                *user_data,
                                int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_three_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_three_int_t    *stg_state = (rpc_three_int_t *)RPC_API_DATA(api_hdr);


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PARAM;
    }

    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_PORT_STATE_SET;
    api_hdr->code = 0;

    stg_state->val1 = port;
    stg_state->val2 = stg;
    stg_state->val3 = state;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_stg_port_state_set(const sai_port_t port, const int stg, const int state)
{
    int     rc;
    int     code;


    rc = rpc_stg_port_state_set_impl(port, stg, state, rpc_client_default_set_ack_cb,
                                     &code, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        code = rc;
    }

    return code;
}


int rpc_stg_port_state_set_async(const sai_port_t     port,
                                 const int            stg,
                                 const int            state,
                                 fhos_rpc_request_cb  cb,
                                 void                *user_data)
{
    return rpc_stg_port_state_set_impl(port, stg, state, cb,
                                      user_data, FHOS_RPC_REQUEST_ASYNC);
}

static
int rpc_stg_port_state_get_impl(const sai_port_t     port,
                                const int            stg,
                                fhos_rpc_request_cb  cb,
                                void                *user_data,
                                int                  flags)
{
    int                 rc;
    char                buf[RPC_API_SPACE(sizeof(rpc_three_int_t))];
    rpc_api_header_t   *api_hdr = RPC_API_HDR(buf);
    rpc_three_int_t    *three = (rpc_three_int_t *)RPC_API_DATA(api_hdr);


    RPC_API_HDR_INIT(api_hdr);
    api_hdr->id = RPC_STG_PORT_STATE_GET;
    api_hdr->code = 0;

    three->val1 = port;
    three->val2 = stg;

    rc = rpc_client_write(buf, sizeof(buf), cb, user_data, flags);

    return rc;
}


int rpc_client_stg_state_get_ack_cb(char *buf, int len, void *user_data)
{
    rpc_api_header_t    *api_hdr = RPC_API_HDR_CAST(buf);
    rpc_three_int_t     *rc = (rpc_three_int_t *)RPC_API_DATA(api_hdr);
    rpc_three_int_t     *three = (rpc_three_int_t *)user_data;


    if (!buf)
    {
        if (FHOS_RPC_REQUEST_IS_TIMOUT(len))
        {
            three->val1 = FHOS_E_TIMEOUT;

            return 0;
        }
        else if (len < RPC_API_HDR_LEN)
        {
            return -1;
        }
    }

    if (user_data)
    {
        three->val1 = api_hdr->code;
        three->val2 = rc->val2;
        three->val3 = rc->val3;
    }

    return 0;
}


int rpc_stg_port_state_get(const sai_port_t port, const int stg, int *state)
{
    int               rc;
    rpc_three_int_t   three;


    if (!SAI_IS_VALID_PORT(port))
    {
        return FHOS_E_PORT;
    }

    if (!state)
    {
        return FHOS_E_NULL_PTR;
    }

    rc = rpc_stg_port_state_get_impl(port, stg, rpc_client_stg_state_get_ack_cb,
                                     &three, FHOS_RPC_REQUEST_SYNC);
    if (rc < 0)
    {
        return rc;
    }

    *state = three.val3;

    return three.val1;
}

