#ifndef FILTER_H
#define FILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>


class Filter
{
public:
    Filter();

    static void equidensiteFilter(const cv::Mat &image, cv::Mat &out);
    static void meanFilter(const cv::Mat &image, cv::Mat &out);
    static void highPass(const cv::Mat &image, cv::Mat &out);
    static void lowPass(const cv::Mat &image, cv::Mat &out);
    static void gausLowPass(const cv::Mat &image, cv::Mat &out);
    static void sobelFilterX(const cv::Mat &image, cv::Mat &out);
    static void sobelFilterY(const cv::Mat &image, cv::Mat &out);
    static void sobelFilterXY(const cv::Mat &image, cv::Mat &out);
    static std::vector<cv::Mat> laplaceFilter(const std::vector<cv::Mat> &image);
    static void laplaceFilter(const cv::Mat &image, cv::Mat &out);

    static void erode(const cv::Mat &image, cv::Mat &out, int kernelSize);
    static void dilate(const cv::Mat &image, cv::Mat &out, int kernelSize);
    static void open(const cv::Mat &image, cv::Mat &out, int kernelSize);
    static void close(const cv::Mat &image, cv::Mat &out, int kernelSize);


    static void edgesN(const cv::Mat &image, cv::Mat &out);
    static void edgesS(const cv::Mat &image, cv::Mat &out);
    static void edgesO(const cv::Mat &image, cv::Mat &out);
    static void edgesW(const cv::Mat &image, cv::Mat &out);

    static void edgesNO(const cv::Mat &image, cv::Mat &out);
    static void edgesNW(const cv::Mat &image, cv::Mat &out);
    static void edgesSO(const cv::Mat &image, cv::Mat &out);
    static void edgesSW(const cv::Mat &image, cv::Mat &out);


    static void redFilter(const cv::Mat &image, cv::Mat &out);
    static std::vector<cv::Mat> redFilter(const std::vector<cv::Mat> &image);
};

#endif // FILTER_H
