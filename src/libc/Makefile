CC=gcc
CCFLAGS=-shared -fPIC -I $(JAVA_HOME)/include -lrdmacm -libverbs
OFLAGS=-o
SRC1=jni_ServerSocket.c server_socket.c jni_ClientSocket.c client_socket.c jni_Selector.c
OUT1=libRDMASocket.so
a.o:jni_ServerSocket.c
	$(CC) $(CCFLAGS) $(SRC1) $(OFLAGS) $(OUT1)
