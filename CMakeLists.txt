cmake_minimum_required(VERSION 3.17)
project(331MediaAritmetica)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror")
# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpic++)

set(SOURCE_FILES E331Media.cpp)
add_executable(${CMAKE_PROJECT_NAME} E331Media.cpp)
# Inclusão do GNU-GSL
target_link_libraries(331MediaAritmetica -lgsl -lgslcblas -lm)

# Rode com mpic++ E311.cpp -o E311
# depois mpirun -np 4 E311
# mpirun -np 8 -use-hwthread-cpus E311
# mpirun -np 9 -oversubscribe E311

#$ mpic++ 334gather.cpp -lgsl -lgslcblas
#$ mpirun -np 2 ./a.out