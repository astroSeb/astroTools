#!/bin/bash

#-----------------------------------------------------------------------
#
#---- Script de configuration a sourcer pour astroViewer
#
#-----------------------------------------------------------------------

EXTERNE_PERSO_ROOT=/home/sether/Externes_perso
EXTERNE_TAROT_ROOT=/home/sether/Externes_tarot

#---- libraw
export LIBRAW_INC=${EXTERNE_PERSO_ROOT}/LibRaw-0.18.2/install/include
export LIBRAW_LIB=${EXTERNE_PERSO_ROOT}/LibRaw-0.18.2/install/lib

#---- opencv
export OPENCV_INC=${EXTERNE_TAROT_ROOT}/opencv-2.4.11/install/include
export OPENCV_LIB=${EXTERNE_TAROT_ROOT}/opencv-2.4.11/install/lib

#---- tiff
export TIFF_INC=${EXTERNE_PERSO_ROOT}/tiff-3.8.0/install/include
export TIFF_LIB=${EXTERNE_PERSO_ROOT}/tiff-3.8.0/install/lib

#---- bitmap
export BMP_INC=${EXTERNE_PERSO_ROOT}/bitmap

#---- libjpeg
#paquet libjpeg-turbo-devel.x86_64


export LD_LIBRARY_PATH=${LIBRAW_LIB}:${OPENCV_LIB}:${TIFF_LIB}:$LD_LIBRARY_PATH
