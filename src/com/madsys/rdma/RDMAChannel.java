package com.madsys.rdma; 

import com.madsys.rdma.RDMASelectionKey;
import com.madsys.rdma.RDMASelector;

public class RDMAChannel{

    private RDMASelector selector;

    private RDMASelectionKey selection_key;

    private boolean isConnect;

    public long struct;

    public boolean server;//true for server, false for client

    private RDMASelectionKey getSelectionKey(RDMASelector sel){
        if(sel != null){
            return this.selection_key;
        }
        return null;
    }

    private int setSelector(RDMASelector sel){
        this.selector = sel;
        return 0;
    }

    private int setSelectionKey(RDMASelector sel){
        this.selection_key = new RDMASelectionKey(this,sel);
        return 0;
    }

    public boolean isServer(){
        return this.server;
    }

    public boolean isConnected(){
        return this.isConnect;
    }

    public void setConnected(boolean status){
        this.isConnect = status;
    }

    public RDMASelectionKey register(RDMASelector sel,int ops){
        int op = RDMASelectionKey.RDMA_ALL;
        if(sel.register(this) == 0){
            this.setSelector(sel);
            this.setSelectionKey(sel);
            return this.getSelectionKey(sel);
        }
        return null;
    }

    public RDMASelectionKey keyFor(RDMASelector sel){
        return this.getSelectionKey(sel);
    }
}
