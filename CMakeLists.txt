cmake_minimum_required(VERSION 3.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/lib)
set_property(DIRECTORY PROPERTY EP_BASE ${CMAKE_BINARY_DIR}/tmp/libs)

include(FeatureSummary)
include(ExternalProject)

project(chatterino)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network Multimedia Svg Concurrent)

add_subdirectory(src)
add_subdirectory(tests)


feature_summary(WHAT ALL)