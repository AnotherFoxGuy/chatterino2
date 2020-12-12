set(Serialize_FOUND TRUE)
add_library(Serialize::Serialize INTERFACE IMPORTED)
set_target_properties(Serialize::Serialize PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib/serialize/include/"
        )