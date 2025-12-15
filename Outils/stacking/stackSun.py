#-----------------------------------------------------------------------------
#---- stackSun.py : Outil de stacking pour des images du Soleil
#-----------------------------------------------------------------------------

import sys
import os
import argparse
import numpy as np
import copy

import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.animation as animation

import cv2

import skimage.measure as skm


pathOfMain = os.path.dirname(os.path.abspath(__file__))
sys.path.append( os.path.join(pathOfMain, "..", "..") )

from libPyAstroTools import utils
from libPyAstroTools import display



#-----------------------------------------------------------------------
#---- Empilement des images
#-----------------------------------------------------------------------
def stackImages(ser_path, flat_path, dark_path, dx_list, dy_list, valid_list):

    #---- Chargement du header SER
    header = utils.readSerHeader(ser_path)

    #---- Ouverture du fichier
    file = open(ser_path, 'rb')

    #---- Declarations
    final_image = None
    first_image = None
    nb_image = 0

    norm = Normalize(vmin=900, vmax=30000)
    frames = []

    #---- Parcours des images
    #for i in range(header['FrameCount']):
    for i in range(len(valid_list)):

        #--- Test validite
        if valid_list[i] == 1:

            print("INFO   : Stacking image", nb_image+1, "sur", np.sum(valid_list))

            #--- Lecture image
            currentIm = utils.loadSerFrame(file, header, i)
            currentIm = currentIm.astype(np.float64)

            #--- Traitement FLAT
            if flat_path is not None:

                #-- Chargement FLAT
                im_flat, header_flat = utils.loadImFitsFloat64(flat_path)

                #-- Traitement
                currentIm /= im_flat
            
            #--- Traitement DARK
            if dark_path is not None:

                #-- Chargement DARK
                im_dark, header_dark = utils.loadImFitsFloat64(dark_path)

                #-- Traitement
                currentIm -= im_dark

            #--- Conversion 16 bit et decalalge de la dynamique
            currentIm -= (np.min(currentIm) - 1000)
            currentIm = currentIm.astype(np.uint16)

            #--- Dematricage
            im_dematric = cv2.cvtColor(currentIm, cv2.COLOR_BAYER_RGGB2BGR)

            #--- Premiere image
            if i == 0:
                final_image = copy.deepcopy(im_dematric.astype(np.float64))
                first_image = copy.deepcopy(im_dematric)

                #frames.append([plt.imshow(im_dematric[:,:,2], cmap='gray', norm=norm, animated=True)])

            else:

                translated_image = np.roll(im_dematric, (dy_list[i], dx_list[i]), axis=(0, 1))
                final_image += translated_image

                #frames.append([plt.imshow(translated_image[:,:,2], cmap='gray', norm=norm, animated=True)])

            nb_image += 1


    
    """ fig_vid = plt.figure()
    ani = animation.ArtistAnimation(fig_vid, frames, interval=50, blit=True, repeat_delay=1000)
    plt.show() """


    #---- Moyenne image finale
    final_image /= float(nb_image)
            

    #---- Fermeture fichier SER
    file.close()

    return final_image, first_image


#-----------------------------------------------------------------------
#---- Calcul des decalages
#-----------------------------------------------------------------------
def computeShift(ser_path, flat_path, dark_path, score_min):

    #---- Chargement du header SER
    header = utils.readSerHeader(ser_path)

    #---- Ouverture du fichier
    file = open(ser_path, 'rb')

    #---- Declarations
    biggestSolarSpot = None
    max_loc = None
    template = None
    dx_list = []
    dy_list = []
    score_list = []
    valid_list = []

    #---- Parcours des images
    for i in range(header['FrameCount']):

        print("INFO   : Calcul translations image", i+1, "sur", header['FrameCount'])

        #--- Lecture image
        currentIm = utils.loadSerFrame(file, header, i)
        currentIm = currentIm.astype(np.float64)
        #print("INFO   : Image brute (Min/max/type)", np.min(currentIm), np.max(currentIm), currentIm.dtype)

        #--- Traitement FLAT
        if flat_path is not None:

            #print("INFO   : Pre-traitement FLAT")

            #-- Chargement FLAT
            im_flat, header_flat = utils.loadImFitsFloat64(flat_path)
            #print("INFO   : Image FLAT (Min/max/type)", np.min(im_flat), np.max(im_flat), im_flat.dtype)
            #display.dispMono16Bit(im_flat)

            #-- Traitement
            currentIm /= im_flat
            #print("INFO   : Image pretraitee FLAT (Min/max/type)", np.min(currentIm), np.max(currentIm), currentIm.dtype)
            #display.dispMono16Bit(currentIm)
        
        #--- Traitement DARK
        if dark_path is not None:

            #print("INFO   : Pre-traitement DARK")

            #-- Chargement DARK
            im_dark, header_dark = utils.loadImFitsFloat64(dark_path)
            #print("INFO   : Image DARK (Min/max/type)", np.min(im_dark), np.max(im_dark), im_dark.dtype)

            #-- Traitement
            currentIm -= im_dark
            #print("INFO   : Image pretraitee FLAT,DARK (Min/max/type)", np.min(currentIm), np.max(currentIm), currentIm.dtype)

        #--- Conversion 16 bit et decalalge de la dynamique
        currentIm -= (np.min(currentIm) - 1000)
        currentIm = currentIm.astype(np.uint16)
        #print("INFO   : Image pretraitee FLAT,DARK 16 bit (Min/max/type)", np.min(currentIm), np.max(currentIm), currentIm.dtype)

        #--- Dematricage
        im_dematric = cv2.cvtColor(currentIm, cv2.COLOR_BAYER_RGGB2BGR)
        
        
        
        """ #---- Affichage custom debug
        print("DEBUG  : ", bgMean, bgSigma)
        norm = Normalize(vmin=900, vmax=30000)
        plt.imshow(im_dematric[:,:,2], cmap='gray', norm=norm)
        plt.show() """

        if i == 0:
            #---- Test seuillage pour detection des taches
            im_seuil = copy.deepcopy(im_dematric[:,:,2])
            bgMean, bgSigma = utils.computeBgStats(im_dematric[:,:,2])
            seuil = bgMean - 12 * bgSigma
            im_seuil[im_seuil<seuil] = 1
            im_seuil[im_seuil>=seuil] = 0
            
            #---- Detection tache la plus grande
            [labeled, ncomponents] = skm.label(im_seuil, connectivity=2, return_num=True)
            props = skm.regionprops(labeled)

            maxArea = 0
            for i in range(0,ncomponents):

                if props[i].area > maxArea:
                    maxArea = props[i].area
                    biggestSolarSpot = props[i].bbox
            
            template = im_dematric[biggestSolarSpot[0]:biggestSolarSpot[2],biggestSolarSpot[1]:biggestSolarSpot[3],2]
            #print("DEBUG  : Pos solar spot", biggestSolarSpot[1], biggestSolarSpot[0])


            """ norm = Normalize(vmin=900, vmax=30000)
            plt.imshow(im_dematric[:,:,2], cmap='gray', norm=norm)
            x_list = [biggestSolarSpot[1], biggestSolarSpot[1], biggestSolarSpot[3], biggestSolarSpot[3], biggestSolarSpot[1]]
            y_list = [biggestSolarSpot[0], biggestSolarSpot[2], biggestSolarSpot[2], biggestSolarSpot[0], biggestSolarSpot[0]]
            plt.plot(x_list, y_list)
            plt.scatter(biggestSolarSpot[1], biggestSolarSpot[0] )
            plt.show() """


        else:

            #--- Recherche de la tache
            # cv2.TM_CCOEFF cv2.TM_CCOEFF_NORMED cv2.TM_CCORR
            # cv2.TM_CCORR_NORMED cv2.TM_SQDIFF cv2.TM_SQDIFF_NORMED

            res = cv2.matchTemplate(im_dematric[:,:,2].astype(np.float32), template.astype(np.float32), cv2.TM_CCORR_NORMED)
            min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)

            #print("DEBUG  : Pos solar spot", max_loc[0], max_loc[1])
            
            dx = biggestSolarSpot[1] - max_loc[0]
            dy = biggestSolarSpot[0] - max_loc[1]
            #print("DEBUG  : Translation", dx, dy)

            dx_list.append(dx)
            dy_list.append(dy)


            """ norm = Normalize(vmin=900, vmax=30000)
            plt.imshow(im_dematric[:,:,2], cmap='gray', norm=norm)
            plt.scatter(max_loc[0], max_loc[1] )
            plt.show() """

            score_list.append(max_val)

            if max_val >= score_min:
                valid_list.append(1)
            else:
                valid_list.append(0)

            

            """ if i == 10:
                break """
            

    #---- Fermeture fichier SER
    file.close()

    return dx_list, dy_list, score_list, valid_list



#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python stackSun.py",
        description="Outil de stacking pour des images du Soleil")
    
    parser.add_argument("-i", "--input_path", type=str, required=True, default=None,
        help="(Obligatoire) Chemin vers le fichier SER")
    
    parser.add_argument("-f", "--input_flat", type=str, required=False, default=None,
        help="(Obligatoire) Chemin vers le fichier master flat (FITS 1 canal float64)")

    parser.add_argument("-d", "--input_dark", type=str, required=False, default=None,
        help="(Obligatoire) Chemin vers le fichier master dark (FITS 1 canal float64)")
    
    parser.add_argument("-o", "--output_path", type=str, required=False, default=None,
        help="(Optionnel) Chemin vers l'image stackee' (FITS 3 canaux)")
    
    args = parser.parse_args()
    
    #---- Calcul des decalages
    score_min = 0.989
    dx_list, dy_list, score_list, valid_list = computeShift(args.input_path, args.input_flat, args.input_dark, score_min)
    
    print("INFO   : Translations X (mean/std) :", np.mean(np.abs(dx_list)), np.std(np.abs(dx_list)))
    print("INFO   : Translations Y (mean/std) :", np.mean(np.abs(dy_list)), np.std(np.abs(dy_list)))


    #---- Filtrage pour les erreurs de correlation
    dx_rel_list = []
    dy_rel_list = []
    #--- Calcul des translations relatives
    for i in range(len(valid_list)):

        if i > 0 and valid_list[i] == 1:
            dx_rel_list.append(np.abs(dx_list[i] - dx_list[i-1]))
            dy_rel_list.append(np.abs(dy_list[i] - dy_list[i-1]))
        
        else:
            dx_rel_list.append(0)
            dy_rel_list.append(0)
    
    #--- Statistiques sur les translations relatives
    mean_dx_rel = np.mean(dx_rel_list)
    std_dx_rel = np.std(dx_rel_list)
    mean_dy_rel = np.mean(dy_rel_list)
    std_dy_rel = np.std(dy_rel_list)
    print("INFO   : Translations relatives X (mean/std) :", mean_dx_rel, std_dx_rel)
    print("INFO   : Translations relatives Y (mean/std) :", mean_dy_rel, std_dy_rel)

    #--- Filtrage
    for i in range(len(valid_list)):

        if dx_rel_list[i] >= mean_dx_rel + 3 * std_dx_rel or dy_rel_list[i] >= mean_dy_rel + 3 * std_dy_rel:
            valid_list[i] = 0

    print("INFO   :", np.sum(valid_list), "image valides sur", len(valid_list))


    #---- Affichage
    fig0, ax0 = plt.subplots(3, 2)

    #--- Translations
    ax0[0][0].plot(np.abs(dx_list), label="dx")
    ax0[0][0].plot(np.abs(dy_list), label="dy")
    ax0[0][0].legend()

    ax0[0][1].plot(score_list, label="score")
    ax0[0][1].legend()

    #--- Translations relatives
    ax0[1][0].plot(np.abs(dx_rel_list), label="dx relatif")
    ax0[1][0].plot(np.abs(dy_rel_list), label="dy relatif")
    ax0[1][0].legend()

    ax0[1][1].plot(score_list, label="score")
    ax0[1][1].legend()

    #--- Avec filtrage
    filtred_dx = []
    filtred_dy = []
    filtred_score = []
    for i in range(len(dx_list)):

        if valid_list[i] == 1:
            filtred_dx.append(dx_list[i])
            filtred_dy.append(dy_list[i])
            filtred_score.append(score_list[i])
    
    ax0[2][0].plot(np.abs(filtred_dx), label="dx filtered")
    ax0[2][0].plot(np.abs(filtred_dy), label="dy filtered")
    ax0[2][0].legend()

    ax0[2][1].plot(filtred_score, label="score filtered")
    ax0[2][1].legend()

    plt.show()



    #---- Stacking
    finale_image, first_image = stackImages(args.input_path, args.input_flat, args.input_dark, dx_list, dy_list, valid_list)


    fig, ax = plt.subplots(1, 2, sharex=True, sharey=True)
    norm = Normalize(vmin=900, vmax=30000)
    ax[0].imshow(first_image[:, :,2], cmap='gray', norm=norm)
    ax[1].imshow(finale_image[:,:,2], cmap='gray', norm=norm)
    plt.show()
    
    if args.output_path is not None:
        print("DEBUG : ", finale_image.dtype, np.min(finale_image), np.max(finale_image))
        cv2.imwrite(args.output_path, finale_image.astype(np.uint16))
    
    

    






#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
