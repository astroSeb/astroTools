//----------------------------------------------------------------------
//
//---- wildFieldCompositor : Outil de compositage astro pour images
//                           de la voie lactee
//
//----------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>

#include "Util.h"
#include "Image.h"
#include "Zncc.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#define NB_LINK_POINTS 4
#define HALF_PATTERN_WIN 20
#define HALF_SEARCH_WIN 100
#define SEUIL_BIN 150



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


//~ //----------------------------------------------------------------------
//~ //---- Selectiion des points de liaison
//~ //----------------------------------------------------------------------
//~ std::vector<cv::Point2f> selectPoints(const Image & currentImage)
//~ {
	//~ std::vector<cv::Point2f> vecLinkPoints;
	
	//~ cv::Mat imMat = currentImage.getCvMatFromRaw();
	//~ cv::Point2f selectedPoint;
	
	//~ for (int index = 0; index < NB_LINK_POINTS; ++index)
	//~ {
	
		//~ namedWindow("Display", cv::WINDOW_NORMAL);
		//~ cv::setMouseCallback("Display", CallBackFunc, &selectedPoint);
		
		//~ cv::imshow("Display", imMat);
		//~ cv::waitKey(0);
		
		//~ std::cout << "INFO  : Point de liaison " << index << " : " << selectedPoint.x << ", " << selectedPoint.y << std::endl;
		
		//~ vecLinkPoints.push_back(selectedPoint);
	//~ }
	
	
	
	//~ return vecLinkPoints;
//~ }

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



//----------------------------------------------------------------------
//---- Recherche par correlation du point de liaison
//----------------------------------------------------------------------
cv::Point2f findPoint(const Image & imageRef, const Image & imageSrc, cv::Point2f refPoint, cv::Point2f searchPoint)
{
	cv::Point2f matchedPoint;
	recalage::Zncc znccOp;
	
	
	//---- Imagette de reference
	Image imRefPatern = extractPatern(refPoint, imageRef);
	znccOp.setImRef(&imRefPatern);
	
	//---- Image source de recherche
	int x0Search = searchPoint.x - HALF_SEARCH_WIN;
	int y0Search = searchPoint.y - HALF_SEARCH_WIN;
	
	//---- Recherche pixellique du pattern
	int dxSearch = ( (HALF_SEARCH_WIN * 2) + 1 ) - imRefPatern.getXSize();
	int dySearch = ( (HALF_SEARCH_WIN * 2) + 1 ) - imRefPatern.getYSize();
	double currentScore = 0.0;
	double bestScore = 0.0;
	
	for (int y = 0; y < dySearch; ++y)
	{
		for (int x = 0; x < dxSearch; ++x)
		{
			//--- Coint HG dans l'image source
			int x0 = x0Search + x;
			int y0 = y0Search + y;
			
			//--- Imagette source pour le calcul du ZNCC
			Image imSrcPattern = imageSrc.getRoi(x0, y0, imRefPatern.getXSize(), imRefPatern.getYSize());
			
			//--- Calcul du ZNCC
			znccOp.setImSec(&imSrcPattern);
			znccOp.computeZncc(currentScore);
			
			if ( currentScore > bestScore )
			{
				bestScore = currentScore;
				matchedPoint.x = x0 + HALF_PATTERN_WIN;
				matchedPoint.y = y0 + HALF_PATTERN_WIN;
			}
			
		}
		
	}
	
	std::cout << "INFO   : Point trouve en " << matchedPoint.x << ", " << matchedPoint.y << ", score: " << bestScore << std::endl;
	
	//imageRef.zoomOnPoint(refPoint, true);
	//imageSrc.zoomOnPoint(matchedPoint, true);
	
	return matchedPoint;
}






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
	//imSeuil.disp();
	
	//--- Detection de l'etoile
	Image::sBoundingBox starBb = imSeuil.detectBiggestObj();
	//std::cout << "### BBox pattern: X " << starBb.xMin << " -> " << starBb.xMax << ", Y " << starBb.yMin << " -> " << starBb.yMax << std::endl;
	Image imStar = imZoom.getRoi(starBb);
	
	//--- Detection du centre
	cv::Point2f starCenter = imStar.computeStarCenter();
	//std::cout << "### star center : " << starCenter << std::endl;
	
	starCenter.x = starCenter.x - HALF_PATTERN_WIN + starBb.xMin + selectedPoint.x;
	starCenter.y = starCenter.y - HALF_PATTERN_WIN + starBb.yMin + selectedPoint.y;
	std::cout << "INFO   : Point de liaison : " << starCenter << std::endl;
	
	currentImage.zoomOnPoint(starCenter, true);
	
	return starCenter;
}





//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./wildFieldCompositor   <Repertoire des images>" << std::endl;
    std::cout << "                               <Format des images (NEF, JPG, tif, ...)" << std::endl;
    std::cout << "                               <Image master dark>" << std::endl;
    std::cout << "                               [<Masque premier plan>]" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //---- Gestion des arguments
    if ( (argc < 4) || (argc > 5) )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return -1;
    }
    std::cout << argc << std::endl;
    bool flagMasque = false;
     std::string premierPlanPath = "";

    std::string imageDir = argv[1];
    std::string format = argv[2];
    std::string masterDarkPath = argv[3];
    
    //--- Gestion optionnelle du masque de premier plan
    if ( argc == 5 )
    {
        flagMasque = true;
        premierPlanPath = argv[4];
        std::cout << "argv4: " << argv[4] << std::endl;
        std::cout << "masque: " << premierPlanPath << std::endl;
    }


    //---- Test sur le format des images
    if ( (format != "NEF") && (format != "JPG") && (format != "jpg") && (format != "tif") )
    {
        std::cout << "ERREUR : Le format demande " << format << " n'est pas pris en compte" << std::endl;
        return 1;
    }


    //---- Parsing du repertoire
    std::vector<std::string> vecImPath;
    if ( !parseDir(imageDir, format, vecImPath) )
    {
        std::cout << "ERROR : Unable to parse input directory " << imageDir << std::endl;
        return 1;
    }
    int nbImages = vecImPath.size();
    std::cout << "INFO   : " << nbImages << " images a traiter" << std::endl;


    //---- Ouverture de la premiere image
    Image firstImage;
    if ( ! firstImage.load(vecImPath[0]) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[0] << std::endl;
        return 1;
    }
    //firstImage.disp();


    //---- Chargement du master dark
    Image masterDark;
    if ( ! masterDark.load(masterDarkPath) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image de dark " << masterDarkPath << std::endl;
        return 1;
    }

    
    //---- Chargement du masque premier plan si demande
    Image masquePremierPlan;
    Image imCardinal(firstImage.getXSize(), firstImage.getYSize());
    if ( flagMasque )
    {
        if ( ! masquePremierPlan.load(premierPlanPath) )
        {
            std::cout << "ERREUR : Echec lors du chargement du masque de premier plan " << premierPlanPath << std::endl;
            return 1;
        }
    }
	
	
	
	//---- Creation de l'image finale (8 bits pour l'instant)
	//cv::Mat finalMat = imRef.getCvMatFromRaw();
	cv::Mat finalMat = firstImage.getCvMatNew(true);
	Image finalImage(finalMat);
	
	//---- Image de cardinalite pour la moyenne
    
	
	//---- Selection manuelle des points de liaison sur la premiere image
	std::cout << "INFO   : Selectionner les points de liaison sur la premiere image" << std::endl;
	std::vector<cv::Point2f> vecFirstImPoints;
	cv::Point2f currentPoint;
	for ( int indexPt = 0; indexPt < NB_LINK_POINTS; ++indexPt)
	{
		std::cout << "INFO   : Selection du point " << indexPt + 1 << " ..." << std::endl;
		currentPoint = selectFirstPoint(firstImage, false);
		vecFirstImPoints.push_back(currentPoint);
	}
	
	
	//---- Recherche des points de liaison sur les autres images
	std::vector<cv::Point2f> vecPreviousImPoints = vecFirstImPoints;
	for ( int indexIm = 1; indexIm < nbImages; ++indexIm )
	//for ( int indexIm = 1; indexIm < 4; ++indexIm )
	{
		std::cout << "INFO   : Recalage image " << indexIm + 1 << " sur " << nbImages << std::endl;
		
		//--- Chargement de l'image courante
		Image currentIm;
		if ( ! currentIm.load(vecImPath[indexIm]) )
		{
			std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[indexIm] << std::endl;
			return 1;
		}
		
		//--- Recherche par correlation
		cv::Point2f currentFoundPoint;
		std::vector<cv::Point2f> vecCurrentFoundPoints;
		for (int indexPt = 0; indexPt < NB_LINK_POINTS; ++indexPt)
		//for ( std::vector<cv::Point2f>::iterator iPt = vecPreviousImPoints.begin(); iPt != vecPreviousImPoints.end(); ++iPt)
		{
			//std::cout << "INFO   : Point de reference " << iPt->x << ", " << iPt->y << std::endl;
			std::cout << "INFO   : Point de reference " << vecPreviousImPoints[indexPt].x << ", " << vecPreviousImPoints[indexPt].y << std::endl;
			currentFoundPoint = findPoint(firstImage, currentIm, vecFirstImPoints[indexPt], vecPreviousImPoints[indexPt]);
			vecCurrentFoundPoints.push_back(currentFoundPoint);
		}
		
		//--- Calcul de la transformation
		cv::Mat currentTransfo = cv::findHomography(vecCurrentFoundPoints, vecFirstImPoints, CV_RANSAC );
		std::cout << "INFO   : matrice de transfo " << currentTransfo << std::endl;
		
		//--- Soustraction du dark
		currentIm.subtract(masterDark);
		
		//---- Resampling
		cv::Size taille(firstImage.getXSize(), firstImage.getYSize());
		cv::Mat correctedMat(taille, CV_8UC3);
		cv::warpPerspective(currentIm.getCvMatNew(true), correctedMat, currentTransfo, taille);
		
        //---- Avec masque
        if ( flagMasque )
        {
            cv::Mat correctedMask(taille, CV_8UC3);
            cv::warpPerspective(masquePremierPlan.getCvMatNew(true), correctedMask, currentTransfo, taille);
            finalImage.addWithMask(correctedMat, correctedMask, imCardinal);
            
        //--- Sans masque
        } else {
        
            finalImage.add(Image(correctedMat));
		}
		
		vecPreviousImPoints = vecCurrentFoundPoints;
	}
	
	
	//---- Moyenne
    if ( flagMasque )
    {
        
        finalImage.divByIm(imCardinal, 1.0);
    } else {
        
        finalImage.div(nbImages);
    }
    
	finalImage.disp();
	
	
	//---- Sauvegarde image finale
	finalMat = finalImage.getCvMatNew(true);
	std::string finalImPath = imageDir + "/finalImage.TIF";
	std::cout << "INFO  : Sauvegarde de l'image: " << finalImPath << std::endl;
	
	if ( ! cv::imwrite(finalImPath, finalMat) )
	{
		std::cout << "ERREUR : Impossible de sauvegarder l'image " << finalImPath << std::endl;
	}
	
	return 0;
}

