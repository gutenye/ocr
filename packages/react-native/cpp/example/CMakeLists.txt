
cmake_minimum_required(VERSION 3.10)

project(run_onnx VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)

file(GLOB SOURCES "../*.cpp")

add_executable(run_onnx run_onnx_main.cpp ${SOURCES})

include_directories(
	${CMAKE_SOURCE_DIR}/..
	${CMAKE_SOURCE_DIR}/onnxruntime/include
	${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(run_onnx PRIVATE 
	${CMAKE_SOURCE_DIR}/onnxruntime/lib/libonnxruntime.dylib
	${OpenCV_LIBS}
)