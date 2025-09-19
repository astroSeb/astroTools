//----------------------------------------------------------------------
//
//---- planetarySelector : Outil de selection d'images planetaires
//
//----------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>

#include "Util.h"
#include "Image.h"
#include "Display.h"
#include "Zncc.h"
#include "ObjectDetector.h"





//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./planetarySelector   <Repertoire des images>" << std::endl;
    std::cout << "                             <Repertoire des images selectionnees>" << std::endl;

}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //------------------------------------------------------------------
    //---- Gestion des arguments
    if ( argc != 3 )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return -1;
    }


    std::string imageDir = argv[1];
    std::string selectDir = argv[2];

    

    //------------------------------------------------------------------
    //---- Parsing du repertoire
    std::vector<std::string> vecImPath;
    if ( !astroT::parseDir(imageDir, "JPG", vecImPath) )
    {
        std::cout << "ERREUR : Unable to parse input directory " << imageDir << std::endl;
        return 1;
    }
    int nbImages = vecImPath.size();
    std::cout << "INFO   : " << nbImages << " images a traiter" << std::endl;
    //------------------------------------------------------------------



    //------------------------------------------------------------------
    //---- Calcul de la qualite des images
    std::vector<double> vecVariance;

    //----Parcours des images
    for (int indexIm = 0; indexIm < nbImages; ++indexIm)
    {
        std::string imName = astroT::getFileName(vecImPath[indexIm]);
        std::cout << "INFO   : Traitement image " << imName << std::endl;

        //---- Ouverture de l'image
        astroT::Image currentIm;
        if ( ! currentIm.load(vecImPath[indexIm]) )
        {
            std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[0] << std::endl;
            return 1;
        }
        //dispIm(currentIm, false);

        //---- Detection des objets dans l'image
        //--- Seuillage de l'image
        astroT::Image imBin = currentIm.seuillageMeanAuto();
        //dispIm(imBin, false);
        
        //--- Detection
        astroT::ObjectDetector detector;
        astroT::sBoundingBox biggestObj;
        detector.detectBiggestObj(imBin, biggestObj);
        
        //--- Calcul de la variance du canal vert dans la ROI de l'objet
        double currentVariance = currentIm.computeVarianceRoi(biggestObj);
        vecVariance.push_back(currentVariance);
        std::cout << "INFO   : Variance : " << currentVariance << std::endl;


        /* //--- Affichage DEBUG
        astroT::Image imObj = currentIm.getRoi(biggestObj);
        dispIm(imObj, false); */

        //---- Selection des images
        double seuil = 5500;
        if ( currentVariance >= seuil )
        {
            std::string newName = selectDir + "/" + imName;
            astroT::copyFile(vecImPath[indexIm].c_str(), newName.c_str());
        }

    }

    //---- Calcul de l'histogramme de la variance
    std::vector<int> histoVar = astroT::computeHisto(vecVariance, 5);

    for (std::vector<int>::iterator iVar = histoVar.begin(); iVar != histoVar.end(); ++iVar)
    {
        std::cout << *iVar << ";";
    }
    std::cout << std::endl;

    return 0;
}