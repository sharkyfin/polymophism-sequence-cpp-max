#ifndef MATRIX_SOLVERS_H
#define MATRIX_SOLVERS_H

#include "matrix_algorithms.hpp"
#include "matrix_common.hpp"
#include "special_matrices.hpp"

struct LUDecompositionResult {
    TriangularMatrix<double> L;
    TriangularMatrix<double> U;

    LUDecompositionResult() : L(), U() {}

    LUDecompositionResult(const TriangularMatrix<double>& lower,
                          const TriangularMatrix<double>& upper)
        : L(lower), U(upper) {}
};

inline Deque<double> CreateDoubleDeque(int size, double value = 0.0) {
    if (size < 0) {
        throw InvalidArgumentException("CreateDoubleDeque: negative size");
    }

    return Deque<double>::CreateFixed(size, value);
}

inline void ValidateVectorSize(const Deque<double>& vector, int expectedSize,
                               const char* message) {
    if (vector.GetLength() != expectedSize) {
        throw InvalidArgumentException(message);
    }
}

inline void ValidateLinearSystem(const SquareMatrix<double>& matrix,
                                 const Deque<double>& rightSide) {
    if (matrix.GetSize() <= 0) {
        throw InvalidArgumentException("Matrix solver: matrix size must be positive");
    }

    ValidateVectorSize(rightSide, matrix.GetSize(),
                       "Matrix solver: matrix and right side sizes do not match");
}

inline Deque<double> SolveDiagonal(const DiagonalMatrix<double>& matrix,
                                   const Deque<double>& rightSide,
                                   double epsilon = MatrixDefaultEpsilon) {
    int size = matrix.GetSize();
    if (size <= 0) {
        throw InvalidArgumentException("SolveDiagonal: matrix size must be positive");
    }
    ValidateVectorSize(rightSide, size, "SolveDiagonal: matrix and right side sizes do not match");

    Deque<double> result = CreateDoubleDeque(size, 0.0);
    for (int i = 0; i < size; ++i) {
        double diagonal = matrix.Get(i, i);
        if (IsNearZero(diagonal, epsilon)) {
            throw CalculationException("SolveDiagonal: zero or too small diagonal element");
        }
        result.Set(i, rightSide.Get(i) / diagonal);
    }

    return result;
}

inline Deque<double> ForwardSubstitution(const TriangularMatrix<double>& lower,
                                         const Deque<double>& rightSide,
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

    Deque<double> y = CreateDoubleDeque(size, 0.0);
    for (int row = 0; row < size; ++row) {
        const double* lowerRow = lower.GetRowPointer(row);
        double sum = rightSide.Get(row);
        for (int col = 0; col < row; ++col) {
            sum -= lowerRow[col] * y.Get(col);
        }

        double diagonal = lowerRow[row];
        if (IsNearZero(diagonal, epsilon)) {
            throw CalculationException("ForwardSubstitution: zero or too small diagonal element");
        }

        y.Set(row, sum / diagonal);
    }

    return y;
}

inline Deque<double> BackwardSubstitution(const TriangularMatrix<double>& upper,
                                          const Deque<double>& rightSide,
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

    Deque<double> x = CreateDoubleDeque(size, 0.0);
    for (int row = size - 1; row >= 0; --row) {
        const double* upperRow = upper.GetRowPointer(row);
        double sum = rightSide.Get(row);
        for (int col = row + 1; col < size; ++col) {
            sum -= upperRow[col] * x.Get(col);
        }

        double diagonal = upperRow[row];
        if (IsNearZero(diagonal, epsilon)) {
            throw CalculationException("BackwardSubstitution: zero or too small diagonal element");
        }

        x.Set(row, sum / diagonal);
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
        lower.SetInsideTriangle(i, i, 1.0);
    }

    for (int i = 0; i < size; ++i) {
        const double* matrixRow = matrix.GetRowPointer(i);
        const double* lowerCurrentRow = lower.GetRowPointer(i);
        for (int col = i; col < size; ++col) {
            double sum = 0.0;
            for (int k = 0; k < i; ++k) {
                sum += lowerCurrentRow[k] * upper.GetRowPointer(k)[col];
            }
            upper.SetInsideTriangle(i, col, matrixRow[col] - sum);
        }

        const double* upperCurrentRow = upper.GetRowPointer(i);
        double pivot = upperCurrentRow[i];
        if (IsNearZero(pivot, epsilon)) {
            throw CalculationException("LUDecompose: zero or too small pivot");
        }

        for (int row = i + 1; row < size; ++row) {
            const double* matrixNextRow = matrix.GetRowPointer(row);
            const double* lowerNextRow = lower.GetRowPointer(row);
            double sum = 0.0;
            for (int k = 0; k < i; ++k) {
                sum += lowerNextRow[k] * upper.GetRowPointer(k)[i];
            }
            lower.SetInsideTriangle(row, i, (matrixNextRow[i] - sum) / pivot);
        }
    }

    return LUDecompositionResult(lower, upper);
}

inline Deque<double> SolveViaLU(const LUDecompositionResult& lu,
                                const Deque<double>& rightSide,
                                double epsilon = MatrixDefaultEpsilon) {
    if (lu.L.GetSize() != lu.U.GetSize()) {
        throw InvalidArgumentException("SolveViaLU: incompatible L and U sizes");
    }

    Deque<double> y = ForwardSubstitution(lu.L, rightSide, epsilon);
    return BackwardSubstitution(lu.U, y, epsilon);
}

inline Deque<double> SolveViaLU(const SquareMatrix<double>& matrix,
                                const Deque<double>& rightSide,
                                double epsilon = MatrixDefaultEpsilon) {
    ValidateLinearSystem(matrix, rightSide);
    return SolveViaLU(LUDecompose(matrix, epsilon), rightSide, epsilon);
}

inline Deque<double> SolveGaussNoPivot(const SquareMatrix<double>& matrix,
                                       const Deque<double>& rightSide,
                                       double epsilon = MatrixDefaultEpsilon) {
    ValidateLinearSystem(matrix, rightSide);

    SquareMatrix<double> working = matrix;
    Deque<double> b = rightSide;
    int size = working.GetSize();

    for (int pivot = 0; pivot < size; ++pivot) {
        double* pivotRow = working.GetRowPointer(pivot);
        double pivotValue = pivotRow[pivot];
        if (IsNearZero(pivotValue, epsilon)) {
            throw CalculationException("SolveGaussNoPivot: zero or too small pivot");
        }

        for (int row = pivot + 1; row < size; ++row) {
            double* currentRow = working.GetRowPointer(row);
            double factor = currentRow[pivot] / pivotValue;
            currentRow[pivot] = 0.0;

            for (int col = pivot + 1; col < size; ++col) {
                currentRow[col] = currentRow[col] - factor * pivotRow[col];
            }

            b.Set(row, b.Get(row) - factor * b.Get(pivot));
        }
    }

    TriangularMatrix<double> upper(working, TriangleKind::Upper);
    return BackwardSubstitution(upper, b, epsilon);
}

inline Deque<double> SolveGaussPartialPivot(const SquareMatrix<double>& matrix,
                                            const Deque<double>& rightSide,
                                            double epsilon = MatrixDefaultEpsilon) {
    ValidateLinearSystem(matrix, rightSide);

    SquareMatrix<double> working = matrix;
    Deque<double> b = rightSide;
    int size = working.GetSize();

    for (int pivot = 0; pivot < size; ++pivot) {
        int bestRow = pivot;
        double bestValue = AbsValue(working.GetRowPointer(pivot)[pivot]);

        for (int row = pivot + 1; row < size; ++row) {
            double candidate = AbsValue(working.GetRowPointer(row)[pivot]);
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

        double* pivotRow = working.GetRowPointer(pivot);
        double pivotValue = pivotRow[pivot];
        for (int row = pivot + 1; row < size; ++row) {
            double* currentRow = working.GetRowPointer(row);
            double factor = currentRow[pivot] / pivotValue;
            currentRow[pivot] = 0.0;

            for (int col = pivot + 1; col < size; ++col) {
                currentRow[col] = currentRow[col] - factor * pivotRow[col];
            }

            b.Set(row, b.Get(row) - factor * b.Get(pivot));
        }
    }

    TriangularMatrix<double> upper(working, TriangleKind::Upper);
    return BackwardSubstitution(upper, b, epsilon);
}

#endif
