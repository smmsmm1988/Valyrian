#include <jni.h>
#include "rdma_structure.h"
#include "../com_madsys_rdma_RDMASocketChannel.h"
#include "client_socket.h"
#include <string.h>

/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    ClientInit
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_madsys_rdma_RDMASocketChannel_ClientInit(JNIEnv *env, jobject obj)
{
    return ClientInit();
}



JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_ClientCreate(JNIEnv *env, jobject obj, jlong ClientStruct)
{
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    return ClientCreation(core);
}


/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    doConnect
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_doConnect(JNIEnv *env, jobject obj, jlong ClientStruct, jstring address, jint port)
{
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    const char *addr = (*env)->GetStringUTFChars(env,address,NULL);
    char port_s[10];
    sprintf(port_s,"%d",port);
    return ClientConnect(core,addr,port_s);
}

/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    doClose
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_doClose(JNIEnv *env, jobject obj, jlong ClientStruct)
{
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    return ClientClose(core);
}


/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    doRead
 * Signature: (JLjava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_doRead(JNIEnv *env, jobject obj, jlong ClientStruct, jobject direct, jint len)
{
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    return ClientRead(core,len);
}

/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    doWrite
 * Signature: (JLjava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_doWrite(JNIEnv *env, jobject obj, jlong ClientStruct, jobject direct, jint len)
{
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    return ClientWrite(core,len);
}

/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    registerMem
 * Signature: (JLjava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_registerMem(JNIEnv *env, jobject obj, jlong ClientStruct, jobject direct, jint len, jint mode)
{
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    if(core->pd == NULL)
    {
        printf("ibv_reg_mr fails.\n");
        return -1;
    }
    
    if(mode == 0)
    {
        core->read_buf = (*env)->GetDirectBufferAddress(env,direct);
        core->read_mr = ibv_reg_mr(core->pd,core->read_buf,sizeof(jbyte)*len, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);

        if(core->read_mr == NULL)
        {
            printf("ibv_reg_mr fails.\n");
            return -1;
        }
    }
    else if(mode == 1)
    {
        core->write_buf = (*env)->GetDirectBufferAddress(env,direct);
        core->write_mr = ibv_reg_mr(core->pd,core->write_buf,sizeof(jbyte)*len, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);

        if(core->write_mr == NULL)
        {
            printf("ibv_reg_mr fails.\n");
            return -1;
        }

    }

    return 0;
}


/*
 * Class:     com_madsys_rdma_RDMASocketChannel
 * Method:    doMemCopy
 * Signature: (JLjava/lang/Object;III)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMASocketChannel_doMemCopy(JNIEnv *env, jobject obj, jlong ClientStruct, jobject direct, jint src_off, jint size, jint des_off)
{
    void *des_buf = (*env)->GetDirectBufferAddress(env,direct);
    struct rdma_core *core = (struct rdma_core *)ClientStruct;
    memcpy(des_buf+des_off,core->read_buf+src_off,size);
    return 0;
}
