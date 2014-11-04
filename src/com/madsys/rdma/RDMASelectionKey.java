package com.madsys.rdma;

import com.madsys.rdma.RDMAChannel;

public class RDMASelectionKey{
    
    public final static int RDMA_CONNECT = 0x00000001;
    public final static int RDMA_ESTABLISHED = 0x00000002;
    public final static int RDMA_READ = 0x00000004;
    public final static int RDMA_WRITE = 0x00000008;
    public final static int RDMA_ALL = 0x00000010;
    public final static int RDMA_DISCONNECT = 0x00000020;
    public final static int RDMA_INVALID = 0x00000040;
    public final static int RDMA_NOTHING = 0x00000080;

    private int type;

    private RDMAChannel chan;

    private RDMASelector selector;

    public RDMASelectionKey(){
        this.type = RDMASelectionKey.RDMA_INVALID;
        this.chan = null;
        this.selector = null;
    }

    public RDMASelectionKey(int type){
        this.type = type;
        this.chan = null;
        this.selector = null;
    }

    public RDMASelectionKey(RDMAChannel chan,RDMASelector sel){
        this.type = RDMASelectionKey.RDMA_INVALID;
        this.chan = chan;
        this.selector = sel;
    }

    public RDMASelectionKey(int type,RDMAChannel chan,RDMASelector sel){
        this.type = type;
        this.chan = chan;
        this.selector = sel;
    }
    
    public void zeroType(){
        this.type = 0;
    }
    public int setType(int type){
        this.type = type;
        return 0;
    }

    public RDMAChannel channel(){
        return this.chan;
    }

    public RDMASelector selector(){
        return this.selector;
    }

    public int cancel(){
        return this.selector.remove(this.chan);
    }

    public boolean isValid(){
        if((this.type & RDMASelectionKey.RDMA_INVALID) != 0)
            return false;
        return true;
    }

    public boolean isAcceptable(){
        if((this.type & RDMASelectionKey.RDMA_CONNECT) != 0)
            return true;
        return false;
    }

    public boolean isConnectable(){
        if((this.type & RDMASelectionKey.RDMA_ESTABLISHED) != 0)
            return true;
        return false;
    }

    public boolean isReadable(){
        if((this.type & RDMASelectionKey.RDMA_READ) != 0)
            return true;
        return false;
    }

    public boolean isWritable(){
        if((this.type & RDMASelectionKey.RDMA_WRITE) != 0)
            return true;
        return false;
    }

    public boolean isDisconnnectable(){
        if((this.type & RDMASelectionKey.RDMA_DISCONNECT) != 0)
            return true;
        return false;
    }
}
