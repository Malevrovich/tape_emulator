#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#include <string>
#include <iostream>

#define TEST(name) void test_##name()

#define RUN_TEST(name)                      \
{                                           \
    std::cout << "------------------------" << std::endl;   \
    std::cout << "Running test " #name "..." << std::endl;  \
    test_##name();                          \
    std::cout << "Success!" << std::endl;                   \
    std::cout << "------------------------" << std::endl;   \
}

#endif
