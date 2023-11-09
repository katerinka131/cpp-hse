#include "ugly.h"
#include "bad.h"

struct SplineImpl {
    std::vector<double> x;
    std::vector<double> y;
    double a;
    double b;

    SplineImpl(const std::vector<double> &x, const std::vector<double> &y, double a, double b)
        : x(x), y(y), a(a), b(b) {
    }

    double Interpolate(double xx) {
        std::vector<double> y2 = y;
        mySplineSnd(&x[0], &y[0], x.size(), a, b, &y2[0]);
        double res;
        mySplintCube(&x[0], &y[0], &y2[0], x.size(), xx, &res);
        return res;
    }
};

Spline::Spline(const std::vector<double> &x, const std::vector<double> &y, double a, double b) {
    impl_ = std::make_shared<SplineImpl>(x, y, a, b);
}

double Spline::Interpolate(double x) {
    return impl_->Interpolate(x);
}