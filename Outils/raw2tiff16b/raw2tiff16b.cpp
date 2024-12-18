//----------------------------------------------------------------------
//
//---- Outil raw2tiff16b : Conversion d'une image NEF (Nikon raw) en
//                         TIFF 16 bit
//
//----------------------------------------------------------------------

#include <iostream>

#include "Image.h"
#include "Display.h"



//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./raw2tiff16b <Image RAW>" << std::endl;
    std::cout << "                     <Image TIFF>" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //---- Gestion des arguments
    if ( argc != 3 )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return 1;
    }

    std::string imPath = argv[1];
    std::string imOutPath = argv[2];

    //---- Chargement de l'image
    astroT::Image im;
    if ( ! im.loadRaw(imPath) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << imPath << std::endl;
        return 1;
    }

    //---- Affichage
    dispIm(im, false);

    
    /* //---- Export
    if ( ! im.writeImTiff16b(imOutPath) )
    {
        std::cout << "ERREUR : Echec lors de l'export de l'image " << imOutPath << std::endl;
        return 1;
    } */

    return 0;
}
