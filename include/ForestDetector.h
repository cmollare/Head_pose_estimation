#ifndef FORESTDETECTOR_H
#define FORESTDETECTOR_H

#include "Forest.h"
#include "Patch.h"
#include "MeanShift.h"
#include "TrainingSet.h"
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>
#include <chrono>

namespace fs = boost::filesystem;

class ForestDetector
{
	public:
		ForestDetector(ForestEnv* forestEnv);
		ForestDetector(ForestEnv *forestEnv, int indice);//For cross validation
		~ForestDetector();
		void detect(std::string folder);
        void detect(cv::Mat& image, std::string imageName);
		void detect(std::vector<std::vector<double> >& vGroundTruth, std::vector<std::string>& vPaths, cv::Size_<int>& templateSize, std::vector<cv::Point_<int> >& vCenters);
		
	protected:
		Forest *pForest;
		ForestEnv* _pForestEnv;

		int _index;//For cross validation
};

#endif
