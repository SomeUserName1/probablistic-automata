project(stochastic-system-minimization)

cmake_minimum_required(VERSION 3.14)

# C++ specifics
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -fsanitize=undefined,address -fno-omit-frame-pointer \
-fno-optimize-sibling-calls -Werror=implicit-function-declaration -Wshadow -Wuninitialized -Wunused-label -Wall \
-Wpedantic -Wextra  -ferror-limit=999  -Weverything -Wno-c++98-compat")

#  -fsanitize=memory
# SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++20 -O3")

SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

ADD_EXECUTABLE(ssm src/main.cpp
                   src/ui/TextUserInterface.cpp
                   src/model/differential_equations/DifferentialEquationModel.cpp
                   src/model/weighted_automata/WeightedAutomatonModel.cpp
                   src/model/weighted_automata/KieferSchuetzenbergerReduction.cpp
                   src/model/weighted_automata/WeightedAutomatonInstance.cpp
                   src/NotImplementedException.cpp
                   src/model/ModelInterface.cpp
                   src/model/ConversionMethodInterface.cpp
                   src/model/ReductionMethodInterface.cpp
                   src/model/RepresentationInterface.cpp
                   src/ui/UserInterface.cpp)

find_package(Catch2 REQUIRED)
add_executable(tests src/test/TestsMain.cpp
        src/test/CLITest.cpp
        src/test/TUITest.cpp
        src/test/WAModelTest.cpp
        src/test/WAInstanceTest.cpp
        src/test/KieferSchuetzenbergerReductionTest.cpp
        src/ui/TextUserInterface.cpp
        src/model/differential_equations/DifferentialEquationModel.cpp
        src/model/weighted_automata/WeightedAutomatonModel.cpp
        src/model/weighted_automata/KieferSchuetzenbergerReduction.cpp
        src/model/weighted_automata/WeightedAutomatonInstance.cpp
        src/NotImplementedException.cpp
        src/model/ModelInterface.cpp
        src/model/ConversionMethodInterface.cpp
        src/model/ReductionMethodInterface.cpp
        src/model/RepresentationInterface.cpp
        src/ui/UserInterface.cpp)
target_link_libraries(tests Catch2::Catch2)



include(CTest)
include(Catch)
catch_discover_tests(tests)