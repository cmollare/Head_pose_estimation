#include "RectFeature.h"

RectFeature::RectFeature(ForestEnv *forestEnv, const std::vector<Patch*>& patchs, cv::Rect_<int>& rect1, cv::Rect_<int>& rect2) :
    Feature(forestEnv), _rect1(rect1), _rect2(rect2)
{
    _patchs = patchs;
}

RectFeature::RectFeature(ForestEnv *forestEnv, cv::Rect_<int>& rect1, cv::Rect_<int>& rect2, int threshold) : Feature(forestEnv), _rect1(rect1), _rect2(rect2)
{
	_pRNG = NULL;
	_threshold = threshold;
}

RectFeature::RectFeature(ForestEnv *forestEnv, TiXmlElement *node) : Feature(forestEnv)
{
	_pRNG = NULL;
	_threshold = 0;

	this->loadFeature(node);
}


void RectFeature::extractFeature()
{
    //Memory allocation for result of feature extraction
    _features.reserve(_patchs.size()); //Feature is the difference between two points

    //Feature extraction
    for (int patch=0 ; patch<_patchs.size() ; patch++)
    {
        //Extract feature (difference between to rectangles in the patch)
		//int sum = cv::sum(_patchs[patch]->getIntPatchRect(_rect1))[0]-cv::sum(_patchs[patch]->getIntPatchRect(_rect2))[0];
		int sum = _patchs[patch]->getIntPatchRect(_rect1)-_patchs[patch]->getIntPatchRect(_rect2);
        _features.push_back(sum);

        //Test to find max and min values
        if(_features[patch] > _maxFeature || (patch==0)) _maxFeature = _features[patch];
        else if(_features[patch] < _minFeature || (patch==0)) _minFeature = _features[patch];
    }
}

void RectFeature::saveFeature(TiXmlElement *node)
{
    node->SetAttribute("threshold", _threshold);
    node->SetAttribute("rect1X", _rect1.x);
    node->SetAttribute("rect1Y", _rect1.y);
    node->SetAttribute("rect1Width", _rect1.width);
    node->SetAttribute("rect1Height", _rect1.height);
    node->SetAttribute("rect2X", _rect2.x);
    node->SetAttribute("rect2Y", _rect2.y);
    node->SetAttribute("rect2Width", _rect2.width);
    node->SetAttribute("rect2Height", _rect2.height);
}

void RectFeature::loadFeature(TiXmlElement *node)
{
    node->QueryIntAttribute("threshold", &_threshold);
    node->QueryIntAttribute("rect1X", &_rect1.x);
    node->QueryIntAttribute("rect1Y", &_rect1.y);
    node->QueryIntAttribute("rect1Width", &_rect1.width);
    node->QueryIntAttribute("rect1Height", &_rect1.height);
    node->QueryIntAttribute("rect2X", &_rect2.x);
    node->QueryIntAttribute("rect2Y", &_rect2.y);
    node->QueryIntAttribute("rect2Width", &_rect2.width);
    node->QueryIntAttribute("rect2Height", &_rect2.height);
}

int RectFeature::regression(Patch& patch)
{
	int feature = patch.getIntPatchRect(_rect1)-patch.getIntPatchRect(_rect2);
	if (feature > _threshold) return 2;
	else return 1;
}
