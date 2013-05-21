#ifndef CROSSVALIDATION_H
#define CROSSVALIDATION_H

#include "ForestEnv.h"
#include "TrainingSet.h"
#include "DatabaseParser.h"

class CrossValidation
{
    public:
        CrossValidation(ForestEnv* forestEnv);
        ~CrossValidation();

		void startCrossValidation();

        void getDataBase();
        void initCrossValidation();
		void buildCurrentDatabase();
		void buildNextDatabase();

    protected:
        ForestEnv* _pForestEnv;
        TrainingSet* _pTrainingSet;
        DatabaseParser* _pDbParser;

		//Database splitted in folders
		std::vector<std::vector<std::vector<double> > > _vGDTruth;
		std::vector<std::vector<std::string> > _vPaths;
		std::vector<std::vector<cv::Point_<int> > > _vCenters;

		//currentDatabase for crossValidation
		std::vector<std::vector<std::vector<double> > > _vCurrGDTruth;
		std::vector<std::vector<std::string> > _vCurrPaths;
		std::vector<std::vector<cv::Point_<int> > > _vCurrCenters;
		int _currDatabase;
		int _maxDatabases;


		std::vector<std::string> _vStateVectorNames;
};

#endif // CROSSVALIDATION_H
