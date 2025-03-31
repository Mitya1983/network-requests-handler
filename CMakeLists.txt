cmake_minimum_required(VERSION 3.25)

project(network-request-handler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_STATIC "Build static library" ON)
option(BUILD_DOCS "Build docs" OFF)
option(ENABLE_ASAN "Enables asan build" OFF)

if (${PROJECT_IS_TOP_LEVEL})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # using Clang
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_CXX_FLAGS "-g -g3 -glldb -ggdb -ggdb3 -O0 -Wall -Wextra -Wpedantic -Wfloat-equal -Werror")
            if (${ENABLE_ASAN})
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -shared-libasan")
            endif ()
        endif ()
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(CMAKE_CXX_FLAGS "-g0 -ggdb0 -glldb0 -O3")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_CXX_FLAGS "-g -ggdb3 -O0 -Wall -Wextra -Wpedantic -Wfloat-equal -Werror")
            if (${ENABLE_ASAN})
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
            endif ()
        endif ()
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(CMAKE_CXX_FLAGS "-g0 -ggdb0 -O3")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # using Visual Studio C++
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_CXX_FLAGS "/Zi /Od /JMC /WX")
        endif ()
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(CMAKE_CXX_FLAGS "/Ot /O2")
        endif ()
    else ()
        message(FATAL_ERROR "Compiler not supported")
    endif ()
endif ()

if (${PROJECT_IS_TOP_LEVEL})

    set(DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)
    if (NOT EXISTS ${DEPS_DIR})
        file(MAKE_DIRECTORY ${DEPS_DIR})
    endif ()

    set(OPENSSL_DIR ${DEPS_DIR}/openssl)
    set(SOCKETS_DIR ${DEPS_DIR}/sockets)
    set(MUTEX_DIR ${DEPS_DIR}/mutex)
    set(RESUMABLE_COROUTINE_DIR ${DEPS_DIR}/resumable-coroutine)
    set(UTILITY_DIR ${DEPS_DIR}/utility)

    if (NOT EXISTS ${OPENSSL_DIR})
        message(STATUS "Cloning openssl")
        execute_process(
                COMMAND git clone --branch main git@github.com:Mitya1983/openssl.git
                WORKING_DIRECTORY ${DEPS_DIR})
    else ()
        message(STATUS "Updating openssl")
        execute_process(
                COMMAND git pull
                WORKING_DIRECTORY ${OPENSSL_DIR})
    endif ()
    if (NOT EXISTS ${SOCKETS_DIR})
        message(STATUS "Cloning sockets")
        execute_process(
                COMMAND git clone --branch develop git@github.com:Mitya1983/sockets.git
                WORKING_DIRECTORY ${DEPS_DIR})
    else ()
        message(STATUS "Updating sockets")
        execute_process(
                COMMAND git pull
                WORKING_DIRECTORY ${SOCKETS_DIR})
    endif ()
    if (NOT EXISTS ${MUTEX_DIR})
        message(STATUS "Cloning mutex")
        execute_process(
                COMMAND git clone --branch develop git@github.com:Mitya1983/mutex.git
                WORKING_DIRECTORY ${DEPS_DIR})
    else ()
        message(STATUS "Updating mutex")
        execute_process(
                COMMAND git pull
                WORKING_DIRECTORY ${MUTEX_DIR})
    endif ()
    if (NOT EXISTS ${RESUMABLE_COROUTINE_DIR})
        message(STATUS "Cloning resumable-coroutine")
        execute_process(
                COMMAND git clone --branch develop git@github.com:Mitya1983/resumable-coroutine.git
                WORKING_DIRECTORY ${DEPS_DIR})
    else ()
        message(STATUS "Updating resumable-coroutine")
        execute_process(
                COMMAND git pull
                WORKING_DIRECTORY ${RESUMABLE_COROUTINE_DIR})
    endif ()
    if (NOT EXISTS ${UTILITY_DIR})
        message(STATUS "Cloning utility")
        execute_process(
                COMMAND git clone --branch develop git@github.com:Mitya1983/utility.git
                WORKING_DIRECTORY ${DEPS_DIR})
    else ()
        message(STATUS "Updating utility")
        execute_process(
                COMMAND git pull
                WORKING_DIRECTORY ${UTILITY_DIR})
    endif ()

    add_subdirectory(${OPENSSL_DIR})
    add_subdirectory(${SOCKETS_DIR})
    add_subdirectory(${MUTEX_DIR})
    add_subdirectory(${RESUMABLE_COROUTINE_DIR})
    add_subdirectory(${UTILITY_DIR})
endif ()


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/deps
        )

file(GLOB INC_FILES CONFIGURE_DEPENDS
     include/*.hpp
     include/http/*.hpp
     include/request/*.hpp
     include/response/*.hpp
     include/handler/*.hpp
     )
file(GLOB PRIVATE_INC_FILES CONFIGURE_DEPENDS
     include/*.hpp
     include/http/*.hpp
     include/request/impl/private/*hpp
     include/request/impl/*.hpp
     include/response/impl/private/*hpp
     include/response/impl/*.hpp
     include/handler/impl/*.hpp
     )
file(GLOB SRC_FILES CONFIGURE_DEPENDS
     source/*.cpp
     )


set(LIB_VERSION 2.0.0)

if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    message(STATUS "${PROJECT_NAME}: CMAKE_INSTALL_PREFIX is not set. ${CMAKE_BINARY_DIR} directory will be used")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/mt_libs")
else ()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/mt_libs")
endif ()

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(PROJECT_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME})
set(LIBRARY_INSTALL_DIR ${PROJECT_INSTALL_DIR}/${BUILD_TYPE})
message(STATUS "${PROJECT_NAME}: Library install directory is set to ${LIBRARY_INSTALL_DIR}")
set(INCLUDE_INSTALL_DIR ${LIBRARY_INSTALL_DIR}/include)
message(STATUS "${PROJECT_NAME}: Include install directory is set to ${INCLUDE_INSTALL_DIR}")

if (NOT ${PROJECT_IS_TOP_LEVEL})
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(MT_DEBUG)
    endif ()
    set(NETWORK_REQUEST_HANDLER_LIB_DIR ${LIBRARY_INSTALL_DIR} PARENT_SCOPE)
    set(NETWORK_REQUEST_HANDLER_INCLUDE_DIR ${INCLUDE_INSTALL_DIR} PARENT_SCOPE)
endif ()

if (BUILD_STATIC)
    message(STATUS "${PROJECT_NAME}: Will build static library")
    add_library(${PROJECT_NAME} STATIC)
else ()
    message(STATUS "${PROJECT_NAME}: Will build shared library")
    add_library(${PROJECT_NAME} SHARED)
endif ()

add_subdirectory(test)

if (${ENABLE_ASAN} AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    string(REPLACE "." ";" CLANG_VERSION ${CMAKE_CXX_COMPILER_VERSION})
    list(GET CLANG_VERSION 0 CLANG_VERSION_MAJOR)
    set(PATH_TO_CLANG_DIRECTORY "/usr/lib/llvm-${CLANG_VERSION_MAJOR}/lib/clang/")

    execute_process(
            COMMAND find ${PATH_TO_CLANG_DIRECTORY} -name libclang_rt.asan-x86_64.so
            OUTPUT_VARIABLE PATH_TO_CLANG_LIB_DIRECTORY
            )
    string(STRIP ${PATH_TO_CLANG_LIB_DIRECTORY} PATH_TO_CLANG_LIB_DIRECTORY)
    target_link_directories(${PROJECT_NAME} PRIVATE
                            ${PATH_TO_CLANG_LIB_DIRECTORY}
                            )
    target_link_libraries(${PROJECT_NAME} PRIVATE
                          -lclang_rt.asan-x86_64
                          )
endif ()

target_sources(
        ${PROJECT_NAME}
        PRIVATE ${SRC_FILES}
        PRIVATE ${PRIVATE_INC_FILES}
        PUBLIC ${INC_FILES}
        )

target_link_libraries(${PROJECT_NAME}
                      ${OPENSSL_LIBS}
                      sockets
                      mutex
                      resumable-coroutine
                      utility
                      )

target_link_libraries(test
                      ${OPENSSL_LIBS}
                      sockets
                      mutex
                      resumable-coroutine
                      utility
                      ${PROJECT_NAME}
                      )

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        VERSION ${LIB_VERSION}
        OUTPUT_NAME ${PROJECT_NAME}
        )