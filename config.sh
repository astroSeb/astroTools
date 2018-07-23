#!/bin/bash

#-----------------------------------------------------------------------
#
#---- Script de configuration a sourcer pour astroViewer
#
#-----------------------------------------------------------------------

EXTERNE_ROOT=/home/STN/Diver/Externes


#---- tiff
export TIFF_INC=${EXTERNE_ROOT}/tiff-3.8.0/install/include
export TIFF_LIB=${EXTERNE_ROOT}/tiff-3.8.0/install/lib

#---- opencv
export OPENCV_INC=${EXTERNE_ROOT}/opencv-2.4.13/install/include
export OPENCV_LIB=${EXTERNE_ROOT}/opencv-2.4.13/install/lib

#---- bitmap
export BMP_INC=${EXTERNE_ROOT}/bitmap

#---- libraw
export LIBRAW_INC=${EXTERNE_ROOT}/LibRaw-0.18.2/install/include
export LIBRAW_LIB=${EXTERNE_ROOT}/LibRaw-0.18.2/install/lib

#---- libjpeg
export LIBJPEG_INC=${EXTERNE_ROOT}/jpeg-6b/install/include
export LIBJPEG_LIB=${EXTERNE_ROOT}/jpeg-6b/install/lib


export LD_LIBRARY_PATH=${TIFF_LIB}:${OPENCV_LIB}:${LIBRAW_LIB}:${LIBJPEG_LIB}:$LD_LIBRARY_PATH
