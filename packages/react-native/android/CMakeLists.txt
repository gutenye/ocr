cmake_minimum_required(VERSION 3.10)
project(Ocr)

set (CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES "./*.cpp" "../cpp/*.cpp")

add_library(
  guten-ocr
  SHARED
  ${SOURCES}
)

include_directories(
  ../cpp
)
