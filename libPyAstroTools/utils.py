#-----------------------------------------------------------------------
#
#---- utils.py : Fonctions utilitaires
#
#-----------------------------------------------------------------------

import numpy as np
from astropy.io import fits


#-----------------------------------------------------------------------
#---- Calcul moyenne ecart-type fond du ciel
#-----------------------------------------------------------------------
def computeBgStats(im, blocSize=7):
    
    #---- Nombre de bloc en ligne
    nbBlocLine = im.shape[0] // blocSize
    
    #---- Nombre de bloc en colonnes
    nbBlocCol = im.shape[1] // blocSize
    
    meanList = []
    stdList = []
    
    for y in range(nbBlocLine):
        for x in range(nbBlocCol):
            
            x0 = x*blocSize
            y0 = y*blocSize
            meanList.append(np.mean(im[y0:y0+blocSize, x0:x0+blocSize]))
            stdList.append(np.std(im[y0:y0+blocSize, x0:x0+blocSize]))
    
    bgMean = np.median(meanList)
    bgSigma = np.median(stdList)
    
    return bgMean, bgSigma



#-----------------------------------------------------------------------
#---- Lecture image fits
#-----------------------------------------------------------------------
def loadImFits(filePath):

    with fits.open(filePath) as hdulist:

        image = hdulist[0].data.astype('u2')
        print("DEBUG  : taille image", image.shape)

        if len(image.shape) == 2:
            nb_channel = 1
        else:
            nb_channel = image.shape[2]
    
    return image, nb_channel


#-----------------------------------------------------------------------
#---- Export image fits
#-----------------------------------------------------------------------
def exportImFits(im, im_path):

    hdu = fits.PrimaryHDU(im)
    hdul = fits.HDUList([hdu])
    hdul.writeto(im_path)



#-----------------------------------------------------------------------
#---- Affichage des metadonnees de l'entete
#-----------------------------------------------------------------------
def dispSerHeader(header):

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
def readSerHeader(file_path):


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
    dispSerHeader(header)

    #---- Fermeture du fichier
    file.close()

    return header



#-----------------------------------------------------------------------
#---- Chargement d'une frame SER
#-----------------------------------------------------------------------
def loadSerFrame(file, header, frameId):

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
