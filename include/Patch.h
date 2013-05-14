#ifndef PATCH_H
#define PATCH_H

#include<opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <iostream>

#include "ForestException.h"

class Patch
{
	public:
		Patch(std::vector<cv::Mat> patchs, cv::Point_<int> patchOffset, cv::Rect_<int> roi, std::vector<double> stateVector, std::vector<std::string> svNames);//Constructor depreciated
		Patch(std::string imagePath, cv::Point_<int> patchOffset, cv::Rect_<int> roi, std::vector<double> stateVector, std::vector<std::string> svNames);
		Patch(std::vector<cv::Mat> patchs, cv::Rect_<int> roi);
		int getOriginPatchPoint(cv::Mat& image, cv::Point_<int>& pt);//Extract a point from the original patch (nb 0 in _patchs)
		int getIntPatchRect(cv::Mat& image, cv::Rect_<int>& roi);//Extract an roi from the integral patch
		int getOriginPatchPoint(cv::Point_<int>& pt);//Extract a point from the original patch (nb 0 in _patchs)
		int getIntPatchRect(cv::Rect_<int>& roi);//Extract an roi from the integral patch
		cv::Rect_<int> getRoi();
		cv::Point_<int> getOffset();
        std::vector<double>& getStateVector();
		std::vector<std::string> getSVNames();

		inline std::string getImagePath()
		{
			return _imagePath;
		}

	private:
		std::vector<cv::Mat> _patchs;//0 : original patch. 1 : integral patch
		cv::Rect_<int> _roi;
		cv::Point_<int> _patchOffset;
		std::vector<double> _stateVector;
		std::vector<std::string> _svNames;
		std::string _imagePath;
};

#endif
