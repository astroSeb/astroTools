//----------------------------------------------------------------------
//
//---- astroStacker : Outil de stacking d'images astro
//
//----------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

#include "Util.h"
#include "Image.h"
#include "Display.h"
#include "ObjectDetector.h"




//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./astroStacker <Repertoire des images>" << std::endl;
    std::cout << "                      <Format des images (RAW, JPG, tif, ...)>" << std::endl;
    std::cout << "                      <Image master dark>" << std::endl;
    std::cout << "                      <Image finale>" << std::endl;

}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //------------------------------------------------------------------
    //---- Gestion des arguments
    if ( argc != 5 )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return -1;
    }

    std::string imageDir = argv[1];
    std::string format = argv[2];
    std::string masterDarkPath = argv[3];
    std::string finalImPath = argv[4];
    //------------------------------------------------------------------
    

    //------------------------------------------------------------------
    //---- Test sur le format des images
    if ( (format != "RAW") && (format != "JPG") && (format != "jpg") && (format != "tif") )
    {
        std::cout << "ERREUR : Le format demande " << format << " n'est pas pris en compte" << std::endl;
        return 1;
    }
    //------------------------------------------------------------------
    

    //------------------------------------------------------------------
    //---- Parsing du repertoire
    std::vector<std::string> vecImPath;
    if ( !astroT::parseDir(imageDir, format, vecImPath) )
    {
        std::cout << "ERREUR : Unable to parse input directory " << imageDir << std::endl;
        return 1;
    }
    int nbImages = vecImPath.size();
    if ( nbImages == 0 )
    {
        std::cout << "ERREUR : Aucune image trouve" << std::endl;
        return 1;
    } else {
        std::cout << "INFO   : " << nbImages << " images a traiter" << std::endl;
    }
    //------------------------------------------------------------------


    //------------------------------------------------------------------
    //---- Chargement premiere image (ref)
    astroT::Image imRef;
    if ( !imRef.load(vecImPath[0]) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[0] << std::endl;
        return 1;
    }
    //astroT::dispIm(imRef, true);
    //------------------------------------------------------------------
    
    //---- Detection des etoiles
    astroT::ObjectDetector detect;
    std::vector<astroT::sBoundingBox> vecObj = detect.detectStar(imRef);
    std::cout << "INFO   : " << vecObj.size() << " etoiles detectees" << std::endl;
    
    astroT::dispRoi(imRef, vecObj, true);

    return 0;
}
