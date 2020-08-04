#!/bin/bash
#set architectute
export ARCH=arm
#set cross compiller path
export CROSS_COMPILE=~/android/utility/gcc-linaro-arm-linux-gnueabihf-4.8-2013.10_linux/bin/arm-linux-gnueabihf-
#set directory for build output
export KBUILD_OUTPUT=out32
#set defconfig
make zte6750_35g_n_defconfig
#start compile
make zImage-dtb -j4 | tee out32/build.log