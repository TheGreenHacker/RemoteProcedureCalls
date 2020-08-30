#ifndef __rpc__
#define __rpc__

/* Constants and data structures for implementing RPC */
#define PORT 8080

#define GET_SENIOR_CITIZENS 1
#define GET_ELDEST_PERSON 2
#define COMPUTE_LIST_SUM 3
#define LIST_SPLITTER 4

typedef struct rpc_hdr_{
    unsigned int rpc_id;
    unsigned int payload_size;
} rpc_hdr_t;

#endif
