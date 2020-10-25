set(Rapidjson_FOUND TRUE)
add_library(Rapidjson::Rapidjson INTERFACE IMPORTED)
set_target_properties(Rapidjson::Rapidjson PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib/rapidjson/include/"
        )