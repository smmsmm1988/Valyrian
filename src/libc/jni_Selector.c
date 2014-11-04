#include <jni.h>
#include "../com_madsys_rdma_RDMASelector.h"
#include <sys/select.h>
#include <stdlib.h>
#include <sys/time.h>
#include "rdma_structure.h"

static int maxfd;
/*
 * Class:     com_madsys_rdma_RDMASelector
 * Method:    InitFD
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_madsys_rdma_RDMASelector_InitFD(JNIEnv *env, jobject obj)
{
    fd_set *readfd = (fd_set *)malloc(sizeof(fd_set));
    if(readfd == NULL)
        return 0;
    return (jlong)readfd;
}

/*
 * Class:     com_madsys_rdma_RDMASelector
 * Method:    doZero
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_madsys_rdma_RDMASelector_doZero(JNIEnv *env, jobject obj, jlong readfd)
{
    fd_set *fd = (fd_set *)readfd;
    FD_ZERO(fd);
    maxfd = 0;
}


/*
 * Class:     com_madsys_rdma_RDMASelector
 * Method:    doSet
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_madsys_rdma_RDMASelector_doSet(JNIEnv *env, jobject obj, jlong readfd, jlong ChannelStruct)
{
    fd_set *fd = (fd_set *)readfd;
    struct rdma_core *core = (struct rdma_core *)ChannelStruct;
    if(core->cm_channel != NULL)
    {
        FD_SET(core->cm_channel->fd,fd);
        if(core->cm_channel->fd > maxfd)
            maxfd = core->cm_channel->fd;
    }
    if(core->comp_chan != NULL)
    {
        FD_SET(core->comp_chan->fd,fd);
        if(core->comp_chan->fd > maxfd)
            maxfd = core->comp_chan->fd;
    }
}

/*
 * Class:     com_madsys_rdma_RDMASelector
 * Method:    doSelect
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASelector_doSelect(JNIEnv *env, jobject obj, jlong readfd)
{
    fd_set *fd = (fd_set *)readfd;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(maxfd+1,fd,NULL,NULL,&timeout);
}

JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASelector_isSet(JNIEnv *env, jobject obj, jlong readfd, jlong ChannelStruct)
{
    fd_set *fd = (fd_set *)readfd;
    struct rdma_core *core = (struct rdma_core *)ChannelStruct;
    int err;
    struct ibv_wc wc[2];
    struct ibv_cq *evt_cq;
    void *cq_context;
    if(core->cm_channel != NULL)
    {
        if(FD_ISSET(core->cm_channel->fd,fd))
        {
            if((err = rdma_get_cm_event(core->cm_channel,&core->event)) < 0)
            {
                printf("rdma_get_cm_event fails.\n");
                return RDMA_INVALID;
            }
            switch(core->event->event)
            {
                case RDMA_CM_EVENT_CONNECT_REQUEST:
                    return RDMA_CONNECT;
                case RDMA_CM_EVENT_ESTABLISHED:
                    rdma_ack_cm_event(core->event);
                    return RDMA_ESTABLISHED;
                case RDMA_CM_EVENT_DISCONNECTED:
                    rdma_ack_cm_event(core->event);
                    return RDMA_DISCONNECT;
                default:
                    rdma_ack_cm_event(core->event);
                    return RDMA_INVALID;
            }

        }
    }
    if(core->comp_chan != NULL)
    {
        int i,num;
        int type = 0;
        if(FD_ISSET(core->comp_chan->fd,fd))
        {
            while(ibv_get_cq_event(core->comp_chan,&evt_cq,&cq_context));
            ibv_req_notify_cq(core->cq,0);
            ibv_ack_cq_events(core->cq,1);
            if( (num = ibv_poll_cq(core->cq,2,wc)) > 0 )
            {
                for(i=0;i<num;i++)
                {
                    if(wc[0].status == IBV_WC_SUCCESS)
                    {
                        if(wc[0].opcode == IBV_WC_SEND)
                            type |= RDMA_WRITE;
                        else if(wc[0].opcode & IBV_WC_RECV)
                            type |= RDMA_READ;
                    }
                }
                if(type == 0)
                    type = RDMA_INVALID;
            }
            else
                return RDMA_INVALID;
            return type;
        }
    }
    return RDMA_NOTHING;
}

JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASelector_doFree(JNIEnv *env, jobject obj, jlong readfd)
{
    fd_set *fd = (fd_set *)readfd;
    free(fd);
    return 0;
}
