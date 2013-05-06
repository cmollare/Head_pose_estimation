#ifndef MEANSHIFT_H
#define MEANSHIFT_H

#include "Leaf.h"
#include <opencv2/opencv.hpp>

class MeanShift
{
    public:
		MeanShift(std::vector<cv::Mat>& detections);
		void iterate(int maxIter=10);
		void getMaxCluster(std::vector<cv::Mat>& cluster, cv::Mat& clusterMean);

	protected:
		std::vector<cv::Mat> _detections;
		std::vector<cv::Mat> _clusterMeans;
		std::vector<std::vector<cv::Mat> > _clusters;
		double _radius, _radius2;
		unsigned int _maxClusters;
};

#endif // MEANSHIFT_H
