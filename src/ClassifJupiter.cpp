#include "ClassifJupiter.h"

#define HALF_WIN 10
#define WIN_LISSAGE 30

//----------------------------------------------------------------------
//---- Constructeur
//----------------------------------------------------------------------
ClassifJupiter::ClassifJupiter(Image & im, Image::sBoundingBox & object)
{
	m_im = im;
	
	m_object = object;
}

//----------------------------------------------------------------------
//---- Classification par detection des minima et maxima locaux
//     present sur un profil et correspondants aux bandes
//----------------------------------------------------------------------
bool ClassifJupiter::classifMinMax()
{
	
	//---- Extraction du profil
	std::vector<double> profil = m_im.lineProfil((m_object.yMin+m_object.yMax)/2, m_object.xMin-20, m_object.xMax+20);
	
	//--- Lissage du profil
	std::vector<double> profilLisse = lissage(profil, WIN_LISSAGE);
	
	//---- Recherche des minimum et maximum locaux
	double localMin, localMax;
	localMinMax(profilLisse, HALF_WIN, localMin, localMax);
	
	//---- Si pas de min local, la turbulence est tres forte
	if ( localMin == std::numeric_limits<double>::max() )
	{
		return false;
	}
	
	//---- Si la difference entre les valeurs max et min est < 10, alors turbulence forte
	if ( (localMax - localMin) < 10.0 )
	{
		return false;
	}
	
	return true;
}
