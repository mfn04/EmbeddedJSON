
#include "stdint.h"
#include "stdbool.h"
#include "stdarg.h"

#define OBJECT(...) ({ field_data arr[] = (field_data[]){__VA_ARGS__}; (json_object) {  \
    .data_size = sizeof(arr)/sizeof(field_data), \
    .data =  arr , \
}; })

#define FIELD_NUMBER(k, v) (field_data) { \
    .key = k, \
    .type = TYPE_NUMBER, \
    .value = { \
        .number = v \
    } \
}
#define FIELD_STRING(k, v) (field_data) { \
    .key = k, \
    .type = TYPE_STRING, \
    .value = { \
        .string = v \
    } \
}
#define FIELD_OBJECT(k, v)
#define FIELD_ARRAY(k, v)

#define FIELD_BOOLEAN(k, v) (field_data) { \
    .key = k, \
    .type = TYPE_BOOLEAN, \
    .value = { \
        .boolean = v \
    } \
}
#define FIELD_NULL(k,v) (field_data) { \
    .key = k, \
    .type = TYPE_NULL, \
    .value = { \
        .number = v \
    } \
}

typedef enum {
    TYPE_NUMBER,
    TYPE_ARRAY,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_NULL
} field_type;

struct json_object;

typedef struct {
    const char* key;
    field_type type;
    union value_t {
        uint32_t number;
        const char* string;
        struct json_object* object;
        bool boolean;
    } value;
} field_data;

typedef struct {
    int data_size;
    field_data* data;
} json_object;

void app_main() {

    int* asd;

    asd = (int[]){4,5,6,7};


    field_data data = FIELD_NUMBER("Hello", 5);

    json_object object = OBJECT(
        FIELD_NUMBER("Balance", 50),
        FIELD_STRING("First_Name", "Hello"));

}