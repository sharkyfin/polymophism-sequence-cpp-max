#ifndef GUI_SAMPLE_DATA_HPP
#define GUI_SAMPLE_DATA_HPP

#include "deque/deque.hpp"
#include "matrix/rectangular_matrix.hpp"
#include "matrix/special_matrices.hpp"
#include "matrix/square_matrix.hpp"

inline Deque<int> CreateSampleDeque() {
    Deque<int> result;
    for (int i = 1; i <= 18; ++i) {
        result.Append(i);
    }
    for (int i = 0; i < 5; ++i) {
        result.PopFront();
    }
    for (int i = 5; i >= 1; --i) {
        result.Prepend(i);
    }
    return result;
}

inline RectangularMatrix<double> CreateRectangularSample(int rows, int cols) {
    RectangularMatrix<double> matrix(rows, cols, 0.0);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            matrix.Set(row, col, row * 10.0 + col + 1.0);
        }
    }
    return matrix;
}

inline SquareMatrix<double> CreateSquareSample(int size) {
    SquareMatrix<double> matrix(size, 0.0);
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            matrix.Set(row, col, row == col ? 10.0 + row : row * 10.0 + col + 1.0);
        }
    }
    return matrix;
}

inline TriangularMatrix<double> CreateTriangularSample(int size, TriangleKind kind) {
    TriangularMatrix<double> matrix(size, kind);
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            bool inside = kind == TriangleKind::Lower ? row >= col : row <= col;
            if (inside) {
                matrix.Set(row, col, row * 10.0 + col + 1.0);
            }
        }
    }
    return matrix;
}

inline DiagonalMatrix<double> CreateDiagonalSample(int size) {
    DiagonalMatrix<double> matrix(size);
    for (int index = 0; index < size; ++index) {
        matrix.SetDiagonal(index, 10.0 + index);
    }
    return matrix;
}

#endif
