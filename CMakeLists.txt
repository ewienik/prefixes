cmake_minimum_required(VERSION 3.18)

project(prefixes)

add_executable(prefixes prefixes.cpp test.cpp)

set_target_properties(prefixes PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_CLANG_TIDY clang-tidy
)
