#include "ProcessingManager.h"
#include "filter.h"
#include "resizer.h"
#include "utility.h"

#include <qdebug.h>


ProcessingManager::ProcessingManager()
{
    m_neuralNetwork = NeuralNetwork({ 1024, 20, 5 }, 0.01);
}



ProcessingManager::~ProcessingManager()
{
}

void ProcessingManager::resettNeuralNetwork(QVector<int> layerInfo, double learnFactor)
{
    m_neuralNetwork = NeuralNetwork(layerInfo, learnFactor);
}
void ProcessingManager::loadImage(QString imagePath)
{
    m_imagePath = imagePath;
    m_image = imread(m_imagePath.toStdString());

    // emit a log message that is clickable to display the attatched image
    emit sendLogMessage("Original Image", QColor(0, 0, 0), matToQPixmap(m_image));
}


void ProcessingManager::startNNTraining()
{
    m_stopNNTraining = false;

    //          input         lables
    QPair<QList<Layer>, QList<Layer>> trainData;
    QPair<QList<Layer>, QList<Layer>> testData;


    Layer label(5, 0);     //   0         1        2        3              4          (index in the label layer)
    QStringList folderNames({ "ohne", "achten", "stop", "vorfahrt", "vorfahrt_str" });
    QString basePathTrainData = "./signData//train//preprocessed//";
    QString basePathTestData = "./signData//test//preprocessed//";

    // generate the train and test data
    for (int labelNr = 0; labelNr < folderNames.size(); labelNr++)
    {
        label[labelNr] = 1;

        vector<Mat> trainImages = Utility::loadImagesFromPathGrayScale((basePathTrainData + folderNames[labelNr]).toStdString());
        // loop through all the image data
        for (size_t i = 0; i < trainImages.size(); i++)
        {
            Layer input = matToInputLayer(trainImages[i]);

            // put it into the train data vector
            trainData.first.push_back(input);
            trainData.second.push_back(label);

        }

        vector<Mat> testImages = Utility::loadImagesFromPathGrayScale((basePathTestData + folderNames[labelNr]).toStdString());
        // loop through all the image data
        for (size_t i = 0; i < testImages.size(); i++)
        {
            Layer input = matToInputLayer(testImages[i]);

            // put it into the train data vector
            testData.first.push_back(input);
            testData.second.push_back(label);

        }

        // reset the lable so it can be used again next iteration
        label = Layer(5, 0);
    }


    // start the nn training with the generated trainData
    QPair<double, double> errors(999, 999);

    while (errors.second > 0.01 && !m_stopNNTraining)
    {
        errors = m_neuralNetwork.train(trainData, testData);
        qDebug() << "Train error: " << errors.first << "  Test error: " << errors.second;
    }
}

void ProcessingManager::stopNNTraining()
{
    m_stopNNTraining = true;
}



Layer ProcessingManager::matToInputLayer(const cv::Mat &image)
{
    Layer toReturn(image.rows*image.cols);
    // loop over every pixel of the image
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            int index = i * image.cols + j;
            int pixel = image.at<uchar>(i, j);
            double layerValue = static_cast<double>(image.at<uchar>(i, j)) / 255;

            toReturn[i*image.cols + j] = static_cast<double>(image.at<uchar>(i, j)) / 255;
        }
    }

    return toReturn;
}
Layer ProcessingManager::matToInputLayer2(const cv::Mat &image)
{
    QList< std::vector< std::vector<double> > > kernals;





    Layer toReturn(image.rows*image.cols);
    // loop over every pixel of the image
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            int index = i * image.cols + j;
            int pixel = image.at<uchar>(i, j);
            double layerValue = static_cast<double>(image.at<uchar>(i, j)) / 255;

            toReturn[i*image.cols + j] = static_cast<double>(image.at<uchar>(i, j)) / 255;
        }
    }

    return toReturn;
}

void ProcessingManager::startProcessing()
{
    if (!m_candidateList.empty())
    {
        m_candidateList.clear();
    }


    qDebug() << "startProcessing()";

    Mat helper = m_image;
    if (m_image.rows*m_image.cols > 1000000)
    {
        Resizer::scaleToPixelAmount(helper, m_image, 1000000);
        qDebug() << "original rows: " << helper.rows << " cols: " << helper.cols << " pixelamount: " << helper.rows*helper.cols;
        qDebug() << "new rows: " << m_image.rows << " cols: " << m_image.cols << " pixelamount: " << m_image.rows*m_image.cols;
    }


    Mat greyIm;
    cvtColor(m_image, greyIm, cv::COLOR_BGR2GRAY);
    emit sendLogMessage("grey", QColor(0, 0, 0), matToQPixmap(greyIm));



    Mat redFilteredIm;
    Filter::redFilter(m_image, redFilteredIm);
    emit sendLogMessage("red and yellow filtered", QColor(0, 0, 0), matToQPixmap(redFilteredIm));



    /*Mat gammaCorrectedOrigin;
    Utility::equalizeLighting(m_image, gammaCorrectedOrigin, 220);
    emit sendLogMessage("gamma corrected origin", QColor(0, 0, 0), matToQPixmap(gammaCorrectedOrigin));

    Mat gammeCorrectedRedFilteredIm;
    Filter::redFilter(gammaCorrectedOrigin, gammeCorrectedRedFilteredIm);
    emit sendLogMessage("gamma corrected red Image", QColor(0, 0, 0), matToQPixmap(gammeCorrectedRedFilteredIm));*/





    // open operator to isolate the objects(conturs) in the image and reduce noise
    Mat imOpen;
    Filter::open(redFilteredIm, imOpen, 3);
    emit sendLogMessage("opened im", QColor(0, 0, 0), matToQPixmap(imOpen));


    Mat edgeIm;
    Filter::laplaceFilter(imOpen, edgeIm);
    emit sendLogMessage("edge im", QColor(0, 0, 0), matToQPixmap(edgeIm));




    // use opencv findcontures to get the pixels that form objects with the neighbour pixels
    std::vector< std::vector<Point> > contours;
    // [Next, Previous, First_Child, Parent]
    std::vector<cv::Vec4i> hirachy;
    cv::findContours(edgeIm, contours, hirachy, RETR_TREE, CHAIN_APPROX_NONE);

    std::vector< std::vector<Point> > contoursExternal;
    cv::findContours(edgeIm, contoursExternal, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    Mat excontourImage(m_image.rows, m_image.cols, CV_8UC3, Scalar(0));
    imageFromContoursRandRGB(contoursExternal, excontourImage);
    emit sendLogMessage("external contour Image", QColor(0, 0, 0), matToQPixmap(excontourImage));

    // make an image to display the pixels that form contoures
    Mat contourImage(m_image.rows, m_image.cols, CV_8UC3, Scalar(0));
    imageFromContoursRandRGB(contours, contourImage);
    emit sendLogMessage("contour Image", QColor(0, 0, 0), matToQPixmap(contourImage));

    mergeParentAndChildContours(contours, hirachy);
    Mat joinedcontourImage(m_image.rows, m_image.cols, CV_8UC3, Scalar(0));
    imageFromContoursRandRGB(contours, joinedcontourImage);
    emit sendLogMessage("joinded contour im", QColor(0, 0, 0), matToQPixmap(joinedcontourImage));


    // the image that will show the resulting rois and their classes assigned by the NN
    Mat resultImage = m_image.clone();

    // turn the vector of points into contours(the class)
    int goodContourCount = 0;
    for (size_t i = 0; i < contours.size(); i++)
    {
        Mat singleFilledContourIm(m_image.rows, m_image.cols, CV_8UC1, Scalar(0));
        drawContours(singleFilledContourIm, contoursExternal, i, Scalar(255), -1);

        Mat contourIm(m_image.rows, m_image.cols, CV_8UC1, Scalar(0));
        // generate a binary image from the contour
        cv::Rect roi = imageFromContour(contours[i], contourIm);


        if (roi.height > 32 && roi.width > 32)
        {
            goodContourCount++;

            // overlap the imOpen with the singleFilledContourIm to get the all the pixels that would form the contour
            Mat originContourWholeIm(m_image.rows, m_image.cols, CV_8UC3, Scalar(0));
            bitwise_and(singleFilledContourIm, greyIm, originContourWholeIm);
            Mat originContour = originContourWholeIm(roi).clone();

            // scall the contour image to the target size 32x32
            Mat targetSizeContourIm;
            Resizer::bilinearBinaryImResize(originContour, targetSizeContourIm, 32, 32);

            Contour c(contours[i], roi, targetSizeContourIm);
            m_candidateList.push_back(c);

            cv::rectangle(joinedcontourImage, roi, Scalar(255, 255, 255), 1);

            emit sendLogMessage("contour", QColor(255, 0, 0), matToQPixmap(contourIm));
            emit sendLogMessage("contour filled whole image", QColor(255, 0, 0), matToQPixmap(singleFilledContourIm));
            emit sendLogMessage("contour origin image whole image", QColor(255, 0, 0), matToQPixmap(originContourWholeIm));
            emit sendLogMessage("contour origin image", QColor(255, 0, 0), matToQPixmap(originContour));



            // put the 32x32 contours into the NN and retreave the output
            Layer input = matToInputLayer(targetSizeContourIm);
            Layer output = m_neuralNetwork.feedforward(input);


            // find out what class the NN put the contour in
            QStringList classes({ "ohne", "achten", "stop", "vorfahrt", "v_str" });

            int indexOfHighest = -1;
            double highest = -1;
            for (int index = 0; index < static_cast<int>(output.size()); index++)
            {
                if (output[index] >= highest)
                {
                    indexOfHighest = index;
                    highest = output[index];
                }
            }

            if (indexOfHighest != 0)
            {
                // draw the roi onto the original image and put the text of the class inside it
                int textThickness = 2;
                Scalar boxBackroundColor(200, 0, 200);

                cv::rectangle(resultImage, roi, boxBackroundColor, 1);

                Size textSize = cv::getTextSize(classes[indexOfHighest].toStdString(), FONT_HERSHEY_SIMPLEX, 1, textThickness, 0);
                cv::rectangle(resultImage, Point(roi.x, roi.y), Point(roi.x + textSize.width, roi.y + textSize.height), boxBackroundColor, -1);
                cv::putText(resultImage, classes[indexOfHighest].toStdString(), Point(roi.x, roi.y + textSize.height), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 2255), textThickness, 8, false);
            }

            emit sendLogMessage("contour origin 32x32: " + classes[indexOfHighest] + "   " + QString::number(output[0]) + "   " + QString::number(output[1]) + "   " + QString::number(output[2]) + "   " + QString::number(output[3]) + "   " + QString::number(output[4]), QColor(200, 0, 0), matToQPixmap(targetSizeContourIm));


        }
    }
    emit sendLogMessage("roi image", QColor(0, 0, 0), matToQPixmap(joinedcontourImage));

    emit sendLogMessage("Result", QColor(0, 200, 0), matToQPixmap(resultImage));


    qDebug() << "finished Processing()";
}

cv::Rect ProcessingManager::imageFromContour(const std::vector<Point> &contour, cv::Mat &image)
{
    int left = INT_MAX;
    int top = INT_MAX;
    int right = 0;
    int bottom = 0;

    cv::Rect roi;

    for (int i = 0; i < contour.size(); i++)
    {
        int x = contour[i].y;
        int y = contour[i].x;

        // find out if it is one of the corners of the roi rect
        left = min(left, x);
        right = max(right, x);
        top = min(top, y);
        bottom = max(bottom, y);
    }
    roi = Rect(top, left, bottom - top + 1, right - left + 1);


    image = Mat(roi.height, roi.width, CV_8UC1, Scalar(0));

    for (int i = 0; i < contour.size(); i++)
    {
        int x = contour[i].y - roi.y;
        int y = contour[i].x - roi.x;
        image.at<uchar>(x, y) = 255;
    }

    return roi;
}

void ProcessingManager::imageFromContoursRandRGB(const std::vector< std::vector<Point> > &contours, cv::Mat &image)
{
    if (image.type() != CV_8UC3)
    {
        image = Mat(image.rows, image.cols, CV_8UC3);
    }

    for (int i = 0; i < contours.size(); i++)
    {
        // generate a random color for the new contour
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;

        for (int j = 0; j < contours[i].size(); j++)
        {
            int x = contours[i][j].y;
            int y = contours[i][j].x;
            image.ptr(x, y)[0] = r;
            image.ptr(x, y)[1] = g;
            image.ptr(x, y)[2] = b;
        }
    }
}

void ProcessingManager::mergeParentAndChildContours(std::vector< std::vector<Point> > &contours, const std::vector<cv::Vec4i> &hirachy)
{
    // [Next, Previous, First_Child, Parent]

    std::vector<int> indexesToDelete;

    for (int i = static_cast<int>(contours.size()) - 1; i >= 0; --i)
    {
        if (hirachy.at(i)[3] != -1)
        {
            // append the points of the contour i to the paretn (hirachy.at(i)[3])
            contours.at(hirachy.at(i)[3]).insert(contours.at(hirachy.at(i)[3]).end(), contours.at(i).begin(), contours.at(i).end());
            indexesToDelete.push_back(i);
        }
    }

    // delete the contours that where appened to another contour
    for (size_t i = 0; i < indexesToDelete.size(); ++i)
    {
        contours.erase(contours.begin() + indexesToDelete.at(i));
    }
}


QPixmap ProcessingManager::matToQPixmap(Mat im)
{
    QPixmap result;



    if (im.type() == CV_8UC3)
    {
        result = QPixmap::fromImage(QImage((unsigned char*)im.data, im.cols, im.rows, im.step, QImage::Format_RGB888).rgbSwapped());
    }
    else if (im.type() == CV_8UC1)
    {
        result = QPixmap::fromImage(QImage((unsigned char*)im.data, im.cols, im.rows, im.step, QImage::Format_Grayscale8).rgbSwapped());
    }
    else if (im.type() == CV_8UC4)
    {
        result = QPixmap::fromImage(QImage((unsigned char*)im.data, im.cols, im.rows, im.step, QImage::Format_ARGB32).rgbSwapped());
        qDebug() << "Note: the image format used to convert the Mat to QImage might not be correct. if it the result doesnt look right try using QImage::Format_RGBA8888 instead.";
    }
    return result;
}