#include "ForestDetector.h"

ForestDetector::ForestDetector(ForestEnv* forestEnv)
{
	_pForestEnv = forestEnv;
	pForest = new Forest(_pForestEnv, 0);
}

ForestDetector::ForestDetector(ForestEnv *forestEnv, int indice)
{
	_pForestEnv = forestEnv;
	pForest = new Forest(_pForestEnv, indice, 0);
	_index = indice;
	std::cout << "indice : " << _index << std::endl;
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
    //std::shuffle (vec.begin(), vec.end(), std::default_random_engine(seed));

	ETHZParser dbParser(folder);
	std::vector<cv::Point_<int> > centers;
	std::vector<std::vector<double> > groundTruth;
	std::vector<std::string> paths;
	dbParser.getDatabase(centers, groundTruth, paths);

	for (int i=0 ; i<groundTruth.size() ; i++)
	{
		fs::path p(paths.at(i));
		cv::Mat image = cv::imread(paths.at(i));
		std::cout << paths.at(i) << std::endl;
		std::cout << "gt : " << groundTruth.at(i).at(4) << " " << groundTruth.at(i).at(5) << " " << groundTruth.at(i).at(6) << std::endl;
		this->detect(image, p.parent_path().filename().string() + p.filename().string());
	}

	/*for (std::vector<fs::path>::const_iterator it=vec.begin() ; it != vec.end() ; ++it)
	{
		std::cout << *it << std::endl;
		if(it->string().find(".png") != std::string::npos)//Verify that it is an image
		{
			cv::Mat image = cv::imread(it->string());
			this->detect(image, it->parent_path().filename().string() + it->filename().string());
			//break;
		}
	}*/
}

void ForestDetector::detect(cv::Mat& image, std::string imageName)//deuxième paramètre peut-être à supprimer
{
	cv::namedWindow("lol");
    cv::namedWindow("detection");
    cv::namedWindow("hough");
	cv::Mat integral;
    cv::cvtColor(image, integral, CV_RGB2GRAY);//Convert image to grayscale image
    cv::integral(integral, integral);
	int height = image.size().height;
	int width = image.size().width;
	unsigned int patchWidth = _pForestEnv->getPatchWidth();
	unsigned int patchHeight = _pForestEnv->getPatchHeight();
	
	float currentMax=0;
	double pas=2;
	
	//std::vector<Patch*> vecPatch(nbPatch);
    cv::Mat result = cv::Mat::zeros(image.size().height/pas, image.size().width/pas, CV_32F);
	cv::Mat angle = cv::Mat::zeros(image.size().height/pas, image.size().width/pas, CV_64F);

	std::vector<cv::Point_<int> > offsetMeans;
	std::vector<cv::Point_<double> > offsetVar;
	std::vector<int> nbPatchs;

	cv::Mat imRoi, imIntRoi;

    double roll=0;
    double meanRoll=0;

    std::vector<Leaf*> detectionVotes;
	std::vector<cv::Mat> detectionMeans;
	
    for (int x=0 ; x < width-patchWidth ; x+=pas)//cols
	{
        for (int y=0 ; y < height-patchHeight ; y+=pas)//rows
		{
			cv::Rect_<int> roi(x, y, patchWidth, patchHeight);
			cv::Rect_<int> roiInt(x, y, patchWidth+1, patchHeight+1); //patch
            cv::Mat meanSV;
			std::vector<cv::Mat> patchs;
            double conf=0;


			image(roi).copyTo(imRoi);
			integral(roiInt).copyTo(imIntRoi);

			//patchs.push_back(image(roi).clone());
			//patchs.push_back(integral(roiInt).clone());

			patchs.push_back(imRoi);
			patchs.push_back(imIntRoi);



			Patch patch(patchs, roi);
			
			std::vector<Leaf*> detectedLeaf;
			pForest->regression(patch, detectedLeaf);


            double test=0;
            double denom=0;

			for (int i=0 ; i<detectedLeaf.size() ; i++)
			{

				offsetMeans.push_back(detectedLeaf[i]->getMeanOffsets());
				offsetVar.push_back(detectedLeaf[i]->getVarOffsets());
				nbPatchs.push_back(detectedLeaf[i]->getNumberPatchs());
				meanSV = detectedLeaf[i]->getSVMean().clone();
                conf = detectedLeaf[i]->getConf();



				//std::cout << offsetMeans.back() << " " << offsetVar.back() << " " << nbPatchs.back() << std::endl;
				cv::Point_<int> offset(-offsetMeans.back().x+patchWidth/2+x, -offsetMeans.back().y+patchHeight/2+y);
				if (offset.x >= 0 && offset.x < width && offset.y >= 0 && offset.y < height)
				{
                    offset.x = offset.x/pas;
                    offset.y = offset.y/pas;
					//if ((offsetVar.back().x <= 200) && (offsetVar.back().y <= 200))
					if ((conf == 1) && (detectedLeaf[i]->getTrace() < 500))
                    {
                        //result.at<float>(offset) += 1.*float(1./(sqrt(offsetVar.back().x*offsetVar.back().x+offsetVar.back().y*offsetVar.back().y)));
                        detectionVotes.push_back(detectedLeaf[i]);
						detectionMeans.push_back(detectedLeaf[i]->getSVMean().clone());
						detectionMeans.back().at<double>(0) = offset.x;
						detectionMeans.back().at<double>(1) = offset.y;
                        result.at<float>(offset) += conf;
                        //result.at<float>(offset) += detectedLeaf[i]->getNumberPatchs();
                        //result.at<float>(offset) += 1.*float(nbPatchs.back()/sqrt(offsetVar.back().x*offsetVar.back().y));
                        //result.at<float>(offset) += 1.*float(nbPatchs.back()/sqrt(offsetVar.back().x*offsetVar.back().x+offsetVar.back().y*offsetVar.back().y));
                        //roll += meanSV.at<double>(6)*conf;
                        denom+=conf;
                        test+=conf*meanSV.at<double>(4);
                        //std::cout << "cooooooooonf : " << test*20 << std::endl;
                        angle.at<double>(y/pas, x/pas) = test/denom;
                        //if (test > maxAngle) maxAngle = test;
                        //if (test < minAngle) minAngle = test;

                        //meanRoll += conf;
                    }
                    else
                    {
                        angle.at<double>(y/pas, x/pas) = -100;
                    }
                    if (result.at<float>(offset) > currentMax) currentMax=result.at<float>(offset);
				}
			}

		}
	}

	if (detectionMeans.size()==0) std::cout << "no detection" << std::endl;

	MeanShift ms(detectionMeans);
	cv::Mat mean;
	ms.getMaxCluster(detectionMeans, mean);

	//cv::Mat imageToSave = result/currentMax;
	//cv::imwrite("../output/"+imageName, result);

	//std::cout << result.depth() << std::endl;
	//std::cout << result.size().height << " " << result.size().width << std::endl;
	result=result/currentMax*255;
    result.convertTo(result, CV_8U);

    //std::cout << "angle : " << angle << std::endl;
    angle=(angle+22)/45*255;
    angle.convertTo(angle, CV_8U);
    cv::applyColorMap(angle, angle, cv::COLORMAP_JET);
    cv::imshow("hough", angle);

	//cv::Rect rect(result.cols/2-50, result.rows/2-50, 50, 50);

	/*cv::RotatedRect rotatedRect(cv::Point2f(result.cols/pas-50/pas, result.rows/pas), cv::Size2f(50/pas, 50/pas), 0);
    for (int i=0 ; i<2 ; i++)
	{
		cv::Rect rect = rotatedRect.boundingRect();
		rotatedRect = cv::CamShift(result, rect, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 1));
	}*/
	cv::RotatedRect rotatedRect(cv::Point2f(mean.at<double>(0), mean.at<double>(1)), cv::Size2f(128/pas, 128/pas), 0);
	std::cout << mean.at<double>(3) << " " << mean.at<double>(4) << " " << mean.at<double>(5) << std::endl;

	cv::Mat imageToSave=result.clone();
	cv::ellipse(imageToSave, rotatedRect, cv::Scalar_<int>(0,255,0), 2);
    rotatedRect.center = rotatedRect.center*pas;
    rotatedRect.size.height = rotatedRect.size.height*pas;
    rotatedRect.size.width = rotatedRect.size.width*pas;
    rotatedRect.angle = roll/meanRoll;

    //cv::rectangle(image, rotatedRect.boundingRect(), cv::Scalar_<int>(125,255,125), 2);
    cv::ellipse(image, rotatedRect, cv::Scalar_<int>(125,255,125), 2);


	cv::applyColorMap(imageToSave, imageToSave, cv::COLORMAP_JET);
    cv::imwrite("../output/"+imageName, angle);
    cv::imshow("detection", image);
	cv::imshow("lol", imageToSave);

	cv::waitKey(5000);
	
}

void ForestDetector::detect(std::vector<std::vector<double> >& vGroundTruth, std::vector<std::string>& vPaths, cv::Size_<int>& templateSize, std::vector<cv::Point_<int> >& vCenters)
{
	std::ostringstream oss;
	oss << "../output/result_learning_" << _index << ".txt" << std::endl;
	std::fstream fileRes(oss.str().c_str(), std::fstream::out);


	for (int imageInd=0 ; imageInd < vPaths.size() ; imageInd++)
	{

		cv::Mat image = cv::imread(vPaths.at(imageInd), CV_LOAD_IMAGE_GRAYSCALE);
		cv::Mat integral;
		cv::namedWindow("lol");
		cv::namedWindow("detection");
		cv::namedWindow("hough");
		cv::integral(image, integral);

		int height = image.size().height;
		int width = image.size().width;
		unsigned int patchWidth = _pForestEnv->getPatchWidth();
		unsigned int patchHeight = _pForestEnv->getPatchHeight();

		float currentMax=0;
		double pas=2;

		//For display
		cv::Mat result = cv::Mat::zeros(image.size().height/pas, image.size().width/pas, CV_32F);
		cv::Mat angle = cv::Mat::zeros(image.size().height/pas, image.size().width/pas, CV_64F);

		std::vector<cv::Point_<int> > offsetMeans;
		std::vector<cv::Point_<double> > offsetVar;
		std::vector<int> nbPatchs;

		cv::Mat imRoi, imIntRoi;

		std::vector<Leaf*> detectionVotes;
		std::vector<cv::Mat> detectionMeans;

		for (int x=0 ; x < width-patchWidth ; x+=pas)//cols
		{
			for (int y=0 ; y < height-patchHeight ; y+=pas)//rows
			{
				cv::Rect_<int> roi(x, y, patchWidth, patchHeight);
				cv::Rect_<int> roiInt(x, y, patchWidth+1, patchHeight+1); //patch
				cv::Mat meanSV;
				std::vector<cv::Mat> patchs;
				double conf=0;


				image(roi).copyTo(imRoi);
				integral(roiInt).copyTo(imIntRoi);

				patchs.push_back(imRoi);
				patchs.push_back(imIntRoi);

				Patch patch(patchs, roi);

				std::vector<Leaf*> detectedLeaf;
				pForest->regression(patch, detectedLeaf);

				double test=0;
				double denom=0;

				for (int i=0 ; i<detectedLeaf.size() ; i++)
				{

					offsetMeans.push_back(detectedLeaf[i]->getMeanOffsets());
					offsetVar.push_back(detectedLeaf[i]->getVarOffsets());
					nbPatchs.push_back(detectedLeaf[i]->getNumberPatchs());
					meanSV = detectedLeaf[i]->getSVMean().clone();
					conf = detectedLeaf[i]->getConf();



					//std::cout << offsetMeans.back() << " " << offsetVar.back() << " " << nbPatchs.back() << std::endl;
					cv::Point_<int> offset(-offsetMeans.back().x+patchWidth/2+x, -offsetMeans.back().y+patchHeight/2+y);
					if (offset.x >= 0 && offset.x < width && offset.y >= 0 && offset.y < height)
					{
						offset.x = offset.x/pas;
						offset.y = offset.y/pas;
						//if ((offsetVar.back().x <= 200) && (offsetVar.back().y <= 200))
						if ((conf == 1) && (detectedLeaf[i]->getTrace() < 500))
						{
							detectionVotes.push_back(detectedLeaf[i]);
							detectionMeans.push_back(detectedLeaf[i]->getSVMean().clone());
							detectionMeans.back().at<double>(0) = offset.x*pas;
							detectionMeans.back().at<double>(1) = offset.y*pas;


							//Might be removed => only for display (and maybe detection with particle filtering)
							result.at<float>(offset) += conf;
							denom+=conf;
							test+=conf*meanSV.at<double>(4);
							angle.at<double>(y/pas, x/pas) = test/denom;

						}
						else
						{
							//still for display
							angle.at<double>(y/pas, x/pas) = -100;
						}
						//display as well
						if (result.at<float>(offset) > currentMax) currentMax=result.at<float>(offset);
					}
				}//End of leaf loop

			}//End of rows loop
		}//End of cols loop

		if (detectionMeans.size()==0)
		{
			std::cout << "no detection" << std::endl;
			continue;
		}

		//Detection with mean shift
		MeanShift ms(detectionMeans);
		cv::Mat mean;
		ms.getMaxCluster(detectionMeans, mean);

		//For display
		result=result/currentMax*255;
		result.convertTo(result, CV_8U);
		angle=(angle+22)/45*255;
		angle.convertTo(angle, CV_8U);
		cv::applyColorMap(angle, angle, cv::COLORMAP_JET);
		cv::imshow("hough", angle);

		//Write in result file
		fileRes << vPaths[imageInd] << " ";//Image
		fileRes << vGroundTruth[imageInd].at(1) << " "<< vGroundTruth[imageInd].at(2) << " "<< vGroundTruth[imageInd].at(3) << " " << vGroundTruth[imageInd].at(4) << " " << vGroundTruth[imageInd].at(5) << " " << vGroundTruth[imageInd].at(6) << " ";//Ground Truth
		fileRes << mean.at<double>(0) << " " << mean.at<double>(1) << " " << mean.at<double>(2) << " " << mean.at<double>(3) << " " << mean.at<double>(4) << " " << mean.at<double>(5) << std::endl;//Detection
		//End of write

		cv::RotatedRect rotatedRect(cv::Point2f(mean.at<double>(0)-128/(2*pas), mean.at<double>(1)-128/(2*pas)), cv::Size2f(128/(2*pas), 128/(2*pas)), 0);
		//std::cout << mean.at<double>(3) << " " << mean.at<double>(4) << " " << mean.at<double>(5) << std::endl;

		cv::Mat imageToSave=result.clone();
		cv::ellipse(imageToSave, rotatedRect, cv::Scalar_<int>(0,255,0), 2);
		rotatedRect.center = rotatedRect.center*pas;
		rotatedRect.size.height = rotatedRect.size.height*pas;
		rotatedRect.size.width = rotatedRect.size.width*pas;

		cv::ellipse(image, rotatedRect, cv::Scalar_<int>(125,255,125), 2);


		cv::applyColorMap(imageToSave, imageToSave, cv::COLORMAP_JET);
		cv::imshow("detection", image);
		cv::imshow("lol", imageToSave);

		cv::waitKey(50);

	}//End of images loop
}
