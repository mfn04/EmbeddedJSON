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

static void append_str_float(char buffer[], const int buffer_size, int* current_index, float value){
    if((*current_index)+sizeof(value) > buffer_size){
        printf("Buffer reached its maximum size, cannot insert anymore data.");
        return;
    }
    sprintf(buffer+(*current_index),"%f", value);
    *current_index += sizeof(value);
}

static void append_str_str(char buffer[], const int buffer_size, int* current_index, const char* str, int str_len){
    if((*current_index)+(sizeof(char)*str_len) > buffer_size){
        printf("Buffer reached its maximum size, cannot insert anymore data.");
        return;
    }
    sprintf(buffer+(*current_index), str);
    *current_index += str_len;
}

static void serialize_internal_scalar(json_scalar* scalar, char buffer[], const int buffer_size, int* current_index){
    if(scalar == NULL) return;
    switch(scalar->base_type){
        case TYPE_NUMBER: {
            append_str_float(buffer,buffer_size,current_index,scalar->base.number);
            break;
        }
        case TYPE_STRING: {
            const int QUOTES_STR_LEN = 2;
            if((*current_index)+(sizeof(scalar->base.string)*(scalar->base.string_len+QUOTES_STR_LEN)) > buffer_size){
                printf("Buffer reached its maximum size, cannot insert anymore data.");
                return;
            }
            append_str_str(buffer,buffer_size,current_index, quote_str, quote_len);
            append_str_str(buffer,buffer_size,current_index, scalar->base.string, strlen(scalar->base.string));
            append_str_str(buffer,buffer_size,current_index, quote_str, quote_len);
            break;
        }
        case TYPE_BOOLEAN: {
            if(scalar->base.boolean){
                append_str_str(buffer,buffer_size,current_index, bool_true_str, bool_true_len);
            } else {
                append_str_str(buffer,buffer_size,current_index, bool_false_str, bool_false_len);
            }
            break;
        }
        case TYPE_NULL: {
            append_str_str(buffer,buffer_size,current_index, null_str, null_len);
            break;
        }
        default:
        break;
    }
}

// Forward declaration
static void serialize_internal_value(json_value* value, char buffer[], const int buffer_size, int* current_index);

static void serialize_internal_object(json_object* object, char buffer[], const int buffer_size, int* current_index){
    if(object == NULL) return;
    append_str_str(buffer,buffer_size,current_index, opening_curly_bracket_str, opening_curly_bracket_len);
    for(int i = 0; i < object->field_size; i++){
        json_field field = object->field[i];
        append_str_str(buffer,buffer_size,current_index, quote_str, quote_len);
        append_str_str(buffer,buffer_size,current_index, field.key, strlen(field.key));
        append_str_str(buffer,buffer_size,current_index, quote_str, quote_len);
        append_str_str(buffer,buffer_size,current_index, colon_str, colon_len);
        serialize_internal_value(&field.value,buffer,buffer_size,current_index);
        if(i < object->field_size-1 && object->field_size > 1){
            append_str_str(buffer,buffer_size,current_index, comma_str, comma_len);
        }
    }
    append_str_str(buffer,buffer_size,current_index, closing_curly_bracket_str, closing_curly_bracket_len);
}
    

static void serialize_internal_array(json_array* array, char buffer[], const int buffer_size, int* current_index){
    if(array == NULL) return;
    int i = 0;
    append_str_str(buffer,buffer_size,current_index, opening_square_bracket_str, opening_square_bracket_len);
    while(i < array->size){
        serialize_internal_value(&array->elements[i],buffer,buffer_size,current_index);
        if(i < array->size - 1) {
            append_str_str(buffer,buffer_size,current_index, comma_str, comma_len);
        }
        i++;
    }
    append_str_str(buffer,buffer_size,current_index, closing_square_bracket_str, closing_square_bracket_len);
}

static void serialize_internal_value(json_value* value, char buffer[], const int buffer_size, int* current_index){
    if(value == NULL) return;
    switch(value->type){
        case SCALAR_VALUE: {
            json_scalar* scalar = &value->value.scalar;
            serialize_internal_scalar(scalar,buffer,buffer_size,current_index);
            break;
        }
        case ARRAY_VALUE: {
            json_array* array = &value->value.array;
            serialize_internal_array(array,buffer,buffer_size,current_index);
            break;
        }
        case OBJECT_VALUE: {
            json_object* object = &value->value.object;
            serialize_internal_object(object,buffer,buffer_size,current_index);
            break;
        }
    }
}

static void serialize_internal_document(json_document* document, char buffer[], const int buffer_size, int* current_index) {
    serialize_internal_value(document,buffer,buffer_size,current_index);
}

void serialize_scalar(json_scalar* scalar, char buffer[], const int buffer_size){
    int current_index = 0;
    serialize_internal_scalar(scalar,buffer,buffer_size,&current_index);
}

void serialize_array(json_array* array, char buffer[], const int buffer_size){
    int current_index = 0;
    serialize_internal_array(array,buffer,buffer_size,&current_index);
}

void serialize_object(json_object* object, char buffer[], const int buffer_size){
    int current_index = 0;
    serialize_internal_object(object,buffer,buffer_size,&current_index);
}

void serialize_value(json_value* value, char buffer[], const int buffer_size){
    int current_index = 0;
    serialize_internal_value(value,buffer,buffer_size,&current_index);
}

void serialize_document(json_document* document, char buffer[], const int buffer_size){
    int current_index = 0;
    serialize_internal_document(document,buffer,buffer_size,&current_index);
}