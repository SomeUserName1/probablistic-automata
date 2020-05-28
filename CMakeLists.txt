project(stochastic-system-minimization)

cmake_minimum_required(VERSION 3.14)

# C++ specifics
SET(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -fsanitize=undefined,address -fno-omit-frame-pointer \
-fno-optimize-sibling-calls -Werror=implicit-function-declaration -Wshadow -Wuninitialized -Wunused-label -Wall \
-Wpedantic -Wextra  -ferror-limit=999 -Wno-deprecated-anon-enum-enum-conversion")
#  -fsanitize=memory
# SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -O3")

SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

INCLUDE_DIRECTORIES(include)

ADD_EXECUTABLE(ssm src/main.cpp
                   src/ui/TextUserInterface.cpp
                   src/model/differential_equations/DifferentialEquationModel.cpp
                   src/model/weighted_automata/WeightedAutomatonModel.cpp
                   src/model/weighted_automata/KieferSchuetzenbergerReduction.cpp
                   src/model/weighted_automata/WeightedAutomatonInstance.cpp
        src/NotImplementedException.cpp
                   src/test/KieferSchuetzenbergerReductionTest.cpp)