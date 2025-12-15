#-----------------------------------------------------------------------
#---- compareDematricage.py : Outil de comparaison de dematricage d'images fits 16 bits
#-----------------------------------------------------------------------

import sys
import os
import argparse
import glob
import copy

import numpy as np

import cv2

import colour_demosaicing

pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..") )

from libPyAstroTools import utils
from libPyAstroTools import display









#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python compareDematricage.py",
        description="Outil de comparaison de dematricage d'images fits 16 bits")
    
    parser.add_argument("-i", "--im_path", type=str, required=True, default=None,
        help="(Obligatoire) Chemin image FITS (RAW)")
    
    parser.add_argument("-m", "--mono", action='store_true', required=False, default=False,
        help="(Optionnel) Convertion monchrome par moyenne")
    
    args = parser.parse_args()
    

    #---- Chargement image
    im_name = os.path.basename(args.im_path)
    print("INFO   : Traitement de l'image", im_name)
    im, nb_channels = utils.loadImFits(args.im_path)
    im_list = []
    #display.dispMono16Bit(im)

    #---- Dematricage openCV
    im_dematric_opencv = cv2.cvtColor(im, cv2.COLOR_BAYER_RGGB2BGR)
    print("DEBUG  : Taille image demosaic", im_dematric_opencv.shape)
    im_list.append(im_dematric_opencv)
    print("INFO   : Dematricage openCV")

    #---- Colour_demosaicing bilineaire
    im_dematric_colour_bil = colour_demosaicing.demosaicing_CFA_Bayer_bilinear(im, "BGGR")
    im_list.append(im_dematric_colour_bil)
    print("INFO   : Dematricage colour_demosaicing bilineaire")

    #---- Colour_demosaicing Malvar 2004
    im_dematric_colour_malvar = colour_demosaicing.demosaicing_CFA_Bayer_Malvar2004(im, "BGGR")
    im_list.append(im_dematric_colour_malvar)
    print("INFO   : Dematricage colour_demosaicing Malvar 2004")

    #---- Colour_demosaicing Menon 2007
    im_dematric_colour_menon = colour_demosaicing.demosaicing_CFA_Bayer_Menon2007(im, "BGGR")
    im_list.append(im_dematric_colour_menon)
    print("INFO   : Dematricage colour_demosaicing Menon 2007")

    # A tester ! demosaicing_CFA_Bayer_VNG

    #---- Affichage
    display.dispMultiColor16Bit(im_list)


    #---- Conversion monochrome si demandee
    if args.mono:

        mono_list = []
        for im_demosaic in im_list:

            im_mono = np.zeros((im_demosaic.shape[0], im_demosaic.shape[1]))
            im_mono = (im_demosaic[:, :, 0] + im_demosaic[:, :, 1] + im_demosaic[:, :, 2]) / 3.0
            im_mono = im_mono.astype(np.uint16)
            mono_list.append(im_mono)

        #---- Affichage
        display.dispMultiMono16Bit(mono_list)









#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
