add_library(Humanize::Humanize INTERFACE IMPORTED)
set_target_properties(Humanize::Humanize PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib/humanize/include/"
        )
