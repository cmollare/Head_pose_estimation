#include "Patch.h"

Patch::Patch(std::vector<cv::Mat> patchs, cv::Point_<int> patchOffset, cv::Rect_<int> roi, std::vector<double> stateVector, std::vector<std::string> svNames)
	: _patchs(patchs), _roi(roi), _patchOffset(patchOffset), _stateVector(stateVector), _svNames(svNames)
{
}

Patch::Patch(std::string imagePath, cv::Point_<int> patchOffset, cv::Rect_<int> roi, std::vector<double> stateVector, std::vector<std::string> svNames)
	: _imagePath(imagePath), _roi(roi), _patchOffset(patchOffset), _stateVector(stateVector), _svNames(svNames)
{

}

Patch::Patch(std::vector<cv::Mat> patchs, cv::Rect_<int> roi) : _patchs(patchs), _roi(roi)
{
}

int Patch::getOriginPatchPoint(cv::Mat& image, cv::Point_<int>& pt)
{
	//cv::Mat img = cv::imread(_imagePath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img = image(_roi);

	int result = img.at<uchar>(pt);

	//img.release();
	return result;
}

int Patch::getIntPatchRect(cv::Mat& image, cv::Rect_<int>& roi)
{
	cv::Rect_<int> roiInt = _roi;
	roiInt.width++;
	roiInt.height++;

	//cv::Mat img = cv::imread(_imagePath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img = image(_roi);
	cv::Mat sum;
	cv::integral(img, sum, CV_32S);

	//cv::Mat patch = sum(roiInt);
	//int resultBIS = _patchs[3].at<int>(roi.y, roi.x)+_patchs[3].at<int>(roi.y+roi.height, roi.x+roi.width)-_patchs[3].at<int>(roi.y, roi.x+roi.width)-_patchs[3].at<int>(roi.y+roi.height, roi.x);
	int result = sum.at<int>(roi.y, roi.x)+sum.at<int>(roi.y+roi.height, roi.x+roi.width)-sum.at<int>(roi.y, roi.x+roi.width)-sum.at<int>(roi.y+roi.height, roi.x);
	//int result = cv::sum(_patchs[0](roi))[0]; //version with sum on original patch

	//img.release();
	//sum.release();
	return result;
}

int Patch::getOriginPatchPoint(cv::Point_<int>& pt)
{
	return _patchs[0].at<uchar>(pt);
}

int Patch::getIntPatchRect(cv::Rect_<int>& roi)
{
	int result = _patchs[1].at<int>(roi.y, roi.x)+_patchs[1].at<int>(roi.y+roi.height, roi.x+roi.width)-_patchs[1].at<int>(roi.y, roi.x+roi.width)-_patchs[1].at<int>(roi.y+roi.height, roi.x);
	return result;
}

cv::Rect_<int> Patch::getRoi()
{
	return _roi;
}

cv::Point_<int> Patch::getOffset()
{
	return _patchOffset;
}

std::vector<double>& Patch::getStateVector()
{
	return _stateVector;
}

std::vector<std::string> Patch::getSVNames()
{
	return _svNames;
}
