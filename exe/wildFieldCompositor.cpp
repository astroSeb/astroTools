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
#include "Display.h"
#include "Zncc.h"
#include "ObjectDetector.h"


#define NB_LINK_POINTS 4
#define HALF_PATTERN_WIN 20
#define HALF_SEARCH_WIN 100
#define SEUIL_BIN 150




//----------------------------------------------------------------------
//---- Calcul d'un centre etoile
//----------------------------------------------------------------------
cv::Point2f computeStarCenter(const astroT::Image & im)
{
    cv::Point2f starCenter;
    int dx = im.getXSize();
    int dy = im.getYSize();

    //---- Calcul DN et somme suivant les colonnes
    double DN = 0.0;
    double ySomme = 0.0;
    double flux = 0.0;
    double currentValue;
    for (int ligne = 0; ligne < dy; ++ligne)
    {
        for (int col = 0; col < dx; ++col)
        {
            currentValue = im.getGreenValue(ligne*dx+col);
            if ( currentValue > 0 )
            {
                DN += currentValue;
                ySomme += (double)ligne * currentValue;

                //--- Calcul du flux
                flux += currentValue;
            }
        }
    }

    //---- Calcul somme suivant les lignes
    double xSomme = 0.0;
    for (int col = 0; col < dx; ++col)
    {
        for (int ligne = 0; ligne < dy; ++ligne)
        {
            currentValue = im.getGreenValue(ligne*dx+col);
            if ( currentValue > 0 )
            {
                xSomme += (double)col * currentValue;
            }
        }
    }

    starCenter.x = (xSomme / DN);
    starCenter.y = (ySomme / DN);

    return starCenter;
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
astroT::Image extractPatern(const cv::Point & imgCenter, const astroT::Image & im)
{

    astroT::sBoundingBox bBox;
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
astroT::Image extractSearchImg(const cv::Point & imgCenter, const astroT::Image & im)
{
    astroT::sBoundingBox bBox;
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
cv::Point2f findPoint(const astroT::Image & imageRef, const astroT::Image & imageSrc, cv::Point2f refPoint, cv::Point2f searchPoint)
{
    cv::Point2f matchedPoint;
    astroT::Zncc znccOp;


    //---- Imagette de reference
    astroT::Image imRefPatern = extractPatern(refPoint, imageRef);
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
            astroT::Image imSrcPattern = imageSrc.getRoi(x0, y0, imRefPatern.getXSize(), imRefPatern.getYSize());
            
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
bool selectFirstPoint(const astroT::Image & currentImage, bool fullDynamic, cv::Point2f & starCenter)
{
    //---- Selection macroscopique
    cv::Mat imMat = astroT::imToCvMat(currentImage, fullDynamic);
    cv::Point2f selectedPoint;

    namedWindow("Display", cv::WINDOW_GUI_EXPANDED);
    cv::setMouseCallback("Display", CallBackFunc, &selectedPoint);

    cv::imshow("Display", imMat);
    cv::resizeWindow("Display", 640, 480);
    cv::waitKey(0);

    //---- Selection minutieuse
    cv::Point2f secondPoint;
    astroT::Image imZoom = extractPatern(selectedPoint, currentImage);
    astroT::dispIm(imZoom, true);

    //---- Detection de l'etoile
    //--- Binarisation
    //Image imSeuil = imZoom.seuillage(SEUIL_BIN);
    astroT::Image imSeuil = imZoom.seuillageMeanAuto();
    astroT::dispIm(imSeuil, false);

    //--- Detection de l'etoile
    astroT::sBoundingBox starBb;
    //astroT::ObjectDetector detector(imSeuil);
    astroT::ObjectDetector detector;

    if ( detector.detectBiggestObj(imSeuil, starBb) )
    {
        //std::cout << "### BBox pattern: X " << starBb.xMin << " -> " << starBb.xMax << ", Y " << starBb.yMin << " -> " << starBb.yMax << std::endl;
        astroT::Image imStar = imZoom.getRoi(starBb);
        
        //--- Detection du centre
        starCenter = computeStarCenter(imStar);
        //std::cout << "### star center : " << starCenter << std::endl;
        
        starCenter.x = starCenter.x - HALF_PATTERN_WIN + starBb.xMin + selectedPoint.x;
        starCenter.y = starCenter.y - HALF_PATTERN_WIN + starBb.yMin + selectedPoint.y;
        std::cout << "INFO   : Point de liaison : " << starCenter << std::endl;
        
        //astroT::zoomOnPoint(currentImage, starCenter, true);
        
    } else {
        
        std::cout << "ERREUR : Aucune etoile detectee" << std::endl;
        return false;
    }

    return true;
}


//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./wildFieldCompositor   <Repertoire des images>" << std::endl;
    std::cout << "                               <Format des images (RAW, JPG, TIF, ...)>" << std::endl;
    std::cout << "                               <Image master dark>" << std::endl;
    std::cout << "                               <Gradient VERTICAL, HORIZONTAL, NULL>" << std::endl;
    std::cout << "                               [<Masque premier plan>]" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //------------------------------------------------------------------
    //---- Gestion des arguments
    if ( (argc < 5) || (argc > 6) )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return -1;
    }

    bool flagMasque = false;
     std::string premierPlanPath = "";

    std::string imageDir = argv[1];
    std::string format = argv[2];
    std::string masterDarkPath = argv[3];
    std::string gradient = argv[4];

    //--- Gestion optionnelle du masque de premier plan
    if ( argc == 6 )
    {
        flagMasque = true;
        premierPlanPath = argv[5];
        std::cout << "argv4: " << argv[5] << std::endl;
        std::cout << "masque: " << premierPlanPath << std::endl;
    }
    //------------------------------------------------------------------
    

    //------------------------------------------------------------------
    //---- Test sur le format des images
    if ( (format != "RAW") && (format != "JPG") && (format != "TIF") )
    {
        std::cout << "ERREUR : Le format demande " << format << " n'est pas pris en compte" << std::endl;
        std::cout << "ERREUR : Les formats pris en compte sont RAW, JPG, TIF" << std::endl;
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
    std::cout << "INFO   : " << nbImages << " images a traiter" << std::endl;
    if ( nbImages < 2 )
    {
        std::cout << "ERREUR : Au moins deux images sont necessaires pour les traitements" << std::endl;
        return 1;
    }
    //------------------------------------------------------------------
    
    
    //------------------------------------------------------------------
    //---- Ouverture de la premiere image
    astroT::Image firstImage;
    if ( ! firstImage.load(vecImPath[0]) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[0] << std::endl;
        return 1;
    }
    std::cout << "INFO   : Premiere image chargee" << std::endl;
    //firstImage.disp();

    //--- Suppression du gradient
    if ( gradient == "VERTICAL" )
    {
        firstImage.removeVerticalGradient();
    } else if ( gradient == "HORIZONTAL" )
    {
        firstImage.removeHorizontalGradient();
    }
    //------------------------------------------------------------------
    

    //------------------------------------------------------------------
    //---- Chargement du master dark
    astroT::Image masterDark;
    if ( ! masterDark.load(masterDarkPath) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image de dark " << masterDarkPath << std::endl;
        return 1;
    }
    std::cout << "INFO   : Image du DARK chargee" << std::endl;
    //------------------------------------------------------------------
    
    
    //------------------------------------------------------------------
    //---- Chargement du masque premier plan si demande
    astroT::Image masquePremierPlan;
    astroT::Image imCardinal(firstImage.getXSize(), firstImage.getYSize());
    if ( flagMasque )
    {
        if ( ! masquePremierPlan.load(premierPlanPath) )
        {
            std::cout << "ERREUR : Echec lors du chargement du masque de premier plan " << premierPlanPath << std::endl;
            return 1;
        }
    }
    //------------------------------------------------------------------


    //------------------------------------------------------------------
    //---- Creation de l'image finale
    astroT::Image finalImage(firstImage);
    //------------------------------------------------------------------
    
    
    //------------------------------------------------------------------
    //---- Selection manuelle des points de liaison sur la premiere image
    std::cout << "INFO   : Selectionner les points de liaison sur la premiere image" << std::endl;
    std::vector<cv::Point2f> vecFirstImPoints;
    cv::Point2f currentPoint;
    for ( int indexPt = 0; indexPt < NB_LINK_POINTS; ++indexPt)
    {
        std::cout << "INFO   : Selection du point " << indexPt + 1 << " ..." << std::endl;
        if ( selectFirstPoint(firstImage, false, currentPoint) )
        {
            vecFirstImPoints.push_back(currentPoint);
        } else {
            std::cout << "ERREUR : Echec lors de la selection du point de liaison " << indexPt+1 << std::endl;
            return 1;
        }
    }
    //------------------------------------------------------------------
    
    
    //------------------------------------------------------------------
    //---- Recherche des points de liaison sur les autres images
    std::vector<cv::Point2f> vecPreviousImPoints = vecFirstImPoints;
    for ( int indexIm = 1; indexIm < nbImages; ++indexIm )
    {
        std::cout << "INFO   : Recalage image " << indexIm + 1 << " sur " << nbImages << std::endl;
        
        //--- Chargement de l'image courante
        astroT::Image currentIm;
        if ( ! currentIm.load(vecImPath[indexIm]) )
        {
            std::cout << "ERREUR : Echec lors du chargement de l'image " << vecImPath[indexIm] << std::endl;
            return 1;
        }
        
        //--- Recherche par correlation
        cv::Point2f currentFoundPoint;
        std::vector<cv::Point2f> vecCurrentFoundPoints;
        for (int indexPt = 0; indexPt < NB_LINK_POINTS; ++indexPt)
        {
            std::cout << "INFO   : Point de reference " << vecPreviousImPoints[indexPt].x << ", " << vecPreviousImPoints[indexPt].y << std::endl;
            currentFoundPoint = findPoint(firstImage, currentIm, vecFirstImPoints[indexPt], vecPreviousImPoints[indexPt]);
            vecCurrentFoundPoints.push_back(currentFoundPoint);
        }
        
        //--- Calcul de la transformation
        cv::Mat currentTransfo = cv::findHomography(vecCurrentFoundPoints, vecFirstImPoints, CV_RANSAC );
        std::cout << "INFO   : matrice de transfo " << currentTransfo << std::endl;
        
        //--- Suppression du gradient
        if ( gradient == "VERTICAL" )
        {
            currentIm.removeVerticalGradient();
        } else if ( gradient == "HORIZONTAL" )
        {
            currentIm.removeHorizontalGradient();
        }
        
        //--- Soustraction du dark
        currentIm.subtract(masterDark);
        
        //---- Resampling
        cv::Size taille(firstImage.getXSize(), firstImage.getYSize());
        cv::Mat correctedMat(taille, CV_16UC3);
        cv::Mat currentMat = astroT::imToCvMatAll(currentIm);
        cv::warpPerspective(currentMat, correctedMat, currentTransfo, taille);
        
        
        
        //---- Avec masque
        if ( flagMasque )
        {
            cv::Mat correctedMask(taille, CV_16UC3);
            cv::warpPerspective(astroT::imToCvMatAll(masquePremierPlan), correctedMask, currentTransfo, taille);
            finalImage.addWithMask(correctedMat, correctedMask, imCardinal);
            
        //--- Sans masque
        } else {
            finalImage.add(astroT::Image(correctedMat));
        }
        
        vecPreviousImPoints = vecCurrentFoundPoints;
    }

    std::cout << "max av div " << finalImage.getMaxRed() << ", " << finalImage.getMaxGreen() << ", " << finalImage.getMaxBlue() << std::endl;
    //------------------------------------------------------------------
    
    
    //------------------------------------------------------------------
    //---- Moyenne
    if ( flagMasque )
    {
        finalImage.divByIm(imCardinal, 1.0);
        
    } else {
        finalImage.div((double)nbImages);
    }

    astroT::dispIm(finalImage, true);
    //------------------------------------------------------------------
    
    
    //------------------------------------------------------------------
    //---- Sauvegarde image finale
    std::string finalImPath = imageDir + "/finalImage.TIF";
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
    
    //------------------------------------------------------------------
    

    return 0;

}

