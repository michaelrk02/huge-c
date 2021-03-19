cmake_minimum_required(VERSION 3.0)

project("huge")
add_library(${PROJECT_NAME} SHARED
    "src/huge.c"
    "src/misc.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PUBLIC "HUGE_BUILD")
target_compile_options(${PROJECT_NAME} PUBLIC "-ansi")

project("huge-test")
add_executable(${PROJECT_NAME}
    "src/test/main.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PUBLIC "-ansi")
target_link_libraries(${PROJECT_NAME} "huge")

