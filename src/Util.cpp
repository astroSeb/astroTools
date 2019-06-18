//----------------------------------------------------------------------
//
//---- Util.cpp : Fonction utilitaires communes
//
//----------------------------------------------------------------------
#include "Util.h"


//----------------------------------------------------------------------
//---- Parsing d'un repertoire 
//----------------------------------------------------------------------
bool parseDir(const std::string & dirPath, const std::string & format, std::vector<std::string> & imagePathVect)
{
	std::string pattern = "";
	
	if ( format == "RAW" )
	{
		pattern = ".NEF";
	} else if ( format == "JPG" )
	{
		pattern = ".JPG";
	} else if ( format == "jpg" )
	{
		pattern = ".jpg";
	} else if ( format == "TIF" )
	{
		pattern = ".tif";
	} else {
		std::cout << "ERREUR : Format d'image " << format << " inconnu" << std::endl;
		return false;
	}
	
	
	DIR * rep = opendir(dirPath.c_str());

	if ( rep == NULL )
	{
		std::cout << "ERREUR : Le repertoire " << dirPath << " n'est pas accessible." << std::endl;
		return false;
	}

	struct dirent * ent;
	std::string currentImName;
	while ((ent = readdir(rep)) != NULL)
	{
		if ( strstr(ent->d_name,pattern.c_str()) != NULL)
		{
			currentImName = dirPath + "/" + ent->d_name;

			imagePathVect.push_back(currentImName);
		}
	}
	
	closedir(rep);
	
	return true;
}


//----------------------------------------------------------------------
//---- Export des profils dans un csv
//----------------------------------------------------------------------
bool exportProfils(const std::string & filePath, const std::map<std::string, std::vector<double> > & mapProfils)
{
	
	//---- Ouverture du fichier
	std::ofstream fichier(filePath.c_str(), std::ios::out);
	if (fichier)
	{
		//--- Parcours des profils
		for (std::map<std::string, std::vector<double> >::const_iterator iProfil = mapProfils.begin(); iProfil != mapProfils.end(); ++iProfil)
		{
			fichier << iProfil->first << ";";
			
			//---- Parcours des valeurs
			for ( std::vector<double>::const_iterator iVal = iProfil->second.begin(); iVal != iProfil->second.end(); ++iVal)
			{
				fichier << *iVal << ";";
			}
			
			fichier << std::endl;

		}
		
		fichier.close();
	} else {
		std::cout << "ERREUR : Impossible d'ecrire le fichier " << filePath << std::endl;
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------
//---- Copis de fichier
//----------------------------------------------------------------------
bool copyFile(const std::string & srcFilePath, const std::string & dstFilePath)
{
	//---- Ouverture du fichier source
	std::ifstream src(srcFilePath.c_str(), std::ios::binary);
	if ( !src )
	{
		std::cout << "ERREUR : Impossible d'ouvrir le fichier source " << srcFilePath << std::endl;
		return false;
	}
	
	//---- Creation du fichier destination
	std::ofstream dst(dstFilePath.c_str(), std::ios::binary);
	if ( !dst )
	{
		std::cout << "ERREUR : Impossible d'ouvrir le fichier destination " << dstFilePath << std::endl;
		return false;
	}
	
	//---- Copie des donnees
	dst << src.rdbuf();
	
	//---- Fermeture des fichiers
	src.close();
	dst.close();
	
	return true;
}


//----------------------------------------------------------------------
//---- Decoupe d'un string par un pattern
//----------------------------------------------------------------------
std::vector<std::string> splitStr(const std::string & str, const std::string & pattern)
{

	//std::cout << "String : " << str << std::endl;

	size_t pos = 0;
	size_t oldPos = 0;
	size_t patternSize = pattern.size();
	std::vector<std::string> result;
	bool first = true;

	while ( pos != std::string::npos )
	{
		if ( first )
		{
			oldPos = pos;
			pos = str.find(pattern.c_str(),pos+1,patternSize);
			result.push_back(str.substr(oldPos,pos-oldPos));
			
			first = false;
		} else {
			
			oldPos = pos;
			pos = str.find(pattern.c_str(),pos+1,patternSize);
			result.push_back(str.substr(oldPos+1,pos-oldPos-1));
		}
		
		//std::cout << "oldPos : " << oldPos << " pos : " << pos << " : param : " << str.substr(oldPos,pos-oldPos) << std::endl;

	}

	return result;
}


//----------------------------------------------------------------------
//---- Extraction d'un nom de fichier
//----------------------------------------------------------------------
std::string getFileName(const std::string & filePathName)
{
	std::vector<std::string> vecStr = splitStr(filePathName, "/");
	
	std::string fileName = vecStr.back();
	
	return fileName;
}


//----------------------------------------------------------------------
//---- Calcul des min et max d'un vecteur de double
//----------------------------------------------------------------------
void minMaxVecD(const std::vector<double> & data, double & min, double & max)
{
	
	min = std::numeric_limits<double>::max();
	max = std::numeric_limits<double>::min();
	
	//---- Recherche min max
	for (std::vector<double>::const_iterator iVal = data.begin(); iVal != data.end(); ++iVal)
	{
		if ( *iVal < min ) { min = *iVal; }
		if ( *iVal > max ) { max = *iVal; }
	}
	
}


//----------------------------------------------------------------------
//---- Calcul d'un histogramme
//----------------------------------------------------------------------
std::vector<int> computeHisto(std::vector<double> vecData, int largeurPlage)
{
	
	double min, max;
	
	//---- Recherche min max
	minMaxVecD(vecData, min, max);
	
	int nbPlage = std::ceil((max) / largeurPlage);
	
	std::vector<int> histo(nbPlage);
	//std::cout << nbPlage << std::endl;
	
	for (std::vector<double>::iterator iVal = vecData.begin(); iVal != vecData.end(); ++iVal)
	{
		int plage = (int)floor( (*iVal) / largeurPlage);
		histo[plage] += 1;
	}
	
	return histo;
}



//----------------------------------------------------------------------
//---- Lissage d'un vecteur de double par une moyenne sur une fenetre
//----------------------------------------------------------------------
std::vector<double> lissage(const std::vector<double> & vecProfil, int fenetre)
{
	std::vector<double> vecLisse;
	
	for (unsigned int index = 0; index < vecProfil.size()-fenetre; ++index)
	{
		double moyenne = 0;
		for (int i = 0; i < fenetre; ++i)
		{
			moyenne += vecProfil[index+i];
		}
		moyenne /= fenetre;
		
		vecLisse.push_back(moyenne);
	}
	
	
	return vecLisse;
}



//----------------------------------------------------------------------
//---- Calcul des min / max locaux d'un vecteur de doubles
//----------------------------------------------------------------------
void localMinMax(const std::vector<double> & vecProfil, int halfWin, double & localMin, double & localMax)
{
	
	localMax = 0.0;
	localMin = std::numeric_limits<double>::max();
	
	for (unsigned int index = halfWin; index < vecProfil.size() - halfWin; ++index)
	{
		double currentVal = vecProfil[index];
		double maxVal = currentVal;
		double minVal = currentVal;
		
		for (int i = -halfWin; i < halfWin; ++i)
		{
			double val = vecProfil[index+i];
			
			if (val > maxVal ) { maxVal = val; }
			if (val < minVal ) { minVal = val; }
			
		}
		
		if ( maxVal == currentVal )
		{
			if ( currentVal > localMax ) { localMax = currentVal; }
		}
		
		if ( minVal == currentVal )
		{
			if ( currentVal < localMin ) { localMin = currentVal; }
		}
		
		
		
	}
	
	
	std::cout << "###min: " << localMin << ", max: " << localMax << std::endl;
	
}


//-------------------------------------------------------------
//---- Comparaison de deux doubles
//-------------------------------------------------------------
bool compareDouble(double val1, double val2)
{
	bool inf = false;
	
	if ( val1 > val2 ) { inf = true; }
	
	return inf;
}


//----------------------------------------------------------------------
//---- Tri d'un vecteur de double
//----------------------------------------------------------------------
void sortVecDouble(std::vector<double> & vecDouble)
{
	std::sort(vecDouble.begin(), vecDouble.end(), compareDouble);
}


//----------------------------------------------------------------------
//---- Calcule la valeur mediane d'un vecteur de double
//----------------------------------------------------------------------
double medianVecD(const std::vector<double> & vec)
{
	double valMediane = 0.0;
	
	//---- Tri du vecteur par ordre croissant
	std::vector<double> vecTmp = vec;
	sortVecDouble(vecTmp);
	
	//---- Recherche de la valeur mediane
	unsigned int vecSize = vecTmp.size();
	if ( vecSize % 2 == 0 )
	{
		int middleIndexSup = vecSize / 2;
		int middleIndexInf = middleIndexSup - 1;
		valMediane = (vecTmp[middleIndexInf] + vecTmp[middleIndexSup]) / 2.0;
	} else {
		int middleIndex = std::floor((double)vecSize / 2.0);
		valMediane = vecTmp[middleIndex];
	}
	
	return valMediane;
	
}

//----------------------------------------------------------------------
//---- Calcule la moyenne d'un vecteur de double
//----------------------------------------------------------------------
double meanVecD(const std::vector<double> & vec)
{
	double moyenne = 0.0;
	
	for ( std::vector<double>::const_iterator i = vec.begin(); i != vec.end(); ++i )
	{
		moyenne += *i;
	}
	
	moyenne /= (double)vec.size();
	
	return moyenne;
}


//----------------------------------------------------------------------
//---- Calcule l'ecart-type d'un vecteur de double
//----------------------------------------------------------------------
double sigmaVecD(const std::vector<double> & vec, double moyenne)
{
	double sommeEcartMoyenne = 0.0;
	
	for ( std::vector<double>::const_iterator i = vec.begin(); i != vec.end(); ++i )
	{
		sommeEcartMoyenne += (*i - moyenne) * (*i - moyenne);
	}
	
	double sigma = sqrt(sommeEcartMoyenne / (double)vec.size());
	
	return sigma;
}


//----------------------------------------------------------------------
//---- Lecture des coordonnees du clic souris dans openCV
//----------------------------------------------------------------------
void CallBackSelectPt(int event, int x, int y, int flags, void* userdata)
{
	cv::Point2i * selectedPoint = (cv::Point2i*)userdata;
	
	
	if ( event == cv::EVENT_LBUTTONDOWN )
	{
		
		selectedPoint->x = x;
		selectedPoint->y = y;
		
		cv::destroyWindow("Display");
	}
	
}
