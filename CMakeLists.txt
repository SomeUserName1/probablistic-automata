project(stochastic-system-minimization CXX)

cmake_minimum_required(VERSION 3.16)

# C++ specifics
SET(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
#  -fsanitize=memory
# SET(CMAKE_C_FLAGS "-Wall -Wpedantic -std=c++20 -O3")

SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

INCLUDE_DIRECTORIES(src/ui)
INCLUDE_DIRECTORIES(src/model)
INCLUDE_DIRECTORIES(src/model/weighted_automata)
INCLUDE_DIRECTORIES(src/model/differential_equations)


ADD_LIBRARY(ui          src/ui/TextUserInterface.cpp)
ADD_LIBRARY(models      src/model/differential_equations/DifferentialEquationModel.cpp
                        src/model/weighted_automata/WeightedAutomatonModel.cpp)
ADD_LIBRARY(util        src/NotImplementedException.cpp)


ADD_EXECUTABLE(ssm src/main.cpp)
TARGET_LINK_LIBRARIES(ssm ui models util)