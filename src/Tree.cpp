#include "Tree.h"

Tree::Tree(ForestEnv* forestEnv, TrainingSet* ts, int treeId) : _pForestEnv(forestEnv), _pTrainingSet(ts), _treeId(treeId)
{
	_pRNG = _pForestEnv->getRNGSeed();
	_minSamples = _pForestEnv->getMinSamples();
	_maxDepth = _pForestEnv->getMaxDepth();
	_trainingSetSize = _pTrainingSet->getNumberOfPatchs();
	_numPotentialFeatures = _pForestEnv->getNumPotentialFeatures();
	_classifiedSize=0;
		
	_treeTable.clear();
	_leafNodes.clear();
	_vpPatchs.clear();

    _pThrdManager=NULL;
}

Tree::Tree(ForestEnv* forestEnv, int treeId) : _pForestEnv(forestEnv), _treeId(treeId)
{
	_pRNG=NULL;
	_minSamples=0;
	_maxDepth=0;
	_trainingSetSize=0;
	_numPotentialFeatures=0;
	_classifiedSize=0;
	_treeTable.clear();
	_leafNodes.clear();
	_vpPatchs.clear();
	_pTrainingSet = NULL;
    _pThrdManager=NULL;
//	_numClasses = _pForestEnv->getNumberOfClasses();
		
	this->loadTree();
}

Tree::~Tree()
{
	for (std::map<int, Leaf*>::iterator it = _leafNodes.begin() ; it != _leafNodes.end() ; it++)// Destroy leafs
	{
		delete it->second;
	}
	_leafNodes.clear();
	
	for (std::map<int, Feature*>::iterator it = _treeTable.begin() ; it != _treeTable.end() ; it++)// Destroy nodes
	{
		delete it->second;
	}
	_treeTable.clear();
}

void Tree::growTree()
{
	//Copy adresses of Patchs in the vector _vpPatchs for training
	_vpPatchs = _pTrainingSet->getVectorTs();
	
	/*_vpPatchs.reserve(ts.size());
	for (int i=0 ; i<ts.size() ; i++)
	{
		_vpPatchs.push_back(&ts[i]);
	}*/
	//End of copy adresses of Patchs in the vector _vpPatchs for training
	int size=0;

    for (int i=0 ; i<_vpPatchs.size() ; i++)
    {
        std::vector<double> sVector = _vpPatchs.at(i)->getStateVector();
        if(sVector.at(0) !=0) size++;
    }

    std::cout << size << " positive and " << _vpPatchs.size()-size << " negative" << std::endl;
	
	std::cout << "Start growing Tree n°" << _treeId << std::endl;

    //*******LEARNING*******//

    _pThrdManager = new ThreadManager;

	this->grow(_vpPatchs, 0, 0);

    delete _pThrdManager;

    //******END LEARNING*****//

	std::cout << std::endl << "Tree grown with " << _leafNodes.size() << " leafs and " << _treeTable.size() << " nodes" << std::endl;
	
	this->saveTree();
	
}

void Tree::grow(const std::vector<Patch*>& nodeTs, int nodeDepth, int nodeInd)
{

	if (nodeDepth < _maxDepth)
	{
		std::cout << "Growing node : " << nodeInd << " ,depth : " << nodeDepth << std::endl;//A supprimer
		std::cout << "py nbNode " << nodeInd << std::endl;

		int sizeLeft=0, sizeRight=0;
		std::vector<Patch*> setLeft;
		std::vector<Patch*> setRight;

		_currentDepth = nodeDepth; //TEMPO

		try
		{
			//Can make a leaf if no correct feature is found
			this->findFeature(nodeTs, setLeft, setRight, nodeInd);

			sizeLeft = setLeft.size();
			sizeRight = setRight.size();

			std::cout << sizeLeft << " "  << sizeRight << std::endl;



			//std::cout << "size " << sizeLeft << " " << sizeRight << std::endl;

			//grow right branch
			if(sizeRight > _minSamples) //Si il reste des classes et que le nb de sample > _minSamples on continue
			{
                _pThrdManager->_mute.lock();
                if (_pThrdManager->getCurrentNumberThreads() < _pThrdManager->getMaxNumberThreads())
                {
                    std::cout << "new threaaaad !!!!" << std::endl;
                    _pThrdManager->_mute.unlock();
                    _pThrdManager->addThread(new boost::thread(&Tree::grow, this, setRight, nodeDepth+1, 2*nodeInd+1)); //right => odd nodeInd
                }
                else
                {
                    _pThrdManager->_mute.unlock();
                    grow(setRight, nodeDepth+1, 2*nodeInd+1);//right => odd nodeInd
                }
			}
            else
			{
				this->makeLeaf(setRight, 2*nodeInd+1);
			}

			//grow left branch
			if(sizeLeft > _minSamples) //Si il reste des classes et que le nb de sample > _minSamples on continue
			{
				grow(setLeft, nodeDepth+1, 2*nodeInd+2); //left => even nodeInd
			}
			else
			{
				this->makeLeaf(setLeft, 2*nodeInd+2);
			}
		}
		catch(ForestException& e)
		{
			std::cout << e.what() << std::endl;
			this->makeLeaf(nodeTs, nodeInd);
		}
		
	}
	else // Create a leaf
	{
		std::cout << "py nbLeaf " << nodeInd << std::endl;
		this->makeLeaf(nodeTs, nodeInd);
	}
}

void Tree::extractFeatures(std::vector<Feature*>& feat, const std::vector<Patch*>& nodeTs)
{
	//A étendre à d'autres features (en fonction de ForestEnv)
    cv::Rect_<int> roi = nodeTs.at(0)->getRoi();

	std::vector<std::string> featureNames = _pForestEnv->getFeatureNames();
	
	feat.reserve(_numPotentialFeatures*featureNames.size());
	for (int featInd=0 ; featInd < featureNames.size() ; featInd++)
	{
		if (!featureNames[featInd].compare("Point"))
		{
			for (int i=0 ; i < _numPotentialFeatures ; i++)
			{
				//PointFeature extraction :
				//Choose 2 points randomly
				cv::Point_<int> pt1(cvRandInt(_pRNG)%roi.width, cvRandInt(_pRNG)%roi.height);
				cv::Point_<int> pt2(cvRandInt(_pRNG)%roi.width, cvRandInt(_pRNG)%roi.height);

                feat.push_back(new PointFeature(_pForestEnv, _pThrdManager, nodeTs, pt1, pt2));
				feat.back()->extractFeature();
				//Uncomment for modified criterion
				/*if (_currentDepth < _maxDepth/2 && (nodeTs[0].size()+nodeTs[1].size())>4*_minSamples)
				{
					feat[i]->findOptimalDatasetThreshold(1);
				}
				else
				{
					feat[i]->findOptimalDatasetThreshold(0);
				}*/
				feat.back()->findOptimalDatasetThreshold(0);
			}
		}
		else if (!featureNames[featInd].compare("Rect"))
		{
			int patchW = _pForestEnv->getPatchWidth();
			int patchH = _pForestEnv->getPatchHeight();
			cv::Mat randNb(_numPotentialFeatures*2, 1, CV_32SC4);
			cv::Scalar_<int> lowerBound(0, 0, 4, 4);
			cv::Scalar_<int> upperBound(patchW-1, patchH-1, patchW, patchH);
			cv::randu(randNb, lowerBound, upperBound);

			for (int i=0 ; i < _numPotentialFeatures ; i++)
			{
				cv::Rect_<int> rect1 = randNb.at<cv::Rect_<int> >(i);
				cv::Rect_<int> rect2 = randNb.at<cv::Rect_<int> >(i+_numPotentialFeatures);

				if((rect1.width+rect1.x) > patchW) rect1.width = patchW-rect1.x;
				if((rect1.height+rect1.y) > patchH) rect1.height = patchH-rect1.y;
				if((rect2.width+rect2.x) > patchW) rect2.width = patchW-rect2.x;
				if((rect2.height+rect2.y) > patchH) rect2.height = patchH-rect2.y;

				//std::cout << randNb.at<int >(i) << " " << randNb.at<int >(i+1) << " " << randNb.at<int >(i+2) << " " << randNb.at<int >(i+3) << std::endl;
				//std::cout << randNb.at<cv::Rect_<int> >(i) << std::endl;
				//std::cout << rect1.x << " " << rect1.y << " " << rect1.height << " " << rect1.width << std::endl;

                feat.push_back(new RectFeature(_pForestEnv, _pThrdManager, nodeTs, rect1, rect2));
				feat.back()->extractFeature();

				feat.back()->findOptimalDatasetThreshold(0);
			}
		}
		else throw ForestException("Feature Unkown");
	}
}

void Tree::findFeature(const std::vector<Patch*>& nodeTs, std::vector<Patch*>& tsLeft, std::vector<Patch*>& tsRight, int nodeInd)
{
	std::vector<Feature*> feat;
	this->extractFeatures(feat, nodeTs);
	std::vector<Patch*> tmpLeft, tmpRight;
	double tmpEntropy=0, maxEntropy=0;
	int indexMax=0;
	
	for (int i=0 ; i<feat.size() ; i++)//Find the best feature
	{
		feat[i]->getSplittedDatasets(tmpLeft, tmpRight, tmpEntropy);
		if (tmpEntropy > maxEntropy || i==0)//Take the feature with max of criterion
		{
			tsLeft = tmpLeft;
			tsRight = tmpRight;
			maxEntropy = tmpEntropy;
			indexMax=i;
		}
	}
	std::cout << maxEntropy << std::endl;

	if ((tsLeft.size() == 0) || (tsRight.size() == 0))
	{
		for (int i=0 ; i<feat.size() ; i++)
		{
            delete feat.at(i);
		}
		throw ForestException("Unable to find a good feature => leaf creation");
	}


    _pThrdManager->_mute.lock();

	if (!feat[indexMax]->featureName().compare("Point"))//Save the feature in treeTable
	{
		PointFeature *tmp = (PointFeature*) feat[indexMax];//Copy value of the best feature
		_treeTable[nodeInd] = new PointFeature(_pForestEnv, tmp->getPoint1(), tmp->getPoint2(), tmp->getThreshold());//Save the best feature un treeTable
	}
	else if (!feat[indexMax]->featureName().compare("Rect"))
	{
		RectFeature *tmp = (RectFeature*) feat[indexMax];//Copy value of the best feature
		_treeTable[nodeInd] = new RectFeature(_pForestEnv, tmp->getRect1(), tmp->getRect2(), tmp->getThreshold());//Save the best feature un treeTable
	}
	else
	{
		throw ForestException("Unknown feature !!!");
	}

    _pThrdManager->_mute.unlock();
	
	for (int i=0 ; i<feat.size() ; i++)
	{
        delete feat.at(i);
	}
}

void Tree::makeLeaf(const std::vector<Patch*>& nodeTs, int nodeInd)
{
	int size=0;

	for (int i=0 ; i<nodeTs.size() ; i++)
	{
        std::vector<double> state = nodeTs.at(i)->getStateVector();
        if (state.at(0)!=0) size++;
	}

    _pThrdManager->_mute.lock();

	_classifiedSize+=nodeTs.size();
	std::cout << size << " "  << nodeTs.size() << std::endl;

	//std::cout.width(50);
	std::cout << "\rTraining... " << (double)_classifiedSize/(double)_trainingSetSize*100 << " % processed           " << std::endl;
	
	_leafNodes[nodeInd] = new Leaf(nodeTs);

    _pThrdManager->_mute.unlock();
}

void Tree::saveTree()
{
	TiXmlDocument doc;
	TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);
	
	TiXmlElement *root = new TiXmlElement("Tree");
	root->SetAttribute("nbNodes", _treeTable.size());
	root->SetAttribute("treeId", _treeId);
	root->SetAttribute("maxDepth", _maxDepth);
	doc.LinkEndChild(root);
	
	this->saveNode(root);//Save recursively
	
	ostringstream oss;
	oss << _pForestEnv->getTreesPath() << "/" << _pForestEnv->getTreePrefix() << _treeId << ".xml";
	doc.SaveFile(oss.str());
}

void Tree::saveNode(TiXmlElement *parentNode, int depth)
{
	if(_treeTable.find(depth) == _treeTable.end())
	{
		if(_leafNodes.find(depth) == _leafNodes.end())
		{
			std::cout << "Error, branch does not end with leaf !!!" << std::endl;
		}
		else
		{
			_leafNodes[depth]->saveLeaf(parentNode, depth);
		}
	}
	else
	{
		//Save node
		TiXmlElement *node = new TiXmlElement("Node");
		node->SetAttribute("id", depth);
		node->SetAttribute("featureType", _treeTable[depth]->featureName());
		_treeTable[depth]->saveFeature(node);//save node's feature
		parentNode->LinkEndChild(node);
		
		this->saveNode(node, 2*depth+1);//Save right recursively
		this->saveNode(node, 2*depth+2);//Save left recursively	
	}
}

void Tree::loadTree()
{
	ostringstream oss;
	oss << _pForestEnv->getTreesPath() << "/" << _pForestEnv->getTreePrefix() << _treeId << ".xml";
	
	TiXmlDocument doc(oss.str());
	if(!doc.LoadFile()) throw ForestException("Error : file not found");
	
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	
	pElem=hDoc.FirstChildElement().Element();
	
	int numNodes=0;
	pElem->QueryIntAttribute("nbNodes", &numNodes);
	
	this->loadNode(pElem);
	
	if (_treeTable.size() != numNodes) throw ("Warning !!! loaded ended with wrong number of nodes");
}

void Tree::loadNode(TiXmlElement *parentNode, int depth)
{
	TiXmlElement *pNode = parentNode->FirstChildElement();
	for (pNode ; pNode ; pNode=pNode->NextSiblingElement())//Search the good node function of the depth
	{
		int nodeId;
		pNode->QueryIntAttribute("id", &nodeId);
		if(nodeId==depth) break;
	}
	
	if (!strcmp(pNode->Value(), "Node"))//If it's a node => load node
	{
		int nodeId;
		std::string featureType;
		pNode->QueryIntAttribute("id", &nodeId);
		pNode->QueryStringAttribute("featureType", &featureType);
		
		if(!featureType.compare("Point"))//Load feature
		{
			_treeTable[nodeId] = new PointFeature(_pForestEnv, pNode);//Call the constructor for loading feature
			this->loadNode(pNode, 2*depth+1);//right
			this->loadNode(pNode, 2*depth+2);//left
			
		}
		else if(!featureType.compare("Rect"))
		{
			_treeTable[nodeId] = new RectFeature(_pForestEnv, pNode);
			this->loadNode(pNode, 2*depth+1);//right
			this->loadNode(pNode, 2*depth+2);//left
		}
		else throw ForestException("Unknown type of feature");

	}
	else if (!strcmp(pNode->Value(), "Leaf"))//If it's a leaf => load leaf
	{
		_leafNodes[depth] = new Leaf(pNode);
	}
}

void Tree::regression(Patch& patch, std::vector<Leaf*>& detectedLeaf)
{
	this->regression(patch, detectedLeaf, _treeTable[0]->regression(patch));
}

void Tree::regression(Patch& patch, std::vector<Leaf*>& detectedLeaf, int node)
{
	if (_treeTable.find(node) != _treeTable.end()) //If node exist, continue recursivity
	{
		this->regression(patch, detectedLeaf, 2*node + _treeTable[node]->regression(patch));
	}
	else //If node doesn't exist => Leaf, store the leaf in the result vector
	{
		detectedLeaf.push_back(_leafNodes[node]);
	}
}
