#ifndef LEAF_H
#define LEAF_H

#include <tinyxml.h>
#include <vector>
#include "Patch.h"

class Leaf
{
	public:
		Leaf(const std::vector<Patch*>& nodeTs);//Constructor
		Leaf(TiXmlElement *node);//Constructor for leaf loading
		void saveLeaf(TiXmlElement *parentNode, int depth=0); //Save leaf params
		void loadLeaf(TiXmlElement *node);
		
		//Getter
		inline cv::Point_<int> getMeanOffsets()
		{
			return _meanOffsets;
		}

		inline cv::Point_<double> getVarOffsets()
		{
			return _varOffsets;
		}

		inline cv::Mat getSVMean()
		{
			return _stateVectorMean;
		}

		inline cv::Mat getSVVar()
		{
			return _stateVectorVariance;
		}

		inline int getNumberPatchs()
		{
			return _nbPatchs;
		}

		inline double getConf()
		{
			return _conf;
		}

	protected:
		std::vector<std::string> _stateVectorNames;
		cv::Point_<int> _meanOffsets;
		cv::Point_<double> _varOffsets;
		cv::Mat _stateVectorMean;
		cv::Mat _stateVectorVariance;
		int _nbPatchs;
		double _conf;

		/*//A supprimer
		std::vector<std::string> _classNames; // indice 0 == negatives
		std::vector<std::vector<cv::Point_<int> > > _classOffsets;
		std::vector<cv::Point_<int> > _classMeanOffsets;
		std::vector<int> _classSizes;//*/
		
};

#endif
