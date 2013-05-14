#ifndef POINTFEATURE_H
#define POINTFEATURE_H

#include "Feature.h"

class PointFeature : public Feature
{
	public:
		PointFeature(ForestEnv *forestEnv, ThreadManager* thread, const std::vector<Patch*>& patchs, cv::Point_<int>& pt1, cv::Point_<int>& pt2);
		PointFeature(ForestEnv *forestEnv, cv::Point_<int>& pt1, cv::Point_<int>& pt2, int threshold);
		PointFeature(ForestEnv *forestEnv, TiXmlElement *node);
		virtual void extractFeature(int patchNum, cv::Mat& image);
		virtual void saveFeature(TiXmlElement *node);
		virtual void loadFeature(TiXmlElement *node);
		inline virtual cv::Point_<int>& getPoint1()
		{
			return _pt1;
		}
		inline virtual cv::Point_<int>& getPoint2()
		{
			return _pt2;
		}
		
		
		inline virtual std::string featureName()
		{
			return "Point";
		}
		
		virtual int regression(Patch& patch);
		//typedef Feature* (*loaderPtr)(std::string);
		//Feature* PointFeatureFromString(std::string);
	protected:
		cv::Point_<int> _pt1, _pt2;
};

#endif
