set(srcs src/embeddedjson.c)
set(incl include)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${incl})
