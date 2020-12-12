set(Settings_FOUND TRUE)

set(SOURCE_FILES ${CMAKE_SOURCE_DIR}/lib/settings/src/settings/settingdata.cpp
        ${CMAKE_SOURCE_DIR}/lib/settings/src/settings/settingmanager.cpp
        ${CMAKE_SOURCE_DIR}/lib/settings/src/settings/detail/realpath.cpp
        )
add_library(Settings ${SOURCE_FILES})

target_include_directories(Settings PUBLIC "${CMAKE_SOURCE_DIR}/lib/settings/include/")