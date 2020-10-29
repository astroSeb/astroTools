//----------------------------------------------------------------------
//
//----  Display : Receuil des fonctions d'affichage d'images
//
//----------------------------------------------------------------------

#ifndef DISPLAY_H
#define DISPLAY_H

#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>

#include "Image.h"


namespace astroT
{
    
    //---- Mise en forme de la couleur
    cv::Scalar setColor(const Ecolor & couleur);

    //---- Ajout d'un point de couleur
    void addColorPoint(cv::Mat & currentIm, const sPointD & pos, const Ecolor & couleur);
    
    //---- Ajout d'une ligne de couleur
    void addColorLine(cv::Mat & currentIm, const sPointD & pos1, const sPointD & pos2, const Ecolor & couleur);
    
    //---- Ajout d'une croix de detection
    void addColorCross(cv::Mat & currentIm, const sPointD & pos, const Ecolor & couleur);
    
    //---- Ajout d'un rectangle de couleur
    void addColorRectangle(cv::Mat & currentIm, const sBoundingBox & roi, const Ecolor & couleur);
    void addColorRectangle(cv::Mat & currentIm, const sPointD & centre, int rayon, const Ecolor & couleur);
    
    //---- Conversion Image -> cv::Mat
    cv::Mat imToCvMat(const Image & im, bool fullDynamic);
    cv::Mat imToCvMatAll(const Image & im);
    
    //---- Affichage ecran d'une image
    int dispIm(const Image & im, bool fullRange);
    
    //---- Zoom centre sur un point donne
    int zoomOnPoint(const Image & im, const cv::Point & pt, bool fullDynamic);
    
    //---- Affichage de ROI dans l'image
    int dispRoi(const Image & im, const std::vector<sBoundingBox> & vecRoi, bool fullRange);
    
    
    
//---- Fin namespace astroT
}

#endif
