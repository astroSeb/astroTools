#include "Image.h"

//----------------------------------------------------------------------
//---- Constructeurs
//----------------------------------------------------------------------
Image::Image()
{
	m_redData = NULL;
	m_greenData = NULL;
	m_blueData = NULL;
	
	m_dx = 0;
	m_dy = 0;
	m_nbPixel = 0;
	
	m_isRaw = false;
	
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
}


Image::Image(int dx, int dy)
{
	m_dx = dx;
	m_dy = dy;
	m_nbPixel = dx * dy;
	
	//---- Allocation memoire
	m_redData = new double[dx*dy];
	memset(m_redData, 0, dx*dy*sizeof(double));
	
	m_greenData = new double[dx*dy];
	memset(m_greenData, 0, dx*dy*sizeof(double));
	
	m_blueData = new double[dx*dy];
	memset(m_blueData, 0, dx*dy*sizeof(double));
	
	m_isRaw = false;
	
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
}


Image::Image(const Image & other)
{

	m_dx = other.m_dx;
	m_dy = other.m_dy;
	m_nbPixel = other.m_nbPixel;
    

	if (other.m_redData != NULL)
	{
        m_redData = new double[m_nbPixel];
		memcpy(m_redData, other.m_redData, sizeof(double) * m_nbPixel);
	}
	
	if (other.m_greenData != NULL)
	{
        m_greenData = new double[m_nbPixel];
		memcpy(m_greenData, other.m_greenData, sizeof(double) * m_nbPixel);
	}
	
	if (other.m_blueData != NULL)
	{
        m_blueData = new double[m_nbPixel];
		memcpy(m_blueData, other.m_blueData, sizeof(double) * m_nbPixel);
	}
	
	m_isRaw = other.m_isRaw;
	
	m_maxRed = other.m_maxRed;
	m_maxGreen = other.m_maxGreen;
	m_maxBlue = other.m_maxBlue;
	
	m_meanRed = other.m_meanRed;
	m_meanGreen = other.m_meanGreen;
	m_meanBlue = other.m_meanBlue;
	
}


Image::Image(const cv::Mat & imMat)
{
    m_dx = imMat.cols;
    m_dy = imMat.rows;
    m_nbPixel = m_dx * m_dy;

    //---- Allocation memoire
    m_redData = new double[m_nbPixel];
    m_greenData = new double[m_nbPixel];
    m_blueData = new double[m_nbPixel];


    m_maxRed = 0.0;
    m_maxGreen = 0.0;
    m_maxBlue = 0.0;

    m_meanRed = 0.0;
    m_meanGreen = 0.0;
    m_meanBlue = 0.0;

    //---- Type de cv::Mat
    //--- CV_8UC3
    if ( imMat.type() == 16 )
    {
        //-- Parcours des pixels
        cv::Vec3b currentPix;
        for (int index = 0; index < m_nbPixel; ++index)
        {
            currentPix = imMat.at<cv::Vec3b>(index);
            
            m_redData[index] = currentPix[2];
            m_greenData[index] = currentPix[1];
            m_blueData[index] = currentPix[0];
            
            if ( currentPix[2] > m_maxRed ) { m_maxRed = currentPix[2]; }
            if ( currentPix[1] > m_maxGreen ) { m_maxGreen = currentPix[1]; }
            if ( currentPix[0] > m_maxBlue ) { m_maxBlue = currentPix[0]; }
            
            m_meanRed += currentPix[2];
            m_meanGreen += currentPix[1];
            m_meanBlue += currentPix[0];
            
        }
        
        m_isRaw = false;
    
    //--- CV_16UC3
    } else if ( imMat.type() == 18 )
    {
        //-- Parcours des pixels
        cv::Vec3w currentPix;
        for (int index = 0; index < m_nbPixel; ++index)
        {
            currentPix = imMat.at<cv::Vec3w>(index);
            
            m_redData[index] = currentPix[2];
            m_greenData[index] = currentPix[1];
            m_blueData[index] = currentPix[0];
            
            if ( currentPix[2] > m_maxRed ) { m_maxRed = currentPix[2]; }
            if ( currentPix[1] > m_maxGreen ) { m_maxGreen = currentPix[1]; }
            if ( currentPix[0] > m_maxBlue ) { m_maxBlue = currentPix[0]; }
            
            m_meanRed += currentPix[2];
            m_meanGreen += currentPix[1];
            m_meanBlue += currentPix[0];
            
        }
        
        m_isRaw = true;
    
    } else {
        
        std::cout << "ERREUR : Type de cv::Mat " << imMat.type() << " inconnu" << std::endl;
        return;
    }
    
    m_meanRed /= m_nbPixel;
    m_meanGreen /= m_nbPixel;
    m_meanBlue /= m_nbPixel;
    
}


//----------------------------------------------------------------------
//---- Desctructeur
//----------------------------------------------------------------------
Image::~Image()
{
	if ( m_redData != NULL )
	{
		delete [] m_redData;
	}
	
	if ( m_greenData != NULL )
	{
		delete [] m_greenData;
	}
	
	if ( m_blueData != NULL )
	{
		delete [] m_blueData;
	}
	
}


//----------------------------------------------------------------------
//---- Ajustement des dimensions et allocation memoire
//----------------------------------------------------------------------
void Image::setDimensions(int dx, int dy)
{
	m_dx = dx;
	m_dy = dy;
	m_nbPixel = m_dx * m_dy;
	
	//---- Test memoire deja allouee
	if ( m_redData != NULL ) { delete [] m_redData; }
	if ( m_greenData != NULL ) { delete [] m_greenData; }
	if ( m_blueData != NULL ) { delete [] m_blueData; }
	
	
	//---- Allocation memoire
	m_redData = new double[m_nbPixel];
	m_greenData = new double[m_nbPixel];
	m_blueData = new double[m_nbPixel];
	
	//---- Mise a zero
	memset(m_redData, 0, m_nbPixel*sizeof(double));
	memset(m_greenData, 0, m_nbPixel*sizeof(double));
	memset(m_blueData, 0, m_nbPixel*sizeof(double));
	
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
}

//----------------------------------------------------------------------
//---- Lecture d'une image
//----------------------------------------------------------------------
bool Image::load(const std::string & filePath)
{
	//---- Recherche de l'extension pour le format
	std::vector<std::string> vecName = splitStr(filePath, ".");
	std::string extension = vecName.back();
	
	bool result = false;
	
	if ( (extension == "jpg") || (extension == "JPG") || (extension == "jpeg") )
	{
		result = loadJpeg(filePath);
		
	} else if ( (extension == "bmp") || (extension == "BMP") )
	{
		result = loadBmp(filePath);
		
	} else if ( (extension == "tif") || (extension == "tiff") || (extension == "TIF") )
	{
		result = loadTiff(filePath);
		
    } else if ( extension == "NEF" )
    {
        result = loadRaw(filePath);
        
	} else {
		std::cout << "ERREUR : Format d'image " << extension << " inconnu" << std::endl;
		result = false;
	}
	
	return result;
}


//----------------------------------------------------------------------
//---- Lecture d'une image tiff
//----------------------------------------------------------------------
bool Image::loadTiff(const std::string & filePath)
{
	
	//---- Ouverture du fichier
	TIFF * in = TIFFOpen(filePath.c_str(), "r");
	
	if ( in == NULL )
	{
		std::cout << "ERREUR : Echec lors de l'ouverture du fichier: " << filePath << std::endl;
		return false;
	}
	
	//---- Taille de l'image
	unsigned long lDx, lDy;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &lDx);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &lDy);
	
	m_dx = (int)lDx;
	m_dy = (int)lDy;
	m_nbPixel = m_dx * m_dy;
	
	
	//---- Test memoire deja allouee
	if ( m_redData != NULL ) { delete [] m_redData; }
	if ( m_greenData != NULL ) { delete [] m_greenData; }
	if ( m_blueData != NULL ) { delete [] m_blueData; }
	
	
	//---- Allocation memoire
	m_redData = new double[m_dx*m_dy];
	m_greenData = new double[m_dx*m_dy];
	m_blueData = new double[m_dx*m_dy];
	
	
	//---- Allocation memoire TIFF
	uint32 * raster = (uint32*)_TIFFmalloc(m_dx*m_dy*sizeof(uint32));
	
	//---- Lecture des donnees
	if ( !TIFFReadRGBAImage(in, (uint32)m_dx, (uint32)m_dy, raster, 0) )
	{
		std::cout << "ERREUR : Echec lors de la lecture du buffer" << std::endl;
		return false;
	}
	
	//---- Formattage des donnees
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
	
	for (int indexPixel = 0; indexPixel < m_nbPixel; ++indexPixel)
	{
		m_redData[indexPixel] = (double)TIFFGetR(raster[indexPixel]);
		m_greenData[indexPixel] = (double)TIFFGetG(raster[indexPixel]);
		m_blueData[indexPixel] = (double)TIFFGetB(raster[indexPixel]);
		
		if ( m_redData[indexPixel] > m_maxRed ) { m_maxRed = m_redData[indexPixel]; }
		if ( m_greenData[indexPixel] > m_maxGreen ) { m_maxGreen = m_greenData[indexPixel]; }
		if ( m_blueData[indexPixel] > m_maxBlue ) { m_maxBlue = m_blueData[indexPixel]; }
		
		m_meanRed += m_redData[indexPixel];
		m_meanGreen += m_greenData[indexPixel];
		m_meanBlue += m_blueData[indexPixel];
		
	}
	
	m_meanRed /= m_nbPixel;
	m_meanGreen /= m_nbPixel;
	m_meanBlue /= m_nbPixel;
	
	
	//---- Liberation memoire
	_TIFFfree(raster);
	
	//---- Fermeture du fichier
	TIFFClose(in);
	
	
	//---- Inversion des lignes pour se mettre en origine Upper Left
	double * redTmp = new double [m_dx*m_dy];
	double * greenTmp = new double [m_dx*m_dy];
	double * blueTmp = new double [m_dx*m_dy];
	
	memcpy(redTmp, m_redData, m_dx*m_dy*sizeof(double));
	memcpy(greenTmp, m_greenData, m_dx*m_dy*sizeof(double));
	memcpy(blueTmp, m_blueData, m_dx*m_dy*sizeof(double));
	
	
	
	int iLigneInv = 0;
	for (int ligne = m_dy-1; ligne >= 0; --ligne)
	{
		for (int col = 0; col < m_dx; ++col)
		{
			m_redData[iLigneInv*m_dx+col] = redTmp[ligne*m_dx+col];
			m_greenData[iLigneInv*m_dx+col] = greenTmp[ligne*m_dx+col];
			m_blueData[iLigneInv*m_dx+col] = blueTmp[ligne*m_dx+col];
		}
		iLigneInv += 1;
	}
	
	
	delete [] redTmp;
	delete [] greenTmp;
	delete [] blueTmp;
	
    
	return true;
}

//----------------------------------------------------------------------
//---- Lecture d'une image bmp
//----------------------------------------------------------------------
bool Image::loadBmp(const std::string & filePath)
{
	//---- Load image
	bitmap_image image(filePath);
	if (!image)
	{
		std::cout << "ERREUR : Echec lors du chargement par lib bitmap de " << filePath << std::endl;
		return false;
	}
	
	//---- Get data
	m_dx = image.width();
	m_dy = image.height();
	m_nbPixel = m_dx * m_dy;
	
	//---- Test memoire deja allouee
	if ( m_redData != NULL ) { delete [] m_redData; }
	if ( m_greenData != NULL ) { delete [] m_greenData; }
	if ( m_blueData != NULL ) { delete [] m_blueData; }
	
	//---- Allocation memoire
	m_redData = new double[m_dx*m_dy];
	m_greenData = new double[m_dx*m_dy];
	m_blueData = new double[m_dx*m_dy];
	
	
	image.export_rgb_normal(m_redData, m_greenData, m_blueData);
	
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
	
	for ( int index = 0; index < m_nbPixel; ++index)
	{
		if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
		if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
		if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
		
		m_meanRed += m_redData[index];
		m_meanGreen += m_greenData[index];
		m_meanBlue += m_blueData[index];
		
	}
	
	m_meanRed /= m_nbPixel;
	m_meanGreen /= m_nbPixel;
	m_meanBlue /= m_nbPixel;
	
	return true;
}

//----------------------------------------------------------------------
//---- Lecture d'une ligne image dans un fichier tiff
//----------------------------------------------------------------------
void Image::loadTiffLine(TIFF * tifFile, int lineNumber)
{
	int lineSize = m_dx*3;
	
	//---- Allocation memoire pour TIFF
	//--- Taille d'une ligne
	//int lineSize = TIFFScanlineSize(tifFile);
	//tdata_t buf = _TIFFmalloc(lineSize);
	tdata_t buf = _TIFFmalloc(lineSize);
	
	//---- Allocation memoire temporaire
	unsigned char * buffer = new unsigned char[lineSize];
	
	//---- Allocation memoire
	m_redData = new double[m_dx*m_dy];
	m_greenData = new double[m_dx*m_dy];
	m_blueData = new double[m_dx*m_dy];
	
	//---- Copie des donnees
	TIFFReadScanline(tifFile, buf, lineNumber);
	memcpy(buffer, buf, lineSize);
	
	//---- Conversion
	int index = 0;
	for (int col = 0; col < m_dx; ++col)
	{
		m_redData[col] = (double)buffer[index];
		index += 1;
		m_greenData[col] = (double)buffer[index];
		index += 1;
		m_blueData[col] = (double)buffer[index];
		index += 1;
	}
	
	
	//---- Liberation memoire
	_TIFFfree(buf);
	delete [] buffer;
	
	
}


//----------------------------------------------------------------------
//---- Lecture d'une image NEF
//----------------------------------------------------------------------
bool Image::loadRaw(const std::string & filePath)
{
    //---- Ouverture du fichier
	LibRaw RawProcessor;
	if ( RawProcessor.open_file(filePath.c_str()) != LIBRAW_SUCCESS )
	{
		std::cout << "ERROR : Impossible d'ouvrir l'image " << filePath << std::endl;
		return false;
	}
	
	//--- Taille de l'image
	m_dx = RawProcessor.imgdata.sizes.iwidth;
	m_dy = RawProcessor.imgdata.sizes.iheight;
	m_nbPixel = m_dx * m_dy;
	
	//---- Test memoire deja allouee
	if ( m_redData != NULL ) { delete [] m_redData; }
	if ( m_greenData != NULL ) { delete [] m_greenData; }
	if ( m_blueData != NULL ) { delete [] m_blueData; }
	
	//---- Allocation memoire
	m_redData = new double[m_dx*m_dy];
	m_greenData = new double[m_dx*m_dy];
	m_blueData = new double[m_dx*m_dy];
	
	//---- Extraction des donnees brutes
	if ( RawProcessor.unpack() != LIBRAW_SUCCESS )
	{
		std::cout << "ERROR : Impossible d'extraire les donnees brutes" << std::endl;
		return false;
	}
	
	
	RawProcessor.imgdata.params.output_color = 1;
	RawProcessor.imgdata.params.user_qual = 3;
	RawProcessor.imgdata.params.output_bps = -4;
	RawProcessor.imgdata.params.bright = 1.0;
    //RawProcessor.imgdata.params.gamm[0] = 1;
    //RawProcessor.imgdata.params.gamm[1] = 1;
    //RawProcessor.imgdata.params.no_auto_scale = 1;
    //RawProcessor.imgdata.params.use_auto_wb = 1;
    RawProcessor.imgdata.params.use_camera_wb = 1;
	
	//---- Decoupe des donnees par couleur
	RawProcessor.dcraw_process();
	
    //---- Initialisations
    m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
    
	//--- Parcours des pixels
	double currentVal;
	for (int index = 0; index < m_nbPixel; ++index)
	{
		//-- Canal rouge
		currentVal = (double)RawProcessor.imgdata.image[index][CH_RED];
		m_redData[index] = currentVal;
		if ( currentVal > m_maxRed ) { m_maxRed = currentVal; }
        m_meanRed += currentVal;
		
		//-- Canal vert 
		currentVal = (double)RawProcessor.imgdata.image[index][CH_GREEN1];
		m_greenData[index] = currentVal;
		if ( currentVal > m_maxGreen ) { m_maxGreen = currentVal; }
		m_meanGreen += currentVal;
        
		//-- Canal bleu
		currentVal = (double)RawProcessor.imgdata.image[index][CH_BLUE];
		m_blueData[index] = currentVal;
		if ( currentVal > m_maxBlue ) { m_maxBlue = currentVal; }
        m_meanBlue += currentVal;
        
	}
	
    m_meanRed /= m_nbPixel;
	m_meanGreen /= m_nbPixel;
	m_meanBlue /= m_nbPixel;
    
    m_isRaw = true;
    
	return true;
}


//----------------------------------------------------------------------
//---- Lecture d'une image jpeg
//----------------------------------------------------------------------
bool Image::loadJpeg(const std::string & filePath)
{
	//---- Declaration de structures propres a libjpeg
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	//---- Ce n'est pas une image RAW
	m_isRaw = false;
	
	//---- Ouverture du fichier
	FILE * infile = fopen(filePath.c_str(), "rb");
	if ( infile == NULL )
	{
		std::cout << "ERREUR : Echec lors de l'ouverture du fichier " << filePath << std::endl;
		return false;
	}
	
	//---- initialisation de la structure
	cinfo.err = jpeg_std_error(&jerr);
	
	//---- Creation du decompresseur
	jpeg_create_decompress(&cinfo);
	
	//---- Set de la source
	jpeg_stdio_src(&cinfo, infile);
	
	//---- Lecture de l'entete
	(void) jpeg_read_header(&cinfo, TRUE);
	
	//---- Debut de la decompression
	(void) jpeg_start_decompress(&cinfo);
	
	//---- JSAMPLEs per row in output buffer
	int row_stride = cinfo.output_width * cinfo.output_components;
	
	m_dx = cinfo.output_width;
	m_dy = cinfo.output_height;
	m_nbPixel = m_dx * m_dy;
	/*
	std::cout << "INFO   : output width " << cinfo.output_width << std::endl;
	std::cout << "INFO   : output height " << cinfo.output_height << std::endl;
	std::cout << "INFO   : output components " << cinfo.output_components << std::endl;
	*/
	
	//---- Allocation memoire libjpeg
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	
	//---- Test memoire deja allouee
	if ( m_redData != NULL ) { delete [] m_redData; }
	if ( m_greenData != NULL ) { delete [] m_greenData; }
	if ( m_blueData != NULL ) { delete [] m_blueData; }
	
	//---- Allocation memoire image
	m_redData = new double[m_nbPixel];
	m_greenData = new double[m_nbPixel];
	m_blueData = new double[m_nbPixel];
	
	//---- Initialisation des valeurs max
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	m_meanRed = 0.0;
	m_meanGreen = 0.0;
	m_meanBlue = 0.0;
	
	//---- Image noir et blanc
	if ( cinfo.output_components == 1 )
	{
		//--- Lecture ligne par ligne
		int lineIndex = 0;
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			
			
			for (int colIndex = 0; colIndex < m_dx; ++colIndex)
			{
				m_redData[lineIndex*m_dx+colIndex] = (double)buffer[0][colIndex];
				m_greenData[lineIndex*m_dx+colIndex] = (double)buffer[0][colIndex];
				m_blueData[lineIndex*m_dx+colIndex] = (double)buffer[0][colIndex];
				
				if ( m_redData[colIndex] > m_maxRed ) { m_maxRed = m_redData[colIndex]; }
				if ( m_greenData[colIndex] > m_maxGreen ) { m_maxGreen = m_greenData[colIndex]; }
				if ( m_blueData[colIndex] > m_maxBlue ) { m_maxBlue = m_blueData[colIndex]; }
				
				m_meanRed += m_redData[lineIndex*m_dx+colIndex];
				m_meanGreen += m_greenData[lineIndex*m_dx+colIndex];
				m_meanBlue += m_blueData[lineIndex*m_dx+colIndex];
				
			}
			
			lineIndex += 1;
		}
		
	//---- Image couleur
	} else if ( cinfo.output_components == 3 ) {
		
		//--- Lecture ligne par ligne
		int lineIndex = 0;
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			
			for (int colIndex = 0; colIndex < m_dx; ++colIndex)
			{
				m_redData[lineIndex*m_dx+colIndex] = (double)buffer[0][colIndex*cinfo.output_components];
				m_greenData[lineIndex*m_dx+colIndex] = (double)buffer[0][colIndex*cinfo.output_components+1];
				m_blueData[lineIndex*m_dx+colIndex] = (double)buffer[0][colIndex*cinfo.output_components+2];
				
				if ( m_redData[colIndex] > m_maxRed ) { m_maxRed = m_redData[colIndex]; }
				if ( m_greenData[colIndex] > m_maxGreen ) { m_maxGreen = m_greenData[colIndex]; }
				if ( m_blueData[colIndex] > m_maxBlue ) { m_maxBlue = m_blueData[colIndex]; }
				
				m_meanRed += m_redData[lineIndex*m_dx+colIndex];
				m_meanGreen += m_greenData[lineIndex*m_dx+colIndex];
				m_meanBlue += m_blueData[lineIndex*m_dx+colIndex];
				
			}
			
			lineIndex += 1;
		}
		
	} else {
		
		std::cout << "ERREUR : Nombre de composantes de l'image inconnu: " << cinfo.output_components << std::endl;
		return false;
	}
	
	m_meanRed /= m_nbPixel;
	m_meanGreen /= m_nbPixel;
	m_meanBlue /= m_nbPixel;
	
	
	//---- Fin decompression
	(void) jpeg_finish_decompress(&cinfo);
	
	//---- Liberation memoire JPEG decompression
	jpeg_destroy_decompress(&cinfo);
	
	
	//---- Fermeture du fichier
	fclose(infile);
	
	return true;
}


//----------------------------------------------------------------------
//---- Lectude d'une ligne image jpeg
//----------------------------------------------------------------------
bool Image::loadJpegLine(const std::string & filePath, int indexLine)
{
	//---- Declaration de structures propres a libjpeg
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	//---- Ce n'est pas une image RAW
	m_isRaw = false;
	
	//---- Ouverture du fichier
	FILE * infile = fopen(filePath.c_str(), "rb");
	if ( infile == NULL )
	{
		std::cout << "ERREUR : Echec lors de l'ouverture du fichier " << filePath << std::endl;
		return false;
	}
	
	//---- initialisation de la structure
	cinfo.err = jpeg_std_error(&jerr);
	
	//---- Creation du decompresseur
	jpeg_create_decompress(&cinfo);
	
	//---- Set de la source
	jpeg_stdio_src(&cinfo, infile);
	
	//---- Lecture de l'entete
	(void) jpeg_read_header(&cinfo, TRUE);
	
	//---- Debut de la decompression
	(void) jpeg_start_decompress(&cinfo);
	
	//---- JSAMPLEs per row in output buffer
	int row_stride = cinfo.output_width * cinfo.output_components;
	
	m_dx = cinfo.output_width;
	m_dy = 1;
	m_nbPixel = m_dx * m_dy;
	/*
	std::cout << "INFO   : output width " << cinfo.output_width << std::endl;
	std::cout << "INFO   : output height " << cinfo.output_height << std::endl;
	std::cout << "INFO   : output components " << cinfo.output_components << std::endl;
	*/
	
	//---- Allocation memoire libjpeg
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	
	//---- Test memoire deja allouee
	if ( m_redData != NULL ) { delete [] m_redData; }
	if ( m_greenData != NULL ) { delete [] m_greenData; }
	if ( m_blueData != NULL ) { delete [] m_blueData; }
	
	//---- Allocation memoire image
	m_redData = new double[m_nbPixel];
	m_greenData = new double[m_nbPixel];
	m_blueData = new double[m_nbPixel];
	
	//---- Initialisation des valeurs max
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	//---- Image noir et blanc
	if ( cinfo.output_components == 1 )
	{
		
		//--- Lecture ligne par ligne
		int lineIndex = 0;
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			
			if ( lineIndex == indexLine )
			{
				for (int colIndex = 0; colIndex < m_dx; ++colIndex)
				{
					m_redData[colIndex] = (double)buffer[0][colIndex];
					m_greenData[colIndex] = (double)buffer[0][colIndex];
					m_blueData[colIndex] = (double)buffer[0][colIndex];
					
					if ( m_redData[colIndex] > m_maxRed ) { m_maxRed = m_redData[colIndex]; }
					if ( m_greenData[colIndex] > m_maxGreen ) { m_maxGreen = m_greenData[colIndex]; }
					if ( m_blueData[colIndex] > m_maxBlue ) { m_maxBlue = m_blueData[colIndex]; }
				}
			}
			
			lineIndex += 1;
		}
		
		
	//---- Image couleur
	} else if ( cinfo.output_components == 3 ) {
		
		
		//--- Lecture ligne par ligne
		int lineIndex = 0;
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			
			if ( lineIndex == indexLine )
			{
				for (int colIndex = 0; colIndex < m_dx; ++colIndex)
				{
					m_redData[colIndex] = (double)buffer[0][colIndex*cinfo.output_components];
					m_greenData[colIndex] = (double)buffer[0][colIndex*cinfo.output_components+1];
					m_blueData[colIndex] = (double)buffer[0][colIndex*cinfo.output_components+2];
					
					if ( m_redData[colIndex] > m_maxRed ) { m_maxRed = m_redData[colIndex]; }
					if ( m_greenData[colIndex] > m_maxGreen ) { m_maxGreen = m_greenData[colIndex]; }
					if ( m_blueData[colIndex] > m_maxBlue ) { m_maxBlue = m_blueData[colIndex]; }
				}
				
			}
			
			lineIndex += 1;
		}
		
	} else {
		
		std::cout << "ERREUR : Nombre de composantes de l'image inconnu: " << cinfo.output_components << std::endl;
		return false;
	}
	
	
	//---- Fin decompression
	(void) jpeg_finish_decompress(&cinfo);
	
	//---- Liberation memoire JPEG decompression
	jpeg_destroy_decompress(&cinfo);
	
	
	//---- Fermeture du fichier
	fclose(infile);
	
	return true;
}


//----------------------------------------------------------------------
//---- Ecriture d'une image en tiff 8 ou 16 bits
//----------------------------------------------------------------------
bool Image::writeImTiff(const std::string & filePath)
{
    
    cv::Mat imMat = getCvMatAll();
    
    if ( ! cv::imwrite(filePath, imMat) )
    {
        std::cout << "ERREUR : Impossible de sauvegarder l'image " << filePath << std::endl;
        return false;
    }
    
    return true;
    
}


//----------------------------------------------------------------------
//---- Retourne la valeur d'un pixel en 8 bits
//----------------------------------------------------------------------
int8_t Image::getRedValue8b(int col, int row, bool fullDynamic) const
{
	int8_t current8Val = 0;
	
	if ( fullDynamic )
	{
		current8Val = (int8_t)std::round(m_redData[row*m_dx+col] * MAX_8_BIT / m_maxRed);
		
	} else {
		
		double currentVal = m_redData[row*m_dx+col];
		if ( currentVal > MAX_8_BIT )
		{
			current8Val = MAX_8_BIT;
		} else {
			current8Val = (int8_t)currentVal;
		}
		
	}
	
	return current8Val;
	
}

int8_t Image::getGreenValue8b(int col, int row, bool fullDynamic) const
{	
	int8_t current8Val = 0;
	
	if ( fullDynamic )
	{
		current8Val = (int8_t)std::round(m_greenData[row*m_dx+col] * MAX_8_BIT / m_maxGreen);
		
	} else {
		
		double currentVal = m_greenData[row*m_dx+col];
		if ( currentVal > MAX_8_BIT )
		{
			current8Val = MAX_8_BIT;
		} else {
			current8Val = (int8_t)currentVal;
		}
		
	}
	
	return current8Val;

}

int8_t Image::getBlueValue8b(int col, int row, bool fullDynamic) const
{
	int8_t current8Val = 0;
	
	if ( fullDynamic )
	{
		current8Val = (int8_t)std::round(m_blueData[row*m_dx+col] * MAX_8_BIT / m_maxBlue);
		
	} else {
		
		double currentVal = m_blueData[row*m_dx+col];
		if ( currentVal > MAX_8_BIT )
		{
			current8Val = MAX_8_BIT;
		} else {
			current8Val = (int8_t)currentVal;
		}
		
	}
	
	return current8Val;
}


//----------------------------------------------------------------------
//---- Creation d'une imagette avec origine pixellaire
//----------------------------------------------------------------------
Image Image::getRoi(int x0, int y0, int dx, int dy) const
{
	Image im(dx, dy);
	
	//---- Verifications
	if ( (x0 < 0) || (x0 + dx > m_dx) )
	{
		std::cout << "ERREUR : La zone demandee sort de l'image en colonne: " << x0 << std::endl;
		return im;
	}
	
	if ( (y0 < 0) || (y0 + dy > m_dy) )
	{
		std::cout << "ERREUR : La zone demandee sort de l'image en ligne: " << y0 << std::endl;
		return im;
	}
	
	
	//---- Copie des donnees
	int imIndex = 0;
	for (int y = y0; y < y0 + dy; ++y)
	{
		for (int x = x0; x < x0 + dx; ++x)
		{
			im.m_redData[imIndex] = m_redData[y*m_dx+x];
			im.m_greenData[imIndex] = m_greenData[y*m_dx+x];
			im.m_blueData[imIndex] = m_blueData[y*m_dx+x];
			imIndex += 1;
			
			if ( m_redData[y*m_dx+x] > im.m_maxRed ) { im.m_maxRed = m_redData[y*m_dx+x]; }
			if ( m_greenData[y*m_dx+x] > im.m_maxGreen ) { im.m_maxGreen = m_greenData[y*m_dx+x]; }
			if ( m_blueData[y*m_dx+x] > im.m_maxBlue ) { im.m_maxBlue = m_blueData[y*m_dx+x]; }
			
			im.m_meanRed += m_redData[y*m_dx+x];
			im.m_meanGreen += m_greenData[y*m_dx+x];
			im.m_meanBlue += m_blueData[y*m_dx+x];
		}
		
	}
	
	im.m_isRaw = m_isRaw;
	
	im.m_meanRed /= im.m_nbPixel;
	im.m_meanGreen /= im.m_nbPixel;
	im.m_meanBlue /= im.m_nbPixel;
	
	return im;
	
}

//----------------------------------------------------------------------
Image Image::getRoi(const Image::sBoundingBox & bBox) const
{
	int dx = bBox.xMax - bBox.xMin + 1;
	int dy = bBox.yMax - bBox.yMin + 1;
	
	Image im(dx, dy);
	
	//---- Verifications
	if ( (bBox.xMin < 0) || (bBox.xMin + dx > m_dx) )
	{
		std::cout << "ERREUR : La zone demandee sort de l'image en colonne: " << bBox.xMin << std::endl;
		return im;
	}
	
	if ( (bBox.yMin < 0) || (bBox.yMin + dy > m_dy) )
	{
		std::cout << "ERREUR : La zone demandee sort de l'image en ligne: " << bBox.yMin << std::endl;
		return im;
	}
	
	//---- Copie des donnees
	int imIndex = 0;
	for (int y = bBox.yMin; y < bBox.yMin + dy; ++y)
	{
		for (int x = bBox.xMin; x < bBox.xMin + dx; ++x)
		{
			im.m_redData[imIndex] = m_redData[y*m_dx+x];
			im.m_greenData[imIndex] = m_greenData[y*m_dx+x];
			im.m_blueData[imIndex] = m_blueData[y*m_dx+x];
			imIndex += 1;
			
			if ( m_redData[y*m_dx+x] > im.m_maxRed ) { im.m_maxRed = m_redData[y*m_dx+x]; }
			if ( m_greenData[y*m_dx+x] > im.m_maxGreen ) { im.m_maxGreen = m_greenData[y*m_dx+x]; }
			if ( m_blueData[y*m_dx+x] > im.m_maxBlue ) { im.m_maxBlue = m_blueData[y*m_dx+x]; }
			
			im.m_meanRed += m_redData[y*m_dx+x];
			im.m_meanGreen += m_greenData[y*m_dx+x];
			im.m_meanBlue += m_blueData[y*m_dx+x];
		}
		
	}

	im.m_isRaw = m_isRaw;
	
	im.m_meanRed /= im.m_nbPixel;
	im.m_meanGreen /= im.m_nbPixel;
	im.m_meanBlue /= im.m_nbPixel;
	
	return im;
}


//----------------------------------------------------------------------
//---- Creation d'une imagette avec origine sub-pixellaire
//----------------------------------------------------------------------
Image Image::getRoiSub(double x0, double y0, int dx, int dy) const
{
	
	Image im(dx, dy);
	
	//---- Copie des donnees
	int imIndex = 0;
	double currentSubX, currentSubY;
	
	for (int y = 0; y < dy; ++y)
	{
		for (int x = 0; x < dx; ++x)
		{
			currentSubX = x0 + (double)x;
			currentSubY = y0 + (double)y;
			
			im.m_redData[imIndex] = interpolBil(currentSubX, currentSubY, RED);
			im.m_greenData[imIndex] = interpolBil(currentSubX, currentSubY, GREEN);
			im.m_blueData[imIndex] = interpolBil(currentSubX, currentSubY, BLUE);
			imIndex += 1;
			
			if ( im.m_redData[imIndex] > im.m_maxRed ) { im.m_maxRed = im.m_redData[imIndex]; }
			if ( im.m_greenData[imIndex] > im.m_maxGreen ) { im.m_maxGreen = im.m_greenData[imIndex]; }
			if ( im.m_blueData[imIndex] > im.m_maxBlue ) { im.m_maxBlue = im.m_blueData[imIndex]; }
			
			im.m_meanRed += im.m_redData[imIndex];
			im.m_meanGreen += im.m_greenData[imIndex];
			im.m_meanBlue += im.m_blueData[imIndex];
			
			
		}
	}
	
	im.m_meanRed /= im.m_nbPixel;
	im.m_meanGreen /= im.m_nbPixel;
	im.m_meanBlue /= im.m_nbPixel;
	
	
	return im;
	
}



//----------------------------------------------------------------------
//---- Interpolation bilineaire
//----------------------------------------------------------------------
double Image::interpolBil(double x, double y, Ecolor color) const
{
	
	//---- Test d'appartenance a l'image
	if ( (x < 0.0) || (x >= m_dx) ||
	     (y < 0.0) || (y >= m_dy) )
	{
		return 0.0;
	}

	double X1 = 0.0;
	double X2 = 0.0;
	double Y1 = 0.0;
	double Y2 = 0.0;

	//---- Calcul de la position des pixels d'appui
	int floorXindex = std::floor(x);
	if ( x - floorXindex < 0.5 )
	{
		X1 = floorXindex - 1;
		X2 = floorXindex;
	} else if ( x - floorXindex >= 0.5 )
	{
		X1 = floorXindex;
		X2 = floorXindex + 1;
	}

	int floorYindex = std::floor(y);
	if ( y - floorYindex < 0.5 )
	{
		Y1 = floorYindex - 1;
		Y2 = floorYindex;
	} else if ( y - floorYindex >= 0.5 )
	{
		Y1 = floorYindex;
		Y2 = floorYindex + 1;
	}

	//std::cout << "### demande: " << x << ", " << y << std::endl;
	//std::cout << "### appui: " << X1 << " -> " << X2 << ", " << Y1 << " -> " << Y2 << std::endl;
	

	//---- Interpolation de la valeur
	double yTmp = (y - (Y1 + 0.5));
	double xTmp = (x - (X1 + 0.5));

	//--- Interpolation suivant les colonnes
	int index11 = (Y1*m_dx)+X1;
	int index12 = (Y2*m_dx)+X1;
	int index21 = (Y1*m_dx)+X2;
	int index22 = (Y2*m_dx)+X2;
	
	
	double valY1 = 0.0, valY2 = 0.0;
	if ( (index11 >= m_nbPixel) || (index12 >= m_nbPixel) || (index21 >= m_nbPixel) || (index22 >= m_nbPixel) )
	{
		return 0.0;
	} else if ( (index11 < 0) || (index12 < 0) || (index21 < 0) || (index22 < 0) )
	{
		return 0.0;
	} else {
		if ( color == RED )
		{
			valY1 = (xTmp * m_redData[index21]) + ((1.0 - xTmp) * m_redData[index11]);
			valY2 = (xTmp * m_redData[index22]) + ((1.0 - xTmp) * m_redData[index12]);
		} else if ( color == GREEN )
		{
			valY1 = (xTmp * m_greenData[index21]) + ((1.0 - xTmp) * m_greenData[index11]);
			valY2 = (xTmp * m_greenData[index22]) + ((1.0 - xTmp) * m_greenData[index12]);
		} else if ( color == BLUE )
		{
			valY1 = (xTmp * m_blueData[index21]) + ((1.0 - xTmp) * m_blueData[index11]);
			valY2 = (xTmp * m_blueData[index22]) + ((1.0 - xTmp) * m_blueData[index12]);
		}
	}

	//--- Interpolation suivant les lignes
	return yTmp * valY2 + (1.0 - yTmp) * valY1;
	
}


//----------------------------------------------------------------------
//---- Conversion en cv::Mat cas general
//----------------------------------------------------------------------
cv::Mat Image::getCvMatAll() const
{
    cv::Mat imMat;
    
    //---- Cas d'une image raw (16 bits)-> CV_16UC3
    if ( m_isRaw )
    {
        imMat = cv::Mat(m_dy, m_dx, CV_16UC3);
        
        //--- Parcours des pixels
        cv::Vec3w currentPix;
        int index = 0;
        
        for (int ligne = 0; ligne < m_dy; ++ligne)
        {
            for (int col = 0; col < m_dx; ++col)
            {
                //-- Canal rouge
                if ( m_redData[index] > MAX_16_BIT )
                {
                    currentPix[2] = (ushort)MAX_16_BIT;
                } else {
                    currentPix[2] = (ushort)m_redData[index];
                }
                
                //-- Canal vert
                if ( m_greenData[index] > MAX_16_BIT )
                {
                    currentPix[1] = (ushort)MAX_16_BIT;
                } else {
                    currentPix[1] = (ushort)m_greenData[index];
                }
                
                //-- Canal bleu
                if ( m_blueData[index] > MAX_16_BIT )
                {
                    currentPix[0] = (ushort)MAX_16_BIT;
                } else {
                    currentPix[0] = (ushort)m_blueData[index];
                }
                
                index += 1;
                
                imMat.at<cv::Vec3w>(ligne, col) = currentPix;
            }
        }
    
    //---- Cas d'une image 8 bits
    } else {
        
        imMat = cv::Mat(m_dy, m_dx, CV_8UC3);
        
        //--- Parcours des pixels
        cv::Vec3b currentPix;
        int index = 0;
        
        for (int ligne = 0; ligne < m_dy; ++ligne)
        {
            for (int col = 0; col < m_dx; ++col)
            {
                //-- Canal rouge
                if ( m_redData[index] > MAX_8_BIT )
                {
                    currentPix[2] = (uchar)MAX_8_BIT;
                } else {
                    currentPix[2] = (uchar)std::round(m_redData[index]);
                }
                
                //-- Canal vert
                if ( m_greenData[index] > MAX_8_BIT )
                {
                    currentPix[1] = (uchar)MAX_8_BIT;
                } else {
                    currentPix[1] = (uchar)std::round(m_greenData[index]);
                }
                
                //-- Canal bleu
                if ( m_blueData[index] > MAX_8_BIT )
                {
                    currentPix[0] = (uchar)MAX_8_BIT;
                } else {
                    currentPix[0] = (uchar)std::round(m_blueData[index]);
                }
                
                index += 1;
                
                imMat.at<cv::Vec3b>(ligne, col) = currentPix;
            }
        }
    }
    
    
    return imMat;
}


//----------------------------------------------------------------------
//---- Conversion en cv::Mat 8 bits pour affichage
//----------------------------------------------------------------------
cv::Mat Image::getCvMat8bit(bool fullDynamic) const
{
    cv::Mat imMat(m_dy, m_dx, CV_8UC3);
    cv::Vec3b currentPix;
    int index = 0;

    //---- Cas ou l'image vient d'un RAW
    if ( m_isRaw )
    {
        //--- On prends toute la plage dynamique
        if ( fullDynamic )
        {
            //-- Test
            if ( (m_maxRed == 0.0 ) || (m_maxGreen == 0.0) || (m_maxBlue == 0.0) )
            {
                std::cout << "WARN   : Une des valeurs RGB max de l'image est a 0 -> Probleme de dynamique" << std::endl;
            }
            
            //-- Conversion
            int index = 0;
            for (int ligne = 0; ligne < m_dy; ++ligne)
            {
                for (int col = 0; col < m_dx; ++col)
                {
                    currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / m_maxRed);
                    currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / m_maxGreen);
                    currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / m_maxBlue);
                    index += 1;
                    
                    imMat.at<cv::Vec3b>(ligne, col) = currentPix;
                }
            }
        
        //--- On etire la dynamique a max / 4
        } else {
            //---- Ajustement radiometrique
            int index = 0;
            for (int y = 0; y < m_dy; ++y)
            {
                for (int x = 0; x < m_dx; ++x)
                {
                    
                    currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / (m_maxRed / 4.0));
                    currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / (m_maxGreen / 4.0));
                    currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / (m_maxBlue / 4.0));
                    
                    
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
            if ( (m_maxRed == 0.0 ) || (m_maxGreen == 0.0) || (m_maxBlue == 0.0) )
            {
                std::cout << "WARN   : Une des valeurs RGB max de l'image est a 0 -> Probleme de dynamique" << std::endl;
            }
            
            //--- Parcours de l'image
            for (int ligne = 0; ligne < m_dy; ++ligne)
            {
                for (int col = 0; col < m_dx; ++col)
                {
                    currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / m_maxRed);
                    currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / m_maxGreen);
                    currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / m_maxBlue);
                    index += 1;
                    
                    imMat.at<cv::Vec3b>(ligne, col) = currentPix;
                }
            }
        } else {
            
            //--- Parcours de l'image
            for (int ligne = 0; ligne < m_dy; ++ligne)
            {
                for (int col = 0; col < m_dx; ++col)
                {
                    currentPix[2] = std::round(m_redData[index]);
                    currentPix[1] = std::round(m_greenData[index]);
                    currentPix[0] = std::round(m_blueData[index]);
                    index += 1;
                    
                    imMat.at<cv::Vec3b>(ligne, col) = currentPix;
                }
            }
        }
    }

    return imMat;

}











//----------------------------------------------------------------------
//---- Conversion en cv::Mat
//----------------------------------------------------------------------
cv::Mat Image::getCvMat() const
{
	cv::Mat imMat(m_dy, m_dx, CV_8UC3);
		
	//---- Conversion
	cv::Vec3b currentPix;
	int index = 0;
	
	for (int ligne = 0; ligne < m_dy; ++ligne)
	{
		for (int col = 0; col < m_dx; ++col)
		{
			currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / m_maxRed);
			currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / m_maxGreen);
			currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / m_maxBlue);
			index += 1;
			
			imMat.at<cv::Vec3b>(ligne, col) = currentPix;
		}
	}
	
	return imMat;
}


//~ //----------------------------------------------------------------------
//~ //---- Conversion en cv::Mat pour une image RAW
//~ //----------------------------------------------------------------------
//~ cv::Mat Image::getCvMatFromRaw(bool fullDynamic) const
//~ {
	
	//~ cv::Mat imMat(m_dy, m_dx, CV_8UC3);
	//~ cv::Vec3b currentPix;
	
	//~ if ( fullDynamic )
	//~ {
		
		//~ //---- Conversion
		//~ int index = 0;
		//~ for (int ligne = 0; ligne < m_dy; ++ligne)
		//~ {
			//~ for (int col = 0; col < m_dx; ++col)
			//~ {
				//~ currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / m_maxRed);
				//~ currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / m_maxGreen);
				//~ currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / m_maxBlue);
				//~ index += 1;
				
				//~ imMat.at<cv::Vec3b>(ligne, col) = currentPix;
			//~ }
		//~ }
		
	//~ } else {
		//~ //---- Ajustement radiometrique
		//~ for (int y = 0; y < m_dy; ++y)
		//~ {
			//~ for (int x = 0; x < m_dx; ++x)
			//~ {
				
				//~ if ( m_redData[y*m_dx+x] > MAX_8_BIT )
				//~ {
					//~ currentPix[2] = MAX_8_BIT;
				//~ } else {
					//~ currentPix[2] = m_redData[y*m_dx+x];
				//~ }
				
				//~ if ( m_greenData[y*m_dx+x] > MAX_8_BIT )
				//~ {
					//~ currentPix[1] = MAX_8_BIT;
				//~ } else {
					//~ currentPix[1] = m_greenData[y*m_dx+x];
				//~ }
				
				//~ if ( m_blueData[y*m_dx+x] > MAX_8_BIT )
				//~ {
					//~ currentPix[0] = MAX_8_BIT;
				//~ } else {
					//~ currentPix[0] = m_blueData[y*m_dx+x];
				//~ }
				
				//~ imMat.at<cv::Vec3b>(y, x) = currentPix;
			//~ }
		//~ }
	
	//~ }
	
	
	//~ return imMat;
	
//~ }


//----------------------------------------------------------------------
//---- Conversion en cv::Mat
//----------------------------------------------------------------------
cv::Mat Image::getCvMatNew(bool fullDynamic) const
{
	cv::Mat imMat(m_dy, m_dx, CV_8UC3);
	cv::Vec3b currentPix;
	int index = 0;
	
	//---- Cas ou l'image vient d'un RAW
	if ( m_isRaw )
	{
		//--- On prends toute la plage dynamique
		if ( fullDynamic )
		{
			//-- Test
			if ( (m_maxRed == 0.0 ) || (m_maxGreen == 0.0) || (m_maxBlue == 0.0) )
			{
				std::cout << "WARN   : Une des valeurs RGB max de l'image est a 0 -> Probleme de dynamique" << std::endl;
			}
			
			//-- Conversion
			int index = 0;
			for (int ligne = 0; ligne < m_dy; ++ligne)
			{
				for (int col = 0; col < m_dx; ++col)
				{
					currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / m_maxRed);
					currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / m_maxGreen);
					currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / m_maxBlue);
					index += 1;
					
					imMat.at<cv::Vec3b>(ligne, col) = currentPix;
				}
			}
		
		//--- On trounque a 254
		} else {
			//---- Ajustement radiometrique
			for (int y = 0; y < m_dy; ++y)
			{
				for (int x = 0; x < m_dx; ++x)
				{
					
					if ( m_redData[y*m_dx+x] > MAX_8_BIT )
					{
						currentPix[2] = MAX_8_BIT;
					} else {
						currentPix[2] = m_redData[y*m_dx+x];
					}
					
					if ( m_greenData[y*m_dx+x] > MAX_8_BIT )
					{
						currentPix[1] = MAX_8_BIT;
					} else {
						currentPix[1] = m_greenData[y*m_dx+x];
					}
					
					if ( m_blueData[y*m_dx+x] > MAX_8_BIT )
					{
						currentPix[0] = MAX_8_BIT;
					} else {
						currentPix[0] = m_blueData[y*m_dx+x];
					}
					
					imMat.at<cv::Vec3b>(y, x) = currentPix;
				}
			}
		
		}
		
		
	//---- Cas ou l'image vient d'un tiff ou JPG
	} else {
		
		if ( fullDynamic )
		{
			//-- Test
			if ( (m_maxRed == 0.0 ) || (m_maxGreen == 0.0) || (m_maxBlue == 0.0) )
			{
				std::cout << "WARN   : Une des valeurs RGB max de l'image est a 0 -> Probleme de dynamique" << std::endl;
			}
			
			//--- Parcours de l'image
			for (int ligne = 0; ligne < m_dy; ++ligne)
			{
				for (int col = 0; col < m_dx; ++col)
				{
					currentPix[2] = std::round(m_redData[index] * MAX_8_BIT / m_maxRed);
					currentPix[1] = std::round(m_greenData[index] * MAX_8_BIT / m_maxGreen);
					currentPix[0] = std::round(m_blueData[index] * MAX_8_BIT / m_maxBlue);
					index += 1;
					
					imMat.at<cv::Vec3b>(ligne, col) = currentPix;
				}
			}
		} else {
			
			//--- Parcours de l'image
			for (int ligne = 0; ligne < m_dy; ++ligne)
			{
				for (int col = 0; col < m_dx; ++col)
				{
					currentPix[2] = std::round(m_redData[index]);
					currentPix[1] = std::round(m_greenData[index]);
					currentPix[0] = std::round(m_blueData[index]);
					index += 1;
					
					imMat.at<cv::Vec3b>(ligne, col) = currentPix;
				}
			}
		}
	}
	
	return imMat;
	
}




//----------------------------------------------------------------------
//---- Affichage a l'ecran
//----------------------------------------------------------------------
int Image::disp(bool fullDynamic) const
{
	//cv::Mat imMat = getCvMatNew(fullDynamic);
    cv::Mat imMat = getCvMat8bit(fullDynamic);
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::imshow("Display", imMat);
	
	int key = cv::waitKey(0);
	
	cv::destroyWindow("Display");
	
	return key;
}



//----------------------------------------------------------------------
//---- Affichage a l'ecran de ROI
//----------------------------------------------------------------------
int Image::dispRoi(const std::vector<Image::sBoundingBox> & vecBBox) const
{
	cv::Mat imMat = getCvMatNew();
	
	//---- Parcours du vecteur
	for (std::vector<Image::sBoundingBox>::const_iterator iObj = vecBBox.begin(); iObj != vecBBox.end(); ++iObj)
	{
		cv::Point UL;
		UL.x = iObj->xMin;
		UL.y = iObj->yMin;
		
		cv::Point LR;
		LR.x = iObj->xMax;
		LR.y = iObj->yMax;
		
		cv::rectangle(imMat, UL, LR, cv::Scalar(0, 255, 0));
		
	}
	
	cv::imshow("Display", imMat);
	int key = cv::waitKey(0);
	
	return key;
	
}


//----------------------------------------------------------------------
//---- Affichage a l'ecran de ROI
//----------------------------------------------------------------------
int Image::dispRoi(const Image::sBoundingBox & vecBBox) const
{
	cv::Mat imMat = getCvMatNew();
	
	cv::Point UL;
	UL.x = vecBBox.xMin;
	UL.y = vecBBox.yMin;
	
	cv::Point LR;
	LR.x = vecBBox.xMax;
	LR.y = vecBBox.yMax;
	
	cv::rectangle(imMat, UL, LR, cv::Scalar(0, 0, 255));
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::imshow("Display", imMat);
	
	int key = cv::waitKey(0);
	
	return key;
	
}

//----------------------------------------------------------------------
//---- Zoom centre sur un point donne
//----------------------------------------------------------------------
void Image::zoomOnPoint(const cv::Point & pt, bool fullDynamic) const
{
	Image roi = getRoi(pt.x - 10, pt.y - 10, 21, 21);
	
	double maxVal;
	if ( m_isRaw )
	{
		maxVal = MAX_16_BIT;
	} else {
		maxVal = MAX_8_BIT;
	}
	
	
	for (int row = 0; row < 21; ++row)
	{
		roi.m_redData[row*21+10] = maxVal;
		roi.m_greenData[row*21+10] = 0;
		roi.m_blueData[row*21+10] = 0;
	}
	
	for (int col = 0; col < 21; ++col)
	{
		roi.m_redData[10*21+col] = maxVal;
		roi.m_greenData[10*21+col] = 0;
		roi.m_blueData[10*21+col] = 0;
	}
	
	roi.disp(fullDynamic);
	
}

//----------------------------------------------------------------------
//---- Retourne une image zoom centre sur un point donne
//----------------------------------------------------------------------
Image Image::getZoomOnPoint(const cv::Point & pt) const
{
	Image roi = getRoi(pt.x - 10, pt.y - 10, 21, 21);
	
	double maxVal;
	if ( m_isRaw )
	{
		maxVal = MAX_16_BIT;
	} else {
		maxVal = MAX_8_BIT;
	}
	
	for (int row = 0; row < 21; ++row)
	{
		roi.m_redData[row*21+10] = maxVal;
		roi.m_greenData[row*21+10] = 0;
		roi.m_blueData[row*21+10] = 0;
	}
	
	for (int col = 0; col < 21; ++col)
	{
		roi.m_redData[10*21+col] = maxVal;
		roi.m_greenData[10*21+col] = 0;
		roi.m_blueData[10*21+col] = 0;
	}
	
	return roi;
	
}


//----------------------------------------------------------------------
//---- Incrustation d'une image dans l'image
//----------------------------------------------------------------------
void Image::incrust(const Image & imToIncrust, int x0, int y0)
{
	int index = 0;
	for (int ligne = y0; ligne < y0 + imToIncrust.getYSize(); ++ligne)
	{
		for (int col = x0; col < x0 + imToIncrust.getXSize(); ++col)
		{
			m_redData[ligne*m_dx+col] += imToIncrust.getRedValue(index);
			m_greenData[ligne*m_dx+col] += imToIncrust.getGreenValue(index);
			m_blueData[ligne*m_dx+col] += imToIncrust.getBlueValue(index);
			
			index += 1;
		}
	}
	
}


//----------------------------------------------------------------------
//---- Incrustation d'une image dans l'image en sous-pixel
//----------------------------------------------------------------------
void Image::incrustSub(const Image & imToIncrust, double x0, double y0)
{
	
	//---- Pixel de depart
	int xMin = std::ceil(x0);
	int yMin = std::ceil(y0);
	
	double shiftX = x0 - std::floor(x0);
	double shiftY = y0 - std::floor(y0);
	
	//---- Parcours des pixels
	int indexLigne = 0;
	int indexCol = 0;
	for (int ligne = yMin; ligne < yMin + imToIncrust.getYSize(); ++ligne)
	{
		indexCol = 0;
		for (int col = xMin; col < xMin + imToIncrust.getXSize(); ++col)
		{
			//---- Get interpolated value
			m_redData[ligne*m_dx+col] += imToIncrust.interpolBil(indexCol+shiftX, indexLigne+shiftY, RED);
			m_greenData[ligne*m_dx+col] += imToIncrust.interpolBil(indexCol+shiftX, indexLigne+shiftY, GREEN);
			m_blueData[ligne*m_dx+col] += imToIncrust.interpolBil(indexCol+shiftX, indexLigne+shiftY, BLUE);
			
			indexCol += 1;
		}
		indexLigne += 1;
	}
	
	
	
}


//----------------------------------------------------------------------
//---- Addition pixel a pixel de 2 images
//----------------------------------------------------------------------
bool Image::add(const Image & im)
{
	//---- Verification des dimensions
	if ( (m_dx != im.m_dx) || (m_dy != im.m_dy) )
	{
		std::cout << "ERREUR : Les images n'ont pas la meme dimension" << std::endl;
		return false;
	}
	
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		m_redData[index] += im.m_redData[index];
		m_greenData[index] += im.m_greenData[index];
		m_blueData[index] += im.m_blueData[index];
		
		if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
		if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
		if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
		
	}
	
	return true;
}


//----------------------------------------------------------------------
//---- Addition pixel a pixel de 2 images avec masque
//----------------------------------------------------------------------
bool Image::addWithMask(const Image & im, const Image & mask, Image & imCardinal)
{
    //---- Verification des dimensions
    if ( (m_dx != im.m_dx) || (m_dy != im.m_dy) )
    {
        std::cout << "ERREUR : Les images n'ont pas la meme dimension" << std::endl;
        return false;
    }

    //---- Parcours des pixels
    for (int index = 0; index < m_nbPixel; ++index)
    {
        if ( mask.m_redData[index] == 0 )
        {
            m_redData[index] += im.m_redData[index];
            m_greenData[index] += im.m_greenData[index];
            m_blueData[index] += im.m_blueData[index];
            
            imCardinal.m_redData[index] += 1;
            imCardinal.m_greenData[index] += 1;
            imCardinal.m_blueData[index] += 1;
        }
        
        if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
        if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
        if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
        
    }

    return true;

}


//----------------------------------------------------------------------
//---- Soustraction de l'image par une autre
//----------------------------------------------------------------------
bool Image::subtract(const Image & im)
{
	//---- Verification des dimensions
	if ( (m_dx != im.m_dx) || (m_dy != im.m_dy) )
	{
		std::cout << "ERREUR : Les images n'ont pas la meme dimension" << std::endl;
		std::cout << "         Image courante " << m_dx << " x " << m_dy << std::endl;
		std::cout << "         Image a soustraire " << im.m_dx << " x " << im.m_dy << std::endl;
		return false;
	}
	
	//---- Reinitialisation des max
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		m_redData[index] -= im.m_redData[index];
		if ( m_redData[index] < 0.0 ) { m_redData[index] = 0.0; }
		
		m_greenData[index] -= im.m_greenData[index];
		if ( m_greenData[index] < 0.0 ) { m_greenData[index] = 0.0; }
		
		m_blueData[index] -= im.m_blueData[index];
		if ( m_blueData[index] < 0.0 ) { m_blueData[index] = 0.0; }
		
		
		if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
		if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
		if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
		
	}
	
	return true;
}


//----------------------------------------------------------------------
//---- Division de l'ensemble des pixels l'image
//----------------------------------------------------------------------
bool Image::div(double denominateur)
{
	//---- Test
	if ( denominateur == 0.0 )
	{
		std::cout << "ERREUR : Division par 0" << std::endl;
		return false;
	}
	
	//---- Reinitialisation des max
	m_maxRed = 0.0;
	m_maxGreen = 0.0;
	m_maxBlue = 0.0;
	
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		m_redData[index] /= denominateur;
		m_greenData[index] /= denominateur;
		m_blueData[index] /=denominateur;
		
		if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
		if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
		if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
	
	}
	
	return true;
}

//----------------------------------------------------------------------
//---- Division pixel a pixel d'une image par une autre
//----------------------------------------------------------------------
bool Image::divByIm(const Image & imDiv, double biais)
{
    //---- Verification des dimensions
    if ( (m_dx != imDiv.m_dx) || (m_dy != imDiv.m_dy) )
    {
        std::cout << "ERREUR : Les images n'ont pas la meme dimension" << std::endl;
        std::cout << "         Image courante " << m_dx << " x " << m_dy << std::endl;
        std::cout << "         Image a diviser " << imDiv.m_dx << " x " << imDiv.m_dy << std::endl;
        return false;
    }

    //---- Reinitialisation des max
    m_maxRed = 0.0;
    m_maxGreen = 0.0;
    m_maxBlue = 0.0;

    //---- Parcours des pixels
    for (int index = 0; index < m_nbPixel; ++index)
    {
        //--- Canal rouge
        if ( imDiv.m_redData[index] != 0 )
        {
            m_redData[index] /= (imDiv.m_redData[index] + biais);
        }
        if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
        
        //--- Canal vert
        if ( imDiv.m_greenData[index] != 0 )
        {
            m_greenData[index] /= (imDiv.m_greenData[index] + biais);
        }
        if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
        
        //--- Canal bleu
        if ( imDiv.m_blueData[index] != 0 )
        {
            m_blueData[index] /= (imDiv.m_blueData[index] + biais);
        }
        if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
        
    }

    return true;

}


//----------------------------------------------------------------------
//---- Calcul de la moyenne de 'limage
//----------------------------------------------------------------------
double Image::getMeanGreenImage() const
{
	double mean = 0.0;
	
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		mean += m_greenData[index];
	}
	
	mean /= m_nbPixel;
	
	return mean;
}


//----------------------------------------------------------------------
//---- Calcul du sigma de l'image
//----------------------------------------------------------------------
double Image::getSigmaGreenImage(double meanGreenImage) const
{
	double sigma = 0.0;
	
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		sigma += (m_greenData[index] - meanGreenImage) * (m_greenData[index] - meanGreenImage);
	}
	
	sigma = sqrt(sigma / m_nbPixel);
	
	return sigma;
}


//----------------------------------------------------------------------
//---- Binarisation par seuillage
//----------------------------------------------------------------------
Image Image::seuillage(double seuil)
{
	Image imSeuil(m_dx, m_dy);
	
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		//std::cout << m_greenData[index] << ", " << seuil << std::endl;
		if ( m_greenData[index] >= seuil )
		{
			imSeuil.m_redData[index] = MAX_8_BIT;
			imSeuil.m_greenData[index] = MAX_8_BIT;
			imSeuil.m_blueData[index] = MAX_8_BIT;
		}
	}
	
	m_maxRed = MAX_8_BIT;
	m_maxGreen = MAX_8_BIT;
	m_maxBlue = MAX_8_BIT;
	
	return imSeuil;
}


//----------------------------------------------------------------------
//---- Binarisation par seuillage automatique sur la moyenne du canal vert
//----------------------------------------------------------------------
Image Image::seuillageMeanAuto()
{
	Image imSeuil(m_dx, m_dy);
    
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		
		if ( m_greenData[index] >= 2.0*m_meanGreen )
		{
			imSeuil.m_redData[index] = MAX_8_BIT;
			imSeuil.m_greenData[index] = MAX_8_BIT;
			imSeuil.m_blueData[index] = MAX_8_BIT;
		}
	}
	
	m_maxRed = MAX_8_BIT;
	m_maxGreen = MAX_8_BIT;
	m_maxBlue = MAX_8_BIT;
	
	return imSeuil;
}



//----------------------------------------------------------------------
//---- Detection d'objets sur image binaire
//----------------------------------------------------------------------
std::vector<Image::sBoundingBox> Image::detectObj() const
{
	std::vector<Image::sBoundingBox> vecColObj;
	std::vector<Image::sBoundingBox> vecObj;
	
	//---- Parcours des colonnes
	double previousSum = 0.0;
	Image::sBoundingBox currentObj;
	for (int col = 0; col < m_dx; ++col)
	{
		//--- Calcul de la somme des pixels pour une colonne
		double currentSum = 0.0;
		for (int ligne = 0; ligne < m_dy; ++ligne)
		{
			currentSum += m_redData[ligne*m_dx+col];
		}
		
		if ( (previousSum == 0.0) && (currentSum > 0.0) )
		{
			currentObj.xMin = col;
		}
		
		if ( (previousSum > 0.0) && (currentSum == 0.0) )
		{
			currentObj.xMax = col;
			vecColObj.push_back(currentObj);
		}
		
		previousSum = currentSum;
	}
	
	
	//---- Parcours des objets "colonne"
	for (std::vector<Image::sBoundingBox>::iterator iObj = vecColObj.begin(); iObj != vecColObj.end(); ++iObj)
	{
		//--- Parcours des lignes
		double previousSum = 0.0;
		Image::sBoundingBox currentObj;
		for (int ligne = 0; ligne < m_dy; ++ligne)
		{
			//- Calcul de la somme des pixels sur les colonnes de l'objet
			double currentSum = 0.0;
			for (int col = iObj->xMin; col <= iObj->xMax; ++col)
			{
				currentSum += m_redData[ligne*m_dx+col];
			}
			
			if ( (previousSum == 0.0) && (currentSum > 0.0) )
			{
				currentObj.xMin = iObj->xMin;
				currentObj.xMax = iObj->xMax;
				currentObj.yMin = ligne;
			}
			
			if ( (previousSum > 0.0) && (currentSum == 0.0) )
			{
				currentObj.yMax = ligne;
				vecObj.push_back(currentObj);
			}
			
			previousSum = currentSum;
		}
	}
	
	return vecObj;
	
	
}


//----------------------------------------------------------------------
//---- Retourne l'objet le plus gros
//----------------------------------------------------------------------
Image::sBoundingBox Image::biggestObj(const std::vector<Image::Image::sBoundingBox> & vecObj) const
{
	Image::sBoundingBox bigObj;
	
	//---- Parcours des objets
	double maxArea = 0.0;
	for (std::vector<Image::Image::sBoundingBox>::const_iterator iObj = vecObj.begin(); iObj != vecObj.end(); ++iObj)
	{
		//--- Calcul de surface
		double area = (iObj->xMax - iObj->xMin + 1) * (iObj->yMax - iObj->yMin + 1);
		
		if ( area > maxArea )
		{
			maxArea = area;
			bigObj = *iObj;
		}
	}
	
	return bigObj;
}


//----------------------------------------------------------------------
//---- Detection d'objets sur image binaire
//----------------------------------------------------------------------
bool Image::detectBiggestObj(Image::sBoundingBox & bbox) const
{
	
	std::vector<Image::sBoundingBox> vecObj = detectObj();
	std::cout << "INFO   : " << vecObj.size() << " objets detectes" << std::endl;
	
    if ( vecObj.size() > 0 )
    {
        bbox = biggestObj(vecObj);
        
    } else {
        
        return false;
    }
    
    return true;
}


//----------------------------------------------------------------------
//---- Calcul le profil suivant une ligne
//----------------------------------------------------------------------
std::vector<double> Image::lineProfil(int line, int colMin, int colMax)
{
	std::vector<double> profil;
	
	//---- Parcours de la ligne
	for (int col = colMin; col <= colMax; ++col)
	{
		profil.push_back(m_greenData[line*m_dx+col]);
	} 
	
	
	return profil;
}


//----------------------------------------------------------------------
//---- Calcul le profil suivant une colonne
//----------------------------------------------------------------------
std::vector<double> Image::colProfil(int col, int lineMin, int lineMax)
{
	std::vector<double> profil;
	
	//---- Parcours de la colonne
	for (int line = lineMin; line <= lineMax; ++line)
	{
		profil.push_back(m_greenData[line*m_dx+col]);
	} 
	
	return profil;
}


//----------------------------------------------------------------------
//---- Application d'un filtre laplacien
//----------------------------------------------------------------------
void Image::computeLaplacian(int kernelSize, int scale, int delta)
{
	//---- Conversion en niveau de gris
	cv::Mat imGrayMat;
	cv::cvtColor(getCvMat(), imGrayMat, CV_BGR2GRAY);
	
	int ddepth = CV_16S;
	cv::Mat abs_dst, dst;
	cv::Laplacian(imGrayMat, dst, ddepth, kernelSize, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(dst, abs_dst);
	std::cout << "### sum : " << cv::sum(abs_dst) << std::endl;
	std::cout << "### mean : " << cv::mean(abs_dst) << std::endl;
	
	cv::imshow("laplacian", abs_dst);
	cv::waitKey(0);
}


//----------------------------------------------------------------------
//---- Application d'un filtre de sobel
//----------------------------------------------------------------------
void Image::computeSobel(int kernelSize, int scale, int delta)
{
	//---- Conversion en niveau de gris
	cv::Mat imGrayMat;
	cv::cvtColor(getCvMat(), imGrayMat, CV_BGR2GRAY);
	
	int ddepth = CV_16S;
	cv::Mat abs_dst, dst;
	cv::Sobel(imGrayMat, dst, ddepth, 1, 0, kernelSize, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(dst, abs_dst);
	
	std::cout << "### sum X: " << cv::sum(abs_dst) << std::endl;
	std::cout << "### mean : " << cv::mean(abs_dst) << std::endl;
	cv::imshow("soblex", abs_dst);
	cv::waitKey(0);
	
	
	cv::Sobel(imGrayMat, dst, ddepth, 0, 1, kernelSize, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(dst, abs_dst);
	
	std::cout << "### sum Y: " << cv::sum(abs_dst) << std::endl;
	std::cout << "### mean : " << cv::mean(abs_dst) << std::endl;
	cv::imshow("sobely", abs_dst);
	cv::waitKey(0);
}


//----------------------------------------------------------------------
//---- Application d'une gaussienne
//----------------------------------------------------------------------
Image Image::computeGauss(int xKernel, int yKernel)
{
	cv::Mat imMat = getCvMat();
	cv::Mat imGauss;
	
	cv::GaussianBlur(imMat, imGauss, cv::Size(xKernel, yKernel), 0, 0);
	
	//cv::imshow("Gauss", imGauss);
	//cv::waitKey(0);
	
	Image im(imGauss);
	
	return im;
}


//----------------------------------------------------------------------
//---- Calcul du centre d'une etoile
//----------------------------------------------------------------------
cv::Point2f Image::computeStarCenter()
{
	cv::Point2f starCenter;
	
	//---- Calcul DN et somme suivant les colonnes
	double DN = 0.0;
	double ySomme = 0.0;
	double flux = 0.0;
	double currentValue;
	for (int ligne = 0; ligne < m_dy; ++ligne)
	{
		for (int col = 0; col < m_dx; ++col)
		{
			currentValue = m_greenData[ligne*m_dx+col];
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
	for (int col = 0; col < m_dx; ++col)
	{
		for (int ligne = 0; ligne < m_dy; ++ligne)
		{
			currentValue = m_greenData[ligne*m_dx+col];
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
//---- Selection d'un point dans une image
//----------------------------------------------------------------------
cv::Point2i Image::selectPoint()
{
	int posXY[2];
	
	cv::Mat imMat = getCvMatNew();
	
	namedWindow("Display", cv::WINDOW_NORMAL);
	cv::setMouseCallback("Display", CallBackSelectPt, posXY);
		
	cv::imshow("Display", imMat);
	cv::waitKey(0);
	
	cv::Point2i selectedPoint;
	selectedPoint.x = posXY[0];
	selectedPoint.y = posXY[1];
	
	return selectedPoint;
	
}


//----------------------------------------------------------------------
//---- Calcul de l'histogramme de l'image
//----------------------------------------------------------------------
std::vector<double> Image::computeHisto(int nbClasse)
{
	//---- Calcul des min / max de l'image
	double valMin = std::numeric_limits<double>::max();
	double valMax = 0.0;
	
	double currentVal = 0.0;
	//---- Parcours des pixels
	for (int index = 0; index < m_nbPixel; ++index)
	{
		currentVal = (m_redData[index] + m_greenData[index] + m_blueData[index]) / 3.0;
		
		if ( currentVal < valMin ) { valMin = currentVal; }
		if ( currentVal > valMax ) { valMax = currentVal; }
		
	}
	std::cout << "#minmax " << valMin << ", " << valMax << std::endl;
	
	//---- Calcul de la largeur radiometrique d'une classe
	double classSize = (valMax - valMin) / (double)nbClasse;
	
	
	int classIndex = 0;
	//--- Calcul de l'histogramme
	int histo[nbClasse];
	memset(histo, 0, nbClasse * sizeof(int));
	for (int index = 0; index < m_nbPixel; ++index)
	{
		currentVal = (m_redData[index] + m_greenData[index] + m_blueData[index]) / 3.0;
		
		classIndex = std::floor(currentVal / classSize) - 1;
		
		histo[classIndex] += 1;
	}
	
	//---- Mise en forme de l'histogramme
	std::vector<double> vecHisto;
	for (unsigned int index = 0; index < vecHisto.size(); ++index)
	{
		vecHisto.push_back(histo[index]);
	}
	
	
	
	return vecHisto;
}


//----------------------------------------------------------------------
//---- Calcul d'un gradient vertical
//----------------------------------------------------------------------
void Image::computeVerticalGradient(double & redCoeff, double & greenCoeff, double & blueCoeff)
{
    int nbLines = 10;
    
    //---- Calcul sur les n premieres lignes
    double beginRedVal = 0.0;
    double beginGreenVal = 0.0;
    double beginBlueVal = 0.0;
    
    for (int lig = 0; lig < nbLines; ++lig)
    {
        double currentMeanValR = 0.0;
        double currentMeanValG = 0.0;
        double currentMeanValB = 0.0;
        for (int col = 0; col < m_dx; ++col)
        {
            currentMeanValR += m_redData[lig*m_dx+col];
            currentMeanValG += m_greenData[lig*m_dx+col];
            currentMeanValB += m_blueData[lig*m_dx+col];
            
        }
        
        beginRedVal += currentMeanValR / double(m_dx);
        beginGreenVal += currentMeanValG / double(m_dx);
        beginBlueVal += currentMeanValB / double(m_dx);
    }
    
    beginRedVal /= double(nbLines);
    beginGreenVal /= double(nbLines);
    beginBlueVal /= double(nbLines);
    
    
    //---- Calcul sur les n dernieres lignes
    double endRedVal = 0.0;
    double endGreenVal = 0.0;
    double endBlueVal = 0.0;
    
    for (int lig = m_dy-nbLines; lig < m_dy; ++lig)
    {
        double currentMeanValR = 0.0;
        double currentMeanValG = 0.0;
        double currentMeanValB = 0.0;
        for (int col = 0; col < m_dx; ++col)
        {
            currentMeanValR += m_redData[lig*m_dx+col];
            currentMeanValG += m_greenData[lig*m_dx+col];
            currentMeanValB += m_blueData[lig*m_dx+col];
            
        }
        
        endRedVal += currentMeanValR / double(m_dx);
        endGreenVal += currentMeanValG / double(m_dx);
        endBlueVal += currentMeanValB / double(m_dx);
    }
    
    endRedVal /= double(nbLines);
    endGreenVal /= double(nbLines);
    endBlueVal /= double(nbLines);
    
    
    redCoeff = (endRedVal - beginRedVal + 1) / double(m_dy);
    greenCoeff = (endGreenVal - beginGreenVal + 1) / double(m_dy);
    blueCoeff = (endBlueVal - beginBlueVal + 1) / double(m_dy);
    
}


//----------------------------------------------------------------------
//---- Suppression d'un gradient vertical
//----------------------------------------------------------------------
void Image::removeVerticalGradient()
{
    //---- Calcul du gradient vertical
    double redCoeff, greenCoeff, blueCoeff;
    computeVerticalGradient(redCoeff, greenCoeff, blueCoeff);
    
    //---- Suppression du gradient
    double redVal, greenVal, blueVal;
    for (int lig = 0; lig < m_dy; ++lig)
    {
        redVal = lig * redCoeff;
        greenVal = lig * greenCoeff;
        blueVal = lig * blueCoeff;
        
        
        for (int col = 0; col < m_dx; ++col)
        {
            m_redData[lig*m_dx+col] -= redVal;
            m_greenData[lig*m_dx+col] -= greenVal;
            m_blueData[lig*m_dx+col] -= blueVal;
            
        }
        
    }
    
}


//----------------------------------------------------------------------
//---- Calcul d'un gradient horizontal
//----------------------------------------------------------------------
void Image::computeHorizontalGradient(double & redCoeff, double & greenCoeff, double & blueCoeff)
{
    int nbCol = 10;
    
    //---- Calcul sur les n premieres lignes
    double beginRedVal = 0.0;
    double beginGreenVal = 0.0;
    double beginBlueVal = 0.0;
    
    for (int col = 0; col < nbCol; ++col)
    {
        double currentMeanValR = 0.0;
        double currentMeanValG = 0.0;
        double currentMeanValB = 0.0;
        for (int lig = 0; lig < m_dy; ++lig)
        {
            currentMeanValR += m_redData[lig*m_dx+col];
            currentMeanValG += m_greenData[lig*m_dx+col];
            currentMeanValB += m_blueData[lig*m_dx+col];
            
        }
        
        beginRedVal += currentMeanValR / double(m_dy);
        beginGreenVal += currentMeanValG / double(m_dy);
        beginBlueVal += currentMeanValB / double(m_dy);
    }
    
    beginRedVal /= double(nbCol);
    beginGreenVal /= double(nbCol);
    beginBlueVal /= double(nbCol);
    
    
    //---- Calcul sur les n dernieres lignes
    double endRedVal = 0.0;
    double endGreenVal = 0.0;
    double endBlueVal = 0.0;
    
    for (int col = m_dx-nbCol; col < m_dx; ++col)
    {
        double currentMeanValR = 0.0;
        double currentMeanValG = 0.0;
        double currentMeanValB = 0.0;
        for (int lig = 0; lig < m_dy; ++lig)
        {
            currentMeanValR += m_redData[lig*m_dx+col];
            currentMeanValG += m_greenData[lig*m_dx+col];
            currentMeanValB += m_blueData[lig*m_dx+col];
            
        }
        
        endRedVal += currentMeanValR / double(m_dy);
        endGreenVal += currentMeanValG / double(m_dy);
        endBlueVal += currentMeanValB / double(m_dy);
    }
    
    endRedVal /= double(nbCol);
    endGreenVal /= double(nbCol);
    endBlueVal /= double(nbCol);
    
    
    redCoeff = (endRedVal - beginRedVal + 1) / double(m_dx);
    greenCoeff = (endGreenVal - beginGreenVal + 1) / double(m_dx);
    blueCoeff = (endBlueVal - beginBlueVal + 1) / double(m_dx);
    
}


//----------------------------------------------------------------------
//---- Suppression d'un gradient horizontal
//----------------------------------------------------------------------
void Image::removeHorizontalGradient()
{
    //---- Calcul du gradient horizontal
    double redCoeff, greenCoeff, blueCoeff;
    computeHorizontalGradient(redCoeff, greenCoeff, blueCoeff);
    
    //---- Suppression du gradient
    double redVal, greenVal, blueVal;
    for (int col = 0; col < m_dx; ++col)
    {
        redVal = col * redCoeff;
        greenVal = col * greenCoeff;
        blueVal = col * blueCoeff;
        
        
        for (int lig = 0; lig < m_dy; ++lig)
        {
            m_redData[lig*m_dx+col] -= redVal;
            m_greenData[lig*m_dx+col] -= greenVal;
            m_blueData[lig*m_dx+col] -= blueVal;
            
        }
        
    }
    
}


//----------------------------------------------------------------------
//---- Merge with mask
//----------------------------------------------------------------------
void Image::mergeWithMask(const Image & firstPlan, const Image & mask)
{
    
    //---- Parcours des pixels
    for (int index = 0; index < m_nbPixel; ++index)
    {
        if ( mask.m_redData[index] > 0 )
        {
            m_redData[index] = firstPlan.m_redData[index];
            m_greenData[index] = firstPlan.m_greenData[index];
            m_blueData[index] = firstPlan.m_blueData[index];
            
        }
        
        if ( m_redData[index] > m_maxRed ) { m_maxRed = m_redData[index]; }
        if ( m_greenData[index] > m_maxGreen ) { m_maxGreen = m_greenData[index]; }
        if ( m_blueData[index] > m_maxBlue ) { m_maxBlue = m_blueData[index]; }
        
    }
    
    
}


