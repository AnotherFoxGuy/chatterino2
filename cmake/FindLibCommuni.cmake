find_path(IrcCore_INCLUDE_DIR irc.h PATH_SUFFIXES IrcCore HINTS ${CMAKE_SOURCE_DIR}/lib/libcommuni/include)
find_library(IrcCore_LIBRARY NAMES Core IrcCore HINTS ${CMAKE_SOURCE_DIR}/lib/libcommuni/lib)
message(STATUS "${IrcCore_LIBRARY}")

find_path(IrcModel_INCLUDE_DIR ircmodel.h PATH_SUFFIXES IrcModel HINTS ${CMAKE_SOURCE_DIR}/lib/libcommuni/include)
find_library(IrcModel_LIBRARY NAMES Model IrcModel HINTS ${CMAKE_SOURCE_DIR}/lib/libcommuni/lib)
message(STATUS "${IrcModel_LIBRARY}")
message(STATUS "${IrcModel_INCLUDE_DIR}")

find_path(IrcUtil_INCLUDE_DIR ircutil.h PATH_SUFFIXES IrcUtil HINTS ${CMAKE_SOURCE_DIR}/lib/libcommuni/include)
find_library(IrcUtil_LIBRARY NAMES Util IrcUtil HINTS ${CMAKE_SOURCE_DIR}/lib/libcommuni/lib)

message(STATUS "${IrcUtil_INCLUDE_DIR}")

set(LibCommuni_INCLUDE_DIRS ${IrcCore_INCLUDE_DIR} ${IrcModel_INCLUDE_DIR} ${IrcUtil_INCLUDE_DIR})
set(LibCommuni_LIBRARIES ${IrcCore_LIBRARY} ${IrcModel_LIBRARY} ${IrcUtil_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibCommuni
        DEFAULT_MSG IrcCore_LIBRARY IrcModel_LIBRARY IrcUtil_LIBRARY IrcCore_INCLUDE_DIR IrcModel_INCLUDE_DIR IrcUtil_INCLUDE_DIR
        )

if (LibCommuni_FOUND)
    add_library(LibCommuni::LibCommuni INTERFACE IMPORTED)
    set_target_properties(LibCommuni::LibCommuni PROPERTIES
            INTERFACE_LINK_LIBRARIES "${LibCommuni_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${LibCommuni_INCLUDE_DIRS}"
            )
endif ()

mark_as_advanced(LibCommuni_INCLUDE_DIRS LibCommuni_LIBRARIES
        IrcCore_LIBRARY IrcModel_LIBRARY IrcUtil_LIBRARY
        IrcCore_INCLUDE_DIR IrcModel_INCLUDE_DIR IrcUtil_INCLUDE_DIR)
