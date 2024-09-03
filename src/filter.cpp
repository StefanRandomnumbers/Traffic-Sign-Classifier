#include "filter.h"
#include "utility.h"

using namespace std;
using namespace cv;



Filter::Filter()
{

}

void Filter::equidensiteFilter(const Mat &image, Mat &out)
{
    Mat equiIm(image.rows, image.cols, image.type());;
    Utility::equidensityse(image, equiIm);
    Utility::equidensityse2(equiIm, out);
}

void Filter::meanFilter(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1} };

    Utility::convolut(image, out, kernel);
}

void Filter::lowPass(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {0.9, 0.9, 0.9}, {0.9, 1.8, 0.9}, {0.9, 0.9, 0.9} };

    Utility::convolut(image, out, kernel);
}

void Filter::highPass(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1.1, 1.1, 1.1}, {1.1, 0.2, 1.1}, {1.1, 1.1, 1.1} };

    Utility::convolut(image, out, kernel);
}


void Filter::gausLowPass(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 2, 1}, {2, 4, 2}, {1, 2, 1} };

    // wende den kernel auf das bild an
    Utility::convolut(image, out, kernel);
}


void Filter::sobelFilterX(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };

    Utility::sobleConvolut(image, out, kernel);
}


void Filter::sobelFilterY(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 0, -1}, {2, 0, -2}, {1, 0, -1} };

    Utility::sobleConvolut(image, out, kernel);
}


void Filter::sobelFilterXY(const cv::Mat &image, cv::Mat &out)
{
    Mat sobelX(image.rows, image.cols, image.type());
    Mat sobelY(image.rows, image.cols, image.type());
    sobelFilterX(image, sobelX);
    sobelFilterY(image, sobelY);

    out = Mat(image.rows, image.cols, image.type());

    for (int i = 0; i < out.rows; i++)
    {
        for (int j = 0; j < out.cols; j++)
        {
            int sx = static_cast<int>(sobelX.at<uchar>(i, j)) - 127;
            int sy = static_cast<int>(sobelY.at<uchar>(i, j)) - 127;

            int pixelValue = sqrt(pow(sx, 2) + pow(sy, 2));

            if (pixelValue < 0)
            {
                pixelValue = 0;
            }
            else if(pixelValue > 255)
            {
                pixelValue = 255;
            }

            out.at<uchar>(i, j) = static_cast<uchar>(pixelValue);
        }
    }
}


void Filter::laplaceFilter(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {0, -1, 0}, {-1, 4, -1}, {0, -1, 0} };

    Utility::convolut(image, out, kernel);
}
vector<Mat> Filter::laplaceFilter(const vector<Mat> &image)
{
    vector<Mat> output;

    for (size_t i = 0; i < image.size(); i++)
    {
        Mat helperOut(image[i].rows, image[i].cols, image[i].type());

        Filter::laplaceFilter(image[i], helperOut);
        output.push_back(helperOut);
    }
    return output;
}


vector<Mat> Filter::redFilter(const vector<Mat> &image)
{
    vector<Mat> output;

    for (size_t i = 0; i < image.size(); i++)
    {
        Mat helperOut(image[i].rows, image[i].cols, image[i].type());
        Filter::redFilter(image[i], helperOut);
        output.push_back(helperOut);
    }
    return output;
}

void Filter::redFilter(const Mat &image, Mat &out)
{
    Mat onlyRed1(image.rows, image.cols, CV_8U, Scalar(0));
    Utility::getColorRangeImage(image, onlyRed1, Scalar(0, 40, 40), Scalar(10, 255, 255));     // hue geht in opencv von 0 - 180 nicht bis 360 wie gewöhnlich
    Mat onlyRed2(image.rows, image.cols, CV_8U, Scalar(0));
    Utility::getColorRangeImage(image, onlyRed2, Scalar(170, 40, 40), Scalar(180, 255, 255));  // weil der rot bereich im hsv raum abgeschnitten ist muss man beide bereiche seperat filtern und anschließend addieren
  

    Mat onlyYellow(image.rows, image.cols, CV_8U, Scalar(0));
    Utility::getColorRangeImage(image, onlyYellow, Scalar(20, 40, 40), Scalar(30, 255, 255));

    // die beiden farbbereiche addieren
    Mat h;
    cv::add(onlyRed1, onlyRed2, h);
    cv::add(h, onlyYellow, out);
}



void Filter::edgesN(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 1, 1}, {1, -2, 1}, {-1, -1, -1} };

    Utility::convolut(image, out, kernel);
}
void Filter::edgesS(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {-1, -1, -1}, {1, -2, 1}, {1, 1, 1} };

    Utility::convolut(image, out, kernel);
}
void Filter::edgesO(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {-1, 1, 1}, {-1, -2, 1}, {-1, 1, 1} };

    Utility::convolut(image, out, kernel);
}
void Filter::edgesW(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 1, -1}, {1, -2, -1}, {1, 1, -1} };

    
    Utility::convolut(image, out, kernel);
}

void Filter::edgesNO(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 1, 1}, {-1, -2, 1}, {-1, -1, 1} };

    Utility::convolut(image, out, kernel);
}
void Filter::edgesNW(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, 1, 1}, {1, -2, -1}, {1, -1, -1} };


    Utility::convolut(image, out, kernel);
}
void Filter::edgesSO(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {-1, -1, 1}, {-1, -2, 1}, {1, 1, 1} };

    Utility::convolut(image, out, kernel);
}
void Filter::edgesSW(const Mat &image, Mat &out)
{
    vector<vector<double>> kernel = { {1, -1, -1}, {1, -2, -1}, {1, 1, 1} };

    Utility::convolut(image, out, kernel);
}


void Filter::erode(const Mat &image, Mat &out, int kernelSize)
{
    vector<vector<int>> kernel;
    vector<int> kernelParts;
    for (int i = 0; i < kernelSize; i++)
    {
        kernelParts.push_back(1);
    }
    for (int i = 0; i < kernelSize; i++)
    {
        kernel.push_back(kernelParts);
    }

    Utility::erode(image, out, kernel);
}
void Filter::dilate(const Mat &image, Mat &out, int kernelSize)
{
    vector<vector<int>> kernel;
    vector<int> kernelParts;
    for (int i = 0; i < kernelSize; i++)
    {
        kernelParts.push_back(1);
    }
    for (int i = 0; i < kernelSize; i++)
    {
        kernel.push_back(kernelParts);
    }

    Utility::dilate(image, out, kernel);
}
void Filter::open(const cv::Mat &image, cv::Mat &out, int kernelSize)
{
    Mat eroded;
    Filter::erode(image, eroded, kernelSize);
    Filter::dilate(eroded, out, kernelSize);
}
void Filter::close(const cv::Mat &image, cv::Mat &out, int kernelSize)
{
    Mat dilated;
    Filter::dilate(image, dilated, kernelSize);
    Filter::erode(dilated, out, kernelSize);
}