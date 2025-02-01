add_subdirectory(extern/boost EXCLUDE_FROM_ALL)

# Fixes warning
target_compile_definitions(boost_process PRIVATE "_WIN32_WINNT=0x0601")
