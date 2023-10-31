#include "bad.h"
#include "ugly.h"

struct SplineImpl {
    SplineImpl(const std::vector<double>& x, const std::vector<double>& y, double a, double b)
        : x(x), y(y), y2(y.size()) {
        mySplineSnd(&x[0], &y[0], y.size(), a, b, &y2[0]);
    }
    double Interpolate(double p) {
        double result;
        mySplintCube(&x[0], &y[0], &y2[0], y.size(), p, &result);
        return result;
    };
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> y2;
};

Spline::Spline(const std::vector<double>& x, const std::vector<double>& y, double a, double b)
    : impl_(std::make_shared<SplineImpl>(x, y, a, b)) {
}

double Spline::Interpolate(double x) {
    return impl_->Interpolate(x);
}
