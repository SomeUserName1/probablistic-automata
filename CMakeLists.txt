project(stochastic-system-minimization)

cmake_minimum_required(VERSION 3.14)

# C++ specifics
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
# SET(CMAKE_CXX_COMPILER "../include/LLOV/bin/clang++")
SET(CMAKE_CXX_STANDARD 20)
#SET(CMAKE_CXX_FLAGS "-fopenmp -std=c++20 -ferror-limit=999 -ftemplate-backtrace-limit=0  -I/usr/local/include -Xclang -disable-O0-optnone -Xclang -load \
# -Xclang /home/someusername/bwsync/workspace/uni/master/include/LLOV/lib/OpenMPVerify.so")
SET(CMAKE_CXX_FLAGS "-fopenmp -std=c++20 -fPIC -march=native  -m64 -fopenmp -std=c++20 -ferror-limit=999 -ftemplate-backtrace-limit=0  -I/usr/local/include \
    -march=native -fsanitize=undefined,address -fno-omit-frame-pointer -fPIC -fno-optimize-sibling-calls -Wall \
    -Wpedantic  -Wextra -Weffc++ -Weverything -Wno-c++98-compat")

#thread , -Xclang -disable-O0-optnone -Xclang -load \
# -Xclang /home/someusername/bwsync/workspace/uni/master/include/LLOV/lib/OpenMPVerify.so
# find_package( OpenMP ) -Werror
# release flags
# SET(CMAKE_CXX_FLAGS " -I${MKLROOT}/include -O3 -mllvm -polly -mllvm \
# -polly-vectorizer=stripmine -polly-parallel -lgomp  \
# -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_intel_thread.a \
# ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lgomp -lpthread -lm -ldl")
SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer))
SET(ASAN_OPTIONS=fast_unwind_on_malloc=0)

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer))

SET(TSAN_OPTIONS ignore_noninstrumented_modules=1)
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/include/eigen/cmake)
find_package(SPQR)
find_package(Cholmod)
find_package(Catch2 REQUIRED)
find_package(Eigen3 REQUIRED)
include(ProcessorCount)
include(CTest)
include(Catch)
SET(MKLROOT /opt/intel/compilers_and_libraries_2020.1.217/linux/mkl)

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
        src/model/weighted_automata/WeightedAutomatonBenchmarks.h src/FloatingPointCompare.h src/TypeDefs.h)
target_compile_definitions(ssm PUBLIC TEST=0)
target_link_libraries(ssm  -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_intel_thread.a
 ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lgomp -lpthread -lm -ldl -lspqr -lcholmod)


enable_testing()
add_executable(tests src/test/TestsMain.cpp
        src/test/CLITest.cpp
        src/test/WATest.cpp
        src/test/KieferSchuetzenbergerReductionTest.cpp
        src/ui/TextUserInterface.cpp
        src/model/weighted_automata/WeightedAutomatonModel.cpp
        src/NotImplementedException.cpp
        src/model/ModelInterface.cpp
        src/model/ConversionMethodInterface.cpp
        src/model/ReductionMethodInterface.cpp
        src/model/RepresentationInterface.cpp
        src/ui/UserInterface.cpp
        src/test/TestUtils.h src/TypeDefs.h)
target_link_libraries(tests Catch2::Catch2 -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_intel_thread.a
        ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lgomp -lpthread -lm -ldl -lspqr -lcholmod)
target_compile_definitions(tests PUBLIC TEST=1)
catch_discover_tests(tests)