#-----------------------------------------------------------------------
#---- selectFits.py : Outil de selections d'images
#-----------------------------------------------------------------------

import sys
import os
import argparse
import numpy as np
import glob



pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import utils
from libPyAstroTools import display

import matplotlib.pyplot as plt

from matplotlib.widgets import  Button
from matplotlib.colors import Normalize


#---- Variables globales
global fig
global index
global imDisp
global im_list
global out_dir


#-----------------------------------------------------------------------
#---- Images suivante
#-----------------------------------------------------------------------
def selectIm(event):

    global index
    global imDisp

    im_name = os.path.basename(im_list[index])
    os.rename(im_list[index], os.path.join(out_dir, im_name))
    print("INFO   : Selection image", im_name)
    


#-----------------------------------------------------------------------
#---- Images suivante
#-----------------------------------------------------------------------
def nextIm(event):

    global index
    global imDisp
    index += 1
    imNext, header = utils.loadImFits(im_list[index])
    print("INFO   : Affichage", os.path.basename(im_list[index]))
    imDisp.set_data(imNext)
    fig.canvas.draw_idle()


#-----------------------------------------------------------------------
#---- Images precedente
#-----------------------------------------------------------------------
def prevIm(event):

    global index
    global imDisp
    index -= 1
    imPrev, header = utils.loadImFits(im_list[index])
    print("INFO   : Affichage", os.path.basename(im_list[index]))
    imDisp.set_data(imPrev)
    fig.canvas.draw_idle()


#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python selectFits.py",
        description="Outil de selection d'images FITS")
    
    parser.add_argument("-i", "--input_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin repertoire des image FITS")
    
    parser.add_argument("-o", "--output_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin repertoire des image FITS selectionnees")
    
    args = parser.parse_args()
    

    #---- Repertoire de sortie
    global out_dir
    out_dir = args.output_dir
    if not os.path.isdir(out_dir):
        os.mkdir(out_dir)


    #---- Recherche des images
    global im_list
    im_list = glob.glob( os.path.join(args.input_dir, "*.fit*"))
    nb_im = len(im_list)
    if nb_im == 0:
        print("ERREUR : Aucune image *.fit* trouvee")
        sys.exit(2)
    else:
        print("INFO   :", nb_im, "images *.fit* trouvees")
    
    #---- Initialisation
    global index
    index = 0
    
    #---- Interface graphique
    global fig
    fig, ax = plt.subplots()
    fig.subplots_adjust(left=0.25, bottom=0.25)
    axselect = fig.add_axes([0.59, 0.05, 0.1, 0.075])
    axprev = fig.add_axes([0.7, 0.05, 0.1, 0.075])
    axnext = fig.add_axes([0.81, 0.05, 0.1, 0.075])
    bnext = Button(axnext, 'Next')
    bnext.on_clicked(nextIm)
    bprev = Button(axprev, 'Previous')
    bprev.on_clicked(prevIm)
    bselect = Button(axselect, 'Select')
    bselect.on_clicked(selectIm)

    #---- Affichage premiere image
    #--- Chargement
    global imDisp
    currentIm, header = utils.loadImFits(im_list[index])
    print("INFO   : Affichage", os.path.basename(im_list[index]))

    #--- Ajustement de la dynamique
    bgMean, bgSigma = utils.computeBgStats(currentIm)
    vMinDyn = bgMean-6*bgSigma
    vMaxDyn = bgMean+10*bgSigma
    #vMinDyn = 10000
    #vMaxDyn = 50000
    norm = Normalize(vmin=vMinDyn, vmax=vMaxDyn)

    #--- Affichage
    imDisp = ax.imshow(currentIm, cmap='gray', norm=norm)
    plt.show()
    

        







#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
