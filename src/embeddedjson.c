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

#include "embeddedjson.h"

/**
 * CONSTANTS
 */
const char nullterminator_str[] = "\0";
const size_t nullterminator_len = sizeof(nullterminator_str)-1;
const char quote_str[] = "\"";
const size_t quote_len = sizeof(quote_str)-1;
const char colon_str[] = ":";
const size_t colon_len = sizeof(colon_str)-1;
const char comma_str[] = ",";
const size_t comma_len = sizeof(comma_str)-1;

const char bool_true_str[] = "true";
const size_t bool_true_len = sizeof(bool_true_str)-1;
const char bool_false_str[] = "false";
const size_t bool_false_len = sizeof(bool_false_str)-1;

const char null_str[] = "null";
const size_t null_len = sizeof(null_str)-1;

const char opening_curly_bracket_str[] = "{";
const size_t opening_curly_bracket_len = sizeof(opening_curly_bracket_str)-1;
const char closing_curly_bracket_str[] = "}";
const size_t closing_curly_bracket_len = sizeof(closing_curly_bracket_str)-1;

const char opening_square_bracket_str[] = "[";
const size_t opening_square_bracket_len = sizeof(opening_square_bracket_str)-1;
const char closing_square_bracket_str[] = "]";
const size_t closing_square_bracket_len = sizeof(closing_square_bracket_str)-1;

ejson_err_t ej_error = EJSON_OK;

static ejson_err_t append_str_float(char buffer[], const int buffer_size, int* current_index, float value){
    if(buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    char val_str[64];
    int written = sprintf(val_str,"%.17e",value);
    if((*current_index)+written > buffer_size){
        ej_error = EJSON_ERROR_OVERFLOW;
        return ej_error;
    }
    memcpy(buffer+(*current_index), val_str, written);
    *current_index += written;
    ej_error = EJSON_OK;
    return ej_error;
}

static ejson_err_t append_str_str(char buffer[], const int buffer_size, int* current_index, const char* str, int str_len){
    if(buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    if((*current_index)+(sizeof(char)*str_len) > buffer_size){
        ej_error = EJSON_ERROR_OVERFLOW;
        return ej_error;
    }
    memcpy(buffer+(*current_index),str,str_len);
    *current_index += str_len;
    ej_error = EJSON_OK;
    return ej_error;
}

static ejson_err_t ejson_serialize_internal_scalar(ejson_scalar* scalar, char buffer[], const int buffer_size, int* current_index){
    if(scalar == NULL || buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    switch(scalar->base_type){
        case TYPE_NUMBER: {
            EJSON_ERROR_CHECK(append_str_float(buffer,buffer_size,current_index,scalar->base.number));
            break;
        }
        case TYPE_STRING: {
            EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, quote_str, quote_len));
            EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, scalar->base.string, strlen(scalar->base.string)));
            EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, quote_str, quote_len));
            break;
        }
        case TYPE_BOOLEAN: {
            if(scalar->base.boolean){
                EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, bool_true_str, bool_true_len));
            } else {
                EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, bool_false_str, bool_false_len));
            }
            break;
        }
        case TYPE_NULL: {
            EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, null_str, null_len));
            break;
        }
        default:
        break;
    }
    return ej_error;
}

// Forward declaration
static ejson_err_t ejson_serialize_internal_value(ejson_value* value, char buffer[], const int buffer_size, int* current_index);

static ejson_err_t ejson_serialize_internal_object(ejson_object* object, char buffer[], const int buffer_size, int* current_index){
    if(object == NULL || buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    append_str_str(buffer,buffer_size,current_index, opening_curly_bracket_str, opening_curly_bracket_len);
    for(int i = 0; i < object->field_size; i++){
        ejson_field field = object->field[i];
        EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, quote_str, quote_len));
        EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, field.key, strlen(field.key)));
        EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, quote_str, quote_len));
        EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, colon_str, colon_len));
        EJSON_ERROR_CHECK(ejson_serialize_internal_value(&field.value,buffer,buffer_size,current_index));
        if(i < object->field_size-1 && object->field_size > 1){
            EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, comma_str, comma_len));
        }
    }
    EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, closing_curly_bracket_str, closing_curly_bracket_len));
    ej_error = EJSON_OK;
    return ej_error;
}
    

static ejson_err_t ejson_serialize_internal_array(ejson_array* array, char buffer[], const int buffer_size, int* current_index){
    if(array == NULL || buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    int i = 0;
    EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, opening_square_bracket_str, opening_square_bracket_len));
    while(i < array->size){
        EJSON_ERROR_CHECK(ejson_serialize_internal_value(&array->elements[i],buffer,buffer_size,current_index));
        if(i < array->size - 1) {
            EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, comma_str, comma_len));
        }
        i++;
    }
    EJSON_ERROR_CHECK(append_str_str(buffer,buffer_size,current_index, closing_square_bracket_str, closing_square_bracket_len));
    ej_error = EJSON_OK;
    return ej_error;
}

static ejson_err_t ejson_serialize_internal_value(ejson_value* value, char buffer[], const int buffer_size, int* current_index){
    if(value == NULL || buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    switch(value->type){
        case SCALAR_VALUE: {
            ejson_scalar* scalar = &value->value.scalar;
            EJSON_ERROR_CHECK(ejson_serialize_internal_scalar(scalar,buffer,buffer_size,current_index));
            break;
        }
        case ARRAY_VALUE: {
            ejson_array* array = &value->value.array;
            EJSON_ERROR_CHECK(ejson_serialize_internal_array(array,buffer,buffer_size,current_index));
            break;
        }
        case OBJECT_VALUE: {
            ejson_object* object = &value->value.object;
            EJSON_ERROR_CHECK(ejson_serialize_internal_object(object,buffer,buffer_size,current_index));
            break;
        }
    }
    ej_error = EJSON_OK;
    return ej_error;
}

static ejson_err_t ejson_serialize_internal_document(ejson_document* document, char buffer[], const int buffer_size, int* current_index) {
    if(document == NULL || buffer == NULL || current_index == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    EJSON_ERROR_CHECK(ejson_serialize_internal_value(document,buffer,buffer_size,current_index));
    return ej_error;
}

ejson_err_t ejson_serialize_scalar(ejson_scalar* scalar, char buffer[], const int buffer_size){
    if(scalar == NULL || buffer == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    int current_index = 0;
    EJSON_ERROR_CHECK(ejson_serialize_internal_scalar(scalar,buffer,buffer_size,&current_index));
    if(current_index < buffer_size){
        buffer[current_index] = '\0';
    } else {
        buffer[buffer_size-1] = '\0';
    }
    return ej_error;
}

ejson_err_t ejson_serialize_array(ejson_array* array, char buffer[], const int buffer_size){
    if(array == NULL || buffer == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    int current_index = 0;
    EJSON_ERROR_CHECK(ejson_serialize_internal_array(array,buffer,buffer_size,&current_index));
    if(current_index < buffer_size){
        buffer[current_index] = '\0';
    } else {
        buffer[buffer_size-1] = '\0';
    }
    return ej_error;
}

ejson_err_t ejson_serialize_object(ejson_object* object, char buffer[], const int buffer_size){
    if(object == NULL || buffer == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    int current_index = 0;
    EJSON_ERROR_CHECK(ejson_serialize_internal_object(object,buffer,buffer_size,&current_index));
    if(current_index < buffer_size){
        buffer[current_index] = '\0';
    } else {
        buffer[buffer_size-1] = '\0';
    }
    return ej_error;
}

ejson_err_t ejson_serialize_value(ejson_value* value, char buffer[], const int buffer_size){
    if(value == NULL || buffer == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    int current_index = 0;
    EJSON_ERROR_CHECK(ejson_serialize_internal_value(value,buffer,buffer_size,&current_index));
    if(current_index < buffer_size){
        buffer[current_index] = '\0';
    } else {
        buffer[buffer_size-1] = '\0';
    }
    return ej_error;
}

ejson_err_t ejson_serialize_document(ejson_document* document, char buffer[], const int buffer_size){
    if(document == NULL || buffer == NULL){
        ej_error = EJSON_ERROR_INVALID_POINTER;
        return ej_error;
    }
    if(buffer_size <= 0){
        ej_error = EJSON_ERROR_INVALID_BUFFERSIZE;
        return ej_error;
    }
    int current_index = 0;
    EJSON_ERROR_CHECK(ejson_serialize_internal_document(document,buffer,buffer_size, &current_index));
    if(current_index < buffer_size){
        buffer[current_index] = '\0';
    } else {
        buffer[buffer_size-1] = '\0';
    }
    return ej_error;
}

ejson_err_t ejson_get_error(){
    return ej_error;
}