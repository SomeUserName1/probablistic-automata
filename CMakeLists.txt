project(stochastic-system-minimization)

cmake_minimum_required(VERSION 3.14)

# C++ specifics
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-fopenmp -Wall -Wpedantic -std=c++20  -fno-omit-frame-pointer \
-fno-optimize-sibling-calls -Werror=implicit-function-declaration -Wshadow -Wuninitialized -Wunused-label -Wall \
-Wpedantic -Wextra  -ferror-limit=999 -Weffc++ -Weverything -Wno-c++98-compat -ftemplate-backtrace-limit=0")

# -fsanitize=undefined,address
SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer))
SET(ASAN_OPTIONS=fast_unwind_on_malloc=0)

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

find_package(Catch2 REQUIRED)
find_package(Eigen3 REQUIRED)
include(ProcessorCount)
include(CTest)
include(Catch)

ProcessorCount(N)
# N/2 processors to account for hyper-threading
add_compile_definitions(THREADS=${N}/2)

ADD_EXECUTABLE(ssm src/main.cpp
                   src/ui/TextUserInterface.cpp
                   src/model/weighted_automata/WeightedAutomatonModel.cpp
                   src/model/differential_equations/DifferentialEquationModel.cpp
                   src/NotImplementedException.cpp
                   src/model/ModelInterface.cpp
                   src/model/ConversionMethodInterface.cpp
                   src/model/ReductionMethodInterface.cpp
                   src/model/RepresentationInterface.cpp
                   src/ui/UserInterface.cpp
        )
target_compile_definitions(ssm PUBLIC TEST=0)


enable_testing()
add_executable(tests src/test/TestsMain.cpp
        src/test/CLITest.cpp
        src/test/TUITest.cpp
        src/test/WAModelTest.cpp
        src/test/WAInstanceTest.cpp
        src/test/KieferSchuetzenbergerReductionTest.cpp
        src/ui/TextUserInterface.cpp
        src/model/weighted_automata/WeightedAutomatonModel.cpp
        src/NotImplementedException.cpp
        src/model/ModelInterface.cpp
        src/model/ConversionMethodInterface.cpp
        src/model/ReductionMethodInterface.cpp
        src/model/RepresentationInterface.cpp
        src/ui/UserInterface.cpp
        )
target_link_libraries(tests Catch2::Catch2)
target_compile_definitions(tests PUBLIC TEST=1)
catch_discover_tests(tests)