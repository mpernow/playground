cmake_minimum_required(VERSION 3.10)
project(move_semantics CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

add_executable(main main.cpp PixelBuffer.cpp basics.cpp elision.cpp forwarding.cpp)
target_compile_options(main PRIVATE -Wall -Wextra)
