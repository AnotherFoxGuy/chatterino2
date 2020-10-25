set(LibCommuni_FOUND TRUE)

# Core
file(GLOB_RECURSE CORE_SOURCE_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/lib/libcommuni/src/core/*.cpp"
        "${CMAKE_SOURCE_DIR}/lib/libcommuni/include/IrcCore/*.h"
        )
add_library(Communi_Core ${CORE_SOURCE_FILES})
target_link_libraries(Communi_Core PRIVATE Qt5::Core Qt5::Network)
target_include_directories(Communi_Core PUBLIC "${CMAKE_SOURCE_DIR}/lib/libcommuni/include/IrcCore/")
target_compile_definitions(Communi_Core PUBLIC IRC_STATIC)

# Model
file(GLOB_RECURSE MODEL_SOURCE_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/lib/libcommuni/src/core/*.cpp"
        "${CMAKE_SOURCE_DIR}/lib/libcommuni/include/IrcModel/*.h"
        )

add_library(Communi_Model ${MODEL_SOURCE_FILES})
target_include_directories(Communi_Model PUBLIC "${CMAKE_SOURCE_DIR}/lib/libcommuni/include/IrcModel/")
target_compile_definitions(Communi_Model PUBLIC IRC_STATIC)

# Util
file(GLOB_RECURSE UTIL_SOURCE_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/lib/libcommuni/src/core/*.cpp"
        "${CMAKE_SOURCE_DIR}/lib/libcommuni/include/IrcUtil/*.h"
        )

add_library(Communi_Util ${UTIL_SOURCE_FILES})
target_include_directories(Communi_Util PUBLIC "${CMAKE_SOURCE_DIR}/lib/libcommuni/include/IrcUtil/")
target_compile_definitions(Communi_Util PUBLIC IRC_STATIC)

