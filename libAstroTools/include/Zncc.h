//----------------------------------------------------------------------
//
//---- Classe Zncc
//
//----------------------------------------------------------------------


#ifndef ZNCC_H
#define ZNCC_H

#include "Image.h"


namespace astroT
{
    
    class Zncc
    {
        
        public:
        
        //---- Constructeur
        Zncc();

        //---- Destructeur
        ~Zncc();
        
        //---- Setteur
        void setImRef(Image * imRef);
        void setImSec(Image * imSec);
        void setImages(Image * imRef, Image * imSec);
        
        //---- Getteur
        
        
        //---- Methodes
        //--- Calcul de la valeur du ZNCC
        bool computeZncc(double & valZncc);
        
        
        protected:
        
        //---- Methodes membre
        
        
        //---- Donnees membres
        //--- Imagette de reference
        Image * m_imRef;
        double m_meanRef;
        
        //--- Imagette secondaire
        Image * m_imSec;
        double m_meanSec;
        
        
    };

//---- Fin du namespace astroT
}

#endif 
