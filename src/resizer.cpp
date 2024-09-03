#include "resizer.h"
using namespace cv;
using namespace std;


Resizer::Resizer()
{

}


void Resizer::bilinearResize(const cv::Mat &input, Mat &out, int rows, int cols)
{
    out = Mat(rows, cols, input.type());

    double scaleY = getScale(input.rows, out.rows);
    double scaleX = getScale(input.cols, out.cols);

    int A, B, C, D, E, F, G;


    for(long int i = 0; i < out.cols; i++)
    {
        for(long int j = 0; j < out.rows; j++)
        {

            double x = scaleX*i;    // koordinate(breite; cols; i) des Targetpixels in dem Source Bild
            double y = scaleY*j;    // koordinate(höhe; rows; j) des Targetpixels in dem Source Bild
            long int u = static_cast<int>(x); // index entlang der i achxe des inputs
            long int v = static_cast<int>(y); // index entlang der j achse des inputs

            // schleife über die colorchannels eines jeden Pixels
            for(int k = 0; k < out.channels(); k++)
            {
                // A is der Pixel oben links und wird immer im Bild sein
                A = static_cast<int>(input.data[index(input, u, v, k)]);

                // folgende If Anweisungen behandeln die Fälle am Bildrand
                if(i + 1 >= out.cols && j + 1 >= out.rows)
                {
                    G = A;

                } else if(j + 1 >= out.rows) {
                    B = static_cast<int>(input.data[index(input, u+1, v, k)]);
                    G = interpolate(x, u, A, B);

                } else if(i + 1 >= out.cols) {
                    C = static_cast<int>(input.data[index(input, u, v+1, k)]);
                    G = interpolate(y, v, A, C);

                } else {
                    B = static_cast<int>(input.data[index(input, u+1, v  , k)]);
                    C = static_cast<int>(input.data[index(input, u  , v+1, k)]);
                    D = static_cast<int>(input.data[index(input, u+1, v+1, k)]);

                    E = interpolate(x, u, A, B);
                    F = interpolate(x, u, C, D);

                    G = interpolate(y, v, E, F);
                }

                //cout << "G = " << G << "   i: " << i << " j: " << j << endl;
                out.data[index(out, i, j, k)] = static_cast<uchar>(G);
            }
        }
    }
}

void Resizer::bilinearBinaryImResize(const cv::Mat &input, Mat &out, int rows, int cols)
{
    if (input.type() != CV_8UC1)
    {
        cout << "Error inside Resizer::bilinearBinaryImResize() function. Input image isnt binary!" << endl;
    }
    else
    {
        out = Mat(rows, cols, input.type());

        double scaleY = getScale(input.rows, out.rows);
        double scaleX = getScale(input.cols, out.cols);

        int A, B, C, D, E, F, G;


        for (long int i = 0; i < out.cols; i++)
        {
            for (long int j = 0; j < out.rows; j++)
            {

                double x = scaleX * i;    // koordinate(breite; cols; i) des Targetpixels in dem Source Bild
                double y = scaleY * j;    // koordinate(höhe; rows; j) des Targetpixels in dem Source Bild
                long int u = static_cast<int>(x); // index entlang der i achxe des inputs
                long int v = static_cast<int>(y); // index entlang der j achse des inputs

                // A is der Pixel oben links und wird immer im Bild sein
                A = static_cast<int>(input.data[index(input, u, v, 0)]);

                // folgende If Anweisungen behandeln die Fälle am Bildrand
                if (i + 1 >= out.cols && j + 1 >= out.rows)
                {
                    G = A;

                }
                else if (j + 1 >= out.rows) {
                    B = static_cast<int>(input.data[index(input, u + 1, v, 0)]);
                    G = max(A, B);

                }
                else if (i + 1 >= out.cols) {
                    C = static_cast<int>(input.data[index(input, u, v + 1, 0)]);
                    G = max(A, C);

                }
                else {
                    B = static_cast<int>(input.data[index(input, u + 1, v, 0)]);
                    C = static_cast<int>(input.data[index(input, u, v + 1, 0)]);
                    D = static_cast<int>(input.data[index(input, u + 1, v + 1, 0)]);

                    E = max(A, B);
                    F = max(C, D);

                    G = max(E, F);
                }

                out.data[index(out, i, j, 0)] = static_cast<uchar>(G);
                
            }
        }
    }
}

void Resizer::scaleToPixelAmount(const cv::Mat &input, cv::Mat &out, int targetPixelAmount)
{
    double pixelAmountRatio = sqrt(static_cast<double>(targetPixelAmount) / (input.rows*input.cols));

    int newRows = static_cast<int>(input.rows * pixelAmountRatio);
    int newCols = static_cast<int>(input.cols * pixelAmountRatio);

    Resizer::bilinearResize(input, out, newRows, newCols);
}

void Resizer::pooling4(const cv::Mat &input, cv::Mat &out)
{
    if (input.type() != CV_8UC1)
    {
        cout << "Error inside Resizer::bilinearBinaryImResize() function. Input image isnt binary!" << endl;
    }
    else
    {
        int newRows = input.rows / 2;
        int newCols = input.cols / 2;

        if (input.rows % 2 != 0)
        {
            newRows = static_cast<int>(input.rows / 2) + 1;
        }
        if (input.cols % 2 != 0)
        {
            newCols = static_cast<int>(input.cols / 2) + 1;
        }

        out = Mat(newRows, newCols, input.type());

        int A, B, C, D, E, F, G;


        for (long int i = 0; i < out.cols; i++)
        {
            for (long int j = 0; j < out.rows; j++)
            {
                int u = i * 2;
                int v = j * 2;

                A = static_cast<int>(input.data[index(input, u, v, 0)]);

                if (i + 1 >= out.cols && j + 1 >= out.rows)
                {
                    G = A;

                }
                else if (j + 1 >= out.rows) {
                    B = static_cast<int>(input.data[index(input, u + 1, v, 0)]);
                    G = max(A, B);

                }
                else if (i + 1 >= out.cols) {
                    C = static_cast<int>(input.data[index(input, u, v + 1, 0)]);
                    G = max(A, C);

                }
                else {
                    B = static_cast<int>(input.data[index(input, u + 1, v, 0)]);
                    C = static_cast<int>(input.data[index(input, u, v + 1, 0)]);
                    D = static_cast<int>(input.data[index(input, u + 1, v + 1, 0)]);

                    E = max(A, B);
                    F = max(C, D);

                    G = max(E, F);
                }

                out.data[index(out, i, j, 0)] = static_cast<uchar>(G);

                
            }
        }
    }
}


// i = index in der Breite; j = index in der höhe; k = colorchannel des Pixels
int Resizer::index(const Mat &m, int i, int j, int k)
{
    if(i < 0 || i >= m.cols || j < 0 || j >= m.rows || k < 0 || k >= m.channels())
    {
        cout << "Error inside Resizer::index() funktion. Index out of range!" << endl;
        return 0;
    }
    return (i*m.channels() + j*m.channels()*m.cols + k);
}



int Resizer::interpolate(double x, int u, int a, int b)
{
    // brechnet den Wert für den Pixel des Tragets
    int result = (a + ((x - u)*(b - a)));
                      //bestimmt die gewichtung für den Pixelwert von b

    if(result < 0 || result > 255)
    {
        cout << "Error inside Resizer::interpolate() funktion. Value out of bounds(overflow)!" << endl;
    }

    return result;
}



double Resizer::getScale(double inLength, double outLength)
{
    if((static_cast<double>(outLength) - 1.0) <= 0)
    {
        return 0.5;
    }
    return (static_cast<double>(inLength) - 1.0) / (static_cast<double>(outLength) - 1.0);
}
