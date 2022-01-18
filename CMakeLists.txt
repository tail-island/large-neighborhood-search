cmake_minimum_required(VERSION 3.8)
project(large-neighborhood-search LANGUAGES CXX)

find_package(Boost 1.78 REQUIRED)

add_executable(large-neighborhood-search
    GreedyAlgorithm.h
    IO.h
    LargeNeighborhoodSearch.h
    Main.cpp
    Model.h
)

target_compile_features(large-neighborhood-search PRIVATE
    cxx_std_20
)

target_compile_options(large-neighborhood-search PRIVATE
    -march=native
)

target_include_directories(large-neighborhood-search PRIVATE
    "${Boost_INCLUDE_DIRS}"
)
