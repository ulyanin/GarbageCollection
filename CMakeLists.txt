cmake_minimum_required(VERSION 3.5)
project(GarbageCollection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wstack-protector")

set(SOURCE_FILES main.cpp i_smart_object.cpp garbage_collection.cpp)
add_executable(GarbageCollection ${SOURCE_FILES})