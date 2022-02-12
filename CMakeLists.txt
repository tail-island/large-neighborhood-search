cmake_minimum_required(VERSION 3.8)
project(large-neighborhood-search LANGUAGES CXX)

cmake_policy(SET CMP0074 NEW)

find_package(Boost 1.78 REQUIRED)

add_executable(large-neighborhood-search
    GreedyAlgorithm.h
    IO.h
    LargeNeighborhoodSearch.h
    Main.cpp
    Model.h
)

if (WIN32)
    target_compile_features(large-neighborhood-search PRIVATE
        cxx_std_23  # C++20までしか使っていないのですけど、Visual Studio 2022だと23にしないとrangeを使えませんでした。。。
    )
else()
    target_compile_features(large-neighborhood-search PRIVATE
        cxx_std_20
    )
endif()

if (WIN32)
    target_compile_options(large-neighborhood-search PRIVATE
        /arch:AVX2
    )
else()
    target_compile_options(large-neighborhood-search PRIVATE
        -march=native
    )
endif()

target_include_directories(large-neighborhood-search PRIVATE
    ${Boost_INCLUDE_DIRS}
)
