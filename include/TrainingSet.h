#ifndef TRAININGSET_H
#define TRAININGSET_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <tinyxml.h>
#include <algorithm>
#include <chrono>

#include <iostream>

using namespace boost::filesystem;
using namespace std;

#include "ForestEnv.h"
#include "Patch.h"

#include "ETHZParser.h"

//TODO : change _features to _patches

class TrainingSet
{
	public:
		TrainingSet(ForestEnv* forestEnv);
		~TrainingSet();
		void initTrainingSet();// Store the training set in memory
		void initCrossValidation(std::vector<std::vector<double> >& vGroundTruth, std::vector<std::string>& vPaths, cv::Size_<int>& templateSize, std::vector<cv::Point_<int> >& vCenters);// Initializer for cross validation
		int getNumberOfPatchs();// Getter
		std::vector<Patch*>& getVectorTs();// Getter
		
	protected:
		void getDataBase();
		void shuffleDBVectors();// Shuffle vectors _vGroundTruth, _vPaths, _vCenters
		void extractPatches();
		void readClasses();
	
		ForestEnv* _pForestEnv;

		CvRNG* _pRNG;
		int _width, _height; //Patch size
		int _nbPatchs;
		int _numParams; //number of parameters in the state vector

		std::vector<std::string> _vSVParamsName;
		std::vector<std::vector<double> > _vGroundTruth;
		std::vector<std::string> _vPaths;
		cv::Size_<int> _templateSize;
		std::vector<cv::Point_<int> > _vCenters;
		std::vector<Patch*> _vFeatures;

		int _foldNumber;
		int _currentTestFolder;


};

#endif
