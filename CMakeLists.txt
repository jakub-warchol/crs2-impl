cmake_minimum_required(VERSION 3.5)

project(CRS2 LANGUAGES C)

file(GLOB_RECURSE SRC . *.h *.c)
add_executable(CRS2 ${SRC} ${LIB})

set(CMAKE_USE_PTHREADS_INIT true)
set(CMAKE_USE_PTHREADS_INIT ON)

find_package(Threads REQUIRED)
target_link_libraries(CRS2 Threads::Threads)
