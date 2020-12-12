cmake_minimum_required(VERSION 3.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR})
include(FeatureSummary)

project(Chatterino2)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network Multimedia Svg Concurrent)

#include(pmm)
#pmm(CONAN REMOTES BINCRAFTERS)

add_subdirectory(src)
add_subdirectory(tests)


feature_summary(WHAT ALL)