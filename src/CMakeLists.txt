project(chatterino)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Humanize REQUIRED)
find_package(LibCommuni REQUIRED)
find_package(Qtkeychain REQUIRED)
find_package(Rapidjson REQUIRED)
find_package(Serialize REQUIRED)
find_package(Settings REQUIRED)
find_package(Signals REQUIRED)
find_package(Websocketpp REQUIRED)
#find_package(WinToast REQUIRED)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false "*.hpp" "*.cpp" "*.ui" "*.qrc")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} Websocketpp)

target_link_libraries(${PROJECT_NAME}
        Communi_Core
        Communi_Model
        Communi_Util
        Settings
        WinToast
        )


target_link_libraries(${PROJECT_NAME}
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::Multimedia
        Qt5::Svg
        Qt5::Concurrent
        #qt5keychain
        Humanize::Humanize
        Rapidjson::Rapidjson
        Serialize::Serialize
        Signals::Signals
        Websocketpp::Websocketpp
        )

#target_precompile_headers(${PROJECT_NAME} PRIVATE PrecompiledHeader.hpp)