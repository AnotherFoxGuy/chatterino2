set(Signals_FOUND TRUE)
add_library(Signals::Signals INTERFACE IMPORTED)
set_target_properties(Signals::Signals PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib/signals/include/"
        )