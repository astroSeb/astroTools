//----------------------------------------------------------------------
//
//---- masterDark : Outil de creation d'un dark depuis un repertoire
//                  contenant des dark
//
//----------------------------------------------------------------------

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>

#include "Util.h"
#include "Image.h"





//----------------------------------------------------------------------
//---- /!\ Calcul du dark par mediane : A TESTER MAIS TRES LONG
//----------------------------------------------------------------------
bool computeMedianDark(const std::vector<std::string> & vecImPath, const std::string & format, Image & finalIm)
{
	/*
	//---- Chargement de la premiere image
	Image firstIm;
	firstIm.loadJpeg(vecImPath[0]);
	
	
	int nbPixels = firstIm.getNbPixel();
	
	//---- Image finale
	Image finalImage(firstIm.getXSize(), firstIm.getYSize());
	
	std::cout << "INFO   : Taille de l'image finale " << firstIm.getXSize() << ", " << firstIm.getYSize() << ", " << nbPixels << std::endl;
	
	Image currentImLine;
	std::vector<double> vecLineRed;
	std::vector<double> vecLineGreen;
	std::vector<double> vecLineBlue;
	
	std::vector<std::vector<double> > vecPixRed;
	std::vector<std::vector<double> > vecPixGreen;
	std::vector<std::vector<double> > vecPixBlue;
	
	std::vector<double> vecDataMedianRed;
	std::vector<double> vecDataMedianGreen;
	std::vector<double> vecDataMedianBlue;
	
	//---- Boucle sur les lignes
	for (int iLine = 0; iLine < firstIm.getYSize(); ++iLine)
	{
		vecPixRed.clear();
		vecPixGreen.clear();
		vecPixBlue.clear();
		
		//--- Boucle sur les images
		for (int indexIm = 0; indexIm < nbImage; ++indexIm)
		{
			vecLineRed.clear();
			vecLineGreen.clear();
			vecLineBlue.clear();
			
			currentImLine.loadJpegLine(vecImPath[indexIm], iLine);
			
			//-- Boucle sur les colonnes
			for (int iCol = 0; iCol < firstIm.getXSize(); ++iCol)
			{
				vecLineRed.push_back(currentImLine.getRedValue(iCol));
				vecLineGreen.push_back(currentImLine.getGreenValue(iCol));
				vecLineBlue.push_back(currentImLine.getBlueValue(iCol));
				
			}
			
			vecPixRed.push_back(vecLineRed);
			vecPixGreen.push_back(vecLineGreen);
			vecPixBlue.push_back(vecLineBlue);
		}
		
		//--- Calcul valeur mediane
		//-- Boucle sur les colonnes
		for (int iCol = 0; iCol < firstIm.getXSize(); ++iCol)
		{
			vecDataMedianRed.clear();
			vecDataMedianGreen.clear();
			vecDataMedianBlue.clear();
			
			//- Boucle sur les images ligne
			for (int indexIm = 0; indexIm < nbImage; ++indexIm)
			{
				vecDataMedianRed.push_back(vecPixRed[indexIm][iCol]);
				vecDataMedianGreen.push_back(vecPixGreen[indexIm][iCol]);
				vecDataMedianBlue.push_back(vecPixBlue[indexIm][iCol]);
				
			}
			
			finalImage.setRedValue(medianVecD(vecDataMedianRed), iLine * firstIm.getXSize() + iCol);
			finalImage.setGreenValue(medianVecD(vecDataMedianGreen), iLine * firstIm.getXSize() + iCol);
			finalImage.setBlueValue(medianVecD(vecDataMedianBlue), iLine * firstIm.getXSize() + iCol);
			
			
		}
		
		std::cout << "### Traitement ligne " << iLine << " sur " << firstIm.getYSize() << std::endl;
		
		int avancement = (float)iLine / (float)firstIm.getYSize() * 100.0;
		if ( (avancement == 10) || (avancement == 20) || (avancement == 30) ||
			 (avancement == 40) || (avancement == 50) || (avancement == 60) ||
			 (avancement == 70) || (avancement == 80) || (avancement == 90) ||
			 (avancement == 100) )
		{
			std::cout << "INFO   : Avancement: " << avancement << std::endl;
		}
	}
	
	
	*/
	
	
	return true;
}




//----------------------------------------------------------------------
//---- Calcul du dark par moyenne
//----------------------------------------------------------------------
bool computeMeanDark(const std::vector<std::string> & vecImPath, const std::string & format, Image & finalIm)
{
	
	//---- Ouverture de la premiere image
	Image firstIm;
	if ( ! firstIm.load(vecImPath[0]) )
	{
		std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[0] << std::endl;
		return false;
	}
	
	//---- Image finale
	finalIm.setDimensions(firstIm.getXSize(), firstIm.getYSize());
	std::cout << "INFO   : Taille de l'image finale " << firstIm.getXSize() << ", " << firstIm.getYSize() << std::endl;
	
    //---- Master dark en raw?
    if ( firstIm.isRaw() )
    {
        finalIm.setRaw();
    }
	
	//---- Boucle sur les images
	Image currentIm;
	for (std::vector<std::string>::const_iterator iPath = vecImPath.begin(); iPath != vecImPath.end(); ++iPath)
	{
		std::cout << "INFO   : Traitement de l'image " << *iPath << std::endl;
		//--- Ouverture de l'image courante
		if ( ! currentIm.load(*iPath) )
		{
			std::cout << "ERREUR : Echec lors du chargement de l'image " << *iPath << std::endl;
			return false;
		}
		//std::cout << "INFO   : Taille de l'image courante " << firstIm.getXSize() << ", " << firstIm.getYSize() << std::endl;
		if ( ! finalIm.add(currentIm) )
		{
			std::cout << "ERREUR : Echec de la somme entre l'image finale et l'image " << *iPath << std::endl;
			return false;
		}
	}
	
	//---- Division par le nombre d'images
	double nbImage = vecImPath.size();
	finalIm.div(nbImage);
	
	return true;
}


//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
	std::cout << "Usage: ./masterDark   <Repertoire des images>" << std::endl;
	std::cout << "                      <Format des images (RAW, TIF, JPG)>" << std::endl;
	std::cout << "                      <Image dark finale>" << std::endl;
	std::cout << "                      <Methode : MOYENNE ou MEDIANE>" << std::endl;
	
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
	//---- Gestion des arguments
	if ( argc != 5 )
	{
		usage();
		return -1;
	}
	
	std::string imageDir = argv[1];
	std::string format = argv[2];
	std::string finalImPath = argv[3];
	std::string methode = argv[4];
	
	
	
	//---- Parsing du repertoire
	std::vector<std::string> vecImPath;
	if ( !parseDir(imageDir, format, vecImPath) )
	{
		std::cout << "ERREUR : Impossible de scanner le repertoire " << imageDir << std::endl;
		return 1;
	}
	
	
	//---- Verification du parsing
	int nbImage = vecImPath.size();
	if ( nbImage == 0 )
	{
		std::cout << "ERREUR : Aucune image " << format << " a traiter dans le repertoire d'entree" << std::endl;
		return -1;
	} else if ( nbImage == 1 )
	{
		std::cout << "ERREUR : Une seule image presente dans le repertoire d'entree!" << std::endl;
		return -1;
	} else {
		std::cout << "INFO   : " << nbImage << " images a traiter" << std::endl;
	}
	
	
	
	//---- Calcul du master dark
	Image finalImage;
	if ( methode == "MOYENNE" )
	{
		computeMeanDark(vecImPath, format, finalImage);
		
		
	} else if ( methode == "MEDIANE" )
	{
		std::cout << "INFO   : Methode non implementee" << std::endl;
		//computeMedianDark();
		return 0;
		
	} else {
		std::cout << "ERREUR : Methode " << methode << " inconnue" << std::endl;
		return -1;
	}
	
	
	//---- Sauvegarde image finale
    if ( ! finalImage.writeImTiff(finalImPath) )
    {
		std::cout << "ERREUR : Impossible de sauvegarder l'image " << finalImPath << std::endl;
        return -1;
	}
	
	return 0;
}
