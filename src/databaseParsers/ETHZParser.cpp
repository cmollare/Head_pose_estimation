#include "ETHZParser.h"

ETHZParser::ETHZParser(std::string root) : DatabaseParser(), _rootPath(root)
{
	_vStateVectorNames.push_back("class");
	_vStateVectorNames.push_back("x");
	_vStateVectorNames.push_back("y");
	_vStateVectorNames.push_back("z");
	_vStateVectorNames.push_back("yaw");
	_vStateVectorNames.push_back("pitch");
	_vStateVectorNames.push_back("roll");

	//To be put in the xml conf file
	_templateSize.height = 150;
	_templateSize.width = 150;

	path p(_rootPath);

	if (!exists(p)) throw ForestException("Error : " + _rootPath + " does not exist");

	if (is_regular_file(p)) throw ForestException("Error : " + _rootPath + " is not a folder");
	else if (!is_regular_file(p) && !is_directory(p)) throw ForestException("Error : " + _rootPath + " is not a folder");

	std::vector<path> vec;

	copy(directory_iterator(p), directory_iterator(), back_inserter(vec));//get all files and folder recursively

	for (std::vector<path>::const_iterator it = vec.begin() ; it != vec.end() ; it++)
	{
		if(is_directory(*it)) this->exploreFolder(*it);
	}
}

ETHZParser::~ETHZParser()
{
}

void ETHZParser::getDatabase(std::vector<cv::Point_<int> >& centers, std::vector<std::vector<double> >& groundTruth, std::vector<std::string>& path)
{
	groundTruth.clear();
	groundTruth.assign(_vGDTruth.begin(), _vGDTruth.end());

	path.clear();
	path.assign(_vPaths.begin(), _vPaths.end());

	centers.clear();
	centers.assign(_vCenters.begin(), _vCenters.end());
}

std::vector<double> ETHZParser::readTextFile(std::string& path, std::vector<double>& calibRGB)
{
	std::vector<double> stateVector;

	std::ifstream textFile(path.c_str());
	float mr11, mr12, mr13, mr21, mr22, mr23, mr31, mr32, mr33;// Rotation matrix coeffs
	float x, y, z;// Translation vector coeffs;
	textFile >> mr11 >> mr12 >> mr13;
	textFile >> mr21 >> mr22 >> mr23;
	textFile >> mr31 >> mr32 >> mr33;
	textFile >> x >> y >> z;

	//Convention R = yaw*pitch*roll <=> rool performed first
	double yaw = atan(mr21/mr11)*180/PI;
	double pitch = atan(-mr31/sqrt(mr32*mr32 + mr33*mr33))*180/PI;
	double roll = atan(mr32/mr33)*180/PI;



	//Convert world positions to image position in pixels
	cv::Point_<int> center;
	double scale = calibRGB[6]*x + calibRGB[7]*y + calibRGB[8]*z;
	center.x = (int)((calibRGB[0]*x + calibRGB[1]*y + calibRGB[2]*z)/scale);
	center.y = (int)((calibRGB[3]*x + calibRGB[4]*y + calibRGB[5]*z)/scale);

	_vCenters.push_back(center);
	stateVector.push_back(1);//Class to modify in patchs extraction
	stateVector.push_back(x);
	stateVector.push_back(y);
	stateVector.push_back(z);
	stateVector.push_back(yaw);
	stateVector.push_back(pitch);
	stateVector.push_back(roll);

	return stateVector;
}

std::vector<double> ETHZParser::readCalibFile(std::string& path)
{
	std::ifstream textFile(path.c_str());
	float mr11, mr12, mr13, mr21, mr22, mr23, mr31, mr32, mr33;// Rotation matrix coeffs
	textFile >> mr11 >> mr12 >> mr13;
	textFile >> mr21 >> mr22 >> mr23;
	textFile >> mr31 >> mr32 >> mr33;

	std::vector<double> calibVec;
	calibVec.push_back(mr11);
	calibVec.push_back(mr12);
	calibVec.push_back(mr13);
	calibVec.push_back(mr21);
	calibVec.push_back(mr22);
	calibVec.push_back(mr23);
	calibVec.push_back(mr31);
	calibVec.push_back(mr32);
	calibVec.push_back(mr33);

	return calibVec;
}

void ETHZParser::exploreFolder(path p)
{
	std::vector<path> vec;
	copy(directory_iterator(p), directory_iterator(), back_inserter(vec));

	std::vector<double> calibRGBMat;
	for (std::vector<path>::const_iterator it = vec.begin() ; it != vec.end() ; it++)
	{
		if(!it->filename().native().compare("rgb.cal"))
		{
			std::string calibFile = it->native();
			calibRGBMat = this->readCalibFile(calibFile);
			break;
		}
	}

	for (std::vector<path>::const_iterator it = vec.begin() ; it != vec.end() ; it++)
	{
		if(it->filename().native().find("pose.txt") != std::string::npos)//Select all file containing poses
		{
			std::string textFile = it->native();
			std::string colorFile = textFile;
			colorFile.replace(it->native().find("pose.txt"), std::string::npos, "rgb.png");//Retrieve paths of image associated to selected ground truth
			_vPaths.push_back(colorFile);
			_vGDTruth.push_back(this->readTextFile(textFile, calibRGBMat));
		}
	}

}

void ETHZParser::getStateVectorNames(std::vector<std::string>& stateVecNames)
{
	stateVecNames.clear();
	stateVecNames.assign(_vStateVectorNames.begin(), _vStateVectorNames.end());
}

void ETHZParser::getTemplateSize(cv::Size_<int>& templateSize)
{
	templateSize.height = _templateSize.height;
	templateSize.width = _templateSize.width;
}
