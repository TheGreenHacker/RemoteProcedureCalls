#include <stdlib.h>

#include "ll_t.h"

#include "../../sentinel.h"

ll_t *init_list() {
    return calloc(1, sizeof(ll_t));
}

void add_data(ll_t *l, void *data) {
    ll_node_t *node = calloc(1, sizeof(ll_node_t));
    node->data = data;
    node->next = l->head;
    l->head = node;
}

void deinit_list(ll_t *l) {
    ll_node_t *head = l->head;
    while (head) {
        ll_node_t *tmp = head;
        head = head->next;
        free(tmp);
    }
    
    free(l);
}

static void serialize_ll_node_t(ll_node_t *node, ser_buff_t *b, void (*serialize_fn_ptr)(void *, ser_buff_t *)) {
    SENTINEL_INSERTION_CODE(node, b);
    
    serialize_data(b, (char *)&node->is_null, sizeof(unsigned int));
    serialize_fn_ptr(node->data, b);
    serialize_ll_node_t(node->next, b, serialize_fn_ptr);
}

void serialize_ll_t(ll_t *l, ser_buff_t *b, void (*serialize_fn_ptr)(void *, ser_buff_t *)) {
    SENTINEL_INSERTION_CODE(l, b);
    
    serialize_ll_node_t(l->head, b, serialize_fn_ptr);
}

static ll_node_t * deserialize_ll_node_t(ser_buff_t *b, void * (*deserialize_fn_ptr)(ser_buff_t *)) {
    SENTINEL_DETECTION_CODE(b);
    
    ll_node_t *node = calloc(1, sizeof(ll_node_t));
    de_serialize_data((char *)&node->is_null, b, sizeof(unsigned int));
    node->data = deserialize_fn_ptr(b);
    node->next = deserialize_ll_node_t(b, deserialize_fn_ptr);
    return node;
}

ll_t *deserialize_ll_t(ser_buff_t *b, void *(*deserialize_fn_ptr)(ser_buff_t *)) {
    SENTINEL_DETECTION_CODE(b);
    
    ll_t *l = calloc(1, sizeof(ll_t));
    l->head = deserialize_ll_node_t(b, deserialize_fn_ptr);
    return l;
}
