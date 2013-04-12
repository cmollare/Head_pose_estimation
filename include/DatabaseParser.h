#ifndef DATABASEPARSER_H
#define DATABASEPARSER_H

#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <math.h>
#include <opencv2/opencv.hpp>

using namespace boost::filesystem;

#include "ForestException.h"

#define PI 3.14159265

class DatabaseParser
{
	public:
		DatabaseParser();
		~DatabaseParser();

		virtual void getDatabase(std::vector<cv::Point_<int> >& centers, std::vector<std::vector<double> >& groundTruth, std::vector<std::string>& path)=0;
		virtual void getStateVectorNames(std::vector<std::string>& stateVec)=0;
		virtual void getTemplateSize(cv::Size_<int>& templateSize)=0;

	protected:
		std::vector<std::vector<double> > _vGDTruth;
		std::vector<std::string> _vPaths;
		std::vector<std::string> _vStateVectorNames;
		std::vector<cv::Point_<int> > _vCenters;
		cv::Size_<int> _templateSize;
};

#endif // DATABASEPARSER_H
