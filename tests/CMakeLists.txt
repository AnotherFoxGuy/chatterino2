if (BUILD_TESTS)
    message("++ Tests enabled")
    find_package(GTest)
    enable_testing()

    set(chatterino_test_SOURCES
            src/common/UsernameSet.cpp
            src/controllers/highlights/HighlightPhrase.cpp
            )


    add_executable(chatterino-test
            ${chatterino_test_SOURCES}

            tests/src/main.cpp
            tests/src/UsernameSet.cpp
            tests/src/HighlightPhrase.cpp
            )

    target_link_libraries(chatterino-test Qt5::Core Qt5::Widgets)

    target_link_libraries(chatterino-test gtest gtest_main)

    set(BUILD_TESTS OFF)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/serialize PajladaSerialize)

    target_link_libraries(chatterino-test PajladaSerialize)

    gtest_discover_tests(chatterino-test)
endif ()