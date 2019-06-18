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

#include "tiffio.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include "bitmap_image.hpp"

#include "libraw/libraw.h"

#include "jpeglib.h"

#include "Util.h"


#define CH_RED 0
#define CH_GREEN1 1
#define CH_BLUE 2
#define CH_GREEN2 3

#define MAX_16_BIT 65535
#define MAX_8_BIT 254

class Image
{
	
	public:
	
	enum Ecolor
	{
		RED,
		GREEN,
		BLUE
	};
	
	
	//---- Structure bounding box
	struct sBoundingBox
	{
		int xMin;
		int xMax;
		int yMin;
		int yMax;
	};
	
	
	//---- Constructeurs
	Image();
	Image(int dx, int dy);
	//Image(double * data, int dx, int dy);
	//Image(const Image & other);
	Image(const cv::Mat & imMat);

	//---- Destructeur
	~Image();
	
	//---- Operateur
	Image & operator=(const Image & other);
	Image & operator=(const cv::Mat & other);
	
	//---- Setteur
	void setDimensions(int dx, int dy);
	
	void setRedValue(double value, int index) { m_redData[index] = value; }
	void setGreenValue(double value, int index) { m_greenData[index] = value; }
	void setBlueValue(double value, int index) { m_blueData[index] = value; }
	
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
	
	
	
	
	
	//---- Creation d'une imagette avec origine pixellaire
	Image getRoi(int x0, int y0, int dx, int dy) const;
	Image getRoi(const Image::sBoundingBox & bBox) const;

	//---- Creation d'une imagette avec origine sub-pixellaire
	Image getRoiSub(double x0, double y0, int dx, int dy) const;
	
	//---- Interpolation bilineaire
	double interpolBil(double x, double y, Ecolor color) const;
	
	//---- Conversion en cv::Mat
	cv::Mat getCvMat() const;
	
	//---- Conversion en cv::Mat pour une image RAW
	cv::Mat getCvMatFromRaw(bool fullDynamic = false) const;
	
	//---- Conversion en cv::Mat pour raw et autres
	cv::Mat getCvMatNew(bool fullDynamic = false) const;
	
	//---- Incrustation d'une image dans l'image
	void incrust(const Image & imToIncrust, int x0, int y0);
	
	//---- Incrustation d'une image dans l'image en sous-pixel
	void incrustSub(const Image & imToIncrust, double x0, double y0);
	
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
	
	//---- Interpolation des donnees RAW pour le dematricage
	void interpolRaw(LibRaw & RawProcessor, int channel, double * buffer);
	
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
    
	//---- Calcul de la moyenne de 'limage
	double getMeanGreenImage() const;
	
	//---- Calcul du sigma de l'image
	double getSigmaGreenImage(double meanGreenImage) const;
	
	//---- Binarisation par seuillage
	Image seuillage(double seuil);
	
	//---- Detection d'objets sur image binaire
	std::vector<Image::sBoundingBox> detectObj() const;
	
	//---- Detection d'objets sur image binaire
	Image::sBoundingBox detectBiggestObj() const;
	
	//---- Retourne l'objet le plus gros
	Image::sBoundingBox biggestObj(const std::vector<Image::sBoundingBox> & vecObj) const;
	
	//---- Calcul le profil suivant une ligne
	std::vector<double> lineProfil(int line, int colMin, int colMax);
	
	//---- Calcul le profil suivant une colonne
	std::vector<double> colProfil(int col, int lineMin, int lineMax);
	
	//---- Application d'un filtre laplacien
	void computeLaplacian(int kernelSize, int scale, int delta);
	
	//---- Application d'un filtre de sobel
	void computeSobel(int kernelSize, int scale, int delta);
	
	//---- Application d'une gaussienne
	Image computeGauss(int xKernel, int yKernel);
	
	//---- Affichage a l'ecran
	int disp(bool fullDynamic = false) const;
	//void disp(bool fullRange, bool color = false) const;
	
	//---- Affichage a l'ecran d'une image RAW
	//int dispRaw() const;
	
	//---- Affichage a l'ecran de ROI
	int dispRoi(const std::vector<sBoundingBox> & bBox) const;
	int dispRoi(const sBoundingBox & bBox) const;
	
	//---- Zoom centre sur un point donne
	void zoomOnPoint(const cv::Point & pt, bool fullDynamic = false) const;
	
	//---- Retourne une image zoom centre sur un point donne
	Image getZoomOnPoint(const cv::Point & pt) const;
	
	//---- Calcul du centre d'une etoile
	cv::Point2f computeStarCenter();
	
	//---- Selection d'un point dans une image
	cv::Point2i selectPoint();
	
	//---- Calcul de l'histogramme de l'image
	std::vector<double> computeHisto(int nbClasse);
	
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


#endif 
