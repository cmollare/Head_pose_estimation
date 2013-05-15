#include "MeanShift.h"

MeanShift::MeanShift(const std::vector<cv::Mat>& detections) : _detections(detections)
{
	//std::cout << detections.size() << std::endl;
	_maxClusters=100;
	_radius = 300;
	_radius2 = floor(log(150*150));

	for (int det=0 ; det < _detections.size() ; det++)
	{
		bool affected=false;
		unsigned int bestCluster=0;

		for (int c=0 ; (c<_clusterMeans.size() && affected==false) ; c++)
		{
			cv::Mat& stateVector = _detections[det];
			double norm=0;
			for (int n=0 ; n<2 ; n++)
			{
				//std::cout << stateVector.at<double>(n) << " " <<  _clusterMeans[c].at<double>(n) << std::endl;
				norm += (stateVector.at<double>(n) - _clusterMeans[c].at<double>(n))*(stateVector.at<double>(n) - _clusterMeans[c].at<double>(n));
			}
			//std::cout << norm << " " << _radius << std::endl;
			if (norm < _radius)
			{
				affected = true;
				bestCluster=c;

				_clusters[bestCluster].push_back(_detections[det]);

				_clusterMeans[bestCluster] = _clusters[bestCluster][0].clone();

				for (int i=1 ; i<_clusters[bestCluster].size() ; i++)
					_clusterMeans[bestCluster] += _clusters[bestCluster][i].clone();

				_clusterMeans[bestCluster] /= _clusters[bestCluster].size();
			}

		}

		if (!affected && _clusters.size()<_maxClusters)
		{
			std::vector<cv::Mat> detection;
			detection.push_back(_detections[det]);
			_clusters.push_back(detection);

			_clusterMeans.push_back(_detections[det].clone());
		}
	}

	this->iterate();
}

void MeanShift::iterate(int maxIter)
{
	std::vector<cv::Mat> newMeans;
	std::vector<std::vector<cv::Mat> > newClusters;

	for (int c=0 ; c<_clusterMeans.size() ; c++)
	{

		std::vector<cv::Mat> newCluster;

		for (int it=0 ; it<maxIter ; it++)
		{
			unsigned int count=0;
			cv::Mat tempMean = cv::Mat(6, 1, CV_64FC1, cv::Scalar_<double>(0));
			newCluster.clear();

			for (int idx=0 ; idx<_clusters[c].size() ; idx++)
			{
				double norm=0;
				for (int n=0 ; n<2 ; n++)
				{
					//std::cout << c << " " << idx << std::endl;
					double test = _clusters[c][idx].at<double>(n);
					double test2 = _clusterMeans[c].at<double>(n);
					norm += double(_clusters[c][idx].at<double>(n) - _clusterMeans[c].at<double>(n))*double(_clusters[c][idx].at<double>(n) - _clusterMeans[c].at<double>(n));
				}
				if (norm < _radius2)
				{
					newCluster.push_back(_clusters[c][idx]);
					//std::cout << _clusters[c][idx]->getSVMean().size().height << " " << _clusters[c][idx]->getSVMean().size().width << std::endl;
					tempMean += _clusters[c][idx].clone();
				}
			}

			tempMean /= newCluster.size();

			double distanceFromPreviousMean=0;
			for (int n=0 ; n<2 ; n++)
				distanceFromPreviousMean += (tempMean.at<double>(n) - _clusterMeans[c].at<double>(n))*(tempMean.at<double>(n) - _clusterMeans[c].at<double>(n));

			_clusterMeans[c] = tempMean;

			if (distanceFromPreviousMean < 1)
			{
				//std::cout << it << std::endl;
				break;
			}
		}

		newMeans.push_back(_clusterMeans[c]);
		newClusters.push_back(newCluster);
	}

	_clusterMeans.clear();
	_clusters.clear();

	for (int c=0 ; c<newClusters.size() ; c++)
	{
		//if (newClusters.size() < 10) continue;

		_clusters.push_back(newClusters[c]);
		_clusterMeans.push_back(newMeans[c]);
	}

}

void MeanShift::getMaxCluster(std::vector<cv::Mat>& cluster, cv::Mat& clusterMean)
{
	cluster.clear();

	int currentSize=0;
	int indMax=0;
	for (int c=0 ; c<_clusters.size() ; c++)
	{
		if (_clusters[c].size() > currentSize)
		{
			indMax=c;
			currentSize=_clusters[c].size();
		}
	}

	cluster.assign(_clusters[indMax].begin(), _clusters[indMax].end());
	clusterMean = _clusterMeans[indMax].clone();
}
