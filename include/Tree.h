#ifndef TREE_H
#define TREE_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <tinyxml.h>
#include <sstream>
#include <iomanip>
#include "Patch.h"
#include "ForestEnv.h"
#include "TrainingSet.h"
#include "PointFeature.h"
#include "RectFeature.h"
#include "Leaf.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>

class Tree
{
	public:
		Tree(ForestEnv* forestEnv, TrainingSet* ts, int treeId);//For learning
		Tree(ForestEnv* forestEnv, int treeId);//For detection
		~Tree();
		void growTree();// For learning
		
		void regression(Patch& patch, std::vector<Leaf*>& detectedLeaf);//For detection
		void regression(Patch& patch, std::vector<Leaf*>& detectedLeaf, int node);//For detection (recursivity)
	protected:
		//For learning
		void grow(const std::vector<Patch*>& nodeTs, int nodeDepth, int nodeInd);// For learning
		void extractFeatures(std::vector<Feature*>& feat, const std::vector<Patch*>& nodeTs);// For learning
		void findFeature(const std::vector<Patch*>& nodeTs, std::vector<Patch*>& tsLeft, std::vector<Patch*>& tsRight, int nodeInd);// For learning
		void makeLeaf(const std::vector<Patch*>& nodeTs, int nodeInd);// For learning
		void saveTree();//To save learning
		void saveNode(TiXmlElement *parentNode, int depth=0);//To save learning
		
		//For detection
		void loadTree();
		void loadNode(TiXmlElement *parentNode, int depth=0);
	
		ForestEnv* _pForestEnv;
		TrainingSet *_pTrainingSet; //TrainingSet
		int _treeId;
		
		CvRNG *_pRNG;
		int _minSamples;
		int _maxDepth;
		int _trainingSetSize;
		int _classifiedSize;
		int _numPotentialFeatures;
		
		std::vector<Patch*> _vpPatchs; //vector of pointers to patchs of trainingSet (in order to not copy them)
		
		std::map<int, Feature*> _treeTable;
		std::map<int, Leaf*> _leafNodes;

        //For multi threading
        int _currentThreadNumber, _maxThreadNumber;
        boost::recursive_mutex _mute;
		
		//tmp
		int _currentDepth;
		//int _numClasses; //Number of classes in the training set
};

#endif
