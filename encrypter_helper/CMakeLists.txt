cmake_minimum_required(VERSION 3.20)

set(EXECUTABLE "encrypter_helper")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

add_executable(${EXECUTABLE} "main.cpp")

target_link_libraries(${EXECUTABLE} PRIVATE cppblowfish)

# Copy executable into a different location
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE}"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/Debug/${EXECUTABLE}.exe"
            "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE}.exe"
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/Release/${EXECUTABLE}.exe"
            "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE}.exe"
        )
    endif()
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

# On Windows set runtime library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET
        ${EXECUTABLE}
        PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET
        ${EXECUTABLE}
        PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded
    )
endif()
