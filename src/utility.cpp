#include "utility.h"
#include <math.h>
#include "resizer.h"
#include <stack>
#include <utility>

#include <sys/types.h>
#include <dirent.h>


using namespace cv;
using namespace std;


Utility::Utility()
{

}


vector<string> Utility::getFileNames(string path)
{
    int typeFile = 32768;
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());

    vector<string> fileNames;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == typeFile)
        {
            fileNames.push_back(path + "/" + entry->d_name);
            cout << entry->d_name << endl;
        }
    }
    closedir(dir);

    /*// löscht überflüssiges. für diesen pfad gibt es als erste datei "." und als zweite ".." (vielleicht ensteht es dadurch das der path der hier verwendet wird 2 ordner beinhält
    fileNames.erase(fileNames.begin());
    fileNames.erase(fileNames.begin());*/

    return fileNames;
}

vector<Mat> Utility::loadImagesFromPath(string path)
{
    vector<string> fileNames = Utility::getFileNames(path);
    vector<Mat> images;

    for (size_t i = 0; i < fileNames.size(); i++)
    {
        images.push_back(imread(fileNames[i]));
    }

    return images;
}
vector<Mat> Utility::loadImagesFromPathGrayScale(string path)
{
    vector<string> fileNames = Utility::getFileNames(path);
    vector<Mat> images;

    for (size_t i = 0; i < fileNames.size(); i++)
    {
        images.push_back(imread(fileNames[i], IMREAD_GRAYSCALE));
    }

    return images;
}


// return den Mittelwert alles pixel des Bildes im.(nur grauwert Bilder)
int Utility::averageGreyValue(const cv::Mat &im)
{
    Mat image = im;
    if (im.channels() > 1 || im.channels() < 1)
    {
        cvtColor(im, image, CV_8UC1);
    }


    int summ = 0;

    for (int i = 0; i < image.rows*image.cols; i++)
    {
        summ += static_cast<int>(image.data[i]);
    }

    return (summ / (image.rows*image.cols));
}


vector<double> Utility::histogramValue(const Mat &im)
{
    // vector der die Anzahlen der auftauchenden grauwerte speichert. initializiert mit 256 einträgen mit dem wert 0
    vector<double> greyValueCount(256, 0);

    if (im.channels() > 1 || im.channels() < 1)
    {
        cout << "Error inside Utility::histogramGreyValue() function. Image must have only one channel!" << endl;
        return greyValueCount;
    }


    long int pixelCount = im.cols*im.rows;

    // durchläuft alle pixel von im
    for (int i = 0; i < pixelCount; i++)
    {
        // addiert die relativen häufigkeiten der einzelnen grauwerte auf. will man die häufigkeit des eines bestimmten grauwertes erfahren, so kann man an den grauwert als index benutzen und in dem vector nachsehen
        greyValueCount.at(im.data[i]) += 1 / (static_cast<double>(pixelCount));
    }

    return greyValueCount;
}


void Utility::histogramEqualization(const cv::Mat &image, cv::Mat &out, const std::vector<double> &realativeFrequencys)
{
    std::vector<double> accumulatedRealativeFrequencys(256, 0);
    // accumulate the relative frequencys
    accumulatedRealativeFrequencys.at(0) = realativeFrequencys.at(0);
    for (size_t i = 1; i < realativeFrequencys.size(); i++)
    {
        accumulatedRealativeFrequencys.at(i) = accumulatedRealativeFrequencys.at(i-1) + realativeFrequencys.at(i);
    }


    out = cv::Mat(image.size(), image.type());

    // loop through the image
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            int pixelValue = static_cast<int>(image.at<uchar>(i, j));
            int nexPixelValue = 255 * accumulatedRealativeFrequencys.at(pixelValue);

            out.at<uchar>(i, j) = static_cast<uchar>(nexPixelValue);
        }
    }
}


Mat Utility::visualiseGreyyHistogram(const Mat &image)
{
    Mat im;
    if (image.channels() < 1 || image.channels() > 1)
    {
        cvtColor(image, im, cv::COLOR_BGR2GRAY);
    }
    else
    {
        im = image;
    }

    // berechne die relative häufigkeit der grauwerte
    vector<double> greyIm = Utility::histogramValue(im);

    // das histogram soll eine breite von 512 haben um zwischen den säulen einnen pixel frei zu lassen und vom typ ein grau bild sein.
    Mat histogram = Mat(400, 513, 0);


    // durchläuft alle relativen grauwert häufigkeiten
    for (size_t i = 0; i < greyIm.size(); i++)
    {
        // berechne die höhe der säule in dem histogram bild
        int pillarHeight = static_cast<int>(greyIm.at(i)*histogram.rows * 2);

        // durchläuft die pixel der spalten(colums) des histogram bildes die eine säule darstellen sollen
        for (int j = 0; j < pillarHeight; j++)
        {
            // jede zweite spalte und pixel startend von unten
            histogram.col(i * 2 + 1).at<uchar>(histogram.rows - 1 - j) = 255;
        }
    }

    return histogram;
}


Mat Utility::visualiseBGRHistogram(const Mat &image)
{
    if (image.channels() < 3 || image.channels() > 3)
    {
        cout << "Error inside Utility::visualiseGreyyHistogram() function. Image does not have 1 colorchannel!" << endl;
        return Mat();
    }


    // teilt image in seine 3 farbkanäle auf
    vector<Mat> bgrPlane;
    split(image, bgrPlane);

    // berechne die relative häufigkeit der farbwerte
    vector<double> b = Utility::histogramValue(bgrPlane.at(0));
    vector<double> g = Utility::histogramValue(bgrPlane.at(1));
    vector<double> r = Utility::histogramValue(bgrPlane.at(2));

    // das histogram soll eine breite von 512 haben um zwischen den säulen einnen pixel frei zu lassen und vom typ ein BGR bild sein.
    Mat histogram = Mat(400, 512, 16);


    // durchläuft alle relativen farbwert häufigkeiten
    for (size_t i = 0; i < b.size(); i++)
    {
        // berechne die höhe der säule in dem histogram bild
        int pillarHeightB = static_cast<int>(b.at(i)*histogram.rows * 30);
        int pillarHeightG = static_cast<int>(g.at(i)*histogram.rows * 30);
        int pillarHeightR = static_cast<int>(r.at(i)*histogram.rows * 30);

        // durchläuft die pixel der spalten(colums) des histogram bildes die eine säule darstellen sollen
        for (int j = 0; j < histogram.rows; j++)
        {
            // jede zweite spalte und pixel startend von unten
            uchar *p = histogram.col(i * 2 + 1).ptr(histogram.rows - 1 - j);

            // blau teil des histograms
            if (pillarHeightB >= j)
            {
                p[0] = 255;
            }
            // grün teil des histograms
            if (pillarHeightG >= j)
            {
                p[1] = 255;
            }
            // rot teil des histograms
            if (pillarHeightR >= j)
            {
                p[2] = 255;
            }
        }
    }

    return histogram;
}

void Utility::equalizeLighting(const cv::Mat &image, cv::Mat &out, int desiredValue)
{
    out = Mat(image.rows, image.cols, image.type());
    Mat greyIm;
    cvtColor(image, greyIm, cv::COLOR_BGR2GRAY);

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            double relativAdjustment = static_cast<double>(desiredValue) / greyIm.at<uchar>(i, j);

            const uchar *p = image.ptr(i, j);

            for (int k = 0; k < image.channels(); k++)
            {
                // den wert des pixels in einen int umwandeln um overflow zu vermeiden
                int pValue = static_cast<int>(p[k]);

                pValue *= relativAdjustment;

                if (pValue < 0)
                {
                    pValue = 0;
                }
                else if (pValue > 255) 
                {
                    pValue = 255;
                }

                out.ptr(i, j)[k] = pValue;
            }
        }
    }
}

void Utility::linearPixelScaling(const cv::Mat &image, Mat &out, double c1, double c2)
{
    // das output bild würde dadurch automatisch schwarz werden (alle pixel werte auf 0)
    if (c2 < 0)
    {
        cout << "Warning inside Utility::linearPixelScaling() function. Negativ value for c2 may not be intended. It will result in a black image." << endl;
    }

    vector<int> lookUP;
    // generiere die look-up tabelle
    for (int i = 0; i < 256; i++)
    {
        lookUP.push_back(c2*i + c1 * c2);
    }

    if (out.rows != image.rows || out.cols != image.cols)
    {
        out = Mat(image.rows, image.cols, image.type(), Scalar(0));
    }

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            // hole den pixel an der stell i, j
            const uchar *p = image.ptr(i, j);

            for (int k = 0; k < image.channels(); k++)
            {
                // den wert des pixels in einen int umwandeln um overflow zu vermeiden
                int pValue = static_cast<int>(p[k]);
                pValue = lookUP.at(pValue);
                if (pValue < 0)
                {
                    pValue = 0;
                }
                else if (pValue > 255) {
                    pValue = 255;
                }

                out.ptr(i, j)[k] = pValue;
            }
        }
    }
}


void Utility::gammaKorrekt(const Mat &image, Mat &out, double gamma)
{
    vector<double> lookUP;
    // generiere die look-up tabelle
    for (int i = 0; i < 256; i++)
    {
        lookUP.push_back(pow((static_cast<double>(1) / 255) * static_cast<double>(i), gamma) * 255);
    }


    if (out.rows != image.rows || out.cols != image.cols)
    {
        out = Mat(image.rows, image.cols, image.type(), Scalar(0));
    }


    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            // hole den pixel an der stell i, j
            const uchar *p = image.ptr(i, j);

            for (int k = 0; k < image.channels(); k++)
            {
                // den wert des pixels in einen int umwandeln um overflow zu vermeiden
                int pValue = static_cast<int>(p[k]);
                pValue = lookUP.at(pValue);
                if (pValue < 0)
                {
                    pValue = 0;
                }
                else if (pValue > 255) {
                    pValue = 255;
                }

                out.ptr(i, j)[k] = pValue;
            }
        }
    }
}



void Utility::equidensityse(const Mat &image, Mat &out)
{
    int h[] = { 63, 127, 255 };
    vector<int> v(begin(h), end(h));

    Utility::equidensityse(image, out, v);
}

// die schwellwerte in dem vector müssen in aufsteigender reihenfolge geordnet sein!
void Utility::equidensityse(const Mat &image, Mat &out, std::vector<int> thresholds)
{
    Mat im;

    if (image.channels() < 1 || image.channels() > 1)
    {
        cvtColor(image, im, COLOR_BGR2GRAY);
    }
    else {
        im = image;
    }


    if (out.rows != image.rows || out.cols != image.cols)
    {
        out = Mat(image.rows, image.cols, image.type(), Scalar(0));
    }

    // durchlaufe alle pixel von im
    for (int i = 0; i < im.rows*im.cols; i++)
    {
        int pValue = static_cast<int>(im.data[i]);

        // durchlaufe den vector und prüfe in welcher range sich der grauwert befindet
        for (size_t j = 0; j < thresholds.size(); j++)
        {
            // finde den neuen wert des pixels
            if (pValue <= thresholds.at(j))
            {
                if (j == 0)
                {
                    out.data[i] = 0;
                    break;
                }
                else if (j == (thresholds.size() - 1)) {
                    out.data[i] = 255;
                    break;
                }
                else {
                    out.data[i] = thresholds.at(j) + 1;
                    break;
                }
            }
        }
    }
}



void Utility::equidensityse2(const Mat &image, Mat &out)
{
    Mat im;

    // wenn image kein grauwert bild ist, wird es in ein umgewandelt und auf im zugewiesen
    if (image.channels() < 1 || image.channels() > 1)
    {
        cvtColor(image, im, COLOR_BGR2GRAY);
    }
    else {
        im = image;
    }


    if (out.rows != image.rows || out.cols != image.cols)
    {
        out = Mat(image.rows, image.cols, image.type(), Scalar(0));
    }


    int factor; // gibt an wie viele der benachbarten pixel zu jedem zeitpunkt in dem bild sind
    int pValue; // grauwert des aktuellen pixels

    int topP;   // grauwert des oberen pixels
    int botP;   // grauwert des unteren pixels
    int rightP; // grauwert des rechten pixels
    int leftP;  // grauwert des linken pixels

    // durchläuft alle pixel des input bildes im
    for (int i = 0; i < im.rows; i++)
    {
        for (int j = 0; j < im.cols; j++)
        {
            // reinizialisierung falls der pixel nicht im bild ist soll er die rechnung nicht beeinflussen
            topP = 0;
            botP = 0;
            rightP = 0;
            leftP = 0;
            factor = 0;

            pValue = static_cast<int>(im.at<uchar>(i, j));

            // fehlerbehandlung am bildrand
            if (i + 1 < im.rows)
            {
                topP = static_cast<int>(im.at<uchar>(i + 1, j));
                factor++;   // es ist ein weiterer pixel im bild also muss für die differenz am ende pValue um eins mehr multipliziert werden
            }
            if (i - 1 >= 0)
            {
                botP = static_cast<int>(im.at<uchar>(i - 1, j));
                factor++;
            }
            if (j + 1 < im.cols)
            {
                rightP = static_cast<int>(im.at<uchar>(i, j + 1));
                factor++;
            }
            if (j - 1 >= 0)
            {
                leftP = static_cast<int>(im.at<uchar>(i, j - 1));
                factor++;
            }

            // gibt aussage ob ein übergang von einem grauwert in einen anderen in der pixelnachbarschafft existiert
            int diff = factor * pValue - topP - botP - rightP - leftP;

            if (diff == 0)
            {
                out.at<uchar>(i, j) = 0;   // die pixel haben alle den selben grauwert => keine kante
            }
            else {
                out.at<uchar>(i, j) = 255; // die pixel hatte nicht alle den selben grauwert => kante gefunden
            }
        }
    }
}


void Utility::erode(const cv::Mat &image, cv::Mat &out, std::vector< std::vector<int> > kernel)
{
    if (image.channels() != 1)
    {
        cout << "Error inside Utility::erode() function. The image needs to have only one channel!" << endl;
    }
    else
    {
        if (out.rows != image.rows || out.cols != image.cols)
        {
            out = Mat(image.rows, image.cols, image.type(), Scalar(0));
        }

        int k = (static_cast<int>(kernel.size()) - 1) / 2;
        int l = (static_cast<int>(kernel[0].size()) - 1) / 2;

        int kernelValueSumm = 0;
        for (int i = 0; i < kernel.size(); i++)
        {
            for (int j = 0; j < kernel.at(0).size(); j++)
            {
                if (kernel[i][j] == 1)
                    kernelValueSumm += 255;
            }
        }

        // durchläuft jeden pixel des input bildes image
        for (int i = k; i < image.rows - k; i++)
        {
            for (int j = l; j < image.cols - l; j++)
            {
                Mat imageMask = image(Range(i - k, i + k + 1), Range(j - l, j + l + 1));

                int lowestValue = 255;
                for (int x = 0; x < kernel.size(); x++)
                {
                    for (int y = 0; y < kernel[0].size(); y++)
                    {
                        int pixelValue = static_cast<int>(imageMask.at<uchar>(x, y)) * kernel[x][y];

                        if (pixelValue < lowestValue)
                        {
                            lowestValue = pixelValue;
                        }
                    }
                }
                out.at<uchar>(i, j) = static_cast<uchar>(lowestValue);
            }
        }
    }
}
void Utility::dilate(const cv::Mat &image, cv::Mat &out, std::vector< std::vector<int> > kernel)
{
    if (image.channels() != 1)
    {
        cout << "Error inside Utility::dilate() function. The image needs to have only one channel!" << endl;
    }
    else
    {
        if (out.rows != image.rows || out.cols != image.cols)
        {
            out = Mat(image.rows, image.cols, image.type(), Scalar(0));
        }

        int k = (static_cast<int>(kernel.size()) - 1) / 2;
        int l = (static_cast<int>(kernel[0].size()) - 1) / 2;

        // durchläuft jeden pixel des input bildes image
        for (int i = k; i < image.rows - k; i++)
        {
            for (int j = l; j < image.cols - l; j++)
            {
                Mat imageMask = image(Range(i - k, i + k + 1), Range(j - l, j + l + 1));

                int highestValue = 0;
                for (int x = 0; x < kernel.size(); x++)
                {
                    for (int y = 0; y < kernel[0].size(); y++)
                    {
                        int pixelValue = static_cast<int>(imageMask.at<uchar>(x, y)) * kernel[x][y];

                        if (pixelValue > highestValue)
                        {
                            highestValue = pixelValue;
                        }
                    }
                }
                out.at<uchar>(i, j) = static_cast<uchar>(highestValue);
            }
        }
    }
}

// faltungs funktion
void Utility::convolut(const Mat &image, Mat &out, vector< vector<double> > kernel)
{
    if (image.channels() != 1)
    {
        cout << "Error inside Utility::convolut() function. The image needs to have only one colorChannel!" << endl;
    }
    else
    {
        if (out.rows != image.rows || out.cols != image.cols)
        {
            out = Mat(image.rows, image.cols, image.type(), Scalar(0));
        }

        int k = (static_cast<int>(kernel.size()) - 1) / 2;
        int l = (static_cast<int>(kernel[0].size()) - 1) / 2;

        double kernelValueSumm = 0;
        for (int i = 0; i < kernel.size(); i++)
        {
            for (int j = 0; j < kernel.at(0).size(); j++)
            {
                kernelValueSumm += kernel[i][j];
            }
        }

        // durchläuft jeden pixel des input bildes image
        for (int i = k; i < image.rows - k; i++)
        {
            for (int j = l; j < image.cols - l; j++)
            {
                // Mat * vector<vector> operator wurde überladen und führt die punkt multiplikation von dem bild ausschnitt mit dem kernel durch
                double pixelValue = (image(Range(i - k, i + k + 1), Range(j - l, j + l + 1)) * kernel);

                if (kernelValueSumm != 0)
                {
                    pixelValue /= kernelValueSumm;
                }

                // overflow vermeiden
                if (pixelValue < 0)
                {
                    pixelValue = 0;
                }
                else if (pixelValue >= 255)
                {
                    pixelValue = 255;
                }

                out.at<uchar>(i, j) = static_cast<uchar>(pixelValue);
            }
        }
    }
}

// the convolution for soble is special cause the negativ values are not supposed to be 0 but instead spread out from 0 to 127 and all the positives from 128 to 255
void Utility::sobleConvolut(const Mat &image, Mat &out, vector< vector<double> > kernel)
{
    if (image.channels() != 1)
    {
        cout << "Error inside Utility::sobleConvolut() function. The image needs to have only one colorChannel!" << endl;
    }
    else
    {
        if (out.rows != image.rows || out.cols != image.cols)
        {
            out = Mat(image.rows, image.cols, image.type(), Scalar(0));
        }

        int k = (static_cast<int>(kernel.size()) - 1) / 2;
        int l = (static_cast<int>(kernel[0].size()) - 1) / 2;

        double kernelValueSumm = 0;
        for (int i = 0; i < kernel.size(); i++)
        {
            for (int j = 0; j < kernel.at(0).size(); j++)
            {
                kernelValueSumm += abs(kernel[i][j]);
            }
        }

        // durchläuft jeden pixel des input bildes image
        for (int i = k; i < image.rows - k; i++)
        {
            for (int j = l; j < image.cols - l; j++)
            {
                // Mat * vector<vector> operator wurde überladen und führt die punkt multiplikation von dem bild ausschnitt mit dem kernel durch
                double pixelValue = (image(Range(i - k, i + k + 1), Range(j - l, j + l + 1)) * kernel);

                if (kernelValueSumm != 0)
                {
                    pixelValue /= kernelValueSumm;
                }
                pixelValue += 127,5;

                out.at<uchar>(i, j) = static_cast<uchar>(pixelValue);
            }
        }
    }
}



double operator *(const Mat &m, const vector< vector<double> > &kernel)
{
    if (m.rows != static_cast<int>(kernel.size()) || m.cols != static_cast<int>(kernel[0].size()))
    {
        cout << "Error inside Utility.cpp operator *(Mat, vector<vector> kernel). Array dimensions must agree." << endl;
        return 0;
    }


    double result = 0;

    for (int i = 0; i < m.rows; i++)
    {
        for (int j = 0; j < m.cols; j++)
        {
            double pixel = static_cast<double>(m.at<uchar>(i, j));  // holt den pixel wert an der aktuellen stelle

            result += pixel * kernel[i][j]; // multipliziert den pixelwert mit dem wet des kernels und addiert es auf das result
        }
    }

    return (result);
}
int operator *(const Mat &m, const vector< vector<int> > &kernel)
{
    if (m.rows != static_cast<int>(kernel.size()) || m.cols != static_cast<int>(kernel[0].size()))
    {
        cout << "Error inside Utility.cpp operator *(Mat, vector<vector> kernel). Array dimensions must agree." << endl;
        return 0;
    }


    int result = 0;

    for (int i = 0; i < m.rows; i++)
    {
        for (int j = 0; j < m.cols; j++)
        {
            int pixel = static_cast<int>(m.at<uchar>(i, j));  // holt den pixel wert an der aktuellen stelle

            result += pixel * kernel[i][j]; // multipliziert den pixelwert mit dem wet des kernels und addiert es auf das result
        }
    }

    return (result);
}









vector<Mat> Utility::gaussPyramid(const Mat &image)
{
    vector<Mat> output;

    if (image.channels() != 1)
    {
        cout << "Error inside Utility::convolut() function. The image needs to have only one colorChannel!" << endl;
        return output;
    }



    output.push_back(image);

    int layer = 0;

    while (output[layer].rows > 1)
    {
        Mat helper;
        Resizer::bilinearResize(output[layer], helper, output[layer].rows / 2, output[layer].cols / 2);
        output.push_back(helper);
        layer++;
    }

    return output;
}



void Utility::getColorRangeImage(const Mat &image, Mat &out, Scalar lowerColorRange, Scalar upperColorRange)
{
    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);


    // die pixel mit der gewünschten farbe werden weiß
    inRange(hsvImage, lowerColorRange, upperColorRange, out);
}



bool Utility::indexInBounds(const Mat &image, int i, int j)
{
    bool toReturn = false;

    if (i >= 0 && j >= 0 && i < image.cols && j < image.rows)
    {
        toReturn = true;
    }

    return toReturn;
}


// implementierung sinngemäß von wikipedia : https://en.wikipedia.org/wiki/Flood_fill
void Utility::floodfill(cv::Mat &image, pair<int, int> startPixel, uchar searchedColor, uchar newColor)
{
    if (searchedColor != newColor)
    {
        std::stack<pair<int, int>> pixelsToCheck;
        pixelsToCheck.push(startPixel);

        while (!pixelsToCheck.empty())
        {
            pair<int, int> currentPixel = pixelsToCheck.top();
            pixelsToCheck.pop();    // deletes the top element

            // check if the currentpixel has the searechedColor
            if (currentPixel.first >= 0 && currentPixel.second >= 0 && currentPixel.first < image.rows && currentPixel.second < image.cols && image.at<uchar>(currentPixel.first, currentPixel.second) == searchedColor)
            {
                // holds the pixels between the west most and the east most pixels while having the same y koord
                std::stack<pair<int, int>> pixelsToCheckWestToEast;
                pixelsToCheckWestToEast.push(currentPixel);

                int westX = currentPixel.first - 1;
                int eastX = currentPixel.first + 1;

                while (eastX < image.rows && image.at<uchar>(eastX, currentPixel.second) == searchedColor)
                {
                    pixelsToCheckWestToEast.push(pair<int, int>(eastX, currentPixel.second));
                    eastX++;
                }


                while (westX >= 0 && image.at<uchar>(westX, currentPixel.second) == searchedColor)
                {
                    pixelsToCheckWestToEast.push(pair<int, int>(westX, currentPixel.second));
                    westX--;
                }


                // go through the west to east stack
                while (!pixelsToCheckWestToEast.empty())
                {
                    pair<int, int> currentPixelWestToEast = pixelsToCheckWestToEast.top();
                    pixelsToCheckWestToEast.pop();    // deletes the top element

                    // give every pixel inside pixelsToCheckWestToEast the new color
                    image.at<uchar>(currentPixelWestToEast.first, currentPixelWestToEast.second) = newColor;

                    // check if the pixels at y + 1 and y - 1 are inside the image and if they have the searchedColor
                    if (currentPixelWestToEast.second + 1 < image.cols && image.at<uchar>(currentPixelWestToEast.first, currentPixelWestToEast.second + 1) == searchedColor)
                    {
                        pixelsToCheck.push(pair<int, int>(currentPixelWestToEast.first, currentPixelWestToEast.second + 1));
                    }
                    if (currentPixelWestToEast.second - 1 >= 0 && image.at<uchar>(currentPixelWestToEast.first, currentPixelWestToEast.second - 1) == searchedColor)
                    {
                        pixelsToCheck.push(pair<int, int>(currentPixelWestToEast.first, currentPixelWestToEast.second - 1));
                    }
                }
            }
        }
    }
    else
    {
        cout << "Warning inside Utility::floodfill() function. searchColor and newColor have to be a different color!" << endl;
    }
}