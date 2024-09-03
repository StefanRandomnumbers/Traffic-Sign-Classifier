#pragma once
#include <vector>
#include "Layer.h"

class Weight :
    public std::vector<double>
{
public:
    Weight();
    Weight(int rows, int cols); // all entrys are random
    Weight(int rows, int cols, double n);   // all entrys are eaqual to n
    ~Weight();

    double &at(int row, int col);
    const double &at(int row, int col) const;

    void reset();
    void print();

    int rows() { return m_rows; }
    int cols() { return m_cols; }


    Weight operator+=(const Weight &other);
    Weight operator-=(const Weight &other);

    Layer operator*(const Layer &other) const;

    Weight operator+(const double n) const;
    Weight operator-(const double n) const;
    Weight operator*(const double n) const;

private:
    int m_rows;
    int m_cols;
};
