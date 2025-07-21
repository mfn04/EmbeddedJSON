### UNDER DEVELOPMENT

## What?
EmbeddedJSON is a minimalistic JSON builder and serializing library, it is not flexible and requires predefined JSON structures due to its being STACK only with no heap allocations. It is made simple to create JSON objects on the fly with macros or functions.

# Why?
This library was created due to the need of a simple stack only JSON builder. It was made to test the stack behavior of the embedded system with the effect of these introduced limitations on lifetimes and  No where does it claim to be fully featured serializer/deserializer. It is not dynamic either by design. Maybe future updates could introduce a dynamic builder, but no need for that right now. 