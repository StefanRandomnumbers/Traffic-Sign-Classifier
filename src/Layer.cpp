#include "Layer.h"
#include <math.h>
#include <iostream>
#include <random>
#include <ctime>



Layer::Layer() :
    std::vector<double>()
{
}

Layer::Layer(int size) :
    std::vector<double>(size)
{
    srand(time(0));

    for (int i = 0; i < size; i++)
    {
        at(i) = ( static_cast<double>(rand()) / (RAND_MAX) ) - 0.5;
    }
}

Layer::Layer(size_t size) :
    std::vector<double>(size)
{
    srand(time(0));

    for (size_t i = 0; i < size; i++)
    {
        std::vector<double>::at(i) = (static_cast<double>(rand()) / (RAND_MAX)) - 0.5;
    }
}

Layer::Layer(size_t size, double n) :
    std::vector<double>(size, n)
{
}

Layer::~Layer()
{
}

Layer Layer::pow(int toPowOf)
{
    Layer result(size());
    for (size_t i = 0; i < size(); i++)
    {
        result.at(i) = std::pow(at(i), toPowOf);
    }

    return result;
}

double Layer::sum()
{
    double sum = 0;
    for (size_t i = 0; i < size(); i++)
    {
        sum += at(i);
    }

    return sum;
}

void Layer::reset()
{
    for (size_t i = 0; i < size(); i++)
    {
        at(i) = 0;
    }
}

void Layer::sigmoid()
{
    for (size_t i = 0; i < size(); i++)
    {
        at(i) = (1 / (1 + exp(-at(i))));
    }
}

// returns the deriv of the sigmoid function if the object is the result of the sigmoid function
Layer Layer::sigmoidDeriv()
{
    Layer result(size(), 0);
    for (size_t i = 0; i < size(); i++)
    {
        result.at(i) = at(i) * (1 - at(i));
    }

    return result;
}

void Layer::print()
{
    using namespace std;

    cout << "\nvector: \n";

    for (size_t i = 0; i < size(); i++)
    {
        cout << " " << at(i);
    }

    cout << "\n";
}

Layer Layer::operator+(const Layer & other) const
{
    Layer result(other.size(), 0);

    if (size() == other.size())
    {
        for (size_t i = 0; i < size(); i++)
        {
            result.at(i) = at(i) + other.at(i);
        }
    }
    else
    {
        std::cout << "\n\nERROR. Layer::operator+(Layer &other). The object and other musst have the same size!\n\n";
    }

    return result;
}

Layer &Layer::operator+=(const Layer & other)
{
    if (size() == other.size())
    {
        for (size_t i = 0; i < size(); i++)
        {
            at(i) += other.at(i);
        }
    }
    else
    {
        std::cout << "\n\nERROR. Layer::operator+=(Layer &other). The object and other musst have the same size!\n\n";
    }

    return *this;
}
Layer &Layer::operator-=(const Layer & other)
{
    if (size() == other.size())
    {
        for (size_t i = 0; i < size(); i++)
        {
            at(i) -= other.at(i);
        }
    }
    else
    {
        std::cout << "\n\nERROR. Layer::operator-=(Layer &other). The object and other musst have the same size!\n\n";
    }

    return *this;
}

Layer Layer::operator-(const Layer & other) const
{
    Layer result(other.size(), 0);

    if (size() == other.size())
    {
        for (size_t i = 0; i < size(); i++)
        {
            result.at(i) = at(i) - other.at(i);
        }
    }
    else
    {
        std::cout << "\n\nERROR. Layer::operator-(Layer &other). The object and other musst have the same size!\n\n";
    }

    return result;
}

Layer Layer::operator*(const Layer & other) const
{
    Layer result(other.size(), 0);

    if (size() == other.size())
    {
        for (size_t i = 0; i < size(); i++)
        {
            result.at(i) = at(i) * other.at(i);
        }
    }
    else
    {
        std::cout << "\n\nERROR. Layer::operator*(Layer &other). The object and other musst have the same size!\n\n";
    }

    return result;
}

Layer Layer::operator+(const double n) const
{
    Layer result(size(), 0);

    for (size_t i = 0; i < size(); i++)
    {
        result.at(i) = at(i) + n;
    }

    return result;
}

Layer Layer::operator-(const double n) const
{
    Layer result(size(), 0);

    for (size_t i = 0; i < size(); i++)
    {
        result.at(i) = at(i) - n;
    }

    return result;
}

Layer Layer::operator*(const double n) const
{
    Layer result(size(), 0);

    for (size_t i = 0; i < size(); i++)
    {
        result.at(i) = at(i) * n;
    }

    return result;
}

Layer operator-(const double n, const Layer & l)
{
    Layer result(l.size(), 0);

    for (size_t i = 0; i < l.size(); i++)
    {
        result.at(i) = n - l.at(i);
    }

    return result;
}
