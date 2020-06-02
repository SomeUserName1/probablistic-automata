#!/usr/bin/env bash

BASE_PATH = $(pwd)

function setup_arch() {
    sudo pacman -Sy cpupower
    sudo cpupower frequency-set -g performance
  sudo pacman -Syu eigen tclap catch2 cuda intel-mkl intel-tbb intel-opencl-clang curl clang base-devel opencl-nvidia \
    intel-compute-runtime lapack clang llvm lldb cpupower openmp nvidia;
    yay -Sy atlas-lapack
}

function setup_ubuntu() {
    sudo apt-get update;
    sudo apt-get upgrade;
    sudo apt-get install linux-tools-common
    sudo cpupower frequency-set -g performance
    sudo apt-get install catch2 eigen3 libtclap-dev nvidia-cuda-toolkit intel-mkl tbb intel-opencl-clang curl \
        build-essential cmake clang intel-compute-runtime opencl-nvidia lapack llvm atlas openmp
}

function setup_magma() {
  curl http://icl.utk.edu/projectsfiles/magma/downloads/magma-2.5.3.tar.gz -o magma-2.5.3.tar.gz
  tar -xvzf magma-2.5.3.tar.gz
  cd magma-2.5.3.tar.gz
  export CUDADIR=/opt/cuda
  # TODO how to find this on ubuntu;
  # TODO on arch need to setup mkl by hand as not included in the package/ ship mklvars.sh with project
  #  source ${BASE_PATH}/include/compilervars.sh intel64
  export ATLASDIR=/usr/lib
  export LAPACKDIR=/usr/lib
  cp make-inc-examples/make.inc.mkl-gcc make.inc
  sed 's/#GPU_TARGET/GPU_TARGET/g' make.inc
  make
  make install prefix=${BASE_PATH}/include/magma
  cd ..
  rm -r magma-2.5.3
}

function setup_suitesparse() {
  curl https://github.com/DrTimothyAldenDavis/SuiteSparse/archive/master.zip -o SuiteSparse.tar.gz
  tar -xvzf SuiteSparse.tar.gz
  cd SuiteSparse
  mkdir build
  cd build
  cmake ..
  make
  make install INSTALL=${BASE_PATH}/include/SuiteSparse
  cd ../..
  rm -r SuiteSparse
}

echo "This is a setup script for Linux only! When using windows you have to setup everything by hand! See Installation
.md"
printf "Please select your OS: Supported are Ubuntu and Arch Linux.\n Press (1) for Arch and (2) for ubuntu";
read OS;
echo "Disabling frequency boosting and setting CPU power govenor to performance before installing atlas"
su -c "sudo echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"
if ((  "$OS" == "1"  )); then
  setup_arch;
elif ((  "$OS" == "2"  )); then
  setup_ubuntu;
fi
# setup_magma;
setup_suitesparse;