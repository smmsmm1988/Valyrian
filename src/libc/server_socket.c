#include <jni.h>
#include "server_socket.h"
#include "rdma_structure.h"
#include <stdlib.h>
#include "client_socket.h"
#include "fcntl.h"

jlong ServerInit()
{
    struct rdma_core *core = (struct rdma_core *)malloc(sizeof(struct rdma_core));
    memset(core,0,sizeof(struct rdma_core));
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
    
    int flags = fcntl(core->cm_channel->fd,F_GETFL,0);
    
    if(fcntl(core->cm_channel->fd,F_SETFL,flags | O_NONBLOCK)<0)
    {
        printf("fnctl Error.\n");
        free(core);
        return 0;
    }
    return (jlong)core;
}

jint ServerBind(struct rdma_cm_id *cm_id,jint port)
{ 
    struct sockaddr_in sin;
    int err;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    err = rdma_bind_addr(cm_id,(struct sockaddr *)&sin);
    if(err < 0)
    {
        return -1;
    }
    return 0;
}

jint ServerListen(struct rdma_cm_id *cm_id)
{
    int err = rdma_listen(cm_id,5);
    if(err < 0)
    {
        return -1;
    }
    return 0;
}

jint ServerClose(struct rdma_core *core)
{
    rdma_destroy_id(core->cm_id);
    rdma_destroy_event_channel(core->cm_channel);
    free(core);
    return 0;
}


jlong ServerAccept(struct rdma_core *core)
{
    struct rdma_core *new_core = (struct rdma_core *)malloc(sizeof(struct rdma_core));
    if(new_core == NULL)
        return 0;

    memset(new_core,0,sizeof(struct rdma_core));

    new_core->cm_id = core->event->id;
    new_core->cm_channel = NULL;
    rdma_ack_cm_event(core->event);
    struct ibv_qp_init_attr qp_attr = {};
    struct rdma_conn_param conn_param = {};
    int err;

    new_core->pd = ibv_alloc_pd(new_core->cm_id->verbs);
    
    if(new_core->pd == NULL)
    {
        printf("ibv_alloc_pd fails.\n");
        goto error;
    }
    
    new_core->comp_chan = ibv_create_comp_channel(new_core->cm_id->verbs);
    
    if(new_core->comp_chan == NULL)
    {
        printf("completion channel creation fails!\n");
        goto error;
    }

    new_core->cq = ibv_create_cq(new_core->cm_id->verbs,1024,NULL,new_core->comp_chan,0);

    if(new_core->cq == NULL)
    {
        printf("Completion queue creation fails!\n");
        goto error;
    }

    if(ibv_req_notify_cq(new_core->cq,0))
    {
        printf("ibv_create_cq fails!\n");
        goto error;
    }
    qp_attr.cap.max_send_wr = 1;
    qp_attr.cap.max_send_sge = 1;
    qp_attr.cap.max_recv_wr = 1;
    qp_attr.cap.max_recv_sge = 1;

    qp_attr.send_cq = new_core->cq;
    qp_attr.recv_cq = new_core->cq;
    qp_attr.qp_type = IBV_QPT_RC;
    err = rdma_create_qp(new_core->cm_id,new_core->pd,&qp_attr);
    if(err)
    {
        printf("Queue pair creation fails!\n");
        goto error;
    }

    conn_param.responder_resources = 1;
    conn_param.retry_count = 255;
    conn_param.rnr_retry_count = 255;
    
    int flags = fcntl(new_core->comp_chan->fd,F_GETFL,0);
    
    if(fcntl(new_core->comp_chan->fd,F_SETFL,flags | O_NONBLOCK)<0)
    {
        printf("fnctl Error.\n");
        goto error;
    }

    rdma_accept(new_core->cm_id,&conn_param);
    return (jlong)new_core;

    error:
    ClientClose(new_core);
    free(new_core);
    return 0;
}
