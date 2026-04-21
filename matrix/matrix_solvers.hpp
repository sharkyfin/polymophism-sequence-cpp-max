#ifndef MATRIX_SOLVERS_H
#define MATRIX_SOLVERS_H

#include "matrix/matrix_algorithms.hpp"
#include "matrix/matrix_common.hpp"
#include "matrix/special_matrices.hpp"

struct LUDecompositionResult {
    TriangularMatrix<double> L;
    TriangularMatrix<double> U;

    LUDecompositionResult() : L(), U() {}

    LUDecompositionResult(const TriangularMatrix<double>& lower,
                          const TriangularMatrix<double>& upper)
        : L(lower), U(upper) {}
};

inline Vector<double> CreateDoubleVector(int size, double value = 0.0) {
    if (size < 0) {
        throw InvalidArgumentException("CreateDoubleVector: negative size");
    }

    return Vector<double>(size, value);
}

inline void ValidateVectorSize(const Vector<double>& vector, int expectedSize,
                               const char* message) {
    if (vector.GetSize() != expectedSize) {
        throw InvalidArgumentException(message);
    }
}

inline void ValidateLinearSystem(const SquareMatrix<double>& matrix,
                                 const Vector<double>& rightSide) {
    if (matrix.GetSize() <= 0) {
        throw InvalidArgumentException("Matrix solver: matrix size must be positive");
    }

    ValidateVectorSize(rightSide, matrix.GetSize(),
                       "Matrix solver: matrix and right side sizes do not match");
}

inline Vector<double> SolveDiagonal(const DiagonalMatrix<double>& matrix,
                                    const Vector<double>& rightSide,
                                   double epsilon = MatrixDefaultEpsilon) {
    int size = matrix.GetSize();
    if (size <= 0) {
        throw InvalidArgumentException("SolveDiagonal: matrix size must be positive");
    }
    ValidateVectorSize(rightSide, size, "SolveDiagonal: matrix and right side sizes do not match");

    Vector<double> result = CreateDoubleVector(size, 0.0);
    for (int i = 0; i < size; ++i) {
        double diagonal = matrix.Get(i, i);
        if (IsNearZero(diagonal, epsilon)) {
            throw CalculationException("SolveDiagonal: zero or too small diagonal element");
        }
        result[i] = rightSide[i] / diagonal;
    }

    return result;
}

inline Vector<double> ForwardSubstitution(const TriangularMatrix<double>& lower,
                                          const Vector<double>& rightSide,
                                         double epsilon = MatrixDefaultEpsilon) {
    if (lower.GetKind() != TriangleKind::Lower) {
        throw InvalidArgumentException("ForwardSubstitution: matrix must be lower triangular");
    }

    int size = lower.GetSize();
    if (size <= 0) {
        throw InvalidArgumentException("ForwardSubstitution: matrix size must be positive");
    }
    ValidateVectorSize(rightSide, size,
                       "ForwardSubstitution: matrix and right side sizes do not match");

    Vector<double> y = CreateDoubleVector(size, 0.0);
    for (int row = 0; row < size; ++row) {
        const double* lowerRow = lower[row];
        double sum = rightSide[row];
        for (int col = 0; col < row; ++col) {
            sum -= lowerRow[col] * y[col];
        }

        double diagonal = lowerRow[row];
        if (IsNearZero(diagonal, epsilon)) {
            throw CalculationException("ForwardSubstitution: zero or too small diagonal element");
        }

        y[row] = sum / diagonal;
    }

    return y;
}

inline Vector<double> BackwardSubstitution(const TriangularMatrix<double>& upper,
                                           const Vector<double>& rightSide,
                                          double epsilon = MatrixDefaultEpsilon) {
    if (upper.GetKind() != TriangleKind::Upper) {
        throw InvalidArgumentException("BackwardSubstitution: matrix must be upper triangular");
    }

    int size = upper.GetSize();
    if (size <= 0) {
        throw InvalidArgumentException("BackwardSubstitution: matrix size must be positive");
    }
    ValidateVectorSize(rightSide, size,
                       "BackwardSubstitution: matrix and right side sizes do not match");

    Vector<double> x = CreateDoubleVector(size, 0.0);
    for (int row = size - 1; row >= 0; --row) {
        const double* upperRow = upper[row];
        double sum = rightSide[row];
        for (int col = row + 1; col < size; ++col) {
            sum -= upperRow[col] * x[col];
        }

        double diagonal = upperRow[row];
        if (IsNearZero(diagonal, epsilon)) {
            throw CalculationException("BackwardSubstitution: zero or too small diagonal element");
        }

        x[row] = sum / diagonal;
    }

    return x;
}

inline LUDecompositionResult LUDecompose(const SquareMatrix<double>& matrix,
                                         double epsilon = MatrixDefaultEpsilon) {
    int size = matrix.GetSize();
    if (size <= 0) {
        throw InvalidArgumentException("LUDecompose: matrix size must be positive");
    }

    TriangularMatrix<double> lower(size, TriangleKind::Lower);
    TriangularMatrix<double> upper(size, TriangleKind::Upper);

    for (int i = 0; i < size; ++i) {
        lower.Set(i, i, 1.0);
    }

    for (int i = 0; i < size; ++i) {
        const double* matrixPivotRow = matrix[i];
        const double* lowerPivotRow = lower[i];
        for (int col = i; col < size; ++col) {
            double sum = 0.0;
            for (int k = 0; k < i; ++k) {
                const double* upperRow = upper[k];
                sum += lowerPivotRow[k] * upperRow[col];
            }
            upper.Set(i, col, matrixPivotRow[col] - sum);
        }

        double pivot = upper[i][i];
        if (IsNearZero(pivot, epsilon)) {
            throw CalculationException("LUDecompose: zero or too small pivot");
        }

        for (int row = i + 1; row < size; ++row) {
            const double* matrixRow = matrix[row];
            const double* lowerRow = lower[row];
            double sum = 0.0;
            for (int k = 0; k < i; ++k) {
                const double* upperRow = upper[k];
                sum += lowerRow[k] * upperRow[i];
            }
            lower.Set(row, i, (matrixRow[i] - sum) / pivot);
        }
    }

    return LUDecompositionResult(lower, upper);
}

inline Vector<double> SolveViaLU(const LUDecompositionResult& lu,
                                 const Vector<double>& rightSide,
                                double epsilon = MatrixDefaultEpsilon) {
    if (lu.L.GetSize() != lu.U.GetSize()) {
        throw InvalidArgumentException("SolveViaLU: incompatible L and U sizes");
    }

    Vector<double> y = ForwardSubstitution(lu.L, rightSide, epsilon);
    return BackwardSubstitution(lu.U, y, epsilon);
}

inline Vector<double> SolveViaLU(const SquareMatrix<double>& matrix,
                                 const Vector<double>& rightSide,
                                double epsilon = MatrixDefaultEpsilon) {
    ValidateLinearSystem(matrix, rightSide);
    return SolveViaLU(LUDecompose(matrix, epsilon), rightSide, epsilon);
}

inline Vector<double> SolveGaussNoPivot(const SquareMatrix<double>& matrix,
                                        const Vector<double>& rightSide,
                                       double epsilon = MatrixDefaultEpsilon) {
    ValidateLinearSystem(matrix, rightSide);

    SquareMatrix<double> working = matrix;
    Vector<double> b = rightSide;
    int size = working.GetSize();

    for (int pivot = 0; pivot < size; ++pivot) {
        double* pivotRow = working[pivot];
        double pivotValue = pivotRow[pivot];
        if (IsNearZero(pivotValue, epsilon)) {
            throw CalculationException("SolveGaussNoPivot: zero or too small pivot");
        }

        for (int row = pivot + 1; row < size; ++row) {
            double* workingRow = working[row];
            double factor = workingRow[pivot] / pivotValue;
            workingRow[pivot] = 0.0;

            for (int col = pivot + 1; col < size; ++col) {
                workingRow[col] = workingRow[col] - factor * pivotRow[col];
            }

            b[row] = b[row] - factor * b[pivot];
        }
    }

    TriangularMatrix<double> upper(working, TriangleKind::Upper);
    return BackwardSubstitution(upper, b, epsilon);
}

inline Vector<double> SolveGaussPartialPivot(const SquareMatrix<double>& matrix,
                                             const Vector<double>& rightSide,
                                            double epsilon = MatrixDefaultEpsilon) {
    ValidateLinearSystem(matrix, rightSide);

    SquareMatrix<double> working = matrix;
    Vector<double> b = rightSide;
    int size = working.GetSize();

    for (int pivot = 0; pivot < size; ++pivot) {
        int bestRow = pivot;
        double bestValue = AbsValue(working[pivot][pivot]);

        for (int row = pivot + 1; row < size; ++row) {
            const double* workingRow = working[row];
            double candidate = AbsValue(workingRow[pivot]);
            if (candidate > bestValue) {
                bestValue = candidate;
                bestRow = row;
            }
        }

        if (bestValue <= epsilon) {
            throw CalculationException("SolveGaussPartialPivot: singular or ill-conditioned system");
        }

        working.SwapRows(pivot, bestRow);
        b.SwapElements(pivot, bestRow);

        double* pivotRow = working[pivot];
        double pivotValue = pivotRow[pivot];
        for (int row = pivot + 1; row < size; ++row) {
            double* workingRow = working[row];
            double factor = workingRow[pivot] / pivotValue;
            workingRow[pivot] = 0.0;

            for (int col = pivot + 1; col < size; ++col) {
                workingRow[col] = workingRow[col] - factor * pivotRow[col];
            }

            b[row] = b[row] - factor * b[pivot];
        }
    }

    TriangularMatrix<double> upper(working, TriangleKind::Upper);
    return BackwardSubstitution(upper, b, epsilon);
}

#endif
