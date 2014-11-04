#include <jni.h>
#include <stdlib.h>
#include "client_socket.h"
#include "rdma_structure.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

int ClientCreation(struct rdma_core *core)
{
    struct ibv_qp_init_attr qp_attr = {};
    int err;

    core->pd = ibv_alloc_pd(core->cm_id->verbs);
    if(core->pd == NULL)
    {
        printf("ibv_alloc_pd fails.\n");
        return -1;
    }
    core->comp_chan = ibv_create_comp_channel(core->cm_id->verbs);
    
    if(core->comp_chan == NULL)
    {
        printf("completion channel creation fails!\n");
        return -1;
    }

    core->cq = ibv_create_cq(core->cm_id->verbs,1024,NULL,core->comp_chan,0);

    if(core->cq == NULL)
    {
        printf("Completion queue creation fails!\n");
        return -1;
    }

    if(ibv_req_notify_cq(core->cq,0))
    {
        printf("ibv_create_cq fails!\n");
        return -1;
    }

    qp_attr.cap.max_send_wr = 1;
    qp_attr.cap.max_send_sge = 1;
    qp_attr.cap.max_recv_wr = 1;
    qp_attr.cap.max_recv_sge = 1;

    qp_attr.send_cq = core->cq;
    qp_attr.recv_cq = core->cq;
    qp_attr.qp_type = IBV_QPT_RC;
    err = rdma_create_qp(core->cm_id,core->pd,&qp_attr);
    if(err)
    {
        printf("Queue pair creation fails!\n");
        return -1;
    }
    int flags = fcntl(core->cm_channel->fd,F_GETFL,0);
    if(fcntl(core->cm_channel->fd,F_SETFL,flags | O_NONBLOCK) <0)
    {
        printf("fnctl error.\n");
        return -1;
    }
    
    flags = fcntl(core->comp_chan->fd,F_GETFL,0);
    
    if(fcntl(core->comp_chan->fd,F_SETFL,flags | O_NONBLOCK)<0)
    {
        printf("fnctl Error.\n");
        return -1;
    }

    return 0;
}


jlong ClientInit()
{ 
    struct rdma_core *core = (struct rdma_core *)malloc(sizeof(struct rdma_core));
    if(core == NULL)
        return 0;
    core->cm_channel = rdma_create_event_channel();
    if(core->cm_channel == NULL)
    {
        return 0;
    }
    if(rdma_create_id(core->cm_channel,&core->cm_id,NULL,RDMA_PS_TCP) < 0)
    {
        return 0;
    }
    return (jlong)core;
}


jint ClientConnect(struct rdma_core *core,const char *addr,char *port)
{
    struct rdma_cm_event *event;
    struct ibv_qp_init_attr qp_attr = {};
    struct rdma_conn_param conn_param = {};
    struct sockaddr_in sin;
    struct addrinfo *res,*t;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };
    int err;
    int i,j;
    err = getaddrinfo(addr,port,&hints,&res);
    if(err < 0)
    {
        printf("getaddrinfo fails.\n");
        return -1;
    }
    for(t=res;t;t=t->ai_next)
    {
        err = rdma_resolve_addr(core->cm_id,NULL,t->ai_addr,1000);
        if(err == 0)
        {
            break;
        }
    }
    if(err)
    {
        printf("rdma_resolve_addr fails.\n");
        return -1;
    }
    rdma_get_cm_event(core->cm_channel,&event);
    rdma_ack_cm_event(event);
    err = rdma_resolve_route(core->cm_id,1000);
    if(err < 0)
    {
        printf("rdma_resolve_route fails.\n");
        return -1;
    }
    rdma_get_cm_event(core->cm_channel,&event);
    rdma_ack_cm_event(event);
    err = ClientCreation(core);
    if(err < 0)
    {
        printf("ClientCreation fails.\n");
        return -1;
    }
    
    conn_param.responder_resources = 1;
    conn_param.retry_count = 255;
    conn_param.rnr_retry_count = 255;

    err = rdma_connect(core->cm_id,&conn_param);
    if(err < 0)
    {
        printf("connecting fails.\n");
        return -1;
    }
    return 0;
}


jint ClientRead(struct rdma_core *core,int len)
{ 
    struct ibv_sge sge;
    struct ibv_recv_wr recv_wr = {};
    struct ibv_recv_wr *bad_recv_wr;
    
    sge.addr = (uintptr_t)core->read_buf;
    sge.length = sizeof(jbyte)*len;
    sge.lkey = core->read_mr->lkey;

    recv_wr.wr_id = core->comp_chan->fd;
    recv_wr.sg_list = &sge;
    recv_wr.num_sge = 1;

    if(ibv_post_recv(core->cm_id->qp,&recv_wr,&bad_recv_wr))
    {
        printf("ibv_post_recv fails.\n");
        return -1;
    }
    return 0;
}


jint ClientWrite(struct rdma_core *core,int len)
{ 
    struct ibv_sge sge;
    struct ibv_send_wr send_wr = {};
    struct ibv_send_wr *bad_send_wr;
    
    sge.addr = (uintptr_t)core->write_buf;
    sge.length = sizeof(jbyte)*len;
    sge.lkey = core->write_mr->lkey;

    send_wr.wr_id = core->comp_chan->fd;
    send_wr.sg_list = &sge;
    send_wr.num_sge = 1;
    send_wr.opcode = IBV_WR_SEND;
    send_wr.send_flags = IBV_SEND_SIGNALED;

    if(ibv_post_send(core->cm_id->qp,&send_wr,&bad_send_wr))
    {
        printf("ibv_post_send fails.\n");
        return -1;
    }
    return 0;
}


jint ClientClose(struct rdma_core *core)
{
    if(core->read_mr != NULL)
    {
        ibv_dereg_mr(core->read_mr);
    }
    
    if(core->write_mr != NULL)
    {
        ibv_dereg_mr(core->write_mr);
    }

    if(core->cm_id->qp != NULL)
    {
        rdma_destroy_qp(core->cm_id);
        ibv_dealloc_pd(core->pd);
        ibv_destroy_cq(core->cq);
        ibv_destroy_comp_channel(core->comp_chan);
    }
    if(core->cm_id != NULL)
    {
        rdma_destroy_id(core->cm_id);
    }

    if(core->cm_channel != NULL)
    {
        rdma_destroy_event_channel(core->cm_channel);
    }
    free(core);
    return 0;
}
