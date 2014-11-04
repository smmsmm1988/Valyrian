#include "jni.h"
#include "rdma_structure.h"

jlong ClientInit();

int ClientCreation(struct rdma_core *);

jint ClientConnect(struct rdma_core *,const char *,char *);

jint ClientClose(struct rdma_core *);

jint ClientRead(struct rdma_core *,int);

jint ClienWrite(struct rdma_core *,int);
