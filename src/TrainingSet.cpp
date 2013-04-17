#include "TrainingSet.h"

TrainingSet::TrainingSet(ForestEnv* forestEnv)
{
	_pForestEnv = forestEnv;
}

TrainingSet::~TrainingSet()
{
	for (int i=0 ; i<_vFeatures.size() ; i++)
	{
		delete _vFeatures[i];
	}
}

void TrainingSet::initTrainingSet()
{
	_pRNG = _pForestEnv->getRNGSeed();
	_width = _pForestEnv->getPatchWidth();
	_height = _pForestEnv->getPatchHeight();

	_vSVParamsName = _pForestEnv->getSVParamsName();
	_numParams = _vSVParamsName.size();

	
	//_features.resize(_numLabels);
	//_dbPathes.resize(_numLabels);
	//_centers.resize(_numLabels);
	
	this->getDataBase();
	this->extractPatches();


	_nbPatchs=_vFeatures.size();
	
}

void TrainingSet::initCrossValidation()
{
	_pRNG = _pForestEnv->getRNGSeed();
	_width = _pForestEnv->getPatchWidth();
	_height = _pForestEnv->getPatchHeight();

	_vSVParamsName = _pForestEnv->getSVParamsName();
	_numParams = _vSVParamsName.size();

	this->getDataBase();
    //this->shuffleDBVectors();
	this->extractPatches();

	_nbPatchs=_vFeatures.size();
}

int TrainingSet::getNumberOfPatchs()
{
	return _nbPatchs;
}

std::vector<Patch*>& TrainingSet::getVectorTs()
{
	return _vFeatures;
}

void TrainingSet::getDataBase()
{
	std::string dbType = _pForestEnv->getDatabaseType();

	DatabaseParser *dbParser = NULL;

	if (!strcmp(dbType.c_str(), "ETH"))
	{
		std::cout << "Retrieving ETH database" << std::endl;
		dbParser = new ETHZParser(_pForestEnv->getClassFolderPath()["positive"]);
		dbParser->getStateVectorNames(_vSVParamsName);
		dbParser->getDatabase(_vCenters, _vGroundTruth, _vPaths);
		dbParser->getTemplateSize(_templateSize);
	}
	else throw ForestException("Unknown database : " + dbType);

	std::cout << "Database retrieved" << std::endl;

	if (dbParser) delete dbParser;
}

void TrainingSet::shuffleDBVectors()
{
	std::cout << "Shuffling database..." << std::endl;

	std::vector<int> indices(_vPaths.size());

	std::vector<cv::Point_<int> > vCenters = _vCenters;
	std::vector<std::vector<double> > vGdTruth = _vGroundTruth;
	std::vector<std::string> vPaths = _vPaths;


	//Randomize indices of database vectors
	for (int i=0 ; i<indices.size() ; i++) indices[i]=i;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(indices.begin(), indices.end(), std::default_random_engine(seed));

	//Reassign new positions in vectors
	for (int i=0 ; i<indices.size() ; i++)
	{
		_vCenters[i]=vCenters[indices[i]];
		_vGroundTruth[i]=vGdTruth[indices[i]];
		_vPaths[i]=vPaths[indices[i]];
	}

	std::cout << "Database shuffled" << std::endl;
}

void TrainingSet::makeKFolders()
{
	int _currentTestFolder=0;
	//_vLearning
}

void TrainingSet::extractPatches()
{

	int nbPatch = _pForestEnv->getNumPatchs();

	//find the position of state "x" and "y" in state vector
	int x, y;
	for (int i=0 ; i<_vSVParamsName.size() ; i++) if (!_vSVParamsName[i].compare("x")) x=i;
	for (int i=0 ; i<_vSVParamsName.size() ; i++) if (!_vSVParamsName[i].compare("y")) y=i;

    //for (int imgNum=0 ; imgNum<_vPaths.size() ; imgNum++)
    for (int imgNum=0 ; imgNum<600 ; imgNum++)
	{
		cv::Mat img = cv::imread(_vPaths[imgNum]);
		cv::cvtColor(img, img, CV_RGB2GRAY);// Only gray images
		//std::cout << img.channels() << std::endl;
		cv::Mat sum;
		cv::integral(img, sum, CV_32S); //compute integral image


		//CvMat* locations = cvCreateMat(nbPatch, 1, CV_32SC2);
		//cvRandArr(_pRNG, locations, CV_RAND_UNI, cvScalar());
		int lowerBoundX = _vCenters[imgNum].x - _templateSize.width/2;
		int upperBoundX = _vCenters[imgNum].x + _templateSize.width/2 - _width;
		int lowerBoundY = _vCenters[imgNum].y - _templateSize.height/2;
		int upperBoundY = _vCenters[imgNum].y + _templateSize.height/2 - _height;

		//std::cout << _vCenters[imgNum].x << " " << _vCenters[imgNum].y << std::endl;
		//std::cout << lowerBoundX << " " << lowerBoundY << " " << upperBoundX << " " << upperBoundY << std::endl;
		cv::Mat locations(nbPatch, 1, CV_32SC2);
		cv::Vec<int, 2> lowerBound(lowerBoundX, lowerBoundY);
		cv::Vec<int, 2> upperBound(upperBoundX, upperBoundY);
		cv::randu(locations, lowerBound, upperBound);

		//Extract positive patchs
		for (int patch=0 ; patch < nbPatch ; patch++)
		{
			std::vector<cv::Mat> patchs;
			cv::Point_<int> pt = locations.at<cv::Point_<int> >(patch);

			cv::Rect_<int> roi(pt.x, pt.y, _width, _height); //patch
			cv::Rect_<int> roiInt(pt.x, pt.y, _width+1, _height+1); //patch extended for integral image (1 line and col of zeros)
			cv::Point_<int> patchOffset(pt.x + (int)(_width/2) - _vCenters[imgNum].x, pt.y + (int)(_height/2) - _vCenters[imgNum].y); //patch Center

			_vGroundTruth[imgNum][0] = 1; //Class positive
			_vGroundTruth[imgNum][1] = pt.x + (int)(_width/2) - _vCenters[imgNum].x;
			_vGroundTruth[imgNum][2] = pt.y + (int)(_height/2) - _vCenters[imgNum].y;
			_vGroundTruth[imgNum][3] = cv::randu<double>();

			patchs.push_back(img(roi).clone());
			patchs.push_back(sum(roiInt).clone());// patch of integral image

			_vFeatures.push_back(new Patch(patchs, patchOffset, roi, _vGroundTruth[imgNum], _vSVParamsName)); //Store the patch into the trainingSet
		}

		//Extract negative patchs
		cv::Mat location(1, 1, CV_32SC2);
		cv::Vec<int, 2> lowerNegBound(0, 0);
		cv::Vec<int, 2> upperNegBound(img.cols-_width, img.rows-_height);

		for (int patch=0 ; patch<nbPatch ; patch++)
		{
			cv::randu(location, lowerNegBound, upperNegBound);
			cv::Point_<int> pt = location.at<cv::Point_<int> >(0);

			//Check if patch is selected out of the template (=> negative). If not, try another
			while(((pt.x >= lowerBoundX) && (pt.x <= upperBoundX)) || ((pt.y >= lowerBoundY) && (pt.y <= upperBoundY)))
			{
				cv::randu(location, lowerNegBound, upperNegBound);
				pt = location.at<cv::Point_<int> >(0);
			}

			std::vector<cv::Mat> patchs;

			cv::Rect_<int> roi(pt.x, pt.y, _width, _height); //patch
			cv::Rect_<int> roiInt(pt.x, pt.y, _width+1, _height+1); //patch extended for integral image (1 line and col of zeros)
			cv::Point_<int> patchOffset(pt.x + (int)(_width/2) - _vCenters[imgNum].x, pt.y + (int)(_height/2) - _vCenters[imgNum].y); //patch Center

			_vGroundTruth[imgNum][0] = 0; //Class negative
			_vGroundTruth[imgNum][1] = pt.x + (int)(_width/2) - _vCenters[imgNum].x;
			_vGroundTruth[imgNum][2] = pt.y + (int)(_height/2) - _vCenters[imgNum].y;
			_vGroundTruth[imgNum][3] = cv::randu<double>();

			patchs.push_back(img(roi).clone());
			patchs.push_back(sum(roiInt).clone());// patch of integral image

			_vFeatures.push_back(new Patch(patchs, patchOffset, roi, _vGroundTruth[imgNum], _vSVParamsName)); //Store the patch into the trainingSet
		}

		std::cout << "image " << imgNum << " extracted !" << std::endl;


	}
}



