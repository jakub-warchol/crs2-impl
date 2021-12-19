cmake_minimum_required(VERSION 3.5)

project(CRS2 LANGUAGES C)

file(GLOB_RECURSE SRC . *.h *.c)
add_executable(CRS2 ${SRC})
