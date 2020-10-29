#ifndef ASTROTYPEDEF_H
#define ASTROTYPEDEF_H

#include <vector>
#include <string>
#include <cmath>

namespace astroT
{
    //---- Constantes
    #define MAX_16_BIT 65535
    #define MAX_8_BIT 254
    
    //---- Enum couleur
    enum Ecolor
    {
        RED,
        GREEN,
        BLUE,
        WHITE,
        BLACK
    };
    
    //---- Structure bounding box
    struct sBoundingBox
    {
        int xMin;
        int xMax;
        int yMin;
        int yMax;
    };
    
    //---- Structure coordonnees d'un point en double
    struct sPointD
    {
        double x;
        double y;
        
        sPointD() { x = 0.0; y = 0.0; }
        sPointD(double X, double Y) { x = X; y = Y; }
    };

//---- Fin namespace astroT
}

#endif 
