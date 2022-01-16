cmake_minimum_required(VERSION 3.5)

project(CRS2 LANGUAGES C)

file(GLOB_RECURSE SRC . *.h *.c)
add_executable(CRS2 ${SRC} ${LIB})

set(CMAKE_USE_PTHREADS_INIT true)
set(CMAKE_USE_PTHREADS_INIT ON)

find_package(Threads REQUIRED)
target_link_libraries(CRS2 Threads::Threads) # add pthread

target_link_libraries(CRS2 m) # add libmath

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
target_link_libraries(CRS2 ${MPI_C_LIBRARIES}) # add OpenMPI
