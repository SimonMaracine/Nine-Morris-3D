cmake_minimum_required(VERSION 3.20)

set(EXECUTABLE_NAME "encrypter")
set(CPPBLOWFISH_BUILD_TESTS OFF)

project("encrypter_helper" LANGUAGES CXX)

add_executable(${EXECUTABLE_NAME} main.cpp)

add_subdirectory("../extern/cppblowfish" build)

target_include_directories(${EXECUTABLE_NAME} PRIVATE "../extern/cppblowfish/include")
target_link_libraries(${EXECUTABLE_NAME} PRIVATE cppblowfish)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME}" "${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME}"
	)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_BINARY_DIR}/Release/${EXECUTABLE_NAME}.exe" "${PROJECT_BINARY_DIR}/${EXECUTABLE_NAME}.exe"
	)
else()
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()
