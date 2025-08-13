/**
 * MIT License
 * 
 * Copyright (c) 2025 Muhanna AlNoaimi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _EMBEDDEDJSON_H_
#define _EMBEDDEDJSON_H_

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define EJ_DOCUMENT(s) _Generic((s), ejson_value: s)

#define EJ_FIELD(k, v) _Generic((k), \
    char*: _Generic((v), \
        ejson_value: (ejson_field) { \
            .key = k, \
            .value = v \
        } \
    ), \
    const char*: _Generic((v), \
        ejson_value: (ejson_field) { \
            .key = k, \
            .value = v \
        } \
    ) \
)

#define EJ_ARRAY(s,...) _Generic((s), \
    int: (ejson_value) { \
        .type = ARRAY_VALUE, \
        .value.array = { \
            .size = s, \
            .elements = (ejson_value[]) {__VA_ARGS__} \
        } \
    } \
)

#define EJ_OBJECT(s,...) _Generic((s), \
    int: (ejson_value) { \
        .type = OBJECT_VALUE, \
        .value.object = { \
            .field_size = s, \
            .field = (ejson_field[]){__VA_ARGS__} \
        } \
    } \
)

#define EJ_NUMBER(v) _Generic((v), \
    int: (ejson_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_NUMBER, \
            .base.number = v \
        } \
    } \
)

#define EJ_STRING(str,len) _Generic((str), \
    char*: (ejson_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_STRING, \
            .base.string = str, \
            .base.string_len = len \
        } \
    }, \
    const char*: (ejson_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_STRING, \
            .base.string = str, \
            .base.string_len = len \
        } \
    } \
)

#define EJ_BOOLEAN(v) _Generic((v), \
    int: (ejson_value) { \
        .type = SCALAR_VALUE, \
        .value.scalar = { \
            .base_type = TYPE_BOOLEAN, \
            .base.boolean = v \
        } \
    } \
)

#define EJ_NULL() (ejson_value) { \
    .type = SCALAR_VALUE, \
    .value.scalar = { \
        .base_type = TYPE_NULL, \
    } \
}

typedef enum ejson_err_t {
    EJSON_ERROR_OVERFLOW,
    EJSON_ERROR_INVALID_POINTER,
    EJSON_ERROR_INVALID_BUFFERSIZE,
    EJSON_OK
} ejson_err_t;

#define EJSON_ERROR_CHECK(x) \
    if(x != EJSON_OK) { \
        return x; \
    } \

typedef enum ejson_scalar_type {
    TYPE_NUMBER,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_NULL
} ejson_scalar_type;

typedef enum ejson_value_type {
    SCALAR_VALUE,
    ARRAY_VALUE,
    OBJECT_VALUE
} ejson_value_type;

typedef struct ejson_object {
    int field_size;
    struct ejson_field* field;
} ejson_object;

typedef struct ejson_scalar {
    ejson_scalar_type base_type;
    union {
            float number;
            struct {
                const char* string;
                int string_len;
            };
            bool boolean;
    } base;
} ejson_scalar;


typedef struct ejson_array {
    struct ejson_value* elements;
    int size;
} ejson_array;

typedef struct ejson_value {
    ejson_value_type type;
    union {
        ejson_array array;
        ejson_scalar scalar;
        ejson_object object;
    } value;
} ejson_value;

typedef struct ejson_field {
    const char* key;
    ejson_value value;
} ejson_field;

typedef ejson_value ejson_document;

ejson_err_t ejson_serialize_scalar(ejson_scalar* scalar, char buffer[], int buffer_size);

ejson_err_t ejson_serialize_array(ejson_array* array, char buffer[], int buffer_size);

ejson_err_t ejson_serialize_object(ejson_object* object, char buffer[], int buffer_size);

ejson_err_t ejson_serialize_value(ejson_value* value, char buffer[], int buffer_size);

ejson_err_t ejson_serialize_document(ejson_document* document, char buffer[], int buffer_size);

static inline ejson_document ejson_make_document(ejson_value value) {
    return value;
}

static inline ejson_value ejson_make_object(int field_size, ejson_field fields[]){
    return (ejson_value) {
        .type = OBJECT_VALUE,
        .value.object = {
            .field = fields,
            .field_size = field_size
        }
    };
}

static inline ejson_value ejson_make_array(int size, ejson_value values[]){
    return (ejson_value) {
        .type = ARRAY_VALUE,
        .value.array = {
            .size = size,
            .elements = values
        }
    };
}

static inline ejson_field ejson_make_field(const char* k, ejson_value v){
    return (ejson_field) {
        .key = k,
        .value = v
    };
}

static inline ejson_value ejson_make_number(float num){
    return (ejson_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_NUMBER,
            .base.number = num
        }
    };
}

static inline ejson_value ejson_make_string(const char* str, int str_len){
    return (ejson_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_STRING,
            .base.string = str,
            .base.string_len = str_len
        }
    };
}

static inline ejson_value ejson_make_boolean(bool b){
    return (ejson_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_BOOLEAN,
            .base.boolean = b
        }
    };
}

static inline ejson_value ejson_make_null(){
    return (ejson_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_NULL
        }
    };
}

ejson_err_t ejson_get_error();

#endif