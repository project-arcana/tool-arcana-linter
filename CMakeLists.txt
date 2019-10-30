cmake_minimum_required(VERSION 3.11)
project(arcana-linter)

# ===============================================
# Global settings

# require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# folders for visual studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# ==============================================================================
# Set bin dir

if(MSVC)
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
elseif(CMAKE_BUILD_TYPE STREQUAL "")
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/Default)
else()
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})

# ===============================================
# Compile flags

set(COMMON_COMPILE_FLAGS "")

if (MSVC)
    list(APPEND COMMON_COMPILE_FLAGS
        /MP
    )
else()
    list(APPEND COMMON_COMPILE_FLAGS
        -Wall
        -Wno-unused-variable
    )
endif()

# ===============================================
# Dependencies

# std replacement
add_subdirectory(extern/clean-core)

# reflection and introspection
add_subdirectory(extern/reflector)

# multithreading
add_subdirectory(extern/task-dispatcher)

# ===============================================
# Dependencies (non-arcana)

# libclang AST parser
add_subdirectory(extern/cppast)

# ===============================================
# Main

file(GLOB_RECURSE SOURCES
    "src/arcana-linter/*.cc"
    "src/arcana-linter/*.hh"
)

# proper folder structure in VS
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# create target

add_executable(arcana-linter ${SOURCES})
target_include_directories(arcana-linter PUBLIC "src")

# set compile flags
target_compile_options(arcana-linter PUBLIC ${COMMON_COMPILE_FLAGS})

if (MSVC)
    message(ERROR "Unsupported platform")
endif()

target_link_libraries(arcana-linter PUBLIC
    clean-core
    reflector
    task-dispatcher
    cppast
    -lstdc++fs
)
