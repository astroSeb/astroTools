//----------------------------------------------------------------------
//
//---- test
//
//----------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/nonfree/features2d.hpp>

#include "Image.h"
#include "Util.h"







//----------------------------------------------------------------------
//--- Recherche de la translation avec l'image de reference
//----------------------------------------------------------------------
bool findTranslation(Image & firstImage, Image & currentImage, double & dx, double & dy)
{
	int halfCorrelSize = 100;
	int correlSize = halfCorrelSize * 2 + 1;
	
	cv::Mat result(currentImage.getYSize() - correlSize + 1, currentImage.getXSize() - correlSize + 1, CV_32FC1);
	
	cv::Mat imSrc = currentImage.getCvMatNew(true);
	
	dx = 0.0;
	dy = 0.0;
	//double nbPt = 0.0;
	
	std::vector<double> vecDx, vecDy;
	
	//---- Parcours des 9 points de controle
	for (int iLine = 1; iLine < 4; ++iLine)
	{
		for (int iCol = 1; iCol < 4; ++iCol)
		{
			//--- Extrait de l'imagette de reference
			int imXQuart = std::floor(firstImage.getXSize() / 4.0);
			int imYQuart = std::floor(firstImage.getYSize() / 4.0);
			int x0 = iCol * imXQuart;
			int y0 = iLine * imYQuart;
			//std::cout << "INFO   : Coin HG " << x0 << ", " << y0 << std::endl;
			Image currentRef = firstImage.getRoi(x0, y0, correlSize, correlSize);
			
			matchTemplate( imSrc, currentRef.getCvMatNew(true), result, 3 );
			normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
			
			double minVal, maxVal;
			cv::Point minLoc, maxLoc;
			
			minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
			
			//std::cout << "INFO   : Matching trouve en " << maxLoc.x << ", " << maxLoc.y << " avec un score de " << maxVal << ", scoreMin: " << minVal << std::endl;
			std::cout << "INFO   : dx " << x0 - maxLoc.x << ", dy " << y0 - maxLoc.y << ", score: " << maxVal << std::endl;
			
			/*
			Image imFound = currentImage.getRoi(maxLoc.x, maxLoc.y, correlSize, correlSize);
			Image imMatch(correlSize*2, correlSize);
			imMatch.incrust(currentRef, 0, 0);
			imMatch.incrust(imFound, correlSize, 0);
			imMatch.disp();
			*/
			
			//nbPt += 1;
			//dx += x0 - maxLoc.x;
			//dy += y0 - maxLoc.y;
			
			vecDx.push_back(x0 - maxLoc.x);
			vecDy.push_back(y0 - maxLoc.y);
			
		}
	}
	
	//---- Filtrage des appariements a 1 sigma
	double meanDx = meanVecD(vecDx);
	double meanDy = meanVecD(vecDy);
	
	double sigmaDx = sigmaVecD(vecDx, meanDx);
	double sigmaDy = sigmaVecD(vecDy, meanDy);
	
	std::cout << "### Moyenne X " << meanDx << ", sigma X " << sigmaDx << std::endl;
	std::cout << "### Moyenne Y " << meanDy << ", sigma Y " << sigmaDy << std::endl;
	
	
	if ( (sigmaDx > 5.0) || (sigmaDy > 5.0) )
	{
		int nbPt = vecDx.size();
		std::vector<double> vecFiltredDx, vecFiltredDy;
		for (int index = 0; index < nbPt; ++index)
		{
			if ( (vecDx[index] > meanDx - sigmaDx / 2.0) && (vecDx[index] < meanDx + sigmaDx / 2.0) &&
				 (vecDy[index] > meanDy - sigmaDy / 2.0) && (vecDy[index] < meanDy + sigmaDy / 2.0) )
			{
				vecFiltredDx.push_back(vecDx[index]);
				vecFiltredDy.push_back(vecDy[index]);
			}
		}
		
		int nbPtFiltred = vecFiltredDx.size();
		
		if ( nbPtFiltred >= 6 )
		{
			meanDx = meanVecD(vecFiltredDx);
			meanDy = meanVecD(vecFiltredDy);
		} else {
			
			return false;
		}
		
	}
	
	
	dx = meanDx;
	dy = meanDy;
	
	
	std::cout << "INFO   : Decalage moyen " << dx << ", " << dy << std::endl;
	
	return true;
}





//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
	std::cout << "Usage: ./test   <Repertoire des images d'entree>" << std::endl;
	std::cout << "                <Image dark>" << std::endl;
	std::cout << "                <Image finale>" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
	//---- Gestion des arguments
	if ( argc != 4)
	{
		usage();
		return -1;
	}
	
	std::string inputDir = argv[1];
	std::string imDarkPath = argv[2];
	std::string outputIm = argv[3];
	
	//---- Parsing du repertoire d'entree
	std::vector<std::string> imagePathVect;
	if ( ! parseDir(inputDir, "JPG", imagePathVect ) )
	{
		std::cout << "ERREUR : Erreur dans la lecture du repertoire d'entree!" << std::endl;
	}
	int nbIm = imagePathVect.size();
	//int nbIm = 2;
	
	std::cout << "INFO   : Lecture du repertoire " << inputDir << " -> OK." << std::endl;
	std::cout << "INFO   : " << nbIm << " image detectees" << std::endl;
	
	//---- Chargement du dark
	Image imDark;
	imDark.loadTiff(imDarkPath);
	
	//---- La premiere image est la reference sur laquelle on vient supperposer les autres
	std::cout << "INFO   : Image de reference " << imagePathVect[0] << std::endl;
	Image firstImage;
	firstImage.loadJpeg(imagePathVect[0]);
	//firstImage.disp();
	
	//--- Creation de l'image finale
	int finalDx = firstImage.getXSize() * 2;
	int finalDy = firstImage.getYSize() * 2;
	Image finalImage(finalDx, finalDy);
	
	//--- Ajout de la premiere image
	int x0 = std::floor(firstImage.getXSize() / 2.0);
	int y0 = std::floor(firstImage.getYSize() / 2.0);
	finalImage.incrust(firstImage, x0, y0);
	//finalImage.disp();
	
	int nbImToAdd = 0;
	//---- Parcours des images suivantes
	for (int index = 1; index < nbIm; ++index)
	{
		//--- Chargement de l'image
		Image currentImage;
		currentImage.loadJpeg(imagePathVect[index]);
		std::cout << "INFO   : Traitement de l'image " << imagePathVect[index] << " (" << index << " sur " << nbIm - 1 << ")" << std::endl;
		
		//--- Recherche de la translation l'image de reference
		double dx, dy;
		if ( !findTranslation(firstImage, currentImage, dx, dy) )
		{
			std::cout << "WARN   : Echec de l'alignement" << std::endl;
			continue;
		}
		
		nbImToAdd += 1;
		
		//--- soustraction du dark
		currentImage.subtract(imDark);
		
		//--- Ajout de l'image courante
		finalImage.incrustSub(currentImage, x0 + dx, y0 + dy);
		
	}
	
	//---- Moyenne des images sommes
	finalImage.div(nbImToAdd);
	finalImage.disp();
	
	//---- Export de l'image sur disque
	cv::Mat finalMat = finalImage.getCvMatNew();
	imwrite(outputIm, finalMat);
	
	
	return 0;
}




