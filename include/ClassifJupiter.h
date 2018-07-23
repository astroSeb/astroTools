//----------------------------------------------------------------------
//
//---- Classe ClassifJupiter
//
//----------------------------------------------------------------------


#ifndef CLASSIF_JUPITER_H
#define CLASSIF_JUPITER_H

#include "Image.h"
#include "Util.h"

class ClassifJupiter
{
	
	public:
	
	
	//---- Constructeurs
	ClassifJupiter(Image & im, Image::sBoundingBox & object);
	
	//---- Destructeur
	~ClassifJupiter() {}
	
	
	
	//---- Classification par detection des minima et maxima locaux
	//     present sur un profil et correspondants aux bandes
	bool classifMinMax();
	
	
	

protected:
	
	
	
	//---- Donnees membres
	Image m_im;
	
	Image::sBoundingBox m_object;
	

};


#endif 
