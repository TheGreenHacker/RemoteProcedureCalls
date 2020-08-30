#ifndef __ll_pair_t__
#define __ll_pair_t__

typedef struct serialized_buffer ser_buff_t;
typedef struct ll_ ll_t;

/* Two sublinked lists data structure, one containing even integers and the others odds */
typedef struct ll_pair_ {
    ll_t *even_lst;
    ll_t *odd_lst;
} ll_pair_t;

ll_pair_t *init_ll_pair();
void deinit_ll_pair(ll_pair_t *ll_pair);
void serialize_ll_pair_t(ll_pair_t *ll_pair, ser_buff_t *b);
ll_pair_t *deserialize_ll_pair_t(ser_buff_t *b);

#endif
