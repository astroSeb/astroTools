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

//---- Fin namespace astroT
}

#endif 
