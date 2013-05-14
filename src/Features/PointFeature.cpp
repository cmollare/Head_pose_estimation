#include "PointFeature.h"

PointFeature::PointFeature(ForestEnv *forestEnv, ThreadManager* thread, const std::vector<Patch*>& patchs, cv::Point_<int>& pt1, cv::Point_<int>& pt2) : Feature(forestEnv, thread), _pt1(pt1), _pt2(pt2)
{
	_patchs = const_cast<std::vector<Patch*>*>(&patchs);
}

PointFeature::PointFeature(ForestEnv *forestEnv, cv::Point_<int>& pt1, cv::Point_<int>& pt2, int threshold) : Feature(forestEnv), _pt1(pt1), _pt2(pt2)
{
	_pRNG = NULL;
	_threshold = threshold;
}

PointFeature::PointFeature(ForestEnv *forestEnv, TiXmlElement *node) : Feature(forestEnv)
{
	_pRNG = NULL;
	_threshold = 0;
	
	this->loadFeature(node);
}

void PointFeature::extractFeature(int patchNum, cv::Mat& image)
{
	//Memory allocation for result of feature extraction
	_features.reserve(_patchs->size()); //Feature is the difference between two points

	//extraction of a particular patch
	_features.push_back((*_patchs)[patchNum]->getOriginPatchPoint(image, _pt1) - (*_patchs)[patchNum]->getOriginPatchPoint(image, _pt2));
	if(_features[patchNum] > _maxFeature || (patchNum==0)) _maxFeature = _features[patchNum];
	else if(_features[patchNum] < _minFeature || (patchNum==0)) _minFeature = _features[patchNum];
	
	//Feature extraction
	/*for (int patch=0 ; patch<_patchs.size() ; patch++)
	{
		//Extract the feature (difference beetween two points)
		_features.push_back(_patchs[patch]->getOriginPatchPoint(_pt1) - _patchs[patch]->getOriginPatchPoint(_pt2));

		//Test to find max and min values
		if(_features[patch] > _maxFeature || (patch==0)) _maxFeature = _features[patch];
		else if(_features[patch] < _minFeature || (patch==0)) _minFeature = _features[patch];
	}*/
}

void PointFeature::saveFeature(TiXmlElement *node)
{
	node->SetAttribute("threshold", _threshold);
	node->SetAttribute("point1X", _pt1.x);
	node->SetAttribute("point1Y", _pt1.y);
	node->SetAttribute("point2X", _pt2.x);
	node->SetAttribute("point2Y", _pt2.y);
}

void PointFeature::loadFeature(TiXmlElement *node)
{
	node->QueryIntAttribute("threshold", &_threshold);
	node->QueryIntAttribute("point1X", &_pt1.x);
	node->QueryIntAttribute("point1Y", &_pt1.y);
	node->QueryIntAttribute("point2X", &_pt2.x);
	node->QueryIntAttribute("point2Y", &_pt2.y);
}

int PointFeature::regression(Patch& patch)
{
	int feature = patch.getOriginPatchPoint(_pt1) - patch.getOriginPatchPoint(_pt2);
	if (feature > _threshold) return 2;
	else return 1;
}

