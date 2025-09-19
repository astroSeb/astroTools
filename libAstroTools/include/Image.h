//----------------------------------------------------------------------
//
//---- Classe Image
//
//----------------------------------------------------------------------


#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstring>
#include <cmath>
#include <limits>
#include <vector>

//~ #include <opencv2/opencv.hpp>
//~ #include <opencv2/core/core.hpp>
//~ #include <opencv2/highgui/highgui.hpp>
//~ #include <opencv2/imgproc/imgproc.hpp>
//~ #include <opencv2/contrib/contrib.hpp>
//~ #include <opencv2/nonfree/features2d.hpp>

#include "tiffio.h"
#include "bitmap_image.hpp"
#include "libraw/libraw.h"
#include "jpeglib.h"

#include "Util.h"
#include "AstroTypeDef.h"


#define CH_RED 0
#define CH_GREEN1 1
#define CH_BLUE 2
#define CH_GREEN2 3

namespace astroT
{

class Image
{
    
    public:

    //---- Constructeurs
    Image();
    Image(int dx, int dy);
    Image(const Image & other);
    Image(const cv::Mat & imMat);

    //---- Destructeur
    ~Image();

    //---- Setteur
    void setDimensions(int dx, int dy);

    void setRedValue(double value, int index) { m_redData[index] = value; }
    void setGreenValue(double value, int index) { m_greenData[index] = value; }
    void setBlueValue(double value, int index) { m_blueData[index] = value; }
    void setValues(double rouge, double vert, double bleu, int index);
    void setRaw() { m_isRaw = true; }

    //---- Getteur
    int getXSize() const { return m_dx; }
    int getYSize() const { return m_dy; }
    int getNbPixel() const { return m_nbPixel; }

    inline double getRedValue(int index) const { return m_redData[index]; }
    inline double getGreenValue(int index) const { return m_greenData[index]; }
    inline double getBlueValue(int index) const { return m_blueData[index]; }
    inline double getMeanValue(int index) const { return (m_redData[index] + m_greenData[index] + m_blueData[index]) / 3.0; }

    double getMaxRed() const { return m_maxRed; }
    double getMaxGreen() const { return m_maxGreen; }
    double getMaxBlue() const { return m_maxBlue; }

    double getMeanRed() const { return m_meanRed; }
    double getMeanGreen() const { return m_meanGreen; }
    double getMeanBlue() const { return m_meanBlue; }
    double getMeanAll() const { return (m_meanRed + m_meanGreen + m_meanBlue) / 3.0; }

    int8_t getRedValue8b(int col, int row, bool fullDynamic = false) const;
    int8_t getGreenValue8b(int col, int row, bool fullDynamic = false) const;
    int8_t getBlueValue8b(int col, int row, bool fullDynamic = false) const;


    double * getRedBuffer() const { return m_redData; }
    double * getGreenBuffer() const { return m_greenData;}
    double * getBlueBuffer() const { return m_blueData; }
    
    
    //---- L'image provient-elle d'un raw
    bool isRaw() const { return m_isRaw; }

    //---- Lecture d'une image
    bool load(const std::string & filePath);

    //---- Lecture d'une image tiff
    bool loadTiff(const std::string & filePath);

    //---- Lecture d'une image bmp
    bool loadBmp(const std::string & filePath);

    //---- Lecture d'une ligne image dans un fichier tiff
    void loadTiffLine(TIFF * tifFile, int lineNumber);

    //---- Lecture d'une image raw nikon (.NEF)
    bool loadRaw(const std::string & filePath);

    //---- Lectude d'une image jpeg
    bool loadJpeg(const std::string & filePath);

    //---- Lectude d'une ligne image jpeg
    bool loadJpegLine(const std::string & filePath, int indexLine);

    //---- Ecriture d'une image en tiff 16 bits
    bool writeImTiff16b(const std::string & filePath);
    
    //---- Ecriture d'une image en tiff 8 bits
    bool writeImTiff8b(const std::string & filePath);
    
    //---- Creation d'une imagette avec origine pixellaire
    Image getRoi(int x0, int y0, int dx, int dy) const;
    Image getRoi(const sBoundingBox & bBox) const;

    //---- Creation d'une imagette avec origine sub-pixellaire
    Image getRoiSub(double x0, double y0, int dx, int dy) const;

    //---- Interpolation bilineaire
    double interpolBil(double x, double y, Ecolor color) const;

    //---- Addition pixel a pixel de 2 images
    bool add(const Image & im);

    //---- Addition pixel a pixel de 2 images avec masque
    bool addWithMask(const Image & im, const Image & mask, Image & imCardinal);

    //---- Soustraction de l'image par une autre
    bool subtract(const Image & im);

    //---- Division de l'ensemble des pixels l'image
    bool div(double denominateur);

    //---- Division pixel a pixel d'une image par une autre
    bool divByIm(const Image & imDiv, double biais = 0.0);
    
    //---- Binarisation par seuillage
    Image seuillage(double seuil) const;
    
    //---- Binarisation par seuillage automatique sur la moyenne du canal vert
    Image seuillageMeanAuto();
    
    //---- Calcul le profil suivant une ligne
    std::vector<double> lineProfil(int line, int colMin, int colMax);

    //---- Calcul le profil suivant une colonne
    std::vector<double> colProfil(int col, int lineMin, int lineMax);

    //---- Calcul de l'histogramme de l'image
    std::vector<double> computeHisto(int nbClasse) const;

    //---- Calcul d'un gradient vertical
    void computeVerticalGradient(double & redCoeff, double & greenCoeff, double & blueCoeff);
    
    //---- Suppression d'un gradient vertical
    void removeVerticalGradient();
    
    //---- Calcul d'un gradient horizontal
    void computeHorizontalGradient(double & redCoeff, double & greenCoeff, double & blueCoeff);
    
    //---- Suppression d'un gradient horizontal
    void removeHorizontalGradient();
    
    //---- Merge with mask
    void mergeWithMask(const Image & firstPlan, const Image & mask);

    //---- Calcul statistiques du fond du ciel
    void computeBgStats(std::vector<double> & vecBgMean, std::vector<double> & vecBgSigma);

    //---- Calcul de la variance dans un bounding box
    float computeVarianceRoi(const sBoundingBox & roi);
    
    
protected:

    //---- Donnees membres
    double * m_redData;
    double * m_greenData;
    double * m_blueData;

    int m_dx;
    int m_dy;
    int m_nbPixel;

    bool m_isRaw;

    double m_maxRed;
    double m_maxGreen;
    double m_maxBlue;

    double m_meanRed;
    double m_meanGreen;
    double m_meanBlue;

};

//---- Fin namespace astroT
}

#endif 
