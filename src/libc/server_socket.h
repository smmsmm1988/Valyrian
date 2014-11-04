#include "rdma_structure.h"
#include "jni.h"

jlong ServerInit();

jint ServerBind(struct rdma_cm_id *,jint);

jint ServerListen(struct rdma_cm_id *);

jint ServerClose(struct rdma_core *);

jlong ServerAccept(struct rdma_core *);
