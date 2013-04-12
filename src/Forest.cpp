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

Forest::~Forest()
{
	if (_treeVector[0] != NULL)
	{
		for(int i=0 ; i<_treeVector.size() ; i++)
		{
			delete _treeVector[i];
		}
	}
	if (_pTrainingSet) delete _pTrainingSet;
}

//For training

void Forest::initTrainingSet()
{
	_pTrainingSet = new TrainingSet(_forestEnv);
	//_pTrainingSet->initTrainingSet();
	_pTrainingSet->initCrossValidation();
}

void Forest::trainForest()
{
	std::cout << "py nbTree " << _treeVector.size() << std::endl;
	for(int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = new Tree(_forestEnv, _pTrainingSet, i); //Creation of Tree
		_treeVector[i]->growTree(); //Start training
	}
}

//End of training methods

//For detection

void Forest::loadForest()
{
	std::cout << "Loading forest..." << std::endl;
	if (!exists(_treePath)) throw ForestException("Trees folder does not exist !");

	for(int i=0 ; i<_treeVector.size() ; i++)
	{
		_treeVector[i] = new Tree(_forestEnv, i); //Load Trees
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
