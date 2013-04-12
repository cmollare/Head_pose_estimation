#include "Feature.h"

Feature::Feature(ForestEnv *forestEnv) : _pForestEnv(forestEnv)
{
	_threshold=0;
	_maxEntropy=0;
	_features.clear();
	_patchs.clear();
	_maxFeature = _minFeature = 0;
	_pRNG = _pForestEnv->getRNGSeed();
}

void Feature::findOptimalDatasetThreshold(int test)
{
	_threshold=0;
	double currEntropy=0;
	_maxEntropy=0;
	for (int i=0 ; i<20 ; i++)
	{
		int diff = _maxFeature-_minFeature;
		if(diff==0) diff=1;//you can't divide by 0 !!!

		int tr = cvRandInt(_pRNG)%diff + _minFeature;

		std::vector<Patch*> left;
		std::vector<Patch*> right;
		
		//Split the dataset function of the threshold
		this->splitDataset(tr, left, right);
		
		//criterion to decide what threshold to keep
		currEntropy = computeEntropy(left, right, 0) + computeEntropy(left, right, 1);
		//currEntropy = computeEntropy(left, right, 1);

		if (currEntropy > _maxEntropy || i==0)
		{
			_threshold=tr;
			_maxEntropy=currEntropy;
		}
		
	}
}

void Feature::getSplittedDatasets(std::vector<Patch*>& left, std::vector<Patch*>& right, double& entropy)
{
	left.clear(); right.clear();

	for (int feature=0 ; feature<_features.size() ; feature++)
	{
		//Warning ! bad alloc possibility
		if(_features[feature]>_threshold) left.push_back(_patchs[feature]);
		else right.push_back(_patchs[feature]);
	}
	entropy = _maxEntropy;
}

void Feature::splitDataset(int tr, std::vector<Patch*>& left, std::vector<Patch*>& right)
{
	left.clear(); right.clear();
	left.reserve(_features.size());
	right.reserve(_features.size());

	for (int feature=0 ; feature<_features.size() ; feature++)
	{
		//Warning ! bad alloc possibility
		if(_features[feature]>tr) left.push_back(_patchs[feature]);
		else right.push_back(_patchs[feature]);
	}
}

double Feature::computeEntropy(std::vector<Patch*>& left, std::vector<Patch*>& right, int test)
{
	if(test==0)//Test sur loi gaussienne multivariée
	{
		double infoGain=0;
		int stateVectorSize=0;

		//If one of the vector is empty => infoGain=0
		if (left.size()==0 || right.size() == 0) return 0;
		else stateVectorSize=left[0]->getStateVector().size();

		int vecSizes = stateVectorSize-1;





		//Mean and variance computation
		cv::Mat muLeft(vecSizes, 1, CV_64FC1, cv::Scalar(0)), muRight(vecSizes, 1, CV_64FC1, cv::Scalar(0)), muTot(vecSizes, 1, CV_64FC1, cv::Scalar(0));
		cv::Mat varLeft(vecSizes, vecSizes, CV_64FC1, cv::Scalar(0)), varRight(vecSizes, vecSizes, CV_64FC1, cv::Scalar(0)), varTot(vecSizes, vecSizes, CV_64FC1, cv::Scalar(0));

		double leftSize=0, rightSize=0;

		for (int i=0 ; i<left.size() ; i++)//Right mean and variance
		{
			std::vector<double> stateVector = left[i]->getStateVector();
			if(stateVector[0]==0) continue;

			for (int j=0 ; j<vecSizes ; j++)
			{
				muLeft.at<double>(j) += (double)stateVector[j+1];
				varLeft.at<double>(j,j) += (double)(stateVector[j+1]*stateVector[j+1]);

				//std::cout << varLeft.at<double>(j,j) << " " << (double)(stateVector[j]*stateVector[j]) << std::endl;
			}
			leftSize++;
		}

		if (leftSize<2) return 0;

		for (int i=0 ; i<right.size() ; i++)//Left mean and variance
		{
			std::vector<double> stateVector = right[i]->getStateVector();
			if(stateVector[0]==0) continue;

			for (int j=0 ; j<vecSizes ; j++)
			{
				muRight.at<double>(j) += stateVector[j+1];
				varRight.at<double>(j,j) += stateVector[j+1]*stateVector[j+1];
			}
			rightSize++;
		}

		if (rightSize<2) return 0;

		double ratioLeft = leftSize/(leftSize + rightSize);
		double ratioRight = rightSize/(leftSize + rightSize);

		//Mean processing
		muTot = muRight + muLeft;
		//std::cout << muTot << " " << muRight << " " << muLeft << std::endl;
		muRight *= 1./rightSize;
		muLeft *= 1./leftSize;
		muTot *= 1./(leftSize + rightSize);
		//std::cout << muTot << " " << muRight << " " << muLeft << std::endl;

		//Variance processing
		varTot = varRight + varLeft;
		//std::cout << varTot << " " << varRight << " " << varLeft << std::endl;
		varRight = (1./rightSize)*varRight;
		varRight -= cv::Mat::diag(muRight)*cv::Mat::diag(muRight);
		varLeft = (1./leftSize)*varLeft;
		varLeft -= cv::Mat::diag(muLeft)*cv::Mat::diag(muLeft);
		varTot = varTot*1./(leftSize+rightSize);
		varTot -= cv::Mat::diag(muTot)*cv::Mat::diag(muTot);
		//std::cout << varTot << " " << varRight << " " << varLeft << std::endl;

		//End of mean and variance computation
		//std::cout << varTot << std::endl;
		//Information gain processing
		infoGain = log(cv::trace(varTot)[0]) - ratioLeft*log(cv::trace(varLeft)[0]) - ratioRight*log(cv::trace(varRight)[0]);
		/*std::cout << varTot << std::endl;
		std::cout << varLeft << std::endl;
		std::cout << varRight << std::endl;
		std::cout << infoGain << std::endl;*/
		return infoGain;
	}
	else if(test==1)//Test classique
	{
		double entropyLeft=0, entropyRight=0, totalEntropy=0;
		double leftSize=0, rightSize=0;

		for (int i=0 ; i<left.size() ; i++)//Right mean and variance
		{
			std::vector<double> stateVector = left[i]->getStateVector();
			if(stateVector[0]!=0) leftSize++;
		}

		if(leftSize==0) return 0;

		for (int i=0 ; i<right.size() ; i++)//Left mean and variance
		{
			std::vector<double> stateVector = right[i]->getStateVector();
			if(stateVector[0]!=0) rightSize++;
		}

		if (rightSize==0) return 0;

		//std::cout << left.size() << " " << leftSize << std::endl;
		double p = leftSize/double(left.size());
		//std::cout << p << std::endl;
		if (p>0) entropyLeft += p*log(p);
		//std::cout << entropyLeft << " " << p*log(p) << std::endl;
		p = (left.size()-leftSize)/double(left.size());
		if (p>0) entropyLeft += p*log(p);

		p = rightSize/double(right.size());
		if (p>0) entropyRight += p*log(p);
		p = (right.size()-rightSize)/double(right.size());
		if (p>0) entropyRight += p*log(p);

		/*for(int label=0 ; label<left.size() ; label++)
		{
			double p = double(left[label].size())/sizeLeft;
			if(p>0) entropyLeft += p*log(p);
		}

		for(int label=0 ; label<right.size() ; label++)
		{
			double p = double(right[label].size())/sizeRight;
			if(p>0) entropyRight += p*log(p);
		}*/
		totalEntropy = -(double(left.size()*entropyLeft) + double(right.size()*entropyRight))/double(left.size()+right.size());
		//std::cout << left.size() << " " << right.size() << std::endl;
		//std::cout << totalEntropy << std::endl;
		return totalEntropy;
	}
	/*else if(test==1)// CRITERE A TESTER !!!!
	{
		long int sizeLeft=0, sizeRight=0;
		double entropyLeft=0, entropyRight=0, totalEntropy=0;
		
		for(int label=0 ; label<left.size() ; label++) sizeLeft+=left[label].size();
		for(int label=0 ; label<right.size() ; label++) sizeRight+=right[label].size();
		
		for(int label=0 ; label<left.size() ; label++)
		{
			double p = double(left[label].size())/sizeLeft;
			if(p>0) entropyLeft += p*log(p);
		}
		
		for(int label=0 ; label<right.size() ; label++)
		{
			double p = double(right[label].size())/sizeRight;
			if(p>0) entropyRight += p*log(p);
		}
		
		double entropyLR=0;
		double p1=double(sizeRight)/double(sizeLeft+sizeRight);
		double p2=double(sizeLeft)/double(sizeLeft+sizeRight);
		if(p1>0) entropyLR = -p1*log(p1);
		if(p2>0) entropyLR -= p2*log(p2);
		static double maxEntropyLR=entropyLR;
		if(entropyLR > maxEntropyLR) maxEntropyLR = entropyLR;
		totalEntropy = (double(sizeLeft*entropyLeft) + double(sizeRight*entropyRight))/double(sizeLeft+sizeRight);
		//std::cout << totalEntropy << " " << entropyLR << " " << maxEntropyLR << std::endl;
		totalEntropy+=0.2*entropyLR;
		
		return totalEntropy;
	}
	else if(0)//Test ac norme2
	{
	long int sizeLeft=0, sizeRight=0;
	double entropyLeft=0, entropyRight=0, totalEntropy=0;
	
	for(int label=0 ; label<left.size() ; label++) sizeLeft+=left[label].size();
	for(int label=0 ; label<right.size() ; label++) sizeRight+=right[label].size();
	
	for(int label=0 ; label<left.size() ; label++)
	{
		double p = double(left[label].size())/sizeLeft;
		if(p>0) entropyLeft += (1+p*log(p))*(1+p*log(p))+(p*log(p))*(p*log(p));
	}
	
	for(int label=0 ; label<right.size() ; label++)
	{
		double p = double(right[label].size())/sizeRight;
		if(p>0) entropyRight += (1+p*log(p))*(1+p*log(p))+(p*log(p))*(p*log(p));
	}
	
	totalEntropy = sqrt(double(sizeLeft*entropyLeft) + double(sizeRight*entropyRight))/double(sizeLeft+sizeRight);
	return totalEntropy;
	}*/
}
