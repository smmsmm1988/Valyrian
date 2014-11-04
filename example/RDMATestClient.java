package com.madsys.rdma;

import com.madsys.rdma.*;
import java.util.*;
import java.nio.ByteBuffer;

public class RDMAClient{
    public static void main(String[] args){
        if(args.length != 2){
            System.out.println("Usage: java com.madsys.rdma.RDMAClient server_ip buffer_size(MB)");
            System.exit(-1);
        }
        int bytes = 0;
        int size = 1024 * 1024 * Integer.parseInt(args[1]);
        int test_cases = 1024 / Integer.parseInt(args[1]);
        int num = 0;
        long start=0,end=0;
        ByteBuffer send = ByteBuffer.allocate(size);
        ByteBuffer recv = ByteBuffer.allocateDirect(size);//Direct buffer is used to receive data so as to avoid an addition data copy
        RDMASelector sel = RDMASelector.open();
        RDMASocketChannel sc = RDMASocketChannel.open();
        if(sel == null || sc == null){
            System.exit(-1);
        }
        byte value = (byte)12;
        for(int i=0;i<size;i++)
            send.put(value);
        sc.register(sel,RDMASelectionKey.RDMA_ALL);
        sc.connect(args[0],30331);
        
        while(true){
            int count = sel.select();
            if(count != 0){
                Iterator<RDMASelectionKey> it = sel.selectedKeys().iterator();
                while(it.hasNext()){
                    RDMASelectionKey key = it.next();
                    it.remove();
                    if(key.isValid()){
                        if(key.isAcceptable()){
                        }
                        else if(key.isConnectable()){
                            RDMAChannel tmp = key.channel();
                            if(!tmp.isServer()){
                                RDMASocketChannel t = (RDMASocketChannel)tmp;
                                t.setCapacity(size);
                                start = System.currentTimeMillis();
                                send.flip();
                                t.write(send);
                            }
                        }
                        else if(key.isReadable()){
                            RDMASocketChannel tmp = (RDMASocketChannel)key.channel();
                            tmp.copy(recv);
                            num++;
                            if(num == test_cases){
                                end = System.currentTimeMillis();
                                System.out.println("Time:"+(end-start)+"ms");
                            }
                            else{
                                send.flip();
                                tmp.write(send);
                            }
                        }
                        else if(key.isWritable()){
                            RDMASocketChannel tmp = (RDMASocketChannel)key.channel();
                            recv.position(0);
                            tmp.read(recv);
                        }
                    }
                    else{
                        System.out.println("Channel Not Valid.");
                    }
                }
            }
        }
    }
}
