cmake_minimum_required(VERSION 3.20)

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
