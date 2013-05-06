#ifndef FORESTDETECTOR_H
#define FORESTDETECTOR_H

#include "Forest.h"
#include "Patch.h"
#include "MeanShift.h"
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>
#include <chrono>

namespace fs = boost::filesystem;

class ForestDetector
{
	public:
		ForestDetector(ForestEnv* forestEnv);
		~ForestDetector();
		void detect(std::string folder);
        void detect(cv::Mat& image, std::string imageName);
		
	protected:
		Forest *pForest;
		ForestEnv* _pForestEnv;
};

#endif
