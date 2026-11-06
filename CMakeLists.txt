cmake_minimum_required(VERSION 3.16)
project(chess_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)          # C++20: <bit> gives us std::popcount / std::countr_zero
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Engine core as a library, so the executable and the tests share the same code.
add_library(engine
  src/core/bitboard.cpp
  src/core/board.cpp
  src/core/move.cpp
  src/movegen/attacks.cpp
)
target_include_directories(engine PUBLIC src)

add_executable(chess src/main.cpp)
target_link_libraries(chess PRIVATE engine)

enable_testing()
add_executable(test_board tests/test_board.cpp)
target_link_libraries(test_board PRIVATE engine)
add_test(NAME board COMMAND test_board)

add_executable(test_make_move tests/test_make_move.cpp)
target_link_libraries(test_make_move PRIVATE engine)
add_test(NAME make_move COMMAND test_make_move)

add_executable(test_attacks tests/test_attacks.cpp)
target_link_libraries(test_attacks PRIVATE engine)
add_test(NAME attacks COMMAND test_attacks)
