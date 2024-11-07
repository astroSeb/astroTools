#include "Display.h"

namespace astroT
{


//----------------------------------------------------------------------
//---- Mise en forme de la couleur
//----------------------------------------------------------------------
cv::Scalar setColor(const Ecolor & couleur)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    
    switch ( couleur )
    {
        case RED:
            r = 254;
            break;
            
        case GREEN:
            g = 254;
            break;
            
        case BLUE:
            b = 254;
            break;
            
        case WHITE:
            r = 254;
            g = 254;
            b = 254;
            break;
            
        case BLACK:
            r = 0;
            g = 0;
            b = 0;
    }
    
    return cv::Scalar(b, g, r);
}


//----------------------------------------------------------------------
//---- Ajout d'un point de couleur
//----------------------------------------------------------------------
void addColorPoint(cv::Mat & currentIm, const sPointD & pos, const Ecolor & couleur)
{
    cv::Point pt(pos.x, pos.y);
    
    cv::Scalar currentColor = setColor(couleur);
    
    circle(currentIm, pt, 3, currentColor, -1);
    
}


//----------------------------------------------------------------------
//---- Ajout d'une ligne de couleur
//----------------------------------------------------------------------
void addColorLine(cv::Mat & currentIm, const sPointD & pos1, const sPointD & pos2, const Ecolor & couleur)
{
    cv::Point pt1(pos1.x, pos1.y);
    cv::Point pt2(pos2.x, pos2.y);
    
    cv::Scalar currentColor = setColor(couleur);
    
    line(currentIm, pt1, pt2, currentColor);
    
}


//----------------------------------------------------------------------
//---- Ajout d'une croix de detection
//----------------------------------------------------------------------
void addColorCross(cv::Mat & currentIm, const sPointD & pos, const Ecolor & couleur)
{
    sPointD pt1(pos.x-5, pos.y);
    sPointD pt2(pos.x+5, pos.y);
    addColorLine(currentIm, pt1, pt2, couleur);
    
    sPointD pt3(pos.x, pos.y-5);
    sPointD pt4(pos.x, pos.y+5);
    addColorLine(currentIm, pt3, pt4, couleur);
    
}


//----------------------------------------------------------------------
//---- Ajout d'un rectangle de couleur
//----------------------------------------------------------------------
void addColorRectangle(cv::Mat & currentIm, const sBoundingBox & roi, const Ecolor & couleur)
{
    sPointD UL(roi.xMin, roi.yMax);
    sPointD UR(roi.xMax, roi.yMax);
    sPointD LR(roi.xMax, roi.yMin);
    sPointD LL(roi.xMin, roi.yMin);
    
    addColorLine(currentIm, UL, UR, couleur);
    addColorLine(currentIm, UR, LR, couleur);
    addColorLine(currentIm, LR, LL, couleur);
    addColorLine(currentIm, LL, UL, couleur);
    
}


void addColorRectangle(cv::Mat & currentIm, const sPointD & centre, int rayon, const Ecolor & couleur)
{
    sPointD UL(centre.x - rayon, centre.y + rayon);
    sPointD UR(centre.x + rayon, centre.y + rayon);
    sPointD LR(centre.x + rayon, centre.y - rayon);
    sPointD LL(centre.x - rayon, centre.y - rayon);
    
    addColorLine(currentIm, UL, UR, couleur);
    addColorLine(currentIm, UR, LR, couleur);
    addColorLine(currentIm, LR, LL, couleur);
    addColorLine(currentIm, LL, UL, couleur);
    
}











//----------------------------------------------------------------------
//---- Conversion Image vers cv::Mat 8 bits
//----------------------------------------------------------------------
cv::Mat imToCvMat(const Image & im, bool fullDynamic)
{
    int dx = im.getXSize();
    int dy = im.getYSize();
    
    cv::Mat imMat(dy, dx, CV_8UC3);
    cv::Vec3b currentPix;
    int index = 0;
    
    double maxRed = im.getMaxRed();
    double maxGreen = im.getMaxGreen();
    double maxBlue = im.getMaxBlue();
    
    
    //---- Cas ou l'image vient d'un RAW
    if ( im.isRaw() )
    {
        //--- On prends toute la plage dynamique
        if ( fullDynamic )
        {
            //-- Test
            if ( (maxRed == 0.0 ) || (maxGreen == 0.0) || (maxBlue == 0.0) )
            {
                std::cout << "WARN   : Une des valeurs RGB max de l'image est a 0 -> Probleme de dynamique" << std::endl;
            }
            
            //-- Conversion
            int index = 0;
            for (int ligne = 0; ligne < dy; ++ligne)
            {
                for (int col = 0; col < dx; ++col)
                {
                    currentPix[2] = im.getRedValue8b(col, ligne, true);
                    currentPix[1] = im.getGreenValue8b(col, ligne, true);
                    currentPix[0] = im.getBlueValue8b(col, ligne, true);
                    index += 1;
                    
                    imMat.at<cv::Vec3b>(ligne, col) = currentPix;
                }
            }
        
        //--- On etire la dynamique a max / 4
        } else {
            //---- Ajustement radiometrique
            int index = 0;
            for (int y = 0; y < dy; ++y)
            {
                for (int x = 0; x < dx; ++x)
                {
                    
                    currentPix[2] = std::round(im.getRedValue(index) * MAX_8_BIT / (maxRed / 4.0));
                    currentPix[1] = std::round(im.getGreenValue(index) * MAX_8_BIT / (maxGreen / 4.0));
                    currentPix[0] = std::round(im.getBlueValue(index) * MAX_8_BIT / (maxBlue / 4.0));
                    
                    imMat.at<cv::Vec3b>(y, x) = currentPix;
                    
                    index += 1;
                }
            }
        
        }
        
        
    //---- Cas ou l'image vient d'un tiff ou JPG
    } else {
        
        if ( fullDynamic )
        {
            //-- Test
            if ( (maxRed == 0.0 ) || (maxGreen == 0.0) || (maxBlue == 0.0) )
            {
                std::cout << "WARN   : Une des valeurs RGB max de l'image est a 0 -> Probleme de dynamique" << std::endl;
            }
            
            //--- Parcours de l'image
            for (int ligne = 0; ligne < dy; ++ligne)
            {
                for (int col = 0; col < dx; ++col)
                {
                    currentPix[2] = std::round(im.getRedValue(index) * MAX_8_BIT / maxRed);
                    currentPix[1] = std::round(im.getGreenValue(index) * MAX_8_BIT / maxGreen);
                    currentPix[0] = std::round(im.getBlueValue(index) * MAX_8_BIT / maxBlue);
                    index += 1;
                    
                    imMat.at<cv::Vec3b>(ligne, col) = currentPix;
                }
            }
        } else {
            
            //--- Parcours de l'image
            for (int ligne = 0; ligne < dy; ++ligne)
            {
                for (int col = 0; col < dx; ++col)
                {
                    currentPix[2] = std::round(im.getRedValue(index));
                    currentPix[1] = std::round(im.getGreenValue(index));
                    currentPix[0] = std::round(im.getBlueValue(index));
                    index += 1;
                    
                    imMat.at<cv::Vec3b>(ligne, col) = currentPix;
                }
            }
        }
    }

    return imMat;

}


//----------------------------------------------------------------------
//---- Conversion image -> cv::Mat avec toute la dynamique
//----------------------------------------------------------------------
cv::Mat imToCvMatAll(const Image & im)
{
    cv::Mat imMat;
    int dx = im.getXSize();
    int dy = im.getYSize();
    double currentVal;
    
    //---- Cas d'une image raw (16 bits)-> CV_16UC3
    if ( im.isRaw() )
    {
        imMat = cv::Mat(dy, dx, CV_16UC3);
        
        //--- Parcours des pixels
        cv::Vec3w currentPix;
        int index = 0;
        
        for (int ligne = 0; ligne < dy; ++ligne)
        {
            for (int col = 0; col < dx; ++col)
            {
                //-- Canal rouge
                currentVal = im.getRedValue(index);
                if ( currentVal > MAX_16_BIT )
                {
                    currentPix[2] = (ushort)MAX_16_BIT;
                } else {
                    currentPix[2] = (ushort)currentVal;
                }
                
                //-- Canal vert
                currentVal = im.getGreenValue(index);
                if ( currentVal > MAX_16_BIT )
                {
                    currentPix[1] = (ushort)MAX_16_BIT;
                } else {
                    currentPix[1] = (ushort)currentVal;
                }
                
                //-- Canal bleu
                currentVal = im.getBlueValue(index);
                if ( currentVal > MAX_16_BIT )
                {
                    currentPix[0] = (ushort)MAX_16_BIT;
                } else {
                    currentPix[0] = (ushort)currentVal;
                }
                
                index += 1;
                
                imMat.at<cv::Vec3w>(ligne, col) = currentPix;
            }
        }
    
    //---- Cas d'une image 8 bits
    } else {
        
        imMat = cv::Mat(dy, dx, CV_8UC3);
        
        //--- Parcours des pixels
        cv::Vec3b currentPix;
        int index = 0;
        
        for (int ligne = 0; ligne < dy; ++ligne)
        {
            for (int col = 0; col < dx; ++col)
            {
                //-- Canal rouge
                currentVal = im.getRedValue(index);
                if ( currentVal > MAX_8_BIT )
                {
                    currentPix[2] = (uchar)MAX_8_BIT;
                } else {
                    currentPix[2] = (uchar)std::round(currentVal);
                }
                
                //-- Canal vert
                currentVal = im.getGreenValue(index);
                if ( currentVal > MAX_8_BIT )
                {
                    currentPix[1] = (uchar)MAX_8_BIT;
                } else {
                    currentPix[1] = (uchar)std::round(currentVal);
                }
                
                //-- Canal bleu
                currentVal = im.getBlueValue(index);
                if ( currentVal > MAX_8_BIT )
                {
                    currentPix[0] = (uchar)MAX_8_BIT;
                } else {
                    currentPix[0] = (uchar)std::round(currentVal);
                }
                
                index += 1;
                
                imMat.at<cv::Vec3b>(ligne, col) = currentPix;
            }
        }
    }
    
    return imMat;
}


//----------------------------------------------------------------------
//---- Affichage ecran d'une image
//----------------------------------------------------------------------
int dispIm(const Image & im, bool fullRange)
{
    //---- Conversion au format openCV en 8 bits
    cv::Mat currentIm = imToCvMat(im, fullRange);

    namedWindow("Display", cv::WINDOW_GUI_EXPANDED);
    cv::imshow("Display", currentIm);
    cv::resizeWindow("Display", 640, 480);

    int key = cv::waitKey(0);

    cv::destroyWindow("Display");

    return key;
}


//----------------------------------------------------------------------
//---- Zoom centre sur un point donne
//----------------------------------------------------------------------
int zoomOnPoint(const Image & im, const cv::Point & pt, bool fullDynamic)
{
    cv::Mat imMat = imToCvMat(im, fullDynamic);
    
    addColorLine(imMat, sPointD(0, 10), sPointD(20, 10), RED);
    addColorLine(imMat, sPointD(10, 0), sPointD(10, 20), RED);
    
    namedWindow("Display", cv::WINDOW_NORMAL);
    cv::imshow("Display", imMat);

    int key = cv::waitKey(0);

    cv::destroyWindow("Display");

    return key;
}


//----------------------------------------------------------------------
//---- Affichage de ROI dans l'image
//----------------------------------------------------------------------
int dispRoi(const Image & im, const std::vector<sBoundingBox> & vecRoi, bool fullRange)
{
    //---- Conversion au format openCV en 8 bits
    cv::Mat currentIm = imToCvMat(im, fullRange);
    
    for (std::vector<sBoundingBox>::const_iterator iRoi = vecRoi.begin(); iRoi != vecRoi.end(); ++iRoi)
    {
        addColorRectangle(currentIm, *iRoi, GREEN);
    }
    
    namedWindow("Display", cv::WINDOW_NORMAL);
    cv::imshow("Display", currentIm);

    int key = cv::waitKey(0);

    cv::destroyWindow("Display");

    return key;
}



//---- Fin namespace astroT
}
