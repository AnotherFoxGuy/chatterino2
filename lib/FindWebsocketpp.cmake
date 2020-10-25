set(Websocketpp_FOUND TRUE)

ExternalProject_Add(
        Websocketpp
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/websocketpp
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
)
ExternalProject_Get_property(Websocketpp INSTALL_DIR)
file(MAKE_DIRECTORY ${INSTALL_DIR})

add_library(Websocketpp::Websocketpp INTERFACE IMPORTED)
set_target_properties(Websocketpp::Websocketpp PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}"
        )