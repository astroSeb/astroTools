//----------------------------------------------------------------------
//
//---- Util.h : Header des fonctions utilitaires
//
//----------------------------------------------------------------------
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include <algorithm>

#include <dirent.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/contrib/contrib.hpp>



//---- Parsing d'un repertoire 
bool parseDir(const std::string & dirPath, const std::string & format, std::vector<std::string> & imagePathVect);

//---- Export des profils dans un csv
bool exportProfils(const std::string & filePath, const std::map<std::string, std::vector<double> > & mapProfils);

//---- Copie de fichier
bool copyFile(const std::string & srcFilePath, const std::string & dstFilePath);

//---- Decoupe d'un string par un pattern
std::vector<std::string> splitStr(const std::string & str, const std::string & pattern);

//---- Extraction d'un nom de fichier
std::string getFileName(const std::string & filePathName);

//---- Calcul des min et max d'un vecteur de double
void minMaxVecD(const std::vector<double> & data, double & min, double & max);

//---- Calcul d'un histogramme
std::vector<int> computeHisto(std::vector<double> vecData, int largeurPlage);

//---- Lissage d'un vecteur de double par une moyenne sur une fenetre
std::vector<double> lissage(const std::vector<double> & vecProfil, int fenetre);

//---- Calcul des min / max locaux d'un vecteur de doubles
void localMinMax(const std::vector<double> & vecProfil, int halfWin, double & localMin, double & localMax);

//---- Comparaison de deux doubles
bool compareDouble(double val1, double val2);

//---- Tri d'un vecteur de double
void sortVecDouble(std::vector<double> & vecDouble);

//---- Calcule la valeur mediane d'un vecteur de double
double medianVecD(const std::vector<double> & vec);

//---- Calcule la moyenne d'un vecteur de double
double meanVecD(const std::vector<double> & vec);

//---- Calcule l'ecart-type d'un vecteur de double
double sigmaVecD(const std::vector<double> & vec, double moyenne);

//---- Lecture des coordonnees du clic souris dans openCV
void CallBackSelectPt(int event, int x, int y, int flags, void* userdata);

#endif
