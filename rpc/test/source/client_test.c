/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/26/18
 */

#include <stdio.h>

#include <rpc/client/client.h>
#include <rpc/client/port.h>
#include <rpc/client/pkt.h>



static int  is_running = 0;

static struct thread_master    *master = NULL;


static
int rpc_port_link_notify_handler(const int port, int status)
{
    printf("%s:%d port %d status %d\n", __FUNCTION__, __LINE__, port, status);

    return  0;
}


static
int port_enable_test(int count)
{
    int   rc;


    rc = rpc_port_enable_set(count & 0xF, count);
    printf("%s:%d (count %d ) rpc rc: %d\n", __FUNCTION__, __LINE__, count, rc);

    return rc;
}


int port_pkt_tx_test(int count)
{
    int         rc;
    int         len = 16;
    char        buf[RPC_PKT_LENGTH(len)];
    rpc_pkt_t  *pkt = RPC_PKT_CAST(buf);


    memset(buf, 0, sizeof(buf));
    pkt->vlan = 0x123;
    pkt->inner_vlan = 0xabc;
    pkt->len = len;
    *(int *)pkt->data = count;
    pkt->data[len - 2] = 0xaa;
    pkt->data[len - 1] = 0x55;

    errno = 0;
    rc = rpc_pkt_tx(0, pkt);
    //hex_dump(pkt->data, pkt->len);
    printf("%s:%d (count %d ) rpc rc: %d %s\n", __FUNCTION__, __LINE__, count, rc, strerror(errno));

    return rc;
}


void* rpc_client_api_worker(void *args)
{
    int     count = 0;
    int     rc;


    (void)args;

    while (is_running)
    {
        /* uncomment the following line to test */
        //port_enable_test(count);
        port_pkt_tx_test(count);
        count++;
    }

    return NULL;
}


static
int rpc_client_pkt_rx_cb(rpc_pkt_t *pkt, void *cookie)
{
    int   i;


    printf("unit %d flags %08x rx_port %d vlan %d len %d\n",
           pkt->unit, pkt->flags, pkt->rx_port, pkt->vlan, pkt->len);

    for (i = 0; i < pkt->len; i++)
    {
        printf("%02x ", pkt->data[i] & 0xFF);
        if (!((i + 1) & 0xF))
        {
            printf("\n");
        }
    }

    if (i & 0xF)
    {
        printf("\n");
    }

    return 0;
}


static inline
void rpc_client_connect_cb()
{
    printf("client connected\n");
}


static inline
void rpc_client_close_cb()
{
    printf("client closed\n");
}


static
void rpc_api_client_test(struct thread_master *master)
{
    int         rc;
    pthread_t   tid;


    rc = rpc_client_init(RPC_PID_STP, master, rpc_client_connect_cb);
    if (rc < 0)
    {
        printf("%s:%d rpc_client_init failure: %d\n", __FUNCTION__, __LINE__, rc);
        exit(1);
    }

    rpc_client_connect_register(RPC_PID_STP, rpc_client_connect_cb);
    rpc_client_close_register(RPC_PID_STP, rpc_client_close_cb);

    rpc_port_link_register(rpc_port_link_notify_handler);
    rpc_pkt_rx_register(0, rpc_client_pkt_rx_cb, NULL);

    pthread_create(&tid, NULL, rpc_client_api_worker, NULL);
}


void sig_hanlder(int signo)
{
    switch (signo)
    {
        case SIGPIPE:
            break;

        case SIGINT:
            printf("SIGINT\n");
            is_running = 0;
            break;

        case SIGQUIT: /* CTRL + \ */
            exit(1);
            break;

        default:
            ;
    }
}


int main(int argc, char *argv[])
{
    struct thread   thread;


    (void)argc;
    (void)argv;

    signal(SIGPIPE, sig_hanlder);
    signal(SIGINT,  sig_hanlder);
    signal(SIGQUIT, sig_hanlder);

    master = thread_master_create(NULL);

    rpc_api_client_test(master);

    while (thread_fetch(master, &thread))
    {
        thread_call(&thread);
    }

    printf("==========CLIENT BYE ========\n");

    return 0;
}


