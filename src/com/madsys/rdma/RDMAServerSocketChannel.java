package com.madsys.rdma;
import com.madsys.rdma.RDMASocketChannel;
import com.madsys.rdma.RDMAException;
import com.madsys.rdma.RDMAChannel;

public class RDMAServerSocketChannel extends RDMAChannel{

    static
    {
        System.loadLibrary("RDMASocket");
    }

    private long ServerStruct;//To store the address of the server structure in C

    private int port;

    private native long doServerInit();

    private native int doBind(long ServerStruct,int port);

    private native int doClose(long ServerStruct);

    private native int doListen(long ServerStruct);

    private native long doAccept(long ServerStruct);

    public RDMAServerSocketChannel() throws RDMAException{
        this.port = 0;
        this.ServerStruct = this.doServerInit();
        if(this.ServerStruct == 0)
            throw new RDMAException("Creating server socket channel fails.");
        this.struct = this.ServerStruct;
        this.server = true;
        this.setConnected(false);
    }

    public static RDMAServerSocketChannel open(){
        try{
            return new RDMAServerSocketChannel();
        }catch(RDMAException e){
            System.out.println(e.getMessage());
            return null;
        }
    }

    
    public int bind(int port){
        this.port = port;
        return this.doBind(this.ServerStruct,this.port);
    }

    
    public int listen(){
        return this.doListen(this.ServerStruct);
    }

    
    public int close(){
        return this.doClose(this.ServerStruct);
    }


    public int getLocalPort(){
        return this.port;
    }

    public RDMASocketChannel accept(){
        long AcceptStruct = this.doAccept(this.ServerStruct);
        if(AcceptStruct == 0)
            return null;
        else{
            try{
                return new RDMASocketChannel(AcceptStruct);
            }catch(Exception e){
                return null;
            }
        }
    }
}
