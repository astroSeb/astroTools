#-----------------------------------------------------------------------
#---- displayFits.py : Outil de visualisation d'images fits 16 bits
#-----------------------------------------------------------------------

import sys
import os
import argparse
import numpy as np



pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import utils
from libPyAstroTools import display



#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python displayFits.py",
        description="Outil de visualisation d'images FITS")
    
    parser.add_argument("-i", "--im_path", type=str, required=True, default=None,
        help="(Obligatoire) Chemin image FITS")
    
    args = parser.parse_args()
    
    
    #---- Chargement de l'image
    im, nb_channel = utils.loadImFits(args.im_path)

    #---- Affichage
    if nb_channel == 1:
        display.dispMono16Bit(im)
        
    elif nb_channel == 3:
        display.dispColor16Bit(im)
        
    else:
        print("ERREUR : Nombre de canal", nb_channel, "inconnu")
        sys.exit(2)







#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
