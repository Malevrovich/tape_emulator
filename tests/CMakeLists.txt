set(INCLUDES 
    ../includes/tape
)

add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic PRIVATE tape)
target_include_directories(test_basic PRIVATE ${INCLUDES})

add_test(NAME test_basic COMMAND $<TARGET_FILE:test_basic>)

add_executable(test_methods test_methods.cpp)
target_link_libraries(test_methods PRIVATE tape)
target_include_directories(test_methods PRIVATE ${INCLUDES})

add_test(NAME test_methods COMMAND $<TARGET_FILE:test_methods>)

add_executable(test_tape_utils test_tape_utils.cpp)
target_link_libraries(test_tape_utils PRIVATE tape)
target_include_directories(test_tape_utils PRIVATE ${INCLUDES})

add_test(NAME test_tape_utils COMMAND $<TARGET_FILE:test_tape_utils>)