#!/bin/bash

#-----------------------------------------------------------------------
#
#---- Script de configuration a sourcer pour astroTools
#
#-----------------------------------------------------------------------

ASTROTOOLS_ROOT=/home/sether/ASTRO/astroTools
EXTERNE_PERSO_ROOT=/home/sether/Externes_perso
EXTERNE_TAROT_ROOT=/home/sether/Externes_tarot
EXTERNE_ROOT=/home/sether/Externes_c

#---- libraw
export LIBRAW_INC=${EXTERNE_ROOT}/LibRaw-0.21.3/install/include
export LIBRAW_LIB=${EXTERNE_ROOT}/LibRaw-0.21.3/install/lib

#---- opencv
export OPENCV_INC=${EXTERNE_ROOT}/opencv-3.4.2/install/include
export OPENCV_LIB=${EXTERNE_ROOT}/opencv-3.4.2/install/lib

#---- tiff
export TIFF_INC=${EXTERNE_ROOT}/tiff-3.8.0/install/include
export TIFF_LIB=${EXTERNE_ROOT}/tiff-3.8.0/install/lib

#---- bitmap
export BMP_INC=${EXTERNE_ROOT}/bitmap

#---- libjpeg
#paquet libjpeg-turbo-devel.x86_64

#---- libAstroTools
export LIBASTROTOOLS_INC=${ASTROTOOLS_ROOT}/libAstroTools/include
export LIBASTROTOOLS_LIB=${ASTROTOOLS_ROOT}/libAstroTools/lib

export LD_LIBRARY_PATH=${LIBRAW_LIB}:${OPENCV_LIB}:${TIFF_LIB}:${LIBASTROTOOLS_LIB}:$LD_LIBRARY_PATH
