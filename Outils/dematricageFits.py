#-----------------------------------------------------------------------
#---- dematricageFits.py : Outil de dematricage d'images fits 16 bits
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
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python dematricageFits.py",
        description="Outil de dematricage d'images FITS")
    
    parser.add_argument("-i", "--im_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire contenant les image FITS (RAW)")
    
    parser.add_argument("-o", "--output_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire de sortie")
    
    parser.add_argument("-s", "--siril_format", action='store_true', required=False, default=False,
        help="(Optionnel) Formattage pour Siril")
    
    parser.add_argument("-d", "--display", action='store_true', required=False, default=False,
        help="(Optionnel) Affichage image dematricee")
    

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

        #display.dispMono16Bit(im)

        #---- Dematricage
        im_dematric = cv2.cvtColor(im, cv2.COLOR_BAYER_RGGB2BGR)
        print("DEBUG  : Taille image demosaic", im_dematric.shape)

        #---- Affichage
        if args.display:
            display.dispColor16Bit(im_dematric)

        #---- Formatage pour siril si demande
        if args.siril_format:
            im_tmp = np.zeros((im_dematric.shape[2], im_dematric.shape[0], im_dematric.shape[1]), dtype=np.uint16)
            im_tmp[0, :, :] = im_dematric[:, :, 0]
            im_tmp[1, :, :] = im_dematric[:, :, 1]
            im_tmp[2, :, :] = im_dematric[:, :, 2]
            im_dematric = copy.deepcopy(im_tmp)

        #---- Export dans une image fits 3 canneaux
        im_out_path = os.path.join(args.output_dir, "Demosaic_" + im_name)
        utils.exportImFits(im_dematric, im_out_path)
        print("INFO   : Export image dematricee :", im_out_path)









#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
