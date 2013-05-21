#ifndef ETHZPARSER_H
#define ETHZPARSER_H

#include <fstream>

#include "DatabaseParser.h"
#include "ETHZParser.h"

class ETHZParser : public DatabaseParser
{
	public:
		ETHZParser(std::string root);
		ETHZParser(std::string root, bool crossValidation);
		~ETHZParser();

		virtual void getDatabaseCV(std::vector<std::vector<cv::Point_<int> > >& centers, std::vector<std::vector<std::vector<double> > >& groundTruth, std::vector<std::vector<std::string> >& path);//For cross validation
		virtual void getDatabase(std::vector<cv::Point_<int> >& centers, std::vector<std::vector<double> >& groundTruth, std::vector<std::string>& path);
		virtual void getStateVectorNames(std::vector<std::string>& stateVecNames);
		virtual void getTemplateSize(cv::Size_<int>& templateSize);
		std::vector<double> readTextFile(std::string& path, std::vector<double>& calibRGB);
		void readTextFile(std::string& path, std::vector<double>& calibRGB, std::vector<std::vector<double> >& stateVectors, std::vector<cv::Point_<int> >& centers);

	protected:
		std::vector<double> readCalibFile(std::string& path);
		void exploreFolder(path p);
		void exploreFolderCV(const path& p, std::vector<std::string>& vPaths, std::vector<std::vector<double> >& vStateVectors, std::vector<cv::Point_<int> >& vCenters); //For cross validation

		std::string _rootPath;
		bool _crossValidation;

};

#endif // ETHZPARSER_H
