#ifndef RDMA_STRUCTURE
#define RDMA_STRUCTURE

#include <infiniband/arch.h>
#include <infiniband/verbs.h>
#include <rdma/rdma_cma.h>
#include <rdma/rdma_verbs.h>

struct pdata
{
    uint64_t buf_va;
    uint32_t buf_rkey;
};

struct rdma_core
{
    struct rdma_event_channel *cm_channel;
    struct rdma_cm_id *cm_id;
    struct ibv_mr *read_mr,*write_mr;
    struct ibv_comp_channel *comp_chan;
    struct ibv_pd *pd;
    struct ibv_cq *cq;
    struct ibv_cq *evt_cq;
    void *cq_contenxt;
    void *read_buf,*write_buf;
    struct rdma_cm_event *event;
};

#define RDMA_CONNECT    0x00000001
#define RDMA_ESTABLISHED    0x00000002
#define RDMA_READ   0x00000004
#define RDMA_WRITE  0x00000008
#define RDMA_ALL    0x00000010
#define RDMA_DISCONNECT 0x00000020
#define RDMA_INVALID 0x00000040
#define RDMA_NOTHING 0x00000080

#endif
