#ifndef FOREST_H
#define FOREST_H

#include "ForestEnv.h"
#include <vector>
#include <string>
#include "TrainingSet.h"
#include "Tree.h"

class Forest
{
	public:
		Forest(ForestEnv* forestEnv);//For training
		Forest(ForestEnv* forestEnv, int dummy);//For detection
		~Forest();
		void regression(Patch& patch, std::vector<Leaf*>& detectedLeaf);
		
	protected:
		void initTrainingSet();//For training
		void trainForest();//For training
		void loadForest();//For detection
	
		ForestEnv *_forestEnv;// Env variable
		TrainingSet *_pTrainingSet; //TrainingSet
	
		std::vector<Tree*> _treeVector; //List of trees
		std::string _treePath;
		CvRNG* _pRNG;
		int _width, _height;// patches width and height
		
};

#endif
