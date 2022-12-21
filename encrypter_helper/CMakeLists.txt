cmake_minimum_required(VERSION 3.20)

set(EXECUTABLE_NAME "encrypter")
set(CPPBLOWFISH_BUILD_TESTS OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project("encrypter_helper" LANGUAGES CXX)

add_executable(${EXECUTABLE_NAME} main.cpp)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE cppblowfish)

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET
            ${EXECUTABLE_NAME}
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        set_property(TARGET
            ${EXECUTABLE_NAME}
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded
        )
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    # add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD  # FIXME make this work for Linux
    #     COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME}" "../${EXECUTABLE_NAME}"
    # )
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_BINARY_DIR}/Debug/${EXECUTABLE_NAME}.exe" "${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME}.exe"
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_BINARY_DIR}/Release/${EXECUTABLE_NAME}.exe" "${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME}.exe"
        )
    endif()
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()
