#ifndef SPECIAL_MATRICES_H
#define SPECIAL_MATRICES_H

#include "exceptions.hpp"
#include "matrix_common.hpp"
#include "square_matrix.hpp"

enum class TriangleKind {
    Lower,
    Upper
};

template <class T>
bool IsZeroForSpecialMatrix(const T& value) {
    return value == T();
}

inline bool IsZeroForSpecialMatrix(const double& value) {
    return IsNearZero(value);
}

template <class T>
class TriangularMatrix {
private:
    SquareMatrix<T> data;
    TriangleKind kind;
    T zero;

    void CheckIndex(int row, int col) const {
        if (row < 0 || col < 0 || row >= GetSize() || col >= GetSize()) {
            throw IndexOutOfRangeException("TriangularMatrix: index out of range");
        }
    }

    bool IsInsideTriangle(int row, int col) const {
        return kind == TriangleKind::Lower ? row >= col : row <= col;
    }

public:
    TriangularMatrix()
        : data(), kind(TriangleKind::Upper), zero(T()) {}

    TriangularMatrix(int size, TriangleKind triangleKind)
        : data(size, T()), kind(triangleKind), zero(T()) {}

    TriangularMatrix(const SquareMatrix<T>& source, TriangleKind triangleKind)
        : data(source.GetSize(), T()), kind(triangleKind), zero(T()) {
        int size = source.GetSize();
        for (int row = 0; row < size; ++row) {
            for (int col = 0; col < size; ++col) {
                if (IsInsideTriangle(row, col)) {
                    data.Set(row, col, source.Get(row, col));
                } else if (!IsZeroForSpecialMatrix(source.Get(row, col))) {
                    throw InvalidArgumentException("TriangularMatrix: source is not triangular");
                }
            }
        }
    }

    int GetSize() const {
        return data.GetSize();
    }

    TriangleKind GetKind() const {
        return kind;
    }

    const T& Get(int row, int col) const {
        CheckIndex(row, col);
        if (!IsInsideTriangle(row, col)) {
            return zero;
        }

        return data.Get(row, col);
    }

    void SetInsideTriangle(int row, int col, const T& value) {
        CheckIndex(row, col);
        if (!IsInsideTriangle(row, col)) {
            throw InvalidArgumentException("TriangularMatrix: cannot set value outside triangle");
        }

        data.Set(row, col, value);
    }

    const T* GetRowPointer(int row) const {
        return data.GetRowPointer(row);
    }

    const T* operator[](int row) const {
        return GetRowPointer(row);
    }

    const SquareMatrix<T>& AsSquare() const {
        return data;
    }
};

template <class T>
class DiagonalMatrix {
private:
    SquareMatrix<T> data;
    T zero;

    void CheckIndex(int row, int col) const {
        if (row < 0 || col < 0 || row >= GetSize() || col >= GetSize()) {
            throw IndexOutOfRangeException("DiagonalMatrix: index out of range");
        }
    }

public:
    DiagonalMatrix() : data(), zero(T()) {}

    explicit DiagonalMatrix(int size) : data(size, T()), zero(T()) {}

    explicit DiagonalMatrix(const Deque<T>& diagonal)
        : data(diagonal.GetLength(), T()), zero(T()) {
        for (int i = 0; i < diagonal.GetLength(); ++i) {
            SetDiagonal(i, diagonal.Get(i));
        }
    }

    int GetSize() const {
        return data.GetSize();
    }

    const T& Get(int row, int col) const {
        CheckIndex(row, col);
        if (row != col) {
            return zero;
        }

        return data.Get(row, col);
    }

    void SetDiagonal(int index, const T& value) {
        CheckIndex(index, index);
        data.Set(index, index, value);
    }

    const SquareMatrix<T>& AsSquare() const {
        return data;
    }
};

#endif
