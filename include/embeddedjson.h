#ifndef _EMBEDDEDJSON_H_
#define _EMBEDDEDJSON_H_

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

// TODO: Fix temporary stored anonymous initialization...
#define JSON_OBJECT(...) ({ json_field arr[] = {__VA_ARGS__}; (json_object) {  \
    .field_size = sizeof(arr)/sizeof(json_field), \
    .field =  arr , \
}; })

#define FIELD(k, v) (json_field) { \
    .key = k, \
    .value = v \
}

#define ARRAY(s,v) (json_value) { \
    .type = ARRAY_VALUE, \
    .value.array = { \
        .size = s, \
        .elements = v \
    }\
}

#define OBJECT(v) (json_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_OBJECT, \
        .base.object = JSON_OBJECT(v) \
    }\
}

#define NUMBER(v) (json_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_NUMBER, \
        .base.number = v \
    }\
}

#define STRING(v) (json_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_STRING, \
        .base.string = v \
    }\
}

#define BOOLEAN(v) (json_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_BOOLEAN, \
        .base.boolean = v \
    }\
}

#define NULL() (json_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_NULL, \
        .base.string = "null" \
    }\
}

typedef struct json_object json_object;

typedef enum {
    OBJECT,
    ARRAY,
    SCALAR
} document_type;

typedef enum scalar_type {
    TYPE_NUMBER,
    TYPE_BOOLEAN,
    TYPE_OBJECT,
    TYPE_STRING,
    TYPE_NULL
} scalar_type;

typedef enum value_type {
    SCALAR_VALUE,
    ARRAY_VALUE
} value_type;

typedef struct json_object {
    int field_size;
    struct json_field* field;
} json_object;

typedef union scalar_t {
        uint32_t number;
        const char* string;
        json_object object;
        bool boolean;
} scalar_t;

typedef struct json_scalar {
    scalar_type base_type;
    scalar_t base;
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
    } value;
} json_value;

typedef struct json_field {
    const char* key;
    json_value value;
} json_field;

typedef union {
    json_array array;
    json_object object;
    json_scalar scalar;
} json_document;

void print_scalar(json_scalar* scalar, int start_tab);

void print_array(json_array* array, int start_tab);

void print_value(json_value* value, int start_tab);

void pretty_print(json_object* object, int start_tab);

inline json_value make_object(int field_size, json_field fields[]);

inline json_value make_array(int size, json_value values[]);

inline json_field make_field(const char* k, json_value v);

inline json_value make_number(uint32_t num);

inline json_value make_string(const char* str);

inline json_value make_boolean(bool b);

inline json_value make_null();

#endif