#include "ForestDetector.h"

ForestDetector::ForestDetector(ForestEnv* forestEnv)
{
	_pForestEnv = forestEnv;
	pForest = new Forest(_pForestEnv, 0);
}

ForestDetector::~ForestDetector()
{
	delete pForest;
}

void ForestDetector::detect(std::string folder)
{
	fs::path path(folder);
	if(!fs::exists(path)) throw ForestException("Folder" + path.string() + "does not exist");

	if(!fs::is_directory(path)) throw ForestException(path.string() + " is not a folder");

	std::vector<fs::path> vec;
	copy(fs::recursive_directory_iterator(path), fs::recursive_directory_iterator(), back_inserter(vec));
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle (vec.begin(), vec.end(), std::default_random_engine(seed));

	for (std::vector<fs::path>::const_iterator it=vec.begin() ; it != vec.end() ; ++it)
	{
		std::cout << *it << std::endl;
		if(it->string().find(".png") != std::string::npos)//Verify that it is an image
		{
			cv::Mat image = cv::imread(it->string());
			this->detect(image, it->parent_path().filename().string() + it->filename().string());
			//break;
		}
	}
}

void ForestDetector::detect(cv::Mat& image, std::string imageName)//deuxième paramètre peut-être à supprimer
{
	cv::namedWindow("lol");
	cv::Mat integral;
	cv::cvtColor(image, image, CV_RGB2GRAY);//Convert image to grayscale image
	cv::integral(image, integral);
	int height = image.size().height;
	int width = image.size().width;
	unsigned int patchWidth = _pForestEnv->getPatchWidth();
	unsigned int patchHeight = _pForestEnv->getPatchHeight();
	unsigned int nbPatch = (height-patchHeight)*(width-patchWidth);
	
	float currentMax=0;
	
	//std::vector<Patch*> vecPatch(nbPatch);
	cv::Mat result = cv::Mat::zeros(image.size(), CV_32F);

	std::vector<cv::Point_<int> > offsetMeans;
	std::vector<cv::Point_<double> > offsetVar;
	std::vector<int> nbPatchs;

	cv::Mat imRoi, imIntRoi;
	
	for (int x=0 ; x < width-patchWidth ; x++)//cols
	{
		for (int y=0 ; y < height-patchHeight ; y++)//rows
		{
			cv::Rect_<int> roi(x, y, patchWidth, patchHeight);
			cv::Rect_<int> roiInt(x, y, patchWidth+1, patchHeight+1); //patch
			std::vector<cv::Mat> patchs;


			image(roi).copyTo(imRoi);
			integral(roiInt).copyTo(imIntRoi);

			//patchs.push_back(image(roi).clone());
			//patchs.push_back(integral(roiInt).clone());

			patchs.push_back(imRoi);
			patchs.push_back(imIntRoi);



			Patch patch(patchs, roi);
			
			std::vector<Leaf*> detectedLeaf;
			pForest->regression(patch, detectedLeaf);

			for (int i=0 ; i<detectedLeaf.size() ; i++)
			{

				offsetMeans.push_back(detectedLeaf[i]->getMeanOffsets());
				offsetVar.push_back(detectedLeaf[i]->getVarOffsets());
				nbPatchs.push_back(detectedLeaf[i]->getNumberPatchs());

				//std::cout << offsetMeans.back() << " " << offsetVar.back() << " " << nbPatchs.back() << std::endl;

				cv::Point_<int> offset(-offsetMeans.back().x+patchWidth/2+x, -offsetMeans.back().y+patchHeight/2+y);
				if (offset.x >= 0 && offset.x < width && offset.y >= 0 && offset.y < height)
				{
					//if ((offsetVar.back().x <= 200) && (offsetVar.back().y <= 200))
					if (detectedLeaf[i]->getConf() > 0.5)
					//result.at<float>(offset) += 1.*float(1./(sqrt(offsetVar.back().x*offsetVar.back().x+offsetVar.back().y*offsetVar.back().y)));
					result.at<float>(offset) += 1.;
					//result.at<float>(offset) += 1.*float(nbPatchs.back()/sqrt(offsetVar.back().x*offsetVar.back().y));
					//result.at<float>(offset) += 1.*float(nbPatchs.back()/sqrt(offsetVar.back().x*offsetVar.back().x+offsetVar.back().y*offsetVar.back().y));
					if (result.at<float>(offset) > currentMax) currentMax=result.at<float>(offset);
				}
			}




			
			/*for (int i=0 ; i<detectedLeaf.size() ; i++)
			{
				std::vector<std::vector<cv::Point_<int> > > vecOffsets = detectedLeaf[i]->getOffsets();
				std::vector<cv::Point_<int> > vecMeanOffsets = detectedLeaf[i]->getMeanOffsets();
				std::vector<int> vecClasseSizes = detectedLeaf[i]->getClassSizes();
				
				//Ajouter le choix entre plusieurs offsets ou l'offset moyen
				
				/*
				for (int j=0 ; j<vecOffsets[1].size() ; j++)
				{
					cv::Point_<int> offset(-vecOffsets[1][j].x+patchWidth/2+x, -vecOffsets[1][j].y+patchHeight/2+y);
					if (offset.x >= 0 && offset.x < width && offset.y >= 0 && offset.y < height)
					{
						//std::cout << vecClasseSizes[1] << std::endl;
						result.at<float>(offset) += 1.*float(vecClasseSizes[1]/float(vecClasseSizes[0]+vecClasseSizes[1]));
						if (result.at<float>(offset) > currentMax) currentMax=result.at<float>(offset);
					}
                }//*/
				
				/*
				cv::Point_<int> offset(-vecMeanOffsets[1].x+patchWidth/2+x, -vecMeanOffsets[1].y+patchHeight/2+y);
				if (offset.x >= 0 && offset.x < width && offset.y >= 0 && offset.y < height)
				{
					//std::cout << vecClasseSizes[1] << std::endl;
					result.at<float>(offset) += 1.*float(vecClasseSizes[1]/float(vecClasseSizes[0]+vecClasseSizes[1]));
					if (result.at<float>(offset) > currentMax) currentMax=result.at<float>(offset);
				}//*/
				//if (vecMeanOffsets[1].x >= 0 && vecMeanOffsets[1].x < height && vecMeanOffsets[1].y >= 0 && vecMeanOffsets[1].y < width)
				//	result.at<int>(vecMeanOffsets[1]) += vecClasseSizes[1];//Warning position of pixel depend of type !!! int == CV_32S, char == CV_8U
				
				//Ajouter une boucle pour faire la detection sur plusieurs classes
				//if (vecMeanOffsets[1].x >= 0 && vecMeanOffsets[1].x < height && vecMeanOffsets[1].y >= 0 && vecMeanOffsets[1].y < width)
				//	result.at<int>(vecMeanOffsets[1]) += vecClasseSizes[1];//Warning position of pixel depend of type !!! int == CV_32S, char == CV_8U
			//}*/
		}
	}
	std::cout << currentMax << std::endl;
	//cv::Mat imageToSave = result/currentMax;
	//cv::imwrite("../output/"+imageName, result);

	//std::cout << result.depth() << std::endl;
	//std::cout << result.size().height << " " << result.size().width << std::endl;
	result=result/currentMax*255;
	result.convertTo(result, CV_8U);

	//cv::Rect rect(result.cols/2-50, result.rows/2-50, 50, 50);

	cv::RotatedRect rotatedRect(cv::Point2f(result.cols/2-50, result.rows/2), cv::Size2f(50, 50), 0);
	for (int i=0 ; i<2 ; i++)
	{
		cv::Rect rect = rotatedRect.boundingRect();
		rotatedRect = cv::CamShift(result, rect, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 1));
	}

	cv::Mat imageToSave=result.clone();
	cv::ellipse(imageToSave, rotatedRect, cv::Scalar_<int>(0,255,0), 2);


	cv::applyColorMap(imageToSave, imageToSave, cv::COLORMAP_JET);
	cv::imwrite("../output/"+imageName, imageToSave);
	cv::imshow("lol", imageToSave);

	cv::waitKey(50);
	
}
