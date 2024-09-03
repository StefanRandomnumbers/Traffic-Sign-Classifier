#pragma once
#include <QtCore>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

// is used to process the image and do the things that need to be done to classify it
class ProcessingUnit
{
    

public:
    ProcessingUnit();
    ~ProcessingUnit();



private:
    QString m_imageName;
    Mat m_image;

};

