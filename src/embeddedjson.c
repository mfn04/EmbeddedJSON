#include "embeddedjson.h"

void print_scalar(json_scalar* scalar, int start_tab){
    switch(scalar->base_type){
        case TYPE_NUMBER: {
            printf("%ld", scalar->base.number);
            break;
        }
        case TYPE_STRING: {
            printf("\"%s\"", scalar->base.string);
            break;
        }
        case TYPE_BOOLEAN: {
            if(scalar->base.boolean){
                printf("true");
            } else {
                printf("false");
            }
            break;
        }
        case TYPE_OBJECT: {
            pretty_print(&scalar->base.object,start_tab);
            break;
        }
        case TYPE_NULL: {
            printf("null");
            break;
        }
        default:
        break;
    }
}

void print_array(json_array* array, int start_tab){
    int i = 0;
    printf("[");
    while(i < array->size){
        print_value(&array->elements[i], start_tab);
        if(i < array->size - 1) {
            printf(", ");
        }
        i++;
    }
    printf("]");
}

void print_value(json_value* value, int start_tab){
    switch(value->type){
        case SCALAR_VALUE: {
            json_scalar* scalar = &value->value.scalar;
            print_scalar(scalar,start_tab);
            break;
        }
        case ARRAY_VALUE: {
            json_array* array = &value->value.array;
            print_array(array,start_tab);
            break;
        }
    }
}

void pretty_print(json_object* object, int start_tab){
    
    int tab = start_tab;
    printf("{\n");
    tab++;
    for(int i = 0; i < object->field_size; i++){
        int tab_done = 0;
        while(tab_done < tab){
            printf("\t");
            tab_done++;
        }
        json_field field = object->field[i];
        printf("\"%s\" : ", field.key);
        print_value(&field.value, tab);
        if(i < object->field_size-1 && object->field_size > 1){
            printf(",");
        }
        printf("\n");
    }
    for(int i = 0; i < start_tab; i++){
        printf("\t");
    }
    printf("}\n");
}

/*json_document make_document(document_type type) {

    switch(type){
        case OBJECT: {
            return (json_object) {
                
            };
        }
    }

}

json_document make_object_root(document_type type, json_root root) {
    return (json_document) {
        .type = type,
        .root = root
    };
}*/

inline json_value make_object(int field_size, json_field fields[]){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_OBJECT,
            .base.object = {
                .field = fields,
                .field_size = field_size
            }
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

inline json_value make_number(uint32_t num){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_NUMBER,
            .base.number = num
        }
    };
}

inline json_value make_string(const char* str){
    return (json_value) {
        .type = SCALAR_VALUE,
        .value.scalar = {
            .base_type = TYPE_STRING,
            .base.string = str
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