cmake_minimum_required(VERSION 3.5)
project(GarbageCollection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall\
 -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal\
 -Wconversion -Wlogical-op -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG\
 -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address\
 -fsanitize=undefined -fno-sanitize-recover -fstack-protector -DGC_LOG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(SOURCE_FILES main.cpp i_smart_object.cpp garbage_collection.cpp)
add_executable(GarbageCollection ${SOURCE_FILES})