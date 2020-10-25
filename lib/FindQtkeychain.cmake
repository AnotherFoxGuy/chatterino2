set(Qtkeychain_FOUND TRUE)

ExternalProject_Add(
        Qtkeychain
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/qtkeychain
        CMAKE_ARGS
        -DQTKEYCHAIN_STATIC=ON
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
)

ExternalProject_Get_property(Qtkeychain INSTALL_DIR)
file(MAKE_DIRECTORY ${INSTALL_DIR})

add_library(Qtkeychain::Qtkeychain INTERFACE IMPORTED)
set_target_properties(Qtkeychain::Qtkeychain PROPERTIES
        INTERFACE_LINK_LIBRARIES "${INSTALL_DIR}/lib/qtkeychain.${CMAKE_STATIC_LIBRARY_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}"
        )