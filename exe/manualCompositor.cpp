//----------------------------------------------------------------------
//
//---- wildFieldCompositor : Outil de compositage astro pour images
//                           de la voie lactee
//
//----------------------------------------------------------------------

/*
 * TODO
 * 
 * - Detection du centre des etoiles par centre des masses a la place de la selection manuelle
 * d'un pixel
 * 
 * - Creation d'un "resamplor" pour faire le decalage en 16 bit et non avec openCV
 * 
 * - Sauvegarde de l'image en 16 bit
 * 
 */ 



#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>

#include "Util.h"
#include "Image.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#define NB_LINK_POINTS 1
#define HALF_PATTERN_WIN 20
#define HALF_SEARCH_WIN 200

#define INDEX_IM_PATH 0

//#define SEUIL_BIN 80
#define SEUIL_BIN 150


//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
	std::cout << "Usage: ./manualCompositor   <Repertoire des images>" << std::endl;
	std::cout << "                            <Format des images (NEF, JPG, tif, ...)" << std::endl;
	std::cout << "                            <Image master dark>" << std::endl;
	std::cout << "                            [<Fichier des points de liaison>]" << std::endl;
	
}


//----------------------------------------------------------------------
//---- Lecture des coordonnees du clic souris
//----------------------------------------------------------------------
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	cv::Point2f * selectedPoint = (cv::Point2f*)userdata;
	
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
		
        selectedPoint->x = x;
		selectedPoint->y = y;
		
		cv::destroyWindow("Display");
    }
    
    
}

//----------------------------------------------------------------------
//---- Extraction d'un pattern de correlation
//----------------------------------------------------------------------
Image extractPatern(const cv::Point & imgCenter, const Image & im)
{
	
	Image::sBoundingBox bBox;
	bBox.xMin = imgCenter.x - HALF_PATTERN_WIN;
	bBox.xMax = imgCenter.x + HALF_PATTERN_WIN;
	bBox.yMin = imgCenter.y - HALF_PATTERN_WIN;
	bBox.yMax = imgCenter.y + HALF_PATTERN_WIN;
	
	//std::cout << "### BBox pattern: X " << bBox.xMin << " -> " << bBox.xMax << ", Y " << bBox.yMin << " -> " << bBox.yMax << std::endl;
	
	return im.getRoi(bBox);
	
}


//----------------------------------------------------------------------
//---- Extraction d'une imagette de recherche
//----------------------------------------------------------------------
Image extractSearchImg(const cv::Point & imgCenter, const Image & im)
{
	Image::sBoundingBox bBox;
	bBox.xMin = imgCenter.x - HALF_SEARCH_WIN;
	bBox.xMax = imgCenter.x + HALF_SEARCH_WIN;
	bBox.yMin = imgCenter.y - HALF_SEARCH_WIN;
	bBox.yMax = imgCenter.y + HALF_SEARCH_WIN;
	
	//std::cout << "### BBox search: X " << bBox.xMin << " -> " << bBox.xMax << ", Y " << bBox.yMin << " -> " << bBox.yMax << std::endl;
	
	return im.getRoi(bBox);
	
}

/*
//----------------------------------------------------------------------
//---- Selection du premier point de liaison
//----------------------------------------------------------------------
cv::Point2f selectFirstPoint(const Image & currentImage, bool fullDynamic)
{
	//---- Selection macroscopique
	cv::Mat imMat = currentImage.getCvMatFromRaw(fullDynamic);
	cv::Point2f selectedPoint;
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::setMouseCallback("Display", CallBackFunc, &selectedPoint);
		
	cv::imshow("Display", imMat);
	cv::waitKey(0);
	
	//---- Selection minutieuse
	cv::Point2f secondPoint;
	Image imZoom = extractPatern(selectedPoint, currentImage);
	imMat = imZoom.getCvMatFromRaw(fullDynamic);
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::setMouseCallback("Display", CallBackFunc, &secondPoint);
		
	cv::imshow("Display", imMat);
	cv::waitKey(0);
	
	selectedPoint.x = selectedPoint.x - HALF_PATTERN_WIN + secondPoint.x;
	selectedPoint.y = selectedPoint.y - HALF_PATTERN_WIN + secondPoint.y;
	
	std::cout << "INFO  : Point de liaison : " << selectedPoint.x << ", " << selectedPoint.y << std::endl;
	
	
	return selectedPoint;
}
*/

//----------------------------------------------------------------------
//---- Selection du premier point de liaison
//----------------------------------------------------------------------
cv::Point2f selectFirstPoint(const Image & currentImage, bool fullDynamic)
{
	//---- Selection macroscopique
	//cv::Mat imMat = currentImage.getCvMatFromRaw(fullDynamic);
	cv::Mat imMat = currentImage.getCvMatNew(fullDynamic);
	cv::Point2f selectedPoint;
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::setMouseCallback("Display", CallBackFunc, &selectedPoint);
		
	cv::imshow("Display", imMat);
	cv::waitKey(0);
	
	//std::cout << "### " << selectedPoint << std::endl;
	
	//---- Selection minutieuse
	cv::Point2f secondPoint;
	Image imZoom = extractPatern(selectedPoint, currentImage);
	//imZoom.disp(true);
	
	//---- Detection de l'etoile
	//--- Binarisation
	Image imSeuil = imZoom.seuillage(SEUIL_BIN);
	imSeuil.disp();
	
	//--- Detection de l'etoile
	Image::sBoundingBox starBb = imSeuil.detectBiggestObj();
	//std::cout << "### BBox pattern: X " << starBb.xMin << " -> " << starBb.xMax << ", Y " << starBb.yMin << " -> " << starBb.yMax << std::endl;
	Image imStar = imZoom.getRoi(starBb);
	
	//--- Detection du centre
	cv::Point2f starCenter = imStar.computeStarCenter();
	//std::cout << "### star center : " << starCenter << std::endl;
	
	starCenter.x = starCenter.x - HALF_PATTERN_WIN + starBb.xMin + selectedPoint.x;
	starCenter.y = starCenter.y - HALF_PATTERN_WIN + starBb.yMin + selectedPoint.y;
	std::cout << "INFO  : Point de liaison : " << starCenter << std::endl;
	
	return starCenter;
}



//----------------------------------------------------------------------
//---- Selection des autres points de liaison
//----------------------------------------------------------------------
cv::Point2f selectOtherPoint(const Image & currentImage, const Image & firstPointIm, bool fullDynamic)
{
	
	//---- Selection macroscopique
	//cv::Mat imMat = currentImage.getCvMatFromRaw(fullDynamic);
	cv::Mat imMat = currentImage.getCvMatNew(fullDynamic);
	cv::Point2f selectedPoint;
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::setMouseCallback("Display", CallBackFunc, &selectedPoint);
		
	cv::imshow("Display", imMat);
	cv::waitKey(0);
	

	//---- Selection minutieuse automatique
	Image imZoom = extractPatern(selectedPoint, currentImage);
	
	//---- Detection de l'etoile
	//--- Binarisation
	Image imSeuil = imZoom.seuillage(SEUIL_BIN);
	
	//--- Detection de l'etoile
	Image::sBoundingBox starBb = imSeuil.detectBiggestObj();
	Image imStar = imZoom.getRoi(starBb);
	
	//--- Detection du centre
	cv::Point2f starCenter = imStar.computeStarCenter();
	
	starCenter.x = starCenter.x - HALF_PATTERN_WIN + starBb.xMin + selectedPoint.x;
	starCenter.y = starCenter.y - HALF_PATTERN_WIN + starBb.yMin + selectedPoint.y;
	std::cout << "INFO  : Point de liaison : " << starCenter << std::endl;
	
	
	//---- Construction de l'image a afficher
	imZoom = currentImage.getZoomOnPoint(starCenter);
	cv::Mat imConstruct = cv::Mat::zeros(imZoom.getYSize(), imZoom.getXSize() + firstPointIm.getXSize() + 1, CV_8UC3);
	//std::cout << "### " << imZoom.getYSize() << " x " << imZoom.getXSize() + firstPointIm.getXSize() + 1 << std::endl;
	cv::Vec3b currentPix;
	
	//--- Ajout du zoom courant
	for (int y = 0; y < imZoom.getYSize(); ++y)
	{
		for (int x = 0; x < imZoom.getXSize(); ++x)
		{
			currentPix[0] = imZoom.getRedValue8b(x, y, fullDynamic);
			currentPix[1] = imZoom.getGreenValue8b(x, y, fullDynamic);
			currentPix[2] = imZoom.getBlueValue8b(x, y, fullDynamic);
			
			imConstruct.at<cv::Vec3b>(y, x) = currentPix;
		}
	}
	
	//---- Ajout de la separation
	for (int y = 0; y < imConstruct.rows; ++y)
	{
		currentPix[0] = 0;
		currentPix[1] = 0;
		currentPix[2] = 254;
		
		imConstruct.at<cv::Vec3b>(y, imZoom.getXSize()) = currentPix;
	}
	
	//---- Ajout du zoom de la premiere image
	for (int y = 0; y < firstPointIm.getYSize(); ++y)
	{
		int indexX = 0;
		
		for (int x = imZoom.getXSize()+1; x < imConstruct.cols; ++x)
		{
			currentPix[0] = firstPointIm.getRedValue8b(indexX, y, fullDynamic);
			currentPix[1] = firstPointIm.getGreenValue8b(indexX, y, fullDynamic);
			currentPix[2] = firstPointIm.getBlueValue8b(indexX, y, fullDynamic);
			
			indexX += 1;
			
			imConstruct.at<cv::Vec3b>(y, x) = currentPix;
		}
	}
	
	return starCenter;
}


//----------------------------------------------------------------------
//---- Sauvegarde des points de liaison
//----------------------------------------------------------------------
bool saveLinkPoints(const std::string & pointListFile, std::vector<std::string> vecImPath, std::vector<std::vector<cv::Point2f> > vecImagePoint)
{
	//--- Test nombre d'images
	if ( vecImPath.size() != vecImagePoint.size() )
	{
		std::cout << "ERREUR le nombre d'images (" << vecImPath.size() << ") est different du nombre de groupes de points (" << vecImagePoint.size() << ")" << std::endl;
		return false;
	}
	
	//---- Ouverture du fichier
	std::ofstream fichier(pointListFile.c_str(), std::ios::out);
	if (fichier)
	{
		//--- Parcours des images
		for (unsigned int indexIm = 0; indexIm < vecImPath.size(); ++indexIm)
		{
			fichier << vecImPath[indexIm] << ";";
			
			//---- Parcours des points
			for (unsigned int indexPt = 0; indexPt < vecImagePoint[indexIm].size(); ++indexPt)
			{
				fichier << vecImagePoint[indexIm][indexPt].x << ";" << vecImagePoint[indexIm][indexPt].y << ";";
			}
			
			fichier << std::endl;

		}
		
		fichier.close();
	} else {
		std::cout << "ERREUR : Impossible d'ecrire le fichier " << pointListFile << std::endl;
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------
//---- Lecture des points de liaison
//----------------------------------------------------------------------
bool readLinkPoints(const std::string & pointListFile, std::vector<std::string> & vecImPath, std::vector<std::vector<cv::Point2f> > & vecImagePoint)
{
	
	//---- Ouverture du fichier
	std::ifstream fichier(pointListFile.c_str(), std::ios::in);
	if (fichier)
	{
		//--- Lecture ligne par ligne
		std::vector<std::string> ligneSplit;
		std::string pattern = ";";
		std::string ligne;
		
		while( getline(fichier,ligne) )
		{
			ligneSplit = splitStr(ligne, pattern);
			
			vecImPath.push_back(ligneSplit[INDEX_IM_PATH]);
			std::cout << "### image " << ligneSplit[INDEX_IM_PATH] << std::endl;
			int nbPoints = (ligneSplit.size() - 1) / 2;
			std::cout << "### nbPoints: " << nbPoints << std::endl;
			
			//-- Boucle sur les points
			cv::Point2f currentPoint;
			std::vector<cv::Point2f> vecPoints;
			for (int iPt = 0; iPt < nbPoints; ++iPt)
			{
				currentPoint.x = atof(ligneSplit[iPt * 2 + 1].c_str());
				currentPoint.y = atof(ligneSplit[iPt * 2 + 2].c_str());
				std::cout << "### point [" << currentPoint.x << ", " << currentPoint.y << "]" << std::endl;
				vecPoints.push_back(currentPoint);
			}
			
			vecImagePoint.push_back(vecPoints);
		}

		fichier.close();
		
	} else {
		std::cout << "ERREUR : Impossible de lire le fichier " << pointListFile << std::endl;
		return false;
	}
	
	
	return true;
}

//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
	//---- Gestion des arguments
	if ( (argc < 4) || (argc > 5) )
	{
		usage();
		return -1;
	}
	
	std::string imageDir = argv[1];
	std::string format = argv[2];
	std::string masterDarkPath = argv[3];
	
	if ( (format != "NEF") && (format != "JPG") && (format != "tif") )
	{
		std::cout << "ERREUR : Le format demande " << format << " n'est pas pris en compte" << std::endl;
		return 1;
	}
	
	std::string linkPointFile;
	bool knownPoints = false;
	
	if ( argc == 5 )
	{
		knownPoints = true;
		linkPointFile = argv[4];
	}
	
	std::vector<std::string> vecImPath;
	std::vector<std::vector<cv::Point2f> > vecImagePoint;
	int nbImage;
	
	
	//---- Parsing du repertoire
	if ( !parseDir(imageDir, format, vecImPath) )
	{
		std::cout << "ERREUR : Impossible de scanner le repertoire " << imageDir << std::endl;
		return 1;
	}
	nbImage = vecImPath.size();
	
	//---- Cas ou les points de liaison sont a definir
	if ( ! knownPoints )
	{
		
		std::vector<std::vector<cv::Point2f> > vecPointImage;
		
		//---- Boucle sur les points de liaison
		for (int indexLink = 0; indexLink < NB_LINK_POINTS; ++indexLink)
		{
			std::vector<cv::Point2f> vecCurrentLink;
			Image imFirstPoint;
			std::cout << "INFO   : Definition du point de liaison " << indexLink+1 << std::endl;
			
			//--- Boucle sur les images
			for (unsigned int indexIm = 0;  indexIm < vecImPath.size(); ++indexIm)
			{
				std::cout << "INFO   : Recherche point de liaison " << indexLink+1 << ", image " << indexIm+1 << " sur " << vecImPath.size() << std::endl;
				//-- Lecture de l'image
				Image currentImage;
				if ( format == "NEF" )
				{
					currentImage.loadRaw(vecImPath[indexIm]);
				} else if ( format == "JPG" )
				{
					cv::Mat imMat;
					imMat = cv::imread(vecImPath[indexIm], CV_LOAD_IMAGE_COLOR);
					currentImage = imMat;
					
				} else if ( format == "tif" )
				{
					currentImage.loadTiff(vecImPath[indexIm]);
				}
				
				//-- Selection d'un point de liaison
				cv::Point2f currentPoint;
				if ( indexIm == 0 )
				{
					currentPoint = selectFirstPoint(currentImage, true);
					imFirstPoint = currentImage.getZoomOnPoint(currentPoint);
					
				} else {
					currentPoint = selectOtherPoint(currentImage, imFirstPoint, true);
				}
				
				currentImage.zoomOnPoint(currentPoint, true);
				
				vecCurrentLink.push_back(currentPoint);
				//std::cout << "###: " << currentPoint.x << ";" << currentPoint.y << std::endl;
			}
			
			vecPointImage.push_back(vecCurrentLink);
		}
		
		
		//---- Mise en forme pour openCV
		//---- Boucle sur les images
		for (int iIm = 0; iIm < nbImage; ++iIm)
		{
			std::vector<cv::Point2f> vecCurrentImage;
			
			for (int iPt = 0; iPt < NB_LINK_POINTS; ++iPt)
			{
				vecCurrentImage.push_back(vecPointImage[iPt][iIm]);
				
			}
			vecImagePoint.push_back(vecCurrentImage);
		}
		
		//---- Sauvegarde des points de liaison
		std::string pointListFile = imageDir + "/pointLink.csv";
		if ( ! saveLinkPoints(pointListFile, vecImPath, vecImagePoint) )
		{
			std::cout << "WARN  : Impossible d'enregistrer les points de liaison dans le fichier " << pointListFile << std::endl;
		}
		
	//---- Cas ou les points de liaison sont connus
	} else {
		
		//--- Lecture du fichier des points de liaison
		std::vector<std::string> vecImPath2;
		if ( ! readLinkPoints(linkPointFile, vecImPath2, vecImagePoint) )
		{
			std::cout << "ERREUR : Impossible de lire le fichier des points de liaison" << std::endl;
			return 1;
		}
		
	}
	
	
	//---- Chargement du master dark
	Image masterDark;
	if ( format == "NEF" )
	{
		masterDark.loadRaw(masterDarkPath);
	} else if ( format == "JPG" )
	{
		cv::Mat imMat;
		imMat = cv::imread(masterDarkPath, CV_LOAD_IMAGE_COLOR);
		masterDark = imMat;
		
	} else if ( format == "tif" )
	{
		masterDark.loadTiff(masterDarkPath);
	}
	
	
	
	//---- Ouverture premiere image (ref)
	Image imRef;
	
	if ( format == "NEF" )
	{
		imRef.loadRaw(vecImPath[0]);
	} else if ( format == "JPG" )
	{
		cv::Mat imMat;
		imMat = cv::imread(vecImPath[0], CV_LOAD_IMAGE_COLOR);
		imRef = imMat;
		
	} else if ( format == "tif" )
	{
		imRef.loadTiff(vecImPath[0]);
	}
	
	//---- Creation de l'image finale (8 bits pour l'instant)
	//cv::Mat finalMat = imRef.getCvMatFromRaw();
	cv::Mat finalMat = imRef.getCvMatNew(true);
	Image finalImage(finalMat);
	
	//---- Boucle sur les images
	for (int indexIm = 1; indexIm < nbImage; ++indexIm)
	{
		//---- Ouverture image suivante
		Image currentIm;
		
		if ( format == "NEF" )
		{
			currentIm.loadRaw(vecImPath[indexIm]);
		} else if ( format == "JPG" )
		{
			cv::Mat imMat;
			imMat = cv::imread(vecImPath[indexIm], CV_LOAD_IMAGE_COLOR);
			currentIm = imMat;
			
		} else if ( format == "tif" )
		{
			currentIm.loadTiff(vecImPath[indexIm]);
		}
		
		//---- Soustraction du dark
		currentIm.subtract(masterDark);
		
		//---- Calcul transfo
		cv::Mat currentTransfo = cv::findHomography(vecImagePoint[indexIm], vecImagePoint[0], CV_RANSAC );
		std::cout << "INFO   : matrice de transfo " << currentTransfo << std::endl;
		
		//---- Resampling
		cv::Size taille(imRef.getXSize(), imRef.getYSize());
		cv::Mat correctedMat(taille, CV_8UC3);
		cv::warpPerspective(currentIm.getCvMatNew(true), correctedMat, currentTransfo, taille);
		
		//---- Stack
		finalImage.add(Image(correctedMat));

	
	}
	
	//---- Moyenne
	finalImage.div(nbImage);
	finalImage.disp();
	
	
	//---- Sauvegarde image finale
	//finalMat = finalImage.getCvMatFromRaw();
	finalMat = finalImage.getCvMatNew(true);
	std::string finalImPath = imageDir + "/finalImage.TIF";
	std::cout << "INFO  : Sauvegarde de l'image: " << finalImPath << std::endl;
	
	if ( ! cv::imwrite(finalImPath, finalMat) )
	{
		std::cout << "ERREUR : Impossible de sauvegarder l'image " << finalImPath << std::endl;
	}
	
	return 0;
}
