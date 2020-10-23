//----------------------------------------------------------------------
//
//---- Outil readRaw : lecture d'une image RAW et test sur le
//                     post processing
//
//----------------------------------------------------------------------

#include <iostream>
#include <cstring>

#include "libraw/libraw.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/nonfree/features2d.hpp>

#define MAX_8_BIT 255.0
#define MAX_16_BIT 65535.0
#define MAX_TEST 15000.0

#define CH_RED 0
#define CH_GREEN1 1
#define CH_BLUE 2
#define CH_GREEN2 3


//----------------------------------------------------------------------
//---- Affichage des buffers RGB
//----------------------------------------------------------------------
void displayRgb(double * redData, double * greenData, double * blueData, int dx, int dy)
{

    cv::Mat imMat(dy, dx, CV_8UC3);
    cv::Vec3b currentPix;
    double currentRed, currentGreen, currentBlue;

    //-- Conversion radio
    int index = 0;
    for (int ligne = 0; ligne < dy; ++ligne)
    {
        for (int col = 0; col < dx; ++col)
        {
            //--- Canal rouge
            currentRed = redData[index] * MAX_8_BIT / MAX_TEST;
            if ( currentRed > MAX_8_BIT ) { currentRed = MAX_8_BIT; }
            currentPix[2] = std::round(currentRed);
            
            //--- Canal vert
            currentGreen = greenData[index] * MAX_8_BIT / MAX_TEST;
            if ( currentGreen > MAX_8_BIT ) { currentGreen = MAX_8_BIT; }
            currentPix[1] = std::round(currentGreen);
            
            //--- Canal bleu
            currentBlue = blueData[index] * MAX_8_BIT / MAX_TEST;
            if ( currentBlue > MAX_8_BIT ) { currentBlue = MAX_8_BIT; }
            currentPix[0] = std::round(currentBlue);
            
            
            index += 1;
            
            imMat.at<cv::Vec3b>(ligne, col) = currentPix;
        }
    }

    namedWindow("Display", cv::WINDOW_NORMAL);
    cv::imshow("Display", imMat);

    cv::waitKey(0);

    cv::destroyWindow("Display");
    
    //~ if ( !imwrite(imOutPath, imMat) )
    //~ {
        //~ std::cout << "ERREUR : Echec lors de l'ecriture sur disque de l'image " << imOutPath << std::endl;
    //~ }

}


//----------------------------------------------------------------------
//---- Affichage d'un buffer
//----------------------------------------------------------------------
void displayBuffer(double * buffer, int dx, int dy, double maxVal, bool fullDynamic)
{
    cv::Mat imMat(dy, dx, CV_8UC3);
    cv::Vec3b currentPix;

    //-- Conversion radio
    int index = 0;
    for (int ligne = 0; ligne < dy; ++ligne)
    {
        for (int col = 0; col < dx; ++col)
        {
            if ( fullDynamic )
            {
                currentPix[2] = std::round(buffer[index] * MAX_8_BIT / maxVal);
                currentPix[1] = std::round(buffer[index] * MAX_8_BIT / maxVal);
                currentPix[0] = std::round(buffer[index] * MAX_8_BIT / maxVal);
                
            } else {
                
                if (buffer[index] > MAX_8_BIT )
                {
                    currentPix[2] = MAX_8_BIT;
                    currentPix[1] = MAX_8_BIT;
                    currentPix[0] = MAX_8_BIT;
                    
                } else {
                    currentPix[2] = std::round(buffer[index]);
                    currentPix[1] = std::round(buffer[index]);
                    currentPix[0] = std::round(buffer[index]);
                }
                
            }
            
            index += 1;
            
            imMat.at<cv::Vec3b>(ligne, col) = currentPix;
        }
    }

    namedWindow("Display", cv::WINDOW_NORMAL);
    cv::imshow("Display", imMat);

    cv::waitKey(0);

    cv::destroyWindow("Display");

}


//----------------------------------------------------------------------
//---- Affichage des parametres
//----------------------------------------------------------------------
void displayParams(LibRaw RawProcessor)
{
    
    std::cout << "INFO   : Parametres:" << std::endl;
    std::cout << "INFO   : Output color " << RawProcessor.imgdata.params.output_color << std::endl;
    std::cout << "INFO   : Interpolateur " << RawProcessor.imgdata.params.user_qual << std::endl;
    std::cout << "INFO   : Profondeur " << RawProcessor.imgdata.params.output_bps << std::endl;
    std::cout << "INFO   : Brightness " << RawProcessor.imgdata.params.bright << std::endl;
    std::cout << "INFO   : Auto WB " << RawProcessor.imgdata.params.use_auto_wb << std::endl;
    std::cout << "INFO   : Camera WB " << RawProcessor.imgdata.params.use_camera_wb << std::endl;
    std::cout << "INFO   : Automatic increase brightness " << RawProcessor.imgdata.params.no_auto_bright << std::endl;
    std::cout << "INFO   : Iteration filtre median " << RawProcessor.imgdata.params.med_passes << std::endl;
    std::cout << "INFO   : Reduction de bruit fbdd " << RawProcessor.imgdata.params.fbdd_noiserd << std::endl;
}


//----------------------------------------------------------------------
//---- Chargement de l'image
//----------------------------------------------------------------------
bool loadRaw(const std::string & imPath)
{
    std::cout << "INFO   : Chargement de l'image " << imPath << " ..." << std::endl;

    //---- Ouverture du fichier
    LibRaw RawProcessor;
    if ( RawProcessor.open_file(imPath.c_str()) != LIBRAW_SUCCESS )
    {
        std::cout << "ERROR : Impossible d'ouvrir l'image " << imPath << std::endl;
        return false;
    }

    //--- Taille de l'image
    int dx = RawProcessor.imgdata.sizes.iwidth;
    int dy = RawProcessor.imgdata.sizes.iheight;
    int nbPixel = dx * dy;

    //---- Allocation memoire
    double * redData = new double[nbPixel];
    double * greenData = new double[nbPixel];
    double * blueData = new double[nbPixel];

    //---- Extraction des donnees brutes
    if ( RawProcessor.unpack() != LIBRAW_SUCCESS )
    {
        std::cout << "ERROR : Impossible d'extraire les donnees brutes" << std::endl;
        return false;
    }


    //---- Options
    //--- Espace colorimetrique de sortie (0-6) : raw, sRGB, Adobe, Wide, ProPhoto, XYZ, ACES
    RawProcessor.imgdata.params.output_color = 6;
    //--- Interpolation (0, 1, 2, 3, 4, 11, 12) : linear, VGN, PPG, AHD, DCB, DHT, AAHD
    RawProcessor.imgdata.params.user_qual = 12;
    //--- Dynamique de sortie (8, 16) : utile si on exporte directement une image
    //RawProcessor.imgdata.params.output_bps = 16;
    //--- Brightness default 1.0
    //RawProcessor.imgdata.params.bright = 1.0;
    //--- Balance des blancs automatique
    //RawProcessor.imgdata.params.use_auto_wb = 1;
    //--- Balance des blancs camera
    //RawProcessor.imgdata.params.use_camera_wb = 1;
    //--- Augmentation automatique de la luminosote (0, 1) : oui, non
    RawProcessor.imgdata.params.no_auto_bright = 1;
    //--- Nombre de passage du filtre median
    //RawProcessor.imgdata.params.med_passes = 0;
    //--- Reduction de bruit fbdd (0, 1, 2) : non, light, full
    //RawProcessor.imgdata.params.fbdd_noiserd = 0;

    //---- Decoupe des donnees par couleur
    RawProcessor.dcraw_process();

    //--- Parcours des pixels
    double maxRed = 0.0;
    double maxGreen = 0.0;
    double maxBlue = 0.0;
    double currentVal;
    for (int index = 0; index < nbPixel; ++index)
    {
        //-- Canal rouge
        currentVal = (double)RawProcessor.imgdata.image[index][CH_RED];
        redData[index] = currentVal;
        if ( currentVal > maxRed ) { maxRed = currentVal; }
        
        //-- Canal vert 
        currentVal = (double)RawProcessor.imgdata.image[index][CH_GREEN1];
        greenData[index] = currentVal;
        if ( currentVal > maxGreen ) { maxGreen = currentVal; }
        
        //-- Canal bleu
        currentVal = (double)RawProcessor.imgdata.image[index][CH_BLUE];
        blueData[index] = currentVal;
        if ( currentVal > maxBlue ) { maxBlue = currentVal; }
    }

    //---- Affichage des parametres
    //displayParams(RawProcessor);

    std::cout << "INFO   : Max RGB: " << maxRed << ", " << maxGreen << ", " << maxBlue << std::endl;

    //displayBuffer(redData, dx, dy, maxRed, true);
    //displayBuffer(greenData, dx, dy, maxGreen, true);
    //displayBuffer(blueData, dx, dy, maxBlue, true);

    displayRgb(redData, greenData, blueData, dx, dy);

    std::cout << "INFO   : Chargement termine" << std::endl;


    delete []  redData;
    delete []  greenData;
    delete []  blueData;


    return true;
}


//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: ./readRaw <Image RAW>" << std::endl;
    //std::cout << "                 <Output>" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char ** argv)
{
    //---- Gestion des arguments
    if ( argc != 2 )
    {
        std::cout << "ERREUR : Nombre d'argument incorrect" << std::endl;
        usage();
        return 1;
    }

    std::string imPath = argv[1];
    //std::string imOutPath = argv[2];

    //---- Chargement de l'image
    if ( ! loadRaw(imPath) )
    {
        std::cout << "ERREUR : Echec lors du chargement de l'image " << imPath << std::endl;
        return 1;
    }

    return 0;
}
