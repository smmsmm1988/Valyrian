package com.madsys.rdma;
import com.madsys.rdma.RDMAException;
import java.util.*;
import com.madsys.rdma.RDMAChannel;

public class RDMASelector{

    static
    {
        System.loadLibrary("RDMASocket");
    }

    private native long InitFD();

    private native void doZero(long readfd);

    private native void doSet(long readfd,long struct);

    private native int doSelect(long readfd);

    private native int isSet(long readfd,long struct);

    private native int doFree(long readfd);

    private HashSet<RDMAChannel> channels;

    private long readfd;
    
    public RDMASelector() throws RDMAException{
        this.channels = new HashSet<RDMAChannel>();
        this.readfd = this.InitFD();
        if(this.readfd == 0)
            throw new RDMAException("RDMASelector initiating fails.");

    }

    public static RDMASelector open(){
        try{
            return new RDMASelector();
        }catch(Exception e){
            System.out.println(e.getMessage());
            return null;
        }
    }

    public int select(){
        this.doZero(this.readfd);
        Iterator<RDMAChannel> it = this.channels.iterator();
        while(it.hasNext()){
            this.doSet(this.readfd,it.next().struct);
        }
        return this.doSelect(this.readfd);
    }

    public HashSet<RDMASelectionKey> selectedKeys(){
        HashSet<RDMASelectionKey> tmp = new HashSet<RDMASelectionKey>();
        Iterator<RDMAChannel> it = this.channels.iterator();
        int type;
        RDMASelectionKey selection_key;
        while(it.hasNext()){
            RDMAChannel chan = it.next();
            type = this.isSet(this.readfd,chan.struct);
            switch(type){
                case RDMASelectionKey.RDMA_CONNECT:
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                case RDMASelectionKey.RDMA_ESTABLISHED:
                    chan.setConnected(true);
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                case RDMASelectionKey.RDMA_READ:
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                case RDMASelectionKey.RDMA_WRITE:
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                case RDMASelectionKey.RDMA_READ | RDMASelectionKey.RDMA_WRITE:
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                case RDMASelectionKey.RDMA_DISCONNECT:
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                case RDMASelectionKey.RDMA_INVALID:
                    selection_key = chan.keyFor(this);
                    selection_key.setType(type);
                    tmp.add(selection_key);
                    break;
                default:
                    break;
            }
        }
        return tmp;
    }

    public int register(RDMAChannel channel){
        this.channels.add(channel);
        return 0;
    }

    public int remove(RDMAChannel channel){
        this.channels.remove(channel);
        return 0;
    }

    public int close(){
        this.channels.clear();
        return this.doFree(this.readfd);
    }

}
