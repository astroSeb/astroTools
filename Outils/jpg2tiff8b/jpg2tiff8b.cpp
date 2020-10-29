//----------------------------------------------------------------------
//
//---- Outil jpg2tiff8b : Conversion d'une image JPEG en TIFF 8 bit
//
//----------------------------------------------------------------------

#include <iostream>

#include "Image.h"



//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./jpg2tiff8b <Image JPG>" << std::endl;
    std::cout << "                    <Image TIFF>" << std::endl;
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
    if ( ! im.load(imPath) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << imPath << std::endl;
        return 1;
    }
    
    //---- Export
    if ( ! im.writeImTiff8b(imOutPath) )
    {
        std::cout << "ERREUR : Echec lors de l'export de l'image " << imOutPath << std::endl;
        return 1;
    }

    return 0;
}
