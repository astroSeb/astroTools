#-----------------------------------------------------------------------
#---- fitsSeq2video.py : Conversion sequence d'images FITS en video
#-----------------------------------------------------------------------

import sys
import os
import glob
import argparse
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.animation as animation

import cv2


pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import utils
from libPyAstroTools import display










#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python fitsSeq2video.py",
        description="Conversion sequence d'images FITS en video")
    
    parser.add_argument("-i", "--im_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire contenant les image FITS")
    
    parser.add_argument("-o", "--output_path", type=str, required=False, default=None,
        help="(Obligatoire) Chemin vers la video exportee (.mp4)")
    
    args = parser.parse_args()
    
    #---- Declarations
    fig_vid = plt.figure()
    frame_list = []

    #---- Recherche des images
    im_path_list = glob.glob(os.path.join(args.im_dir, "*.fit*"))
    nb_im = len(im_path_list)
    print("INFO   :", nb_im, "images a traiter")

    if nb_im == 0:
        print("ERREUR : Aucune image *.fit* trouvees")
        sys.exit(2)
    
    #---- Boucle sur les images
    index = 0
    for im_path in im_path_list:
        
        im_name = os.path.basename(im_path)
        print("INFO   : Traitement de l'image", im_name)

        #--- Chargement de l'image
        im, nb_channels = utils.loadImFits(im_path)

        if index == 0:

            #-- Ajustement dynamique 16 bit vers 8 bit
            bgMean, bgSigma = utils.computeBgStats(im)
            vMinDyn = bgMean-6*bgSigma
            vMaxDyn = bgMean+10*bgSigma
            norm = Normalize(vmin=vMinDyn, vmax=vMaxDyn)

        #--- Ajout a la liste
        frame_list.append([plt.imshow(im, cmap='gray', norm=norm, animated=True)])

        index+=1


    ani = animation.ArtistAnimation(fig_vid, frame_list, interval=50, blit=True, repeat_delay=1000)
    
    if args.output_path is not None:
        ani.save(args.output_path)
    else:
        plt.show()




#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
