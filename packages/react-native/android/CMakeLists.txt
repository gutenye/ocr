cmake_minimum_required(VERSION 3.10)
project(guten-ocr)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/build/opencv-mobile-4.9.0-android/sdk/native/jni")

set(build_DIR ${CMAKE_SOURCE_DIR}/build)

find_package(OpenCV REQUIRED)
# find_package(curl REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)
# find_package(jsoncpp REQUIRED CONFIG)



file(GLOB SOURCES "./*.cpp" "../cpp/*.cpp")

add_library(
  ${PROJECT_NAME}
  SHARED
  ${SOURCES}
)

include_directories(
  ../cpp
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/build/onnxruntime/headers
)

target_link_libraries(
  ${PROJECT_NAME} 
  ${OpenCV_LIBS}
  ${CMAKE_SOURCE_DIR}/build/onnxruntime/jni/${CMAKE_ANDROID_ARCH_ABI}/libonnxruntime.so
  # fbjni::fbjni 
)

