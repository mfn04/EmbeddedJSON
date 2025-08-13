#ifndef _EMBEDDEDJSON_H_
#define _EMBEDDEDJSON_H_

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define JSON_DOCUMENT(s) _Generic((s), json_value: s)

#define FIELD(k, v) _Generic((k), \
    char*: _Generic((v), \
        json_value: (json_field) { \
            .key = k, \
            .value = v \
        } \
    ), \
    const char*: _Generic((v), \
        json_value: (json_field) { \
            .key = k, \
            .value = v \
        } \
    ) \
)

#define ARRAY(s,...) _Generic((s), \
    int: (json_value) { \
        .type = ARRAY_VALUE, \
        .value.array = { \
            .size = s, \
            .elements = (json_value[]) {__VA_ARGS__} \
        } \
    } \
)

#define OBJECT(s,...) _Generic((s), \
    int: (json_value) { \
        .type = OBJECT_VALUE, \
        .value.object = { \
            .field_size = s, \
            .field = (json_field[]){__VA_ARGS__} \
        } \
    } \
)

#define NUMBER(v) _Generic((v), \
    int: (json_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_NUMBER, \
            .base.number = v \
        } \
    } \
)

#define STRING(str,len) _Generic((str), \
    char*: (json_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_STRING, \
            .base.string = str, \
            .base.string_len = len \
        } \
    }, \
    const char*: (json_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_STRING, \
            .base.string = str, \
            .base.string_len = len \
        } \
    } \
)

#define BOOLEAN(v) _Generic((v), \
    int: (json_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_BOOLEAN, \
            .base.boolean = v \
        } \
    } \
)

#define NILL() (json_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_NULL, \
    } \
}

typedef enum scalar_type {
    TYPE_NUMBER,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_NULL
} scalar_type;

typedef enum value_type {
    SCALAR_VALUE,
    ARRAY_VALUE,
    OBJECT_VALUE
} value_type;

typedef struct json_object {
    int field_size;
    struct json_field* field;
} json_object;

typedef struct json_scalar {
    scalar_type base_type;
    union {
            float number;
            struct {
                const char* string;
                int string_len;
            };
            bool boolean;
    } base;
} json_scalar;


typedef struct json_array {
    struct json_value* elements;
    int size;
} json_array;

typedef struct json_value {
    value_type type;
    union {
        json_array array;
        json_scalar scalar;
        json_object object;
    } value;
} json_value;

typedef struct json_field {
    const char* key;
    json_value value;
} json_field;

typedef json_value json_document;

void serialize_scalar(json_scalar* scalar, char buffer[], int buffer_size);

void serialize_array(json_array* array, char buffer[], int buffer_size);

void serialize_object(json_object* object, char buffer[], int buffer_size);

void serialize_value(json_value* value, char buffer[], int buffer_size);

void serialize_document(json_document* document, char buffer[], int buffer_size);

inline json_document make_document(json_value value) {
    return value;
}

inline json_value make_object(int field_size, json_field fields[]){
    return (json_value) {
        .type = OBJECT_VALUE,
        .value.object = {
            .field = fields,
            .field_size = field_size
        }
    };
}

inline json_value make_array(int size, json_value values[]){
    return (json_value) {
        .type = ARRAY_VALUE,
        .value.array = {
            .size = size,
            .elements = values
        }
    };
}

inline json_field make_field(const char* k, json_value v){
    return (json_field) {
        .key = k,
        .value = v
    };
}

inline json_value make_number(float num){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_NUMBER,
            .base.number = num
        }
    };
}

inline json_value make_string(const char* str, int str_len){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_STRING,
            .base.string = str,
            .base.string_len = str_len
        }
    };
}

inline json_value make_boolean(bool b){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_BOOLEAN,
            .base.boolean = b
        }
    };
}

inline json_value make_null(){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_NULL
        }
    };
}

#endif