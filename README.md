Valyrian
========

Valyrian is an RDMA-based communicating library for Java-based applications to use Remote Direct Memory Access to transfer messages or data. Compared with Java.nio.channels.\*, one of the libraries that is used in Spark to transfer data among nodes, Valyrian is able to improve the data transferring performance effectively. Such improvement benefits from the zero-data-copy of RDMA to transfer data from one node to another. In contrast, TCP/IP (on which many libraries such as Java.nio.channels.\* or Java.net.\*) has complex prococol stack and requires an explicit data copy from user memory space to kernel memory space, which limits the communication performance.
