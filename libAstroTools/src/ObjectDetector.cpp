#include "ObjectDetector.h"

namespace astroT
{

//----------------------------------------------------------------------
//---- Detection d'objets sur image binaire
//----------------------------------------------------------------------
std::vector<sBoundingBox> ObjectDetector::detectObj(const Image & im) const
{
    std::vector<sBoundingBox> vecColObj;
    std::vector<sBoundingBox> vecObj;

    //---- Parcours des colonnes
    int dx = im.getXSize();
    int dy = im.getYSize();
    double previousSum = 0.0;
    sBoundingBox currentObj;
    for (int col = 0; col < dx; ++col)
    {
        //--- Calcul de la somme des pixels pour une colonne
        double currentSum = 0.0;
        for (int ligne = 0; ligne < dy; ++ligne)
        {
            currentSum += im.getRedValue(ligne*dx+col);
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
    for (std::vector<sBoundingBox>::iterator iObj = vecColObj.begin(); iObj != vecColObj.end(); ++iObj)
    {
        //--- Parcours des lignes
        double previousSum = 0.0;
        sBoundingBox currentObj;
        for (int ligne = 0; ligne < dy; ++ligne)
        {
            //- Calcul de la somme des pixels sur les colonnes de l'objet
            double currentSum = 0.0;
            for (int col = iObj->xMin; col <= iObj->xMax; ++col)
            {
                currentSum += im.getRedValue(ligne*dx+col);
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
sBoundingBox ObjectDetector::biggestObj(const std::vector<sBoundingBox> & vecObj) const
{
    sBoundingBox bigObj;

    //---- Parcours des objets
    double maxArea = 0.0;
    for (std::vector<sBoundingBox>::const_iterator iObj = vecObj.begin(); iObj != vecObj.end(); ++iObj)
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
//---- Detection de l'objet le plus gros sur image binaire
//----------------------------------------------------------------------
bool ObjectDetector::detectBiggestObj(const Image & im, sBoundingBox & bbox) const
{
    std::vector<sBoundingBox> vecObj = detectObj(im);
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
//---- Detection des etoiles
//----------------------------------------------------------------------
std::vector<sBoundingBox> ObjectDetector::detectStar(const Image & im)
{
    //---- Calcul du seuil pour binarisation
    int nbClasse = 100;
    std::vector<double> histo = im.computeHisto(nbClasse);
    
    double nbPixel = im.getNbPixel();
    double currentNbPix = 0.0;
    
    int classeSeuil = 0;
    for (int index = 0; index < nbClasse; ++index)
    {
        currentNbPix += histo[index];
        //std::cout << "### index " << index << ", histo " << histoRef[index] << ", proportion " << currentNbPix / nbPixel << std::endl;
        if ( (currentNbPix / nbPixel) >= 0.998 )
        {
            classeSeuil = index;
            break;
        }
    }
    double seuil;
    if (im.isRaw()) { seuil = (double)MAX_16_BIT / (double)nbClasse * (double)classeSeuil; }
    else { seuil = (double)MAX_8_BIT / (double)nbClasse * (double)classeSeuil; }
    //std::cout << "INFO   : Seuil " << seuil << ", classe seuil " << classeSeuil << std::endl;
    
    //---- Binarisation
    Image imBin = im.seuillage(seuil);
    //astroT::dispIm(imRefBin, true);
    
    
    //---- Detection des objets dans l'image binaire par floodFill
    int dx = imBin.getXSize();
    int dy = imBin.getYSize();
    std::vector<sBoundingBox> vecRoi;
    double seuilDet = 0;
    //--- Parcour de l'image binaire
    for (int y = 0; y < dy; ++y)
    {
        for (int x = 0; x < dx; ++x)
        {
            sPointD currentPos(x, y);
            
            if ( imBin.getMeanValue(y*dx+x) > seuilDet )
            {
                sBoundingBox currentBbox;
                currentBbox.xMin = imBin.getXSize();
                currentBbox.xMax = 0;
                currentBbox.yMin = imBin.getYSize();
                currentBbox.yMax = 0;
                
                floodFill(imBin, currentPos, currentBbox, seuilDet);
                
                vecRoi.push_back(currentBbox);
            }
            
        }
    }
    
    return vecRoi;
}


//----------------------------------------------------------------------
//--- Segmentation par accroissement de region
//----------------------------------------------------------------------
void ObjectDetector::floodFill(Image & im, const sPointD & currentPos, sBoundingBox & currentBbox, double seuil)
{
    
    //---- Mise a jour de l'objet courant
    if ( currentPos.x < currentBbox.xMin ) { currentBbox.xMin = currentPos.x; }
    if ( currentPos.x > currentBbox.xMax ) { currentBbox.xMax = currentPos.x; }
    if ( currentPos.y < currentBbox.yMin ) { currentBbox.yMin = currentPos.y; }
    if ( currentPos.y > currentBbox.yMax ) { currentBbox.yMax = currentPos.y; }
    
    //---- Mise a jour de l'image temporaire
    im.setValues(0.0, 0.0, 0.0, currentPos.y*im.getXSize()+currentPos.x);

    //---- Recherche dans les pixels voisins
    for (int y = currentPos.y-1; y <= currentPos.y+1; ++y)
    {
        if ( (y >= 0) && (y < im.getYSize()) )
        {
            for (int x = currentPos.x-1; x <= currentPos.x+1; ++x)
            {
                if ( (x >= 0) && (x <im.getXSize()) )
                {
                    if ( im.getMeanValue(y*im.getXSize()+x) > seuil )
                    {
                        sPointD nextPos(x, y);
                        floodFill(im, nextPos, currentBbox, seuil);
                    }
                }
            }
        }
    }
}



//---- Fin namespace astroT
}
