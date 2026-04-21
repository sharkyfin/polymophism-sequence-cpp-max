#ifndef MATRIX_COMMON_H
#define MATRIX_COMMON_H

const double MatrixDefaultEpsilon = 1e-12;

inline double AbsValue(double value) {
    return value < 0.0 ? -value : value;
}

inline double SqrtValue(double value) {
    if (value <= 0.0) {
        return 0.0;
    }

    double current = value > 1.0 ? value : 1.0;
    for (int i = 0; i < 60; ++i) {
        current = 0.5 * (current + value / current);
    }

    return current;
}

inline bool IsNearZero(double value, double epsilon = MatrixDefaultEpsilon) {
    return AbsValue(value) <= epsilon;
}

#endif
