#include "Contour.h"



Contour::Contour(std::vector<cv::Point> points, cv::Rect roi, cv::Mat image) :
    m_points(points),
    m_roi(roi),
    m_image(image)
{

}


Contour::~Contour()
{
}
