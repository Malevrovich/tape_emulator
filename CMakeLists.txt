cmake_minimum_required(VERSION 3.0)
set(CMAKE_CURRENT_BINARY_DIR, ./build)
project(yadro-test-task)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(SOURCES
    src/tape/file_tape.cpp
    src/tape/tape_utils.cpp
)

set(INCLUDES 
    includes/tape
)

enable_testing()
add_subdirectory(tests)

add_library(tape ${SOURCES})
target_include_directories(tape PRIVATE ${INCLUDES})

add_executable(generate src/generate.cpp)
target_link_libraries(generate PRIVATE tape)
target_include_directories(generate PRIVATE ${INCLUDES})

add_executable(sort src/sort.cpp)
target_link_libraries(sort PRIVATE tape)
target_include_directories(sort PRIVATE ${INCLUDES})
