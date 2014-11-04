package com.madsys.rdma;

import com.madsys.rdma.RDMAException;
import java.nio.ByteBuffer;
import com.madsys.rdma.RDMAChannel;

public class RDMASocketChannel extends RDMAChannel{

    static
    {
        System.loadLibrary("RDMASocket");
    }

    private String address;

    private int port;

    private long ClientStruct;

    private int WriteBufferSize;

    private int ReadBufferSize;
    
    private ByteBuffer write_direct;

    private ByteBuffer read_direct;

    private native long ClientInit();

    private native int ClientCreate(long ClientStruct);

    private native int doConnect(long ClientStruct,String address,int port);

    private native int doClose(long ClientStruct);

    private native int doRead(long ClientStruct,Object buffer,int size);

    private native int doWrite(long ClientStruct,Object buffer,int size);

    private native int registerMem(long ClientStruct,Object buffer,int size,int mode);

    private native int doMemCopy(long ClientStruct,Object buffer,int src_off,int size,int des_off);

    public RDMASocketChannel() throws RDMAException{
        this.ClientStruct = this.ClientInit();
        if(this.ClientStruct == 0)
            throw new RDMAException("Initiating client fails.");
        this.struct = this.ClientStruct;
        this.write_direct = null;
        this.read_direct = null;
        this.server = false;
        this.setConnected(false);
    }

    public RDMASocketChannel(long AcceptStruct) throws RDMAException{
        this.ClientStruct = AcceptStruct;
        if(this.ClientStruct == 0) 
            throw new RDMAException("Initiating client fails.");
        this.struct = this.ClientStruct;
        this.write_direct = null;
        this.read_direct = null;
        this.server = false;
        this.setConnected(true);
    }

    public static RDMASocketChannel open(){
        try{
            return new RDMASocketChannel();
        }catch(Exception e){
            System.out.println(e.getMessage());
            return null;
        }
    }

    public int connect(String address,int port){
        this.address = address;
        this.port = port;
        return this.doConnect(this.ClientStruct,address,port);
    }

    public int close(){
        return this.doClose(this.ClientStruct);
    }

    public int setCapacity(int size){
        int err;

        if(this.write_direct == null)
        {
            this.write_direct = ByteBuffer.allocateDirect(size);
            if(this.write_direct == null)
                return -1;
            this.WriteBufferSize = size;
            err = this.registerMem(this.ClientStruct,this.write_direct,this.WriteBufferSize,1);//0 for read buffer, 1 for write buffer
            if(err < 0)
                return err;
        }
        
        if(this.read_direct == null)
        {
            this.read_direct = ByteBuffer.allocateDirect(size);
            if(this.read_direct == null){
                return -1;
            }
            this.ReadBufferSize = size;
            err = this.registerMem(this.ClientStruct,this.read_direct,this.ReadBufferSize,0);//0 for read buffer, 1 for write buffer
            if(err < 0)
                return err;
        }
        return 0;
    }

    public int read(ByteBuffer buffer){
        this.read_direct.position(0);
        return this.doRead(this.ClientStruct,this.read_direct,this.ReadBufferSize);
    }


    public int write(ByteBuffer buffer){
        int position = buffer.position();
        int limit = buffer.limit();
        int real_size;
        if((limit-position) > this.WriteBufferSize)
            real_size = this.WriteBufferSize;
        else
            real_size = limit - position;
        this.write_direct.position(0);
        buffer.position(position+real_size);
        return this.doWrite(this.ClientStruct,this.write_direct,real_size);
    }

    public int copy(ByteBuffer buffer){
        if(!buffer.isDirect())
            return -1;
        int size = buffer.capacity();
        int real_size,capacity,position;
        capacity = this.read_direct.capacity();
        position = this.read_direct.position();
        if((position + size) > capacity)
            real_size = capacity - position;
        else
            real_size = size;
        this.doMemCopy(this.ClientStruct,buffer,position,real_size,buffer.position());
        buffer.position(buffer.position()+real_size);
        return 0;
    }
}
