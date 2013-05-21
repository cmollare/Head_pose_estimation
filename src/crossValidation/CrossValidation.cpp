#include "CrossValidation.h"

CrossValidation::CrossValidation(ForestEnv *forestEnv) : _pForestEnv(forestEnv)
{
    _pTrainingSet = NULL;
    _pDbParser = NULL;
	_currDatabase = 0;

	_vCurrGDTruth.resize(2);
	_vCurrPaths.resize(2);
	_vCurrCenters.resize(2);
}

CrossValidation::~CrossValidation()
{
    if (_pTrainingSet) delete _pTrainingSet;
    if (_pDbParser) delete _pDbParser;
}

void CrossValidation::startCrossValidation()
{
	for (int i=0 ; i<_maxDatabases ; i++)
	{
		std::cout << "start cross validation. Step " << _currDatabase + 1 << " on " << _maxDatabases << std::endl;
		this->buildCurrentDatabase();


		std::cout << "start cross validation. Step " << _currDatabase + 1 << " on " << _maxDatabases << std::endl;
		_currDatabase++;
	}
}

void CrossValidation::initCrossValidation()
{
	this->getDataBase();

    _pTrainingSet = new TrainingSet(_pForestEnv);
    _pTrainingSet->initCrossValidation();
}

void CrossValidation::getDataBase()
{
	std::string dbType = _pForestEnv->getDatabaseType();

	DatabaseParser *_pDbParser = NULL;

	if (!strcmp(dbType.c_str(), "ETH"))
	{
		std::cout << "Retrieving ETH database" << std::endl;
		_pDbParser = new ETHZParser(_pForestEnv->getClassFolderPath()["positive"], true);
		_pDbParser->getStateVectorNames(_vStateVectorNames);
		_pDbParser->getDatabaseCV(_vCenters, _vGDTruth, _vPaths);
		_maxDatabases = _pDbParser->getnumFolders();
	}
	else throw ForestException("Unknown database : " + dbType);

	std::cout << "Database retrieved" << std::endl;
}

void CrossValidation::buildCurrentDatabase()
{
	_vCurrGDTruth[0].clear();
	_vCurrPaths[0].clear();
	_vCurrCenters[0].clear();
	_vCurrGDTruth[1].clear();
	_vCurrPaths[1].clear();
	_vCurrCenters[1].clear();

	for (int i=0 ; i < _maxDatabases ; i++)
	{
		if (i!=_currDatabase)
		{
			for (int j=0 ; j<_vCenters[i].size() ; j++)
			{
				_vCurrGDTruth[0].push_back(_vGDTruth[i].at(j));
				_vCurrPaths[0].push_back(_vPaths[i].at(j));
				_vCurrCenters[0].push_back(_vCenters[i].at(j));
			}
		}
		else
		{
			for (int j=0 ; j<_vCenters[i].size() ; j++)
			{
				_vCurrGDTruth[1].push_back(_vGDTruth[i].at(j));
				_vCurrPaths[1].push_back(_vPaths[i].at(j));
				_vCurrCenters[1].push_back(_vCenters[i].at(j));
			}
		}
	}
}

void CrossValidation::buildNextDatabase()
{
	_currDatabase++;

	this->buildCurrentDatabase();
}
