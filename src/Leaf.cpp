#include "Leaf.h"

Leaf::Leaf(const std::vector<Patch*>& nodeTs)
{
	if (nodeTs.size() == 0) throw ForestException("Leaf created with 0 patch");
	_stateVectorNames = nodeTs[0]->getSVNames();

	_stateVectorMean = cv::Mat(nodeTs[0]->getStateVector().size()-1, 1, CV_64F, cv::Scalar_<double>(0));
	_stateVectorVariance = cv::Mat(nodeTs[0]->getStateVector().size()-1, 1, CV_64F, cv::Scalar_<double>(0));
	_meanOffsets.x = 0;
	_meanOffsets.y = 0;
	_varOffsets.x = 0;
	_varOffsets.y = 0;
	_nbPatchs = nodeTs.size();
	double nbPatchs = 0;

	for (int numPatch=0 ; numPatch < _nbPatchs ; numPatch++)
	{
		std::vector<double> stateVector = nodeTs[numPatch]->getStateVector();
		if (stateVector[0] == 0) continue;

		cv::Point_<int> offset = nodeTs[numPatch]->getOffset();

		_meanOffsets += offset;
		_varOffsets.x += (double)offset.x*offset.x;
		_varOffsets.y += (double)offset.y*offset.y;

		for (int i=0 ; i<stateVector.size()-1 ; i++)
		{
			_stateVectorMean.at<double>(i) += stateVector[i+1];
			_stateVectorVariance.at<double>(i) += stateVector[i+1]*stateVector[i+1];

			//std::cout << _stateVectorVariance.at<double>(i) << " " << _stateVectorMean.at<double>(i) << std::endl;

		}
		nbPatchs++;
		//std::cout << stateVector[0] << std::endl;
	}
	_conf = (double)nbPatchs/nodeTs.size();

	if (nbPatchs > 0) //Compute means and variances only if there are some positive patchs
	{
		_stateVectorMean /= nbPatchs;
		//std::cout << nodeTs.size() << std::endl;
		//std::cout <<  _stateVectorMean.mul(_stateVectorMean) << std::endl;
		//std::cout << _stateVectorVariance/nodeTs.size() << std::endl;
		_stateVectorVariance = _stateVectorVariance/nbPatchs - _stateVectorMean.mul(_stateVectorMean);
		//std::cout << _stateVectorVariance << std::endl;
		//std::cout << _meanOffsets << std::endl;
		_meanOffsets.x = (double)_meanOffsets.x/nbPatchs;
		_meanOffsets.y = (double)_meanOffsets.y/nbPatchs;
		//std::cout << _meanOffsets << std::endl;
		//std::cout << _varOffsets << std::endl;
		_varOffsets.x = floor((double)_varOffsets.x/nbPatchs) - (double)_meanOffsets.x*_meanOffsets.x;
		_varOffsets.y = floor((double)_varOffsets.y/nbPatchs) - (double)_meanOffsets.y*_meanOffsets.y;
		//std::cout << _varOffsets << std::endl;
	}
}

Leaf::Leaf(TiXmlElement *node)
{
	this->loadLeaf(node);
}

void Leaf::saveLeaf(TiXmlElement *parentNode, int depth)
{
	TiXmlElement *leaf = new TiXmlElement("Leaf");
	leaf->SetAttribute("id", depth);
	leaf->SetAttribute("nbPatchs", _nbPatchs);
	leaf->SetDoubleAttribute("conf", _conf);
	leaf->SetAttribute("nbStates", (int)_stateVectorNames.size());
	leaf->SetAttribute("meanOffsetX", _meanOffsets.x);
	leaf->SetAttribute("meanOffsetY", _meanOffsets.y);
	leaf->SetDoubleAttribute("varianceOffsetX", _varOffsets.x);
	leaf->SetDoubleAttribute("varianceOffsetY", _varOffsets.y);
	parentNode->LinkEndChild(leaf);

	//Do not write the class
	for (int i=0 ; i<_stateVectorNames.size()-1 ; i++)//Write only infos on positive classes
	{
		TiXmlElement *state = new TiXmlElement(_stateVectorNames[i+1]);
		state->SetDoubleAttribute("mean", (double)_stateVectorMean.at<double>(i));//Save mean offsets
		state->SetDoubleAttribute("variance", (double)_stateVectorVariance.at<double>(i));
		leaf->LinkEndChild(state);
	}
}

void Leaf::loadLeaf(TiXmlElement *node)
{
	int nbStates=0, i=1;
	node->QueryIntAttribute("nbPatchs", &_nbPatchs);
	node->QueryDoubleAttribute("conf", &_conf);
	node->QueryIntAttribute("nbStates", &nbStates);
	node->QueryIntAttribute("meanOffsetX", &_meanOffsets.x);
	node->QueryIntAttribute("meanOffsetY", &_meanOffsets.y);
	node->QueryDoubleAttribute("varianceOffsetX", &_varOffsets.x);
	node->QueryDoubleAttribute("varianceOffsetY", &_varOffsets.y);

	_stateVectorMean = cv::Mat(nbStates, 1, CV_64F, cv::Scalar_<double>(0));
	_stateVectorVariance = cv::Mat(nbStates, 1, CV_64F, cv::Scalar_<double>(0));

	for (TiXmlElement *pState = node->FirstChildElement() ; pState ; pState = pState->NextSiblingElement())
	{
		double mean, var;
		_stateVectorNames.push_back(pState->ValueStr());
		pState->QueryDoubleAttribute("mean", &mean);
		pState->QueryDoubleAttribute("variance", &var);

        _stateVectorMean.at<double>(i) = mean;
		_stateVectorVariance.at<double>(i) = var;
		i++;
	}
}




