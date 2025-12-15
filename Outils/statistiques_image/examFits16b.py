#-----------------------------------------------------------------------
#---- examFits16b.py : Outil d'etude d'une image fits 16 bit
#-----------------------------------------------------------------------

import sys
import os
import argparse
import glob

import numpy as np
import cv2

import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
from matplotlib.widgets import  Slider
import matplotlib.cm as cm


pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import utils
from libPyAstroTools import display



#---- Variables globales
global fig
global imDisp
global image_data
global maxVal_slider
global minVal_slider


#-------------------------------------------------------------------
#---- Calcul de la dynamique 16 vers 8 bit
#-------------------------------------------------------------------
def computeDynamique(image_data, minVal, maxVal):

    im_dyn = np.clip( (image_data - minVal) * 255 / maxVal, 0, 255)

    return im_dyn


#-------------------------------------------------------------------
#---- Fonction de mise a jour de l'image par action sur le slider
#-------------------------------------------------------------------
def update(val):
    
    im_dyn = computeDynamique(image_data, minVal_slider.val, maxVal_slider.val)
    #image_process.imThreshold(image_data, seuil_slider.val)
    imDisp.set_data(im_dyn)
    fig.canvas.draw_idle()


#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python examFits16b.py",
        description="Outil d'etude d'une image fits 16 bit")
    
    parser.add_argument("-i", "--im_path", type=str, required=True, default=None,
        help="(Obligatoire) Chemin de l'image FITS (RAW)")
    
    
    args = parser.parse_args()
    
    #---- Chargement image
    im, nb_channels = utils.loadImFits(args.im_path)

    #---- Dematricage
    im_dematric = cv2.cvtColor(im, cv2.COLOR_BAYER_RGGB2BGR)
    

    #---- Affichage
    global image_data
    channel = 2
    image_data = im_dematric[:,:,channel]

    #--- Statistiques
    minAdu = np.min(image_data)
    maxAdu = np.max(image_data)
    print("INFO   : Min/max", minAdu, maxAdu, "ADU")

    im_dyn = computeDynamique(image_data, minAdu, maxAdu)


    #---- Creation de la figure et affichage de l'image
    global fig
    fig, ax = plt.subplots()
    fig.subplots_adjust(left=0.25, bottom=0.25)
    global imDisp
    imDisp = ax.imshow(im_dyn, cmap='gray')
    

    #---- Ajout d'un slider horizontal pour la valeur minimale
    axMinVal = fig.add_axes([0.25, 0.03, 0.65, 0.03])
    global minVal_slider
    minVal_slider = Slider(
        ax=axMinVal,
        label='Min value (ADU)',
        valmin=0,
        valmax=65535,
        valinit=minAdu,
    )

    #---- Ajout d'un slider horizontal pour la valeur maximale
    axMaxVal = fig.add_axes([0.25, 0.01, 0.65, 0.03])
    global maxVal_slider
    maxVal_slider = Slider(
        ax=axMaxVal,
        label='Max value (ADU)',
        valmin=0,
        valmax=65535,
        valinit=maxAdu,
    )

    #---- Liens pour l'action des slider
    maxVal_slider.on_changed(update)
    minVal_slider.on_changed(update)

    #---- Affichage general
    plt.show()



#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
