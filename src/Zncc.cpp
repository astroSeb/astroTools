#include "Zncc.h"


namespace recalage
{


//----------------------------------------------------------------------
//---- Constructeur
//----------------------------------------------------------------------
Zncc::Zncc()
{
    m_imRef = NULL;
    m_meanRef = 0.0;
    
    m_imSec = NULL;
    m_meanSec = 0.0;
}


//----------------------------------------------------------------------
//---- Destructeur
//----------------------------------------------------------------------
Zncc::~Zncc()
{
    
}


//----------------------------------------------------------------------
//---- Renseigne l'image de reference
//----------------------------------------------------------------------
void Zncc::setImRef(Image * imRef)
{
    m_imRef = imRef;
    m_meanRef = m_imRef->getMeanAll();
}


//----------------------------------------------------------------------
//---- Renseigne l'image secondeaire
//----------------------------------------------------------------------
void Zncc::setImSec(Image * imSec)
{
    m_imSec = imSec;
    m_meanSec = m_imSec->getMeanAll();
}


//----------------------------------------------------------------------
//---- Renseigne les images
//----------------------------------------------------------------------
void Zncc::setImages(Image * imRef, Image * imSec)
{
    m_imRef = imRef;
    m_meanRef = m_imRef->getMeanAll();
    
    m_imSec = imSec;
    m_meanSec = m_imSec->getMeanAll();
}


//----------------------------------------------------------------------
//---- Calcul de la valeur du Zncc
//----------------------------------------------------------------------
bool Zncc::computeZncc(double & valZncc)
{
    //---- Verification de la presence des images
    if ( m_imRef == NULL )
    {
        std::cout << "ERREUR : L'image de reference n'est pas renseignee" << std::endl;
        return false;
    }
    
    if ( m_imSec == NULL )
    {
        std::cout << "ERREUR : L'image secondaire n'est pas renseignee" << std::endl;
        return false;
    }
    
    //---- Verification de la taille des images
    if ( (m_imRef->getXSize() != m_imSec->getXSize()) || (m_imRef->getYSize() != m_imSec->getYSize()) )
    {
        std::cout << "ERREUR : Les imagettes a correler doivent avoir les memes dimensions" << std::endl;
        std::cout << "ERREUR : Ref " << m_imRef->getXSize() << ", " << m_imRef->getYSize() << ", Sec " << m_imSec->getXSize() << ", " << m_imSec->getYSize() << std::endl;
        return false;
    }
    
    //---- Calcul du ZNCC
    int nbPixel = m_imRef->getNbPixel();

    //--- Calcul sigma + zncc
    valZncc = 0.0;
    double imRefSigma = 0.0;
    double imSecSigma = 0.0;
    double currentRefDMean, currentSecDMean;
    
    for (int index = 0; index < nbPixel; ++index)
    {
        currentRefDMean = m_imRef->getMeanValue(index) - m_meanRef;
        currentSecDMean = m_imSec->getMeanValue(index) - m_meanSec;
        
        imRefSigma += currentRefDMean * currentRefDMean;
        imSecSigma += currentSecDMean * currentSecDMean;
            
        valZncc += currentRefDMean * currentSecDMean;
    }
    
    imRefSigma = sqrt(imRefSigma / nbPixel);
    imSecSigma = sqrt(imSecSigma / nbPixel);
    
    valZncc /= (nbPixel * imRefSigma * imSecSigma);
    
    return true;
}



//---- Fin namespace recalage
}
