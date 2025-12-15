#-----------------------------------------------------------------------
#---- rgbFits2Mono.py : Outil de conversion d'un FITS 16b RGB en FITS
#                       16b monochrome
#-----------------------------------------------------------------------

import sys
import os
import argparse
import glob
import copy

import numpy as np

import cv2

pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..") )

from libPyAstroTools import utils
from libPyAstroTools import display



#-----------------------------------------------------------------------
#---- Conversion RGB -> Mono
#-----------------------------------------------------------------------
def rgb2mono(im):

    im_mono = np.zeros((im.shape[0], im.shape[1]))
    print("DEBUG  : im mono size", im_mono.shape)

    im_mono = (im[:, :, 0] + im[:, :, 1] + im[:, :, 2]) / 3.0
    im_mono = im_mono.astype(np.uint16)

    return im_mono


#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python rgbFits2Mono.py",
        description="Outil de conversion d'un FITS 16b RGB en FITS 16b monochrome")
    
    parser.add_argument("-i", "--im_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire contenant les image FITS (RGB)")
    
    parser.add_argument("-o", "--output_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire de sortie")
    
    parser.add_argument("-d", "--display", action='store_true', required=False, default=False,
        help="(Optionnel) Affichage image monochrome")
    

    args = parser.parse_args()
    
    #---- Recherche des images
    im_path_list = glob.glob(os.path.join(args.im_dir, "*.fit*"))
    nb_im = len(im_path_list)
    print("INFO   :", nb_im, "images a traiter")

    if nb_im == 0:
        print("ERREUR : Aucune image *.fit* trouvees")
        sys.exit(2)
    
    #---- Boucle sur les images
    for im_path in im_path_list:

        im_name = os.path.basename(im_path)
        print("INFO   : Traitement de l'image", im_name)
        #--- Chargement de l'image
        im, nb_channels = utils.loadImFits(im_path)

        if nb_channels != 3:
            print("ERREUR : L'image d'entree de comporte pas 3 cannaux")
            continue

        #---- Conversion RGB -> Mono
        im_mono = rgb2mono(im)
        #print("DEBUG  : Taille image demosaic", im_mono.shape)

        #---- Affichage
        if args.display:
            display.dispMono16Bit(im_mono)

        #---- Export dans une image fits 1 cannal
        im_out_path = os.path.join(args.output_dir, "Mono_" + im_name)
        utils.exportImFits(im_mono, im_out_path)
        print("INFO   : Export image monochrome :", im_out_path)









#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
