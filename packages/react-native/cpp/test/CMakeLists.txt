
cmake_minimum_required(VERSION 3.10)

project(run_onnx VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)

# file(GLOB_RECURSE SOURCES "../*.cc")
# add_executable(run_onnx run_onnx_main.cc ${SOURCES})
add_executable(run_onnx run_onnx_main.cc ../run_onnx.cc)

include_directories(${CMAKE_SOURCE_DIR}/onnxruntime/include)

target_link_libraries(run_onnx PRIVATE ${CMAKE_SOURCE_DIR}/onnxruntime/lib/libonnxruntime.dylib)