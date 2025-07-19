
#include "stdint.h"
#include "stdbool.h"
#include "stdarg.h"
#include "stdio.h"
#include "freertos/FreeRTOS.h"

#define OBJECT(...) ({ json_field arr[] = {__VA_ARGS__}; (json_object) {  \
    .field_size = sizeof(arr)/sizeof(json_field), \
    .field =  arr , \
}; })

#define FIELD_NUMBER(k, v) (json_field) { \
    .key = k, \
    .value_type = TYPE_NUMBER, \
    .value = { \
        .number = v \
    } \
}
#define FIELD_STRING(k, v) (json_field) { \
    .key = k, \
    .value_type = TYPE_STRING, \
    .value = { \
        .string = v \
    } \
}
#define FIELD_OBJECT(k, v) (json_field) { \
    .key = k, \
    .value_type = TYPE_OBJECT, \
    .value = { \
        .object = v \
    } \
}
#define FIELD_ARRAY(k, v)

#define FIELD_BOOLEAN(k, v) (json_field) { \
    .key = k, \
    .value_type = TYPE_BOOLEAN, \
    .value = { \
        .boolean = v \
    } \
}
#define FIELD_NULL(k,v) (json_field) { \
    .key = k, \
    .value_type = TYPE_NULL, \
    .value = { \
        .number = v \
    } \
}

typedef enum {
    TYPE_NUMBER,
    TYPE_ARRAY,
    TYPE_BOOLEAN,
    TYPE_OBJECT,
    TYPE_STRING,
    TYPE_NULL
} field_type;

struct json_field;

typedef struct {
    int field_size;
    struct json_field* field;
} json_object;

typedef struct json_field {
    const char* key;
    field_type value_type;
    union value_t {
        uint32_t number;
        const char* string;
        json_object object;
        bool boolean;
    } value;
} json_field;

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
        printf("\"%s\":", field.key);
        switch (field.value_type)
        {
        case TYPE_STRING:
            printf("\"%s\"\n", field.value.string);
            break;
        
        case TYPE_NUMBER:
            printf("%ld\n", field.value.number);
            break;
        
        case TYPE_BOOLEAN:
            if(field.value.boolean){
                printf("true\n");
            } else {
                printf("false\n");
            }
            break;
        
        case TYPE_OBJECT:
            pretty_print(&field.value.object,tab);
            break;
        
        case TYPE_NULL:
            printf("null\n");
            break;
        default:
            break;
        }
    }
    for(int i = 0; i < start_tab; i++){
        printf("\t");
    }
    printf("}\n");
}

void app_main() {

    int* asd;

    asd = (int[]){4,5,6,7};


    json_field data = FIELD_NUMBER("Hello", 5);

    json_object object = OBJECT(
        FIELD_NUMBER("Balance", 50),
        FIELD_STRING("First_Name", "Hello"),
        FIELD_OBJECT("OBJECT",OBJECT(FIELD_NUMBER("TestSub",50))));

    while(1){
        pretty_print(&object,0);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}