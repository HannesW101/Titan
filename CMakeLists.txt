cmake_minimum_required (VERSION 3.28)

project (Titan LANGUAGES CXX VERSION 1.0.0)

# ============================================================================
# Standalone vs. consumed-as-dependency detection
# ----------------------------------------------------------------------------
# When Titan is the top-level project we build the demo executable and apply
# strict warnings. When Titan is pulled in via FetchContent/add_subdirectory by
# another project, we build ONLY the library target (Titan::Titan) and do not
# force our strict flags onto the consumer.
# ============================================================================

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(TITAN_STANDALONE ON)
else()
    set(TITAN_STANDALONE OFF)
endif()

# Titan is primarily a LIBRARY. The demo executable (src/main.cpp) is built only
# when Titan is the top-level project, and can be turned off explicitly. When
# Titan is consumed via FetchContent, only the Titan::Titan library is produced.
option(TITAN_BUILD_DEMO "Build the standalone Titan demo executable" ${TITAN_STANDALONE})

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

if (TITAN_STANDALONE)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

include(FetchContent)

# ============================================================================
# Dependencies (fetched automatically; shared with the consumer if reused)
# ----------------------------------------------------------------------------

# SFML
# Titan does not use networking, so disable the Network module. In SFML 3.1+
# the Network module pulls in MbedTLS for TLS sockets; turning it off keeps
# consumers from needing that extra system dependency.
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# nlohmann/json (JSON)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(json)

# toml++ (TOML)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(tomlplusplus)

# msgpack-c (Binary)
set(MSGPACK_USE_BOOST OFF CACHE BOOL "" FORCE)
set(MSGPACK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MSGPACK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MSGPACK_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(MSGPACK_CXX17 ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    msgpack
    GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
    GIT_TAG cpp-7.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(msgpack)

# ============================================================================
# Source files
# ----------------------------------------------------------------------------
# The library is every .cpp under src EXCEPT main.cpp (the demo entry point).
# ============================================================================

file(GLOB_RECURSE TITAN_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE TITAN_INCLUDE_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# Separate the demo entry point from the library sources.
set(TITAN_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM TITAN_SRC_FILES ${TITAN_MAIN})

if (NOT TITAN_SRC_FILES)
    message(WARNING "No Titan library source files were found")
endif()

# ============================================================================
# Library target: titan  (alias Titan::Titan)
# ----------------------------------------------------------------------------

add_library(titan STATIC ${TITAN_SRC_FILES} ${TITAN_INCLUDE_FILES})
add_library(Titan::Titan ALIAS titan)

target_compile_features(titan PUBLIC cxx_std_20)

# PUBLIC so consumers automatically get the include path and can write
#   #include "titan.hpp"
target_include_directories(titan PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
)

# PUBLIC so consumers link the same libraries transitively.
target_link_libraries(titan PUBLIC
    SFML::Audio
    SFML::System
    SFML::Window
    SFML::Graphics
    nlohmann_json::nlohmann_json
    tomlplusplus::tomlplusplus
    msgpack-cxx
)

# ============================================================================
# Strict warnings — applied to Titan's own build only, never to consumers.
# ----------------------------------------------------------------------------

if (MSVC)
    target_compile_options(titan PRIVATE /W4 /WX)
else()
    target_compile_options(titan PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror

        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference

        -Wundef
        -Wswitch-default
        -Wuninitialized
    )
endif()

# ============================================================================
# Demo executable — only when building Titan standalone.
# ----------------------------------------------------------------------------

if (TITAN_BUILD_DEMO)
    add_executable(titan_demo ${TITAN_MAIN})
    target_link_libraries(titan_demo PRIVATE Titan::Titan)

    if (MSVC)
        target_compile_options(titan_demo PRIVATE /W4 /WX)
    else()
        target_compile_options(titan_demo PRIVATE
            -Wall -Wextra -Wpedantic -Werror
            -Wshadow -Wconversion -Wsign-conversion -Wnull-dereference
            -Wundef -Wswitch-default -Wuninitialized
        )
    endif()
endif()
