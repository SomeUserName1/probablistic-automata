project(stochastic-system-minimization)

cmake_minimum_required(VERSION 3.14)

# C++ specifics
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -fsanitize=undefined,address -fno-omit-frame-pointer \
-fno-optimize-sibling-calls -Werror=implicit-function-declaration -Wshadow -Wuninitialized -Wunused-label -Wall \
-Wpedantic -Wextra  -ferror-limit=999 -Wno-deprecated-anon-enum-enum-conversion")
# -Weverything -Wdouble-promotion -Wformat=2 -Wundef -Wconversion -Wno-c++-compat \
#-Wno-c++98-compat -Werror
#  -fsanitize=memory
# SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -O3")

SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

# INCLUDE_DIRECTORIES(SYSTEM /usr/include/c++/10.1.0)
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(src/ui)
INCLUDE_DIRECTORIES(src/model)
INCLUDE_DIRECTORIES(src/model/weighted_automata)
INCLUDE_DIRECTORIES(src/model/differential_equations)


ADD_LIBRARY(ui          src/ui/TextUserInterface.cpp)

ADD_LIBRARY(models      src/model/differential_equations/DifferentialEquationModel.cpp
                        src/model/weighted_automata/WeightedAutomatonModel.cpp
                        src/model/weighted_automata/KieferSchuetzenbergerReduction.cpp
                        src/model/weighted_automata/WeightedAutomatonInstance.cpp)

ADD_LIBRARY(util src/NotImplementedException.h src/NotImplementedException.cpp)


ADD_EXECUTABLE(ssm src/main.cpp)
TARGET_LINK_LIBRARIES(ssm ui models util)