#pragma once
#include <stdio.h>
#include <vector>


class Layer :
    public std::vector<double>
{
public:
    Layer();
    Layer(int size);
    Layer(size_t size);
    Layer(size_t size, double n);
    ~Layer();

    Layer pow(int toPowOf);
    double sum();
    void reset();
    void sigmoid();
    Layer sigmoidDeriv();   // returns the deriv of the sigmoid function if the object is the result of the sigmoid function

    void print();

    Layer operator+(const Layer &other) const;
    Layer &operator+=(const Layer &other);
    Layer &operator-=(const Layer &other);
    Layer operator-(const Layer &other) const;
    Layer operator*(const Layer &other) const;

    Layer operator+(const double n) const;
    Layer operator-(const double n) const;
    Layer operator*(const double n) const;
};

Layer operator-(const double n, const Layer &l);
