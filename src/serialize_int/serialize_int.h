#ifndef __serialize_int__
#define __serialize_int__

typedef struct serialized_buffer ser_buff_t;

static void serialize_int(int *obj, ser_buff_t *b);
void serialize_int_wrapper(void *obj, ser_buff_t *b);
static int *deserialize_int(ser_buff_t *b);
void *deserialize_int_wrapper(ser_buff_t *b);


#endif
