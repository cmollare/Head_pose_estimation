#ifndef FORESTENV_H
#define FORESTENV_H

#include <iostream>
#include <stdlib.h>
#include <tinyxml.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#include "ForestException.h"

#define LEARN "Learn"
#define DETECT "Detect"

class ForestEnv
{
	public:
		ForestEnv(std::string confFile);
		~ForestEnv();
		
		//Getters
		std::string getMode();
		
		std::string getDatabasePath();
		std::string getTreesPath();
		std::string getTestDatabase();
		
		unsigned int getNbTrees();
		unsigned int getMinSamples();
		unsigned int getMaxDepth();
		std::string getTreePrefix();
		unsigned int getNumPotentialFeatures();
		
		CvRNG* getRNGSeed();
		
		unsigned int getNumPatchs();
		unsigned int getPatchHeight();
		unsigned int getPatchWidth();
		std::vector<std::string> getSVParamsName();
		std::map<std::string, std::string> getClassFolderPath();
		std::string getDatabaseType();

		std::string getLearningType();
		int getKFoldNumber();

		std::vector<std::string> getFeatureNames();
	
	protected:
		void initRandomNumbers();
		void getPaths(TiXmlHandle& hRoot); //Get "Paths" field
		void getDatabase(TiXmlHandle& hRoot);
		void getForest(TiXmlHandle& hRoot); //Get "Forest" field
		void getFeatures(TiXmlHandle& hRoot);//Get "Features" field
		void getLearning(TiXmlHandle& hRoot);//Get "Learning" field
	
		bool _envIsOk; //Bool true if environment is set properly
		std::string _mode;
		
		std::string _confFile; //path to the environment file
		std::string _dataBasePath; //path to the root database
		std::string _treesPath; //path to the folder containing learned trees
		std::string _testDataBase; //path to the folder containing the test Database
		
		unsigned int _nbTrees; //Number of trees in the forest
		unsigned int _minSamples; //Stopping criterion for leaf
		unsigned int _maxDepth; //Stopping criterion for tree
		std::string _treePrefix;
		unsigned int _numPotentialFeatures;
		
		std::string _databaseType;
		int _nbPatchs, _patchHeight, _patchWidth; //Patchs infos
		std::vector<std::string> _vSVParamsName; //Names of each paramater of the state vector
		std::map<std::string, std::string> _vClassFoldersPath; //Path to folders of positive et negative class labels

		std::string _learningType;
		int _foldNumber;


		std::vector<std::string> _vFeatureNames;
		
		CvRNG* _pRNG;
};

#endif
