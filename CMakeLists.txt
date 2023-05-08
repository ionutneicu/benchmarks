cmake_minimum_required(VERSION 3.15)
project(benchmarks_boilerplate LANGUAGES CXX)
set( CMAKE_CXX_STANDARD 17 )
find_package(benchmark REQUIRED)

file(GLOB BENCHMARKS_SRC ./src/*.cpp ./src/*.h )

#add_subdirectory ( benchmark_copy_move )
add_executable( benchmarks ${BENCHMARKS_SRC} )

target_link_libraries( benchmarks benchmark::benchmark)
