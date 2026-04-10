#ifndef MATRIX_ALGORITHMS_H
#define MATRIX_ALGORITHMS_H

#include "exceptions.hpp"
#include "matrix_common.hpp"
#include "rectangular_matrix.hpp"
#include "square_matrix.hpp"

template <class T>
RectangularMatrix<T> Add(const RectangularMatrix<T>& left,
                         const RectangularMatrix<T>& right) {
    if (left.GetRows() != right.GetRows() || left.GetCols() != right.GetCols()) {
        throw InvalidArgumentException("Matrix Add: incompatible matrix sizes");
    }

    RectangularMatrix<T> result(left.GetRows(), left.GetCols(), T());
    for (int row = 0; row < left.GetRows(); ++row) {
        const T* leftRow = left.GetRowPointer(row);
        const T* rightRow = right.GetRowPointer(row);
        T* resultRow = result.GetRowPointer(row);
        for (int col = 0; col < left.GetCols(); ++col) {
            resultRow[col] = leftRow[col] + rightRow[col];
        }
    }

    return result;
}

template <class T>
RectangularMatrix<T> Subtract(const RectangularMatrix<T>& left,
                              const RectangularMatrix<T>& right) {
    if (left.GetRows() != right.GetRows() || left.GetCols() != right.GetCols()) {
        throw InvalidArgumentException("Matrix Subtract: incompatible matrix sizes");
    }

    RectangularMatrix<T> result(left.GetRows(), left.GetCols(), T());
    for (int row = 0; row < left.GetRows(); ++row) {
        const T* leftRow = left.GetRowPointer(row);
        const T* rightRow = right.GetRowPointer(row);
        T* resultRow = result.GetRowPointer(row);
        for (int col = 0; col < left.GetCols(); ++col) {
            resultRow[col] = leftRow[col] - rightRow[col];
        }
    }

    return result;
}

template <class T>
RectangularMatrix<T> MultiplyByScalar(const RectangularMatrix<T>& matrix,
                                      const T& scalar) {
    RectangularMatrix<T> result(matrix.GetRows(), matrix.GetCols(), T());
    for (int row = 0; row < matrix.GetRows(); ++row) {
        const T* matrixRow = matrix.GetRowPointer(row);
        T* resultRow = result.GetRowPointer(row);
        for (int col = 0; col < matrix.GetCols(); ++col) {
            resultRow[col] = matrixRow[col] * scalar;
        }
    }

    return result;
}

template <class T>
RectangularMatrix<T> Multiply(const RectangularMatrix<T>& left,
                              const RectangularMatrix<T>& right) {
    if (left.GetCols() != right.GetRows()) {
        throw InvalidArgumentException("Matrix Multiply: incompatible matrix sizes");
    }

    RectangularMatrix<T> result(left.GetRows(), right.GetCols(), T());
    for (int row = 0; row < left.GetRows(); ++row) {
        const T* leftRow = left.GetRowPointer(row);
        T* resultRow = result.GetRowPointer(row);
        for (int k = 0; k < left.GetCols(); ++k) {
            T value = leftRow[k];
            const T* rightRow = right.GetRowPointer(k);
            for (int col = 0; col < right.GetCols(); ++col) {
                resultRow[col] = resultRow[col] + value * rightRow[col];
            }
        }
    }

    return result;
}

inline Deque<double> Multiply(const RectangularMatrix<double>& matrix,
                              const Deque<double>& vector) {
    if (matrix.GetCols() != vector.GetLength()) {
        throw InvalidArgumentException("Matrix-vector Multiply: incompatible sizes");
    }

    Deque<double> result = Deque<double>::CreateFixed(matrix.GetRows(), 0.0);
    for (int row = 0; row < matrix.GetRows(); ++row) {
        const double* matrixRow = matrix.GetRowPointer(row);
        double sum = 0.0;
        for (int col = 0; col < matrix.GetCols(); ++col) {
            sum += matrixRow[col] * vector.Get(col);
        }
        result.Set(row, sum);
    }

    return result;
}

inline double L2Norm(const Deque<double>& vector) {
    double sum = 0.0;
    for (int i = 0; i < vector.GetLength(); ++i) {
        sum += vector.Get(i) * vector.Get(i);
    }

    return SqrtValue(sum);
}

inline double ResidualNorm(const SquareMatrix<double>& matrix,
                           const Deque<double>& solution,
                           const Deque<double>& rightSide) {
    if (matrix.GetRows() != rightSide.GetLength()) {
        throw InvalidArgumentException("ResidualNorm: incompatible matrix and right side sizes");
    }

    Deque<double> residual = Multiply(matrix, solution);
    if (residual.GetLength() != rightSide.GetLength()) {
        throw InvalidArgumentException("ResidualNorm: incompatible residual size");
    }

    for (int i = 0; i < residual.GetLength(); ++i) {
        residual.Set(i, residual.Get(i) - rightSide.Get(i));
    }

    return L2Norm(residual);
}

inline double MaxAbsDifference(const RectangularMatrix<double>& left,
                               const RectangularMatrix<double>& right) {
    if (left.GetRows() != right.GetRows() || left.GetCols() != right.GetCols()) {
        throw InvalidArgumentException("MaxAbsDifference: incompatible matrix sizes");
    }

    double maximum = 0.0;
    for (int row = 0; row < left.GetRows(); ++row) {
        const double* leftRow = left.GetRowPointer(row);
        const double* rightRow = right.GetRowPointer(row);
        for (int col = 0; col < left.GetCols(); ++col) {
            double difference = AbsValue(leftRow[col] - rightRow[col]);
            if (difference > maximum) {
                maximum = difference;
            }
        }
    }

    return maximum;
}

inline SquareMatrix<double> HilbertMatrix(int size) {
    if (size < 0) {
        throw InvalidArgumentException("HilbertMatrix: negative size");
    }

    SquareMatrix<double> result(size, 0.0);
    for (int row = 0; row < size; ++row) {
        double* resultRow = result.GetRowPointer(row);
        for (int col = 0; col < size; ++col) {
            resultRow[col] = 1.0 / (row + col + 1);
        }
    }

    return result;
}

#endif
