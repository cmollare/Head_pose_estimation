#ifndef RECTFEATURE_H
#define RECTFEATURE_H

#include "Feature.h"

class RectFeature : public Feature
{
    public:
        RectFeature(ForestEnv *forestEnv, ThreadManager* thread, const std::vector<Patch*>& patchs, cv::Rect_<int>& rect1, cv::Rect_<int>& rect2);
		RectFeature(ForestEnv *forestEnv, cv::Rect_<int>& rect1, cv::Rect_<int>& rect2, int threshold);
		RectFeature(ForestEnv *forestEnv, TiXmlElement *node);

        virtual void extractFeature();
		virtual int regression(Patch& patch);
        virtual void saveFeature(TiXmlElement *node);
        virtual void loadFeature(TiXmlElement *node);

		inline virtual cv::Rect_<int>& getRect1()
		{
			return _rect1;
		}

		inline virtual cv::Rect_<int>& getRect2()
		{
			return _rect2;
		}

        inline virtual std::string featureName()
        {
			return "Rect";
        }

    protected:
        cv::Rect_<int> _rect1, _rect2;
};

#endif // RECTFEATURE_H
