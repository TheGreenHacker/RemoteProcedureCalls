#ifndef __person_t__
#define __person_t__

typedef struct serialized_buffer ser_buff_t;

typedef struct person_ {
    char name[32];
    int age;
    int weight;
} person_t;

static void serialize_person_t(person_t *obj, ser_buff_t *b);
void serialize_person_t_wrapper(void *obj, ser_buff_t *b);
static person_t *deserialize_person_t(ser_buff_t *b);
void *deserialize_person_t_wrapper(ser_buff_t *b);


#endif
