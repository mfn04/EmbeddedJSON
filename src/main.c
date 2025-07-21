
#include "stdint.h"
#include "stdbool.h"
#include "stdarg.h"
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "embeddedjson.h"


void app_main() {

    json_value innerarray_values[] = {
        make_number(5), 
        make_boolean(false)
    };

    json_field subobj_fields[] = {
        make_field("inner_array", 
                        make_array(2, innerarray_values)),
        make_field("health", make_number(100))
    };

    json_value arr[] = {
        make_boolean(true), 
            make_string("123.123.421.43"), 

            make_object(2, subobj_fields)
    };

    json_field baseobj[] = {
        make_field("Temperature", make_number(50)),
        make_field("Active", make_boolean(true)),
        make_field("future_field", make_null()),
        make_field("version", make_string("1.0.1")),
        make_field("previous data", make_array(3, arr))
    };

    // Create an object using functions based on stack stored fields and values, they exist for the lifetime of the main function
    json_value o1 = make_object(5, baseobj);


    json_field data = FIELD("Hello", NUMBER(5));


    // This method is bad because it creates a temporary anonymous initialization that dies right after the semicolon
    json_value array = make_array(3, (json_value[]) {
        make_number(10),
        make_boolean(true),
        make_string("ArrayString!")
    });


    // Create JSON using MACROS
    json_object obj = JSON_OBJECT(FIELD("Test", NUMBER(5)),
                            FIELD("First_Name",STRING("HiThere")),
                            FIELD("array", ARRAY(3,&array)),
                            FIELD("Object", OBJECT(FIELD("Hi",NUMBER(5)))));

while(1){
        pretty_print(&obj,0);
        print_value(&o1,0);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}