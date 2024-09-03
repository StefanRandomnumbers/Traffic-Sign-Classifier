#ifndef UTILITY_H
#define UTILITY_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>


class Utility
{
public:
    Utility();

    static std::vector<std::string> getFileNames(const std::string path);
    static std::vector<cv::Mat> loadImagesFromPath(const std::string path);
    static std::vector<cv::Mat> loadImagesFromPathGrayScale(const std::string path);

    static int averageGreyValue(const cv::Mat &im);

    // provides the relative frequency of all values
    static std::vector<double> histogramValue(const cv::Mat &im);
    static void histogramEqualization(const cv::Mat &image, cv::Mat &out, const std::vector<double> &realativeFrequencys);

    static cv::Mat visualiseGreyyHistogram(const cv::Mat &image);
    static cv::Mat visualiseBGRHistogram(const cv::Mat &image);

    static void equalizeLighting(const cv::Mat &image, cv::Mat &out, int desiredValue);

    static void linearPixelScaling(const cv::Mat &image, cv::Mat &out, double c1 = 0, double c2 = 1);
    static void gammaKorrekt(const cv::Mat &image, cv::Mat &out, double gamma = 1);

    static void equidensityse(const cv::Mat &image, cv::Mat &out);
    static void equidensityse(const cv::Mat &image, cv::Mat &out, std::vector<int> thresholds);
    static void equidensityse2(const cv::Mat &image, cv::Mat &out);

    static void convolut(const cv::Mat &image, cv::Mat &out, std::vector< std::vector<double> > kernel);
    static void sobleConvolut(const cv::Mat &image, cv::Mat &out, std::vector< std::vector<double> > kernel);
    static void erode(const cv::Mat &image, cv::Mat &out, std::vector< std::vector<int> > kernel);
    static void dilate(const cv::Mat &image, cv::Mat &out, std::vector< std::vector<int> > kernel);

    static std::vector<cv::Mat> gaussPyramid(const cv::Mat &image);
    static std::vector<cv::Mat> laplacePyramid(const cv::Mat &image);

    static void getColorRangeImage(const cv::Mat &image, cv::Mat &out, cv::Scalar lowerColorRange, cv::Scalar upperColorRange);

    static bool indexInBounds(const cv::Mat &image, int i, int j);

    // implementierung sinngem‰ﬂ von wikipedia : https://en.wikipedia.org/wiki/Flood_fill
    static void floodfill(cv::Mat &image, std::pair<int, int>, uchar searchedColor, uchar newColor);
};

double operator *(const cv::Mat &m, const std::vector< std::vector<double> > &kernel);
int operator *(const cv::Mat &m, const std::vector< std::vector<int> > &kernel);

#endif // UTILITY_H
