#include "ForestEnv.h"

ForestEnv::ForestEnv(std::string confFile) : _confFile(confFile), _pRNG(NULL)
{
	TiXmlDocument doc;
	
	if (!doc.LoadFile(_confFile)) throw ForestException("Unable to open the configuration file : " + _confFile);

	this->initRandomNumbers(); //To get CvRNG seed
		
	std::cout << "Retrieving environment from : " << _confFile << std::endl;
		
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
		
	pElem=hDoc.FirstChildElement().Element(); //get xml Root
	if (!pElem) throw ForestException("Invalid XML file : " + _confFile);

	pElem->QueryStringAttribute("mode", &_mode);//Use define for values of mode
	hRoot=TiXmlHandle(pElem);
		
	//Paths recuperation
	this->getPaths(hRoot);
	this->getForest(hRoot);
	this->getDatabase(hRoot);
	this->getFeatures(hRoot);
	this->getLearning(hRoot);

}

ForestEnv::~ForestEnv()
{
	if(_pRNG)
	{
		delete _pRNG;
	}
}

void ForestEnv::initRandomNumbers()
{
	// Init random generator
	time_t t = time(NULL);
	int seed = (int)t;

	_pRNG = new CvRNG(seed);
}

void ForestEnv::getPaths(TiXmlHandle& hRoot)
{
	TiXmlElement* pElem = hRoot.FirstChild("Paths").FirstChild().Element();
	if(!pElem) throw ForestException("Error ! No field \"Paths\" !!!");
	
	int nbEntry = 0;
	for(pElem ; pElem ; pElem=pElem->NextSiblingElement())
	{
		if (!strcmp(pElem->Value(), "DatabaseRoot"))
		{
			_dataBasePath = pElem->GetText();
			nbEntry++;
		}
		else if(!strcmp(pElem->Value(), "TreesPath"))
		{
			_treesPath = pElem->GetText();
			nbEntry++;
		}
		else if(!strcmp(pElem->Value(), "TestDatabase"))
		{
			_testDataBase = pElem->GetText();
			nbEntry++;
		}
		else
		{
			throw ForestException("Error in Paths recuperation : unknown entry " + std::string(pElem->Value()));
		}
	}
	
	if(nbEntry != 3)
	{
		throw ForestException("Error : Invalid number of paths !");
	}
}

void ForestEnv::getDatabase(TiXmlHandle& hRoot)
{
	TiXmlElement* pElem = hRoot.FirstChild("Database").FirstChild().Element();
	if(!pElem) throw ForestException("Error ! No field \"Database\" !!!");

	if(hRoot.FirstChild("Database").Element()->QueryStringAttribute("type", &_databaseType)) throw ForestException("Error, no attribute type in field Database");
	
	for(pElem ; pElem ; pElem=pElem->NextSiblingElement())
	{
		if (!strcmp(pElem->Value(), "Patchs"))
		{	
			if(pElem->QueryIntAttribute("number", &_nbPatchs)) throw ForestException("Error in attribute number !");

			if(pElem->QueryIntAttribute("width", &_patchWidth)) throw ForestException("Error in attribute width !");

			if(pElem->QueryIntAttribute("height", &_patchHeight)) throw ForestException("Error in attribute height !");

		}
		else if(!strcmp(pElem->Value(), "StateVector"))
		{
			std::string temp;
			if(pElem->QueryStringAttribute("name", &temp)) throw ForestException("Error in attribute name");
			else _vSVParamsName.push_back(temp);
		}
		else if(!strcmp(pElem->Value(), "Class"))
		{
			std::string name, folder;
			if(pElem->QueryStringAttribute("name", &name)) throw ForestException("Error in attribute name of Class field");
			if(pElem->QueryStringAttribute("folder", &folder)) throw ForestException("Error in attribute folder of Class field");

			_vClassFoldersPath[name] = folder;
		}
		else throw ForestException("Error in database's fields recuperation : unknown entry " + std::string(pElem->Value()));

	}
}

void ForestEnv::getForest(TiXmlHandle& hRoot)
{
	TiXmlElement* pElem = hRoot.FirstChild("Forest").FirstChild().Element();
	if(!pElem) throw ForestException("Error ! No field \"Forest\" !!!");

	
	int nbEntry = 0;
	for(pElem ; pElem ; pElem=pElem->NextSiblingElement())
	{
		if (strcmp(pElem->Value(), "Trees")) throw ForestException("Error in Forest recuperation : unknown entry " + std::string(pElem->Value()));

		int tmpInt;
		if(pElem->QueryIntAttribute("number", &tmpInt)) throw ForestException("Error in attribute number !");
		else _nbTrees = tmpInt;


		if(pElem->QueryIntAttribute("minSamples", &tmpInt)) throw ForestException("Error in attribute minSamples !");
		else _minSamples = tmpInt;

		if(pElem->QueryIntAttribute("maxDepth", &tmpInt)) throw ForestException("Error in attribute maxDepth !");
		else _maxDepth = tmpInt;

		if(pElem->QueryStringAttribute("treePrefix", &_treePrefix)) throw ForestException("Error in attribute treePrefix !");

		if(pElem->QueryIntAttribute("numPotentialFeatures", &tmpInt)) throw ForestException("Error in attribute numPotentialFeatures !");
		else _numPotentialFeatures = tmpInt;

		nbEntry++;
	}
	
	if(nbEntry != 1) throw ForestException("Error : Invalid number of field in Forest !");
}

void ForestEnv::getFeatures(TiXmlHandle& hRoot)
{
	TiXmlElement* pElem = hRoot.FirstChild("Features").FirstChild().Element();
	if(!pElem) throw ForestException("Error ! No field \"Features\" !!!");

	_vFeatureNames.clear();

	int nbEntry=0;
	for(pElem ; pElem ; pElem=pElem->NextSiblingElement())
	{
		std::string tmp;
		pElem->QueryStringAttribute("name", &tmp);
		_vFeatureNames.push_back(tmp);

		nbEntry++;
	}

	if(!nbEntry) throw ForestException("Error : Invalid number of field int Features !");
}

void ForestEnv::getLearning(TiXmlHandle& hRoot)
{
	TiXmlElement* pElem = hRoot.FirstChild("Learning").FirstChild().Element();
	if(!pElem) throw ForestException("Error ! No field \"Learning\" !!!");

	if (!strcmp(pElem->Value(), "CrossValidation"))
	{
		for(pElem ; pElem ; pElem=pElem->NextSiblingElement())
		{
			pElem->QueryStringAttribute("type", &_learningType);

			TiXmlElement* pCrossValidation = pElem->FirstChild(_learningType)->ToElement();
			if(!pCrossValidation) throw ForestException("No field " + _learningType + " in CrossValidation !!!");

			if(!_learningType.compare("KFold"))
			{
				if(pCrossValidation->QueryIntAttribute("foldNumber", &_foldNumber)) throw ForestException("Error int attribute \"foldNumber\"");
			}
		}
	}
	else throw ForestException("Error : No field \"CrossValidation\" !!!");

}


//*****************************************//
//*****************GETTERS*****************//
//*****************************************//

std::string ForestEnv::getMode()
{
	if (strcmp(_mode.c_str(), LEARN) && strcmp(_mode.c_str(), DETECT)) throw ForestException("Error !!! Mode unknown !");

	return _mode;
}

std::string ForestEnv::getDatabasePath()
{
	return _dataBasePath;
}

std::string ForestEnv::getTreesPath()
{
	return _treesPath;
}

std::string ForestEnv::getTestDatabase()
{
	return _testDataBase;
}

unsigned int ForestEnv::getNbTrees()
{
	return _nbTrees;
}

unsigned int ForestEnv::getMinSamples()
{
	return _minSamples;
}

unsigned int ForestEnv::getMaxDepth()
{
	return _maxDepth;
}

std::string ForestEnv::getTreePrefix()
{
	return _treePrefix;
}

unsigned int ForestEnv::getNumPotentialFeatures()
{
	return _numPotentialFeatures;
}

CvRNG* ForestEnv::getRNGSeed()
{
	return _pRNG;
}

unsigned int ForestEnv::getNumPatchs()
{
	return _nbPatchs;
}

unsigned int ForestEnv::getPatchHeight()
{
	return _patchHeight;
}

unsigned int ForestEnv::getPatchWidth()
{
	return _patchWidth;
}

std::vector<std::string> ForestEnv::getSVParamsName()
{
	return _vSVParamsName;
}

std::map<std::string, std::string> ForestEnv::getClassFolderPath()
{
	return _vClassFoldersPath;
}

std::string ForestEnv::getDatabaseType()
{
	if(strcmp(_databaseType.c_str(), "ETH")) throw ForestException("Error in database type : " + _databaseType);

	return _databaseType;
}

std::string ForestEnv::getLearningType()
{
	return _learningType;
}

int ForestEnv::getKFoldNumber()
{
	return _foldNumber;
}

std::vector<std::string> ForestEnv::getFeatureNames()
{
	return _vFeatureNames;
}

