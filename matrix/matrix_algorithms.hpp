#ifndef MATRIX_ALGORITHMS_H
#define MATRIX_ALGORITHMS_H

#include "core/exceptions.hpp"
#include "matrix/matrix_common.hpp"
#include "matrix/rectangular_matrix.hpp"
#include "matrix/square_matrix.hpp"
#include "matrix/vector.hpp"

template <class T>
RectangularMatrix<T> Add(const RectangularMatrix<T>& left,
                         const RectangularMatrix<T>& right) {
    if (left.GetRows() != right.GetRows() || left.GetCols() != right.GetCols()) {
        throw InvalidArgumentException("Matrix Add: incompatible matrix sizes");
    }

    RectangularMatrix<T> result(left.GetRows(), left.GetCols(), T());
    for (int row = 0; row < left.GetRows(); ++row) {
        const T* leftRow = left[row];
        const T* rightRow = right[row];
        T* resultRow = result[row];
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
        const T* leftRow = left[row];
        const T* rightRow = right[row];
        T* resultRow = result[row];
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
        const T* matrixRow = matrix[row];
        T* resultRow = result[row];
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
        const T* leftRow = left[row];
        T* resultRow = result[row];
        for (int k = 0; k < left.GetCols(); ++k) {
            T value = leftRow[k];
            const T* rightRow = right[k];
            for (int col = 0; col < right.GetCols(); ++col) {
                resultRow[col] = resultRow[col] + value * rightRow[col];
            }
        }
    }

    return result;
}

template <class T>
Vector<T> Multiply(const RectangularMatrix<T>& matrix,
                   const Vector<T>& vector) {
    if (matrix.GetCols() != vector.GetSize()) {
        throw InvalidArgumentException("Matrix-vector Multiply: incompatible sizes");
    }

    Vector<T> result(matrix.GetRows(), T());
    for (int row = 0; row < matrix.GetRows(); ++row) {
        const T* matrixRow = matrix[row];
        T sum = T();
        for (int col = 0; col < matrix.GetCols(); ++col) {
            sum += matrixRow[col] * vector[col];
        }
        result[row] = sum;
    }

    return result;
}

inline double L2Norm(const Vector<double>& vector) {
    double sum = 0.0;
    for (int i = 0; i < vector.GetSize(); ++i) {
        sum += vector[i] * vector[i];
    }

    return SqrtValue(sum);
}

inline double ResidualNorm(const SquareMatrix<double>& matrix,
                           const Vector<double>& solution,
                           const Vector<double>& rightSide) {
    if (matrix.GetRows() != rightSide.GetSize()) {
        throw InvalidArgumentException("ResidualNorm: incompatible matrix and right side sizes");
    }

    Vector<double> residual = Multiply(matrix, solution);
    if (residual.GetSize() != rightSide.GetSize()) {
        throw InvalidArgumentException("ResidualNorm: incompatible residual size");
    }

    for (int i = 0; i < residual.GetSize(); ++i) {
        residual[i] = residual[i] - rightSide[i];
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
        const double* leftRow = left[row];
        const double* rightRow = right[row];
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
        for (int col = 0; col < size; ++col) {
            result[row][col] = 1.0 / (row + col + 1);
        }
    }

    return result;
}

#endif
