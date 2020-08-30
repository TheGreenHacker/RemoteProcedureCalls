#include <stdlib.h>

#include "../../serialize.h"
#include "../../sentinel.h"
#include "../ll_t/ll_t.h"
#include "../serialize_int/serialize_int.h"
#include "ll_pair_t.h"

ll_pair_t *init_ll_pair() {
    return calloc(1, sizeof(ll_pair_t));
}

void deinit_ll_pair(ll_pair_t *ll_pair) {
    deinit_list(ll_pair->even_lst);
    deinit_list(ll_pair->odd_lst);
    free(ll_pair);
}

void serialize_ll_pair_t(ll_pair_t *ll_pair, ser_buff_t *b) {
    SENTINEL_INSERTION_CODE(ll_pair, b);
    
    serialize_ll_t(ll_pair->even_lst, b, serialize_int_wrapper);
    serialize_ll_t(ll_pair->odd_lst, b, serialize_int_wrapper);
}

ll_pair_t *deserialize_ll_pair_t(ser_buff_t *b) {
    SENTINEL_DETECTION_CODE(b);
    
    ll_pair_t *ll_pair = init_ll_pair();
    ll_pair->even_lst = deserialize_ll_t(b, deserialize_int_wrapper);
    ll_pair->odd_lst = deserialize_ll_t(b, deserialize_int_wrapper);
    return ll_pair;
}
