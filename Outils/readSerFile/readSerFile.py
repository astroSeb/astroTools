#-----------------------------------------------------------------------
#---- readSerFile.py : Lecture d'un fichier SER
#-----------------------------------------------------------------------

import sys
import os
import argparse
import numpy as np

import cv2

from astropy.io import fits


import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.cm as cm

pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import display




#-----------------------------------------------------------------------
#---- Affichage des metadonnees de l'entete
#-----------------------------------------------------------------------
def loadFrame(file, header, frameId):

    frame = np.array([])
    frameDimension = header['ImageWidth'] * header['ImageHeight']
    
    offset = 178 + frameId * frameDimension * header['BytesPerPixel']
    
    file.seek(offset)
    
    if header['PixelDepthPerPlane'] == 8 :
        frame=np.fromfile(file, dtype='uint8',count=frameDimension)
    else :
        frame=np.fromfile(file, dtype='uint16',count=frameDimension)
    
    frame=np.reshape( frame, (header['ImageHeight'], header['ImageWidth']) )

    
    return frame



#-----------------------------------------------------------------------
#---- Affichage des metadonnees de l'entete
#-----------------------------------------------------------------------
def dispHeader(header):

    print("INFO   : Entete SER :")
    print(" - FileID : ", header['FileID'])
    print(" - LuID : ", header['LuID'])
    print(" - ColorID : ", header['ColorID'])
    print(" - LittleEndian : ", header['LittleEndian'])
    print(" - ImageWidth : ", header['ImageWidth'])
    print(" - ImageHeight : ", header['ImageHeight'])
    print(" - PixelDepthPerPlane : ", header['PixelDepthPerPlane'])
    print(" - FrameCount : ", header['FrameCount'])
    print(" - Observer : ", header['Observer'])
    print(" - Instrument : ", header['Instrument'])
    print(" - Telescope : ", header['Telescope'])
    print(" - DateTime : ", header['DateTime'])
    print(" - DateTimeUTC : ", header['DateTimeUTC'])
    print(" - BytesPerPixel : ", header['BytesPerPixel'])
    print(" - Trail : ", header['Trail'])


#-----------------------------------------------------------------------
#---- Chargement du fichier SER
#-----------------------------------------------------------------------
def readHeader(file_path):


    """ ColorId Documentation : 
    Content:MONO= 0
    BAYER_RGGB= 8
    BAYER_GRBG= 9
    BAYER_GBRG= 10
    BAYER_BGGR= 11
    BAYER_CYYM= 16
    BAYER_YCMY= 17
    BAYER_YMCY= 18
    BAYER_MYYC= 19
    RGB= 100
    BGR= 10 """


    #---- Ouverture du fichier
    file = open(file_path, 'rb')

    #---- Lecture de l'entete
    header = {}
    FileID = np.fromfile(file, dtype='int8', count=14).tobytes().decode()
    header['FileID'] = FileID.strip()
    offset=14
    
    file.seek(offset)
    
    LuID = np.fromfile(file, dtype='uint32', count=1)[0]
    header['LuID'] = LuID
    offset += 4
    
    file.seek(offset)

    ColorID = np.fromfile(file, dtype='uint32', count=1)[0]
    header['ColorID'] = ColorID
    offset += 4
    
    file.seek(offset)
    
    LittleEndian = np.fromfile(file, dtype='uint32', count=1)[0]
    header['LittleEndian'] = LittleEndian
    offset += 4
    
    file.seek(offset)
    
    ImageWidth = np.fromfile(file, dtype='uint32', count=1)[0]
    header['ImageWidth'] = ImageWidth
    offset += 4
    
    file.seek(offset)
    
    ImageHeight = np.fromfile(file, dtype='uint32', count=1)[0]
    header['ImageHeight'] = ImageHeight
    offset += 4
    
    file.seek(offset)
    
    PixelDepthPerPlane = np.fromfile(file, dtype='uint32', count=1)[0]
    header['PixelDepthPerPlane'] = PixelDepthPerPlane
    offset += 4
    
    file.seek(offset)
    
    FrameCount = np.fromfile(file, dtype='uint32', count=1)[0]
    header['FrameCount'] = FrameCount
    offset += 4
    
    file.seek(offset)
    
    Observer = np.fromfile(file, dtype='int8', count=40).tobytes().decode()
    header['Observer'] = Observer.strip()
    offset+=40
    
    file.seek(offset)
    
    Instrument = np.fromfile(file, dtype='int8', count=40).tobytes().decode()
    header['Instrument'] = Instrument.strip()
    offset+=40
    
    file.seek(offset)
    
    Telescope = np.fromfile(file, dtype='int8', count=40).tobytes().decode()
    header['Telescope'] = Telescope.strip()
    offset+=40
    
    file.seek(offset)
    
    DateTime = np.fromfile(file, dtype='uint64', count=1)[0]
    header['DateTime'] = DateTime
    offset += 8
    
    file.seek(offset)
    
    DateTimeUTC = np.fromfile(file, dtype='uint64', count=1)[0]
    header['DateTimeUTC'] = DateTimeUTC

    file.seek(0,2)
        
    #---- Gestion des couleurs
    if (ColorID <= 19):
        NumberOfPlanes = 1
    else:
        NumberOfPlanes = 3
    
    if (PixelDepthPerPlane <= 8):
        BytesPerPixel = NumberOfPlanes
    else:
        BytesPerPixel = 2 * NumberOfPlanes
    
    header['BytesPerPixel'] = BytesPerPixel

    #---- Gestion du trail
    lengthWithoutTrail = ImageHeight * ImageWidth * FrameCount * BytesPerPixel + 178
    lengthWithTrail = ImageHeight * ImageWidth * FrameCount * BytesPerPixel + 178 + 8*FrameCount #SPECIFICATION : trail contain 1 date per frame.
    readOK = True
    trail = False
    if int(file.tell()) == int(lengthWithoutTrail) : 
        trail = False
    elif int(file.tell()) == int(lengthWithTrail) :
        trail = True
    
    header['Trail'] = trail


    #---- Affichage header
    dispHeader(header)

    #---- Fermeture du fichier
    file.close()

    return header



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
    header = readHeader(args.file_path)

    #---- Exemple chargement premiere image
    file = open(args.file_path, 'rb')
    im0 = loadFrame(file, header, 0)
    
    #---- Test type de donnees 
    if header["ColorID"] == 8:

        print("INFO   : Image 3 canaux -> dematricage")
        im_dematric = cv2.cvtColor(im0, cv2.COLOR_BAYER_RGGB2BGR)
        display.dispColor16Bit(im_dematric)
    






#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
