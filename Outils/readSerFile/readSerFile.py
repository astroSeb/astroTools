#-----------------------------------------------------------------------
#---- readSerFile.py : Lecture d'un fichier SER
#-----------------------------------------------------------------------

import sys
import os
import argparse

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
    parser = argparse.ArgumentParser(prog="python readSerFile.py",
        description="Lecture d'un fichier SER")
    
    parser.add_argument("-i", "--file_path", type=str, required=True, default=None,
        help="(Obligatoire) Chemin vers le fichier SER")
    
    args = parser.parse_args()
    
    
    #---- Chargement du fichier SER
    header = utils.readSerHeader(args.file_path)

    #---- Exemple chargement premiere image
    file = open(args.file_path, 'rb')
    im0 = utils.loadSerFrame(file, header, 0)
    
    #---- Test type de donnees 
    if header["ColorID"] == 8:

        print("INFO   : Image 3 canaux -> dematricage")
        im_dematric = cv2.cvtColor(im0, cv2.COLOR_BAYER_RGGB2BGR)
        display.dispColor16Bit(im_dematric)
    






#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
