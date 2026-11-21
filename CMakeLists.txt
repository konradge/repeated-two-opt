cmake_minimum_required(VERSION 3.20)
project(repeated_two_opt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_library(twoopt
  src/instance.cpp
  src/tour.cpp
  src/naive_two_opt.cpp
  src/treap_sequence.cpp
  src/generators.cpp
)
target_include_directories(twoopt PUBLIC include)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(twoopt PUBLIC -Wall -Wextra -Wpedantic)
endif()

add_executable(two_opt apps/two_opt.cpp)
target_link_libraries(two_opt PRIVATE twoopt)

add_executable(generate apps/generate.cpp)
target_link_libraries(generate PRIVATE twoopt)

enable_testing()
foreach(name test_tour test_treap_sequence test_repeated_two_opt test_paper_example)
  add_executable(${name} tests/${name}.cpp)
  target_link_libraries(${name} PRIVATE twoopt)
  add_test(NAME ${name} COMMAND ${name})
endforeach()
