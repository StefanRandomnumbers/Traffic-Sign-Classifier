#pragma once
#include <QtCore>
#include <qpixmap.h>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "Contour.h"
#include "NeuralNetwork.h"
#include "Layer.h"

using namespace cv;

class ProcessingManager :
    public QObject
{
    Q_OBJECT

public:
    ProcessingManager();
    ~ProcessingManager();

    void loadImage(QString imagePath);

    void startProcessing();
    void startNNTraining();
    void stopNNTraining();
   

    QPixmap matToQPixmap(Mat im);

    void mergeParentAndChildContours(std::vector< std::vector<Point> > &contours, const std::vector<cv::Vec4i> &hirachy);

    void imageFromContoursRandRGB(const std::vector< std::vector<Point> > &contours, cv::Mat &image);
    cv::Rect imageFromContour(const std::vector<Point> &contour, cv::Mat &image); // also generates the roi rect for convenience

    Layer matToInputLayer(const cv::Mat &image);
    Layer matToInputLayer2(const cv::Mat &image);

    void resettNeuralNetwork(QVector<int> layerInfo, double learnFactor);
    const QList<Contour>* candidateList() const { return &m_candidateList; }
    const NeuralNetwork* neuralNetwork() const { return &m_neuralNetwork; }

signals:
    void sendLogMessage(QString message, QColor color, QPixmap image);

private:

    QString m_imagePath;
    Mat m_image;
    QList<Contour> m_candidateList;

    NeuralNetwork m_neuralNetwork;
    bool m_stopNNTraining;
};


