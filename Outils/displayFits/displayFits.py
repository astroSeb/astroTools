#-----------------------------------------------------------------------
#---- displayFits.py : Outil de visualisation d'images fits 16 bits
#-----------------------------------------------------------------------

import sys
import argparse
import numpy as np

from astropy.io import fits



import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.cm as cm



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
#---- Lecture image
#-----------------------------------------------------------------------
def loadIm(filePath):

    with fits.open(filePath) as hdulist:

        image = hdulist[0].data.astype('u2')
        print("DEBUG  : taille image", image.shape)

        if len(image.shape) == 2:
            nb_channel = 1
        else:
            nb_channel = image.shape[2]
    
    return image, nb_channel


#-----------------------------------------------------------------------
#---- Affichage image
#-----------------------------------------------------------------------
def dispIm(im, nb_channel):
    
    #---- Image mono canal
    if nb_channel == 1:

        print("INFO   : Affichage image mono canal")

        #---- Ajustement de la dynamique
        bgMean, bgSigma = computeBgStats(im)
        vMinDyn = bgMean-6*bgSigma
        vMaxDyn = bgMean+10*bgSigma
        norm = Normalize(vmin=vMinDyn, vmax=vMaxDyn)
        print("INFO   : Normalisation de la dynamique entre", vMinDyn, "et", vMaxDyn, "ADU")

        #---- Affichage
        fig, ax = plt.subplots()
        ax.imshow(im, cmap='gray', norm=norm)
        fig.colorbar(cm.ScalarMappable(norm=norm, cmap='gray'), ax=ax)
        plt.show()


    #---- Image 3 canals
    elif nb_channel == 3:

        print("INFO   : Affichage image multi canal")
        fig, ax = plt.subplots(1, 3, sharex=True, sharey=True)

        #--- Canal 0
        bgMean0, bgSigma0 = computeBgStats(im[:, :, 0])
        vMinDyn0 = bgMean0-6*bgSigma0
        vMaxDyn0 = bgMean0+10*bgSigma0
        norm0 = Normalize(vmin=vMinDyn0, vmax=vMaxDyn0)
        print("INFO   : Canal 0 normalisation de la dynamique entre", vMinDyn0, "et", vMaxDyn0, "ADU")
        ax[0].imshow(im[:,:,0], cmap='gray', norm=norm0)

        #--- Canal 1
        bgMean1, bgSigma1 = computeBgStats(im[:, :, 1])
        vMinDyn1 = bgMean1-6*bgSigma1
        vMaxDyn1 = bgMean1+10*bgSigma1
        norm1 = Normalize(vmin=vMinDyn1, vmax=vMaxDyn1)
        print("INFO   : Canal 1 normalisation de la dynamique entre", vMinDyn1, "et", vMaxDyn1, "ADU")
        ax[1].imshow(im[:,:,1], cmap='gray', norm=norm1)

        #--- Canal 2
        bgMean2, bgSigma2 = computeBgStats(im[:, :, 2])
        vMinDyn2 = bgMean2-6*bgSigma2
        vMaxDyn2 = bgMean2+10*bgSigma2
        norm2 = Normalize(vmin=vMinDyn2, vmax=vMaxDyn2)
        print("INFO   : Canal 2 normalisation de la dynamique entre", vMinDyn2, "et", vMaxDyn2, "ADU")
        ax[2].imshow(im[:,:,2], cmap='gray', norm=norm2)

        plt.show()


        #--- Affichage couleur
        im_color = np.zeros(im.shape, dtype='u1')
        im_color[:,:,0] = np.clip( ((im[:,:,2]-vMinDyn2) * 255) / vMaxDyn2, 0, 255 )
        im_color[:,:,1] = np.clip( ((im[:,:,1]-vMinDyn1) * 255) / vMaxDyn1, 0, 255 )
        im_color[:,:,2] = np.clip( ((im[:,:,0]-vMinDyn0) * 255) / vMaxDyn0, 0, 255 )

        plt.imshow(im_color)
        plt.show()

    #---- Erreur
    else:
        print("ERREUR : Nombre de cannaux inconnu", nb_channel)
        sys.exit(2)
    


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
    im, nb_channel = loadIm(args.im_path)

    #---- Affichage
    dispIm(im, nb_channel)






#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
