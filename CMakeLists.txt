cmake_minimum_required(VERSION 3.14)

project(prefixes)

add_executable(prefixes prefixes.cpp test.cpp)

set_target_properties(prefixes PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_CLANG_TIDY "clang-tidy;-header-filter=.*;-checks=,-*,clang*,bugprone*,cppcoreguidelines*,-cppcoreguidelines-avoid-magic-numbers,misc*,modernize*,performance*,readability*,-readability-magic-numbers"
)
