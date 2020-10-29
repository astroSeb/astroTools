//----------------------------------------------------------------------
//
//---- firstPlanStacker : Outil permettant le stacking du premier
//                        plan
//
//----------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

#include "Util.h"
#include "Image.h"




//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./firstPlanStacker   <Repertoire des images>" << std::endl;
    std::cout << "                            <Format des images (NEF, JPG, tif, ...)>" << std::endl;
    std::cout << "                            <Image de sortie>" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //---- Gestion des arguments
    if ( argc != 4 )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return -1;
    }
    
    std::string imageDir = argv[1];
    std::string format = argv[2];
    std::string finalImPath = argv[3];
    
    
    //---- Test sur le format des images
    if ( (format != "NEF") && (format != "JPG") && (format != "jpg") && (format != "tif") )
    {
        std::cout << "ERREUR : Le format demande " << format << " n'est pas pris en compte" << std::endl;
        return 1;
    }


    //---- Parsing du repertoire
    std::vector<std::string> vecImPath;
    if ( !astroT::parseDir(imageDir, format, vecImPath) )
    {
        std::cout << "ERROR : Unable to parse input directory " << imageDir << std::endl;
        return 1;
    }
    int nbImages = vecImPath.size();
    std::cout << "INFO   : " << nbImages << " images a traiter" << std::endl;
    
    
    //---- Ouverture de la premiere image
    astroT::Image finalImage;
    if ( ! finalImage.load(vecImPath[0]) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[0] << std::endl;
        return 1;
    }
    
    
    //---- Stacking des images
    //--- Parcours des images
    for (int indexImage = 1; indexImage < nbImages; ++indexImage)
    {
        //-- Chargement de l'image courante
        astroT::Image currentImage;
        if ( ! currentImage.load(vecImPath[indexImage]) )
        {
            std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[indexImage] << std::endl;
            return 1;
        }
        
        //-- Ajout des pixels
        finalImage.add(currentImage);
        
    }
    
    //--- Moyenne
    finalImage.div(nbImages);
    
    //---- Sauvegarde image finale
    //--- Si images raw -> export en 16 bit
    if ( finalImage.isRaw() )
    {
        if ( ! finalImage.writeImTiff16b(finalImPath) )
        {
            std::cout << "ERREUR : Impossible de sauvegarder l'image " << finalImPath << std::endl;
            return -1;
        }
    
    //--- Si image non raw -> export en 8 bit
    } else {
        if ( ! finalImage.writeImTiff8b(finalImPath) )
        {
            std::cout << "ERREUR : Impossible de sauvegarder l'image " << finalImPath << std::endl;
            return -1;
        }
        
    }
    
    
    
    return 0;
    
}
