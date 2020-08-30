#ifndef __ll_t__
#define __ll_t__

typedef struct serialized_buffer ser_buff_t;

/* Generic linked list node prototype */
typedef struct ll_node_ {
    /* The field 'is_null' handles cases in which *data stores the sentinel value
     but it's intended to represent actual data. A value of 0 means the node is not
     actually null. */
    unsigned int is_null;
    void *data;
    struct ll_node_ *next;
} ll_node_t;

typedef struct ll_ {
    ll_node_t *head;
} ll_t;

ll_t *init_list();
void add_data(ll_t *l, void *data);
void deinit_list(ll_t *l);

/* (De)Serialization functions */
static void serialize_ll_node_t(ll_node_t *node, ser_buff_t *b, void (*serialize_fn_ptr)(void *, ser_buff_t *));
void serialize_ll_t(ll_t *l, ser_buff_t *b, void (*serialize_fn_ptr)(void *, ser_buff_t *));
static ll_node_t *deserialize_ll_node_t(ser_buff_t *b, void * (*deserialize_fn_ptr)(ser_buff_t *));
ll_t *deserialize_ll_t(ser_buff_t *b, void *(*deserialize_fn_ptr)(ser_buff_t *));

#endif
