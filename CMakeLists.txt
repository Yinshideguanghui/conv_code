cmake_minimum_required(VERSION 3.8.0)
project(convolutional_erasure_code)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/exec)

add_library(${PROJECT_NAME} SHARED
    src/binary_file.cpp
    src/conv_code.cpp
    include/binary_file.h
    include/conv_code.h
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(spread_error examples/spread_error.cpp)
target_include_directories(spread_error PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(spread_error ${PROJECT_NAME})

add_executable(burst_error examples/burst_error.cpp)
target_include_directories(burst_error PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(burst_error ${PROJECT_NAME})

add_executable(storage_file examples/storage_file.cpp)
target_include_directories(storage_file PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(storage_file ${PROJECT_NAME})
