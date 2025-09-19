#-----------------------------------------------------------------------
#---- dematricageFits.py : Outil de dematricage d'images fits 16 bits
#-----------------------------------------------------------------------

import sys
import os
import argparse
import glob

import cv2

from astropy.io import fits




#-----------------------------------------------------------------------
#---- Export image fits dematricee : 3 canneaux
#-----------------------------------------------------------------------
def exportIm(im, im_path):

    hdu = fits.PrimaryHDU(im)
    hdul = fits.HDUList([hdu])
    hdul.writeto(im_path)



#-----------------------------------------------------------------------
#---- Lecture image
#-----------------------------------------------------------------------
def loadIm(filePath):

    with fits.open(filePath) as hdulist:
        image = hdulist[0].data.astype('u2')
        print("DEBUG  : taille image", image.shape)

        #print("DEBUG  : Header")
        #print(hdulist[0].header)
    
    return image


#-----------------------------------------------------------------------
#---- Main script
#-----------------------------------------------------------------------
def main():

    #---- Gestion des arguments
    parser = argparse.ArgumentParser(prog="python dematricageFits.py",
        description="Outil de dematricage d'images FITS")
    
    parser.add_argument("-i", "--im_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire contenant les image FITS (RAW)")
    
    parser.add_argument("-o", "--output_dir", type=str, required=True, default=None,
        help="(Obligatoire) Chemin du repertoire de sortie")
    
    args = parser.parse_args()
    
    #---- Recherche des images
    im_path_list = glob.glob(os.path.join(args.im_dir, "*.fit*"))
    nb_im = len(im_path_list)
    print("INFO   :", nb_im, "images a traiter")

    if nb_im == 0:
        print("ERREUR : Aucune image *.fit* trouvees")
        sys.exit(2)
    
    #---- Boucle sur les images
    for im_path in im_path_list:

        im_name = os.path.basename(im_path)
        print("INFO   : Traitement de l'image", im_name)
        #--- Chargement de l'image
        im = loadIm(im_path)

        #---- Dematricage
        im_dematric = cv2.cvtColor(im, cv2.COLOR_BAYER_RGGB2BGR)
        print("DEBUG  : Taille image demosaic", im_dematric.shape)

        #---- Export dans une image fits 3 canneaux
        im_out_path = os.path.join(args.output_dir, "Demosaic_" + im_name)
        exportIm(im_dematric, im_out_path)
        print("INFO   : Export image dematricee :", im_out_path)









#-----------------------------------------------------------------------
if __name__ =="__main__":
    main()
