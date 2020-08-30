#include <stdlib.h>
#include <assert.h>

#include "serialize_int.h"
#include "../../sentinel.h"
#include "../../serialize.h"

static void serialize_int(int *obj, ser_buff_t *b) {
    assert(obj);
    serialize_data(b, (char *)obj, sizeof(int));
}

void serialize_int_wrapper(void *obj, ser_buff_t *b) {
    serialize_int((int *)obj, b);
}

static int *deserialize_int(ser_buff_t *b) {
    int *i = calloc(1, sizeof(int));
    de_serialize_data((char *)i, b, sizeof(int));
    return i;
}

void *deserialize_int_wrapper(ser_buff_t *b) {
    return (void *) deserialize_int(b);
}
