#include <stdlib.h>
#include <assert.h>

#include "person_t.h"
#include "../../sentinel.h"
#include "../../serialize.h"

static void serialize_person_t(person_t *obj, ser_buff_t *b) {
    SENTINEL_INSERTION_CODE(obj, b);
    
    serialize_data(b, obj->name, sizeof(char) * 32);
    serialize_data(b, (char *)&obj->age, sizeof(int));
    serialize_data(b, (char *)&obj->weight, sizeof(int));
}

void serialize_person_t_wrapper(void *obj, ser_buff_t *b) {
    serialize_person_t((person_t *) obj, b);
}

static person_t *deserialize_person_t(ser_buff_t *b) {
    SENTINEL_DETECTION_CODE(b);
    
    person_t *p = calloc(1, sizeof(person_t));
    de_serialize_data(p->name, b, sizeof(char) * 32);
    de_serialize_data((char *)&p->age, b, sizeof(int));
    de_serialize_data((char *)&p->weight, b, sizeof(int));
    
    return p;
}

void *deserialize_person_t_wrapper(ser_buff_t *b) {
    return (void *) deserialize_person_t(b);
}
