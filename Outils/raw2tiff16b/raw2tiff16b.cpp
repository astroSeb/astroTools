//----------------------------------------------------------------------
//
//---- Outil raw2tiff16b : Conversion d'une image NEF (Nikon raw) en
//                         TIFF 16 bit
//
//----------------------------------------------------------------------

#include <iostream>

#include "Image.h"



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
    Image im;
    if ( ! im.loadRaw(imPath) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << imPath << std::endl;
        return 1;
    }
    
    //---- Export
    if ( ! im.writeImTiff(imOutPath) )
    {
        std::cout << "ERREUR : Echec lors de l'export de l'image " << imOutPath << std::endl;
        return 1;
    }

    return 0;
}
