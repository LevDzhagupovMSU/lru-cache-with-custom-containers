cmake_minimum_required(VERSION 3.10)
project(LRUCache)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(LRUCache
    test.cpp
)

target_include_directories(LRUCache PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/containers
)

target_compile_options(LRUCache PRIVATE -O2)
