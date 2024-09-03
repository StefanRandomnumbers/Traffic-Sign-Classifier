#ifndef RESIZER_H
#define RESIZER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>


class Resizer
{
public:
    Resizer();

    // output und input MÜSSEN den selben typ aufweisen!
    static void bilinearResize(const cv::Mat &input, cv::Mat &out, int rows, int cols);
    static void Resizer::bilinearBinaryImResize(const cv::Mat &input, cv::Mat &out, int rows, int cols);
    static void scaleToPixelAmount(const cv::Mat &input, cv::Mat &out, int targetPixelAmount);
    static void pooling4(const cv::Mat &input, cv::Mat &out);

protected:
    static double getScale(double inLength, double outLength);
    static int interpolate(double x, int u, int a, int b);
    static int index(const cv::Mat& m, int i, int j, int k);
};


#endif // RESIZER_H
