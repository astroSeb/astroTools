//----------------------------------------------------------------------
//
//---- Classe ObjectDetector
//
//----------------------------------------------------------------------


#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <iostream>
#include <cstring>
#include <cmath>
#include <limits>
#include <vector>

#include "Image.h"


namespace astroT
{

class ObjectDetector
{
    
    public:

    //---- Constructeurs
    ObjectDetector() {}

    //---- Destructeur
    ~ObjectDetector() {}

    //---- Setteur

    //---- Getteur
    
    
    //---- Methodes
    //---- Detection d'objets sur image binaire
    std::vector<sBoundingBox> detectObj(const Image & im) const;
    
    //---- Retourne l'objet le plus gros
    sBoundingBox biggestObj(const std::vector<sBoundingBox> & vecObj) const;
    
    //---- Detection de l'objet le plus gros sur image binaire
    bool detectBiggestObj(const Image & im, sBoundingBox & bbox) const;
    
    //---- Detection des etoiles
    std::vector<sBoundingBox> detectStar(const Image & im);
    
    
protected:

    //---- Methodes membres
    void floodFill(Image & im, const sPointD & currentPos, sBoundingBox & currentBbox, double seuil);
    
    //---- Donnees membres
    

};

//---- Fin namespace astroT
}

#endif 
