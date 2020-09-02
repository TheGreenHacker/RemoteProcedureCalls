# RemoteProcedureCalls
Remote procedure calls(RPCs) are essential in distributed computing environments. A client process invokes a procedure to be executed in a different address space by a server process running on a different machine, usually over the network. This is useful since the client machine may not have the necessary hardware/software to perform a certain task, or perhaps the task runs more efficiently utilizing server side computing resources/storage (e.g. data centers). 

However, there are challenges that must be addressed. Since the procedure is invoked and executed on processes running on different machines with different virtual addresses, the virtual address of the client process' address space is meaningless in the remote server's address space. This is particularly true when dealing with data structures containing attributes of pointer/reference types such as linked lists, binary trees, etc. Data (de)serialization is needed to ensure that the server correctly receives the raw data (as a stream of bytes) representation of the arguments stored on client side. Similarly, the server must send back the results(which may also be a complicated data structure with pointer types) to the client using (de)serialization over the network. 

In addition, a real-world data center server handles requests for a variety of RPC's. Thus, it must be able to distinguish which RPC each client is invoking. It would also be helpful for both server and client to inform each other of how much data they're sending each other to avoid wasting memory by allocating too big of a buffer. This in turn makes the architecture more flexible by not imposing a limit on how much data can be transferred. 


## Architecture
In a real-world application, the server would be multi-threaded to handle multiple clients. TCP sockets would be used to keep track of all client connections. However, the focus of this project is on implementing several basic RPCs to demonstrate a basic RPC architecture; socket programming is merely a tool to transfer the data between server and client. Thus, UDP sockets shall suffice.

Client steps are as follows:
1. Invoke the RPC
2. Serialize the arguments in a buffer(packet)
3. Send a header containing the ID of the invoked RPC and a payload size value indicating the number of raw bytes in the arguments buffer.
4. Send the arguments buffer to the server
-------------------------------------------------
5. Wait for server to respond with a header first 
6. Allocate another buffer adequate enough to store the result, as indicated by the payload size in the header received from the server
7. Once the server has responded with the result, obtain it by deserializing the buffer sent by the server. 

Server steps are as follows:
1. Wait for client request
2. Receive header from client 
3. Allocate an arguments buffer big enough to store the arguments from the client
4. Deserialize the arguments buffer 
5. Invoke the appropriate RPC 
6. Serialize the results in another buffer
7. Send a header to indicate number of raw bytes in the results buffer 
8. Send the results buffer to the client

To reflect the above client and server protocols, the following naming convention for different RPCs has been adopted. For an RPC that performs a task,

Client side functions:
void rpc_send_recv - Delegation function responsible for communicating with server, sending/receiving buffers and headers, and allocating received buffers
<RETURN_TYPE> rpc_TASK (ARGS) - Serialize arguments, prepare arguments buffer and header, deserialize results from server
ser_buff_t *<TASK>_stub_marshal (ARGS) - Serializes arguments accordingly for ARGS
<RETURN_TYPE> <TASK>_stub_unmarshal(ser_buff_t *recv_b) - Deserializes results accordingly for RETURN_TYPE
  
Server side functions:
main - No explicity delegation function, but all communication with client and buffer handling is done in the main function
<RETURN_TYPE> TASK (ARGS) - Implements functionality of the task
<RETURN_TYPE> <TASK>_stub_unmarshal(ser_buff_t *recv_b) - Invoke TASK (ARGS) from deserialized arguments
void <TASK>_stub_marshal (RETURN_TYPE, er_buff_t *recv_b) - Serialize results accordingly for RETURN_TYPE
