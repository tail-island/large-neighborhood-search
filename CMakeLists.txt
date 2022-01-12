cmake_minimum_required(VERSION 3.8)
project(large-neighborhood-search LANGUAGES CXX)

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
