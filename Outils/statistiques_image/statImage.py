#-----------------------------------------------------------------------
#---- statImage.py : Outil de calcul des statistiques sur une liste d'images
#-----------------------------------------------------------------------

import sys
import os
import argparse
import glob

import numpy as np
import cv2

import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.cm as cm


pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import utils
from libPyAstroTools import display









#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python statImage.py",
        description="Outil de calcul des statistiques sur une liste d'images")
    
    parser.add_argument("-i", "--im_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire contenant les image FITS (RAW)")
    
    
    args = parser.parse_args()
    
    #---- Recherche des images
    im_path_list = glob.glob(os.path.join(args.im_dir, "*.fit*"))
    nb_im = len(im_path_list)
    print("INFO   :", nb_im, "images a traiter")

    if nb_im == 0:
        print("ERREUR : Aucune image *.fit* trouvees")
        sys.exit(2)
    
    #---- Boucle sur les images
    mean_list = []
    std_list = []
    min_list = []
    max_list = []
    for im_path in im_path_list:

        im_name = os.path.basename(im_path)
        print("INFO   : Traitement de l'image", im_name)
        #--- Chargement de l'image
        im, nb_channels = utils.loadImFits(im_path)

        #---- Calcul des statistiques
        mean = np.mean(im)
        std = np.std(im)
        minVal = np.min(im)
        maxVal = np.max(im)
        print("INFO   : Min/Max/Mean/Std :", minVal, maxVal, mean, std)
        mean_list.append(mean)
        std_list.append(std)
        min_list.append(minVal)
        max_list.append(maxVal)
        

    #---- Affichage des courbes*
    fig, ax = plt.subplots(2, 2)
    ax[0][0].plot(mean_list)
    ax[0][0].set_title("Moyenne")
    ax[0][1].plot(std_list)
    ax[0][1].set_title("Ecart-type")
    ax[1][0].plot(min_list)
    ax[1][0].set_title("Valeur min")
    ax[1][1].plot(max_list)
    ax[1][1].set_title("Valeur_max")

    plt.show()




#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
