cmake_minimum_required(VERSION 3.20)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

add_executable(encrypter_helper "main.cpp")

target_link_libraries(encrypter_helper PRIVATE cppblowfish)

# Copy executable into a different location
if(UNIX)
    add_custom_command(TARGET encrypter_helper POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/encrypter_helper"
        "${CMAKE_CURRENT_SOURCE_DIR}/encrypter_helper"
    )
elseif(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET encrypter_helper POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/Debug/encrypter_helper.exe"
            "${CMAKE_CURRENT_SOURCE_DIR}/encrypter_helper.exe"
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(TARGET encrypter_helper POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/Release/encrypter_helper.exe"
            "${CMAKE_CURRENT_SOURCE_DIR}/encrypter_helper.exe"
        )
    endif()
endif()

# On Windows set runtime library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET encrypter_helper PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET encrypter_helper PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
