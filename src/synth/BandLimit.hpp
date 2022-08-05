#pragma once

// The musicdsp.org page at http://www.musicdsp.org/showone.php?id=39 credits
// this to Dave from Muon Software
// Linked source: http://www.musicdsp.org/files/BandLimit.h

// Slightly modified by adding clearing functions

class CAllPassFilter
{
public:

    CAllPassFilter(const double coefficient);
    ~CAllPassFilter();
    double process(double input);

    void clear() { x0 = 0.0; x1 = 0.0; x2 = 0.0; y0 = 0.0; y1 = 0.0; y2 = 0.0; };

private:
    double a;

    double x0;
    double x1;
    double x2;

    double y0;
    double y1;
    double y2;
};


class CAllPassFilterCascade
{
public:
    CAllPassFilterCascade(const double* coefficients, int N);
    ~CAllPassFilterCascade();

    double process(double input);

    void clear() { for (int ii = 0; ii < numfilters; ++ii) { allpassfilter[ii]->clear(); } };

private:
    CAllPassFilter** allpassfilter;
    int numfilters;
};


class CHalfBandFilter
{
public:
    CHalfBandFilter(const int order, const bool steep);
    ~CHalfBandFilter();

    float process(const float input) { return static_cast<float>(process(static_cast<double>(input))); };
    double process(const double input);

    void clear() { filter_a->clear(); filter_b->clear(); };

private:
    CAllPassFilterCascade* filter_a;
    CAllPassFilterCascade* filter_b;
    double oldout;
};
