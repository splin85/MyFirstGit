/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc.
 *
 * @file
 * @author zhang jian ming
 * @date 02/26/18
 */

#include <stdio.h>

#include <sai/sai.h>

#include <rpc/common/msg.h>

#include <rpc/server/server.h>
#include <rpc/server/port.h>
#include <rpc/server/pkt.h>



static int  is_running = 1;

static int  pid = RPC_PID_STP;

static struct thread_master  *master = NULL;



int rpc_server_port_link_notify(int count)
{
    int     rc;


    rc = rpc_port_link_notify(pid, count & 0xF, count);
    printf("%s:%d (count %d ) rpc rc: %d\n", __FUNCTION__, __LINE__, count, rc);

    return rc;
}


int rpc_server_tx_pkt(int count)
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
    rc = rpc_pkt_tx(pid, pkt);
    printf("%s:%d (count %d ) rpc rc: %d %s\n", __FUNCTION__, __LINE__, count, rc, strerror(errno));

    return rc;
}


static
void* rpc_server_api_worker(void *args)
{
    int     count = 0;
    int     rc;


    (void)args;

    sleep(5);
    while (is_running)
    {
        /* uncomment the following line to test */
        rpc_server_port_link_notify(count);
        rpc_server_tx_pkt(count);
        count++;
    }

    return NULL;
}



static
void rpc_api_server_test(struct thread_master *master)
{
    int         rc;
    pthread_t   tid;


    rc = rpc_server_init(master);
    if (rc < 0)
    {
        printf("%s:%d rpc_server_init failure: %d\n", __FUNCTION__, __LINE__, rc);
    }

    pthread_create(&tid, NULL, rpc_server_api_worker, NULL);
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

    rpc_api_server_test(master);

    while (thread_fetch(master, &thread))
    {
        thread_call(&thread);
    }

    printf("========== SERVER BYE ========\n");

    return 0;
}


