package com.madsys.rdma;

import com.madsys.rdma.*;
import java.util.*;
import java.nio.ByteBuffer;

public class RDMAServer{
    public static void main(String[] args){
        if(args.length != 1){
            System.out.println("Usage: java com.madsys.rdma.RDMAServer buffer_size(MB)");
            System.exit(-1);
        }
        int size = 1024 * 1024 * Integer.parseInt(args[0]);
        int bytes = 0;
        int num = 0;
        ByteBuffer send = ByteBuffer.allocate(size);
        ByteBuffer recv = ByteBuffer.allocateDirect(size);//Direct buffer is used to receive data so as to avoid an additional data copy
        RDMASelector sel = RDMASelector.open();
        RDMAServerSocketChannel ssc = RDMAServerSocketChannel.open();
        if(sel == null || ssc == null){
            System.exit(-1);
        }
        byte value = (byte)16;
        for(int i=0;i<size;i++)
            send.put(value);
        ssc.register(sel,RDMASelectionKey.RDMA_ALL);
        ssc.bind(30331);
        ssc.listen();
        while(true){
            int count = sel.select();
            if(count != 0){
                Iterator<RDMASelectionKey> it = sel.selectedKeys().iterator();
                while(it.hasNext()){
                    RDMASelectionKey key = it.next();
                    it.remove();
                    if(key.isValid()){
                        if(key.isAcceptable()){
                            RDMAServerSocketChannel tmp = (RDMAServerSocketChannel)key.channel();
                            RDMASocketChannel sc = tmp.accept();
                            sc.register(sel,RDMASelectionKey.RDMA_ALL);
                            sc.setCapacity(size);
                            sc.read(recv);
                        }
                        else if(key.isConnectable()){
                            RDMAChannel chan = key.channel();
                            if(!chan.isServer()){//client
                                RDMASocketChannel socket_channel = (RDMASocketChannel)chan;
                            }
                            else{
                                RDMAServerSocketChannel server_socket_channel = (RDMAServerSocketChannel)chan;
                            }
                        }
                        else if(key.isReadable()){
                            RDMASocketChannel tmp = (RDMASocketChannel)key.channel();
                            tmp.copy(recv);
                            send.flip();
                            tmp.write(send);
                        }
                        else if(key.isWritable()){
                            RDMASocketChannel tmp = (RDMASocketChannel)key.channel();
                            recv.position(0);
                            tmp.read(recv);
                        }
                    }
                    else System.out.println("Channel Invalid.");
                }
            }
        }
    }
}
