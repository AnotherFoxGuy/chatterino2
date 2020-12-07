include_guard(GLOBAL)

find_program(QMAKE_COMMAND qmake REQUIRED)

set(EXTERNAL_LIB_PATH "${CMAKE_BINARY_DIR}/ext-libs")

function(buildQmakePro name path)
    set(build_path "${EXTERNAL_LIB_PATH}/build/${name}")
    file(MAKE_DIRECTORY ${build_path})

    set(lib ${EXTERNAL_LIB_PATH}/${name}${CMAKE_LINK_LIBRARY_SUFFIX})

    add_custom_command(
            OUTPUT lib
            COMMAND ${QMAKE_COMMAND} -set prefix ${EXTERNAL_LIB_PATH} ${path}
            COMMAND make -j
            WORKING_DIRECTORY "${build_path}"
    )

    set(${name}_LIB ${lib} PARENT_SCOPE)
endfunction()