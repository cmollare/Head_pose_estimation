#ifndef ETHZPARSER_H
#define ETHZPARSER_H

#include <fstream>

#include "DatabaseParser.h"
#include "ETHZParser.h"

class ETHZParser : public DatabaseParser
{
	public:
		ETHZParser(std::string root);
		~ETHZParser();

		virtual void getDatabase(std::vector<cv::Point_<int> >& centers, std::vector<std::vector<double> >& groundTruth, std::vector<std::string>& path);
		virtual void getStateVectorNames(std::vector<std::string>& stateVecNames);
		virtual void getTemplateSize(cv::Size_<int>& templateSize);
		std::vector<double> readTextFile(std::string& path, std::vector<double>& calibRGB);

	protected:
		std::vector<double> readCalibFile(std::string& path);
		void exploreFolder(path p);

		std::string _rootPath;

};

#endif // ETHZPARSER_H
