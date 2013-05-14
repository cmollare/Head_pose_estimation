#ifndef FEATURE_H
#define FEATURE_H

#include "Patch.h"
#include "ForestEnv.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <tinyxml.h>
#include "ThreadManager.h"

//class ForestEnv;

class Feature
{
	public:
        Feature(ForestEnv *forestEnv, ThreadManager* thread=NULL);
		
		virtual void extractFeature(int patchNum, cv::Mat& image)=0;
		virtual int regression(Patch& patch)=0;

		virtual void findOptimalDatasetThreshold(int test);
		virtual void getSplittedDatasets(std::vector<Patch*>& left, std::vector<Patch*>& right, double& entropy);
		virtual void saveFeature(TiXmlElement *node)
		{
			std::cout << "Warning !!! Feature unknown !!!" << std::endl;
		}
		virtual void loadFeature(TiXmlElement *node)
		{
			std::cout << "Warning !!! Feature unknown !!!" << std::endl;
		}
		inline virtual std::string featureName()
		{
			std::cout << "Warning !!! Feature with no name !" << std::endl;
			return "";
		}
		inline virtual int getThreshold()
		{
			return _threshold;
		}
		
	
	protected:
		virtual void splitDataset(int tr, std::vector<Patch*>& left, std::vector<Patch*>& right);
		virtual double computeEntropy(std::vector<Patch*>& left, std::vector<Patch*>& right, int test);
	
		ForestEnv* _pForestEnv;
		CvRNG* _pRNG;
		int _threshold;
		std::vector<Patch*>* _patchs;
		std::vector<int> _features;
		int _maxFeature, _minFeature;
		double _maxEntropy; //Entropy calculated in function find OptimalDatasetThreshold

        ThreadManager* _pThreadManager;
};

#endif
