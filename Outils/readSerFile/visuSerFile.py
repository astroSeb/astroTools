#-----------------------------------------------------------------------
#---- visuSerFile.py : Visualisation d'un fichier SER
#-----------------------------------------------------------------------

import sys
import os
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
    parser = argparse.ArgumentParser(prog="python visuSerFile.py",
        description="Visualisation d'un fichier SER")
    
    parser.add_argument("-i", "--file_path", type=str, required=True, default=None,
        help="(Obligatoire) Chemin vers le fichier SER")
    
    parser.add_argument("-o", "--output_path", type=str, required=False, default=None,
        help="(Obligatoire) Chemin vers la video exportee (.mp4)")
    
    args = parser.parse_args()
    
    #---- Declarations
    fig_vid = plt.figure()
    frame_list = []

    #---- Chargement du header du fichier SER
    header = utils.readSerHeader(args.file_path)

    #---- Parcours des frames
    file = open(args.file_path, 'rb')
    for i in range(header['FrameCount']):
        
        #--- Lecture frame
        currentIm = utils.loadSerFrame(file, header, i)

        #--- Dematricage
        im_dematric = cv2.cvtColor(currentIm, cv2.COLOR_BAYER_RGGB2BGR)

        #--- Ajustement dynamique
        if i == 0:
            norm = Normalize(vmin=np.min(im_dematric[:,:,2]), vmax=np.max(im_dematric[:,:,2]))
        
        """ plt.imshow(im_dematric[:,:,2], cmap='gray', norm=norm)
        plt.show() """

        #--- Ajout a la liste
        frame_list.append([plt.imshow(im_dematric[:,:,2], cmap='gray', norm=norm, animated=True)])


    ani = animation.ArtistAnimation(fig_vid, frame_list, interval=50, blit=True, repeat_delay=1000)
    
    if args.output_path is not None:
        ani.save(args.output_path)
    else:
        plt.show()




#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
