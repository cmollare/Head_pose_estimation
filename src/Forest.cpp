#include "Forest.h"

Forest::Forest(ForestEnv* forestEnv)
{
	//TODO : vérifier si le dossier existe et sinon, le créer
	
	_forestEnv = forestEnv;
	
	_treePath = _forestEnv->getTreesPath();
	_treeVector.resize(_forestEnv->getNbTrees());
	for (int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = NULL;
	}
	
	// Create Tree directory
	remove_all(_treePath);
	create_directory(_treePath);
	// End of directory creation
	
	_pTrainingSet=NULL;
	
	this->initTrainingSet();
	this->trainForest();
	
}

Forest::Forest(ForestEnv* forestEnv, int dummy)
{
	_forestEnv = forestEnv;
	
	_pTrainingSet=NULL;
	_treePath = _forestEnv->getTreesPath();
	_treeVector.resize(_forestEnv->getNbTrees());
	for (int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = NULL;
	}
	
	this->loadForest();//Load forest
}

Forest::Forest(ForestEnv *forestEnv, int index, int dummy)
{
	_forestEnv = forestEnv;

	_pTrainingSet=NULL;

	_treePath = _forestEnv->getTreesPath();
	std::ostringstream oss;
	oss << "/" << index;
	_treePath+=oss.str();

	_treeVector.resize(_forestEnv->getNbTrees());
	for (int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = NULL;
	}

	this->loadForest(_treePath);//Load forest
}

Forest::Forest(ForestEnv *forestEnv, TrainingSet *ts, int index) : _forestEnv(forestEnv), _pTrainingSet(ts)
{
	_treeVector.resize(_forestEnv->getNbTrees());
	_treePath = _forestEnv->getTreesPath();

	std::ostringstream oss;
	oss << "/" << index;
	_treePath+=oss.str();

	create_directory(_treePath);

	for (int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = NULL;
	}

	this->trainForest(index);
}

Forest::~Forest()
{

    for(int i=0 ; i<_treeVector.size() ; i++)
    {
        if (_treeVector[i]) delete _treeVector[i];
    }

    if (_pTrainingSet) delete _pTrainingSet;
}

//For training

void Forest::initTrainingSet()
{
	_pTrainingSet = new TrainingSet(_forestEnv);
	_pTrainingSet->initTrainingSet();
	//_pTrainingSet->initCrossValidation();
}

void Forest::trainForest()
{
    using std::chrono::system_clock;

    std::cout << "py nbTree " << _treeVector.size() << std::endl;
    for(int i=0 ; i<_treeVector.size() ; i++)
    {
        system_clock::time_point begin = system_clock::now();
        _treeVector[i] = new Tree(_forestEnv, _pTrainingSet, i); //Creation of Tree
        _treeVector[i]->growTree(); //Start training
        delete _treeVector[i];
        _treeVector[i]=NULL;
        system_clock::time_point end = system_clock::now();

        double time = system_clock::to_time_t(end)-system_clock::to_time_t(begin);
        std::cout << "temps : " << time << std::endl;
    }
}

void Forest::trainForest(int id)
{
	using std::chrono::system_clock;

	std::cout << "py nbTree " << _treeVector.size() << std::endl;
	for(int i=0 ; i<_treeVector.size() ; i++)
	{
		system_clock::time_point begin = system_clock::now();
		_treeVector[i] = new Tree(_forestEnv, _pTrainingSet, i); //Creation of Tree
		_treeVector[i]->growTree(_treePath); //Start training
		delete _treeVector[i];
		_treeVector[i]=NULL;
		system_clock::time_point end = system_clock::now();

		double time = system_clock::to_time_t(end)-system_clock::to_time_t(begin);
		std::cout << "temps : " << time << std::endl;
	}
}

//End of training methods

//For detection

void Forest::loadForest(std::string forestPath)
{
	std::cout << "Loading forest..." << std::endl;
	if (!exists(_treePath)) throw ForestException("Trees folder does not exist !");

	for(int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = new Tree(_forestEnv, i, forestPath); //Load Trees
	}

	std::cout << "Forest loaded" << std::endl;
}

void Forest::regression(Patch& patch, std::vector<Leaf*>& detectedLeaf)
{
	for (int i=0 ; i < _treeVector.size() ; i++)
	{
		_treeVector[i]->regression(patch, detectedLeaf);
	}
}

//End of detection methods
