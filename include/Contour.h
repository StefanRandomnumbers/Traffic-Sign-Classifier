#pragma once
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class Contour
{
public:
    Contour(std::vector<cv::Point> points, cv::Rect roi, cv::Mat image);
    ~Contour();

    const cv::Mat image() const { return m_image; }

private:
    std::vector<cv::Point> m_points;

    // provides info on the original location in the image
    cv::Rect m_roi;

    // a 32x32 pixel image of area where the contour is
    cv::Mat m_image;
};

