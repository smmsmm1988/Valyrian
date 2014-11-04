#include <jni.h>
#include "rdma_structure.h"
#include "server_socket.h"
#include "../com_madsys_rdma_RDMAServerSocketChannel.h"

/*
 * Class:     com_madsys_rdma_RDMAServerSocketChannel
 * Method:    doServerInit
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_madsys_rdma_RDMAServerSocketChannel_doServerInit(JNIEnv *env, jobject obj)
{
    return ServerInit(); 
}

/*
 * Class:     com_madsys_rdma_RDMAServerSocketChannel
 * Method:    doBind
 * Signature: (JLjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMAServerSocketChannel_doBind(JNIEnv *evn, jobject obj, jlong server_struct, jint port)
{
    const char *addr;
    struct rdma_core *core = (struct rdma_core *)server_struct;
    return ServerBind(core->cm_id,port); 
}

/*
 * Class:     com_madsys_rdma_RDMAServerSocketChannel
 * Method:    doClose
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMAServerSocketChannel_doClose(JNIEnv *env, jobject obj, jlong server_struct)
{
    struct rdma_core *core = (struct rdma_core *)server_struct;
    return ServerClose(core); 
}

/*
 * Class:     com_madsys_rdma_RDMAServerSocketChannel
 * Method:    doAccept
 * Signature: (J)J
 */
JNIEXPORT jint JNICALL Java_com_madsys_rdma_RDMAServerSocketChannel_doListen(JNIEnv *env, jobject obj, jlong server_struct)
{
    int err;
    struct rdma_core *core = (struct rdma_core *)server_struct;
    return ServerListen(core->cm_id);
}

JNIEXPORT jlong JNICALL Java_com_madsys_rdma_RDMAServerSocketChannel_doAccept(JNIEnv *env, jobject obj, jlong server_struct)
{
    struct rdma_core *core = (struct rdma_core *)server_struct;
    return ServerAccept(core);
}
