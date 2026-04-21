#ifndef SQUARE_MATRIX_H
#define SQUARE_MATRIX_H

#include "matrix/rectangular_matrix.hpp"

template <class T>
class SquareMatrix : public RectangularMatrix<T> {
public:
    SquareMatrix() : RectangularMatrix<T>() {}

    explicit SquareMatrix(int size) : RectangularMatrix<T>(size, size) {}

    SquareMatrix(int size, const T& value) : RectangularMatrix<T>(size, size, value) {}

    int GetSize() const {
        return this->GetRows();
    }

    static SquareMatrix<T> Identity(int size) {
        SquareMatrix<T> result(size, T());
        for (int i = 0; i < size; ++i) {
            result.Set(i, i, static_cast<T>(1));
        }
        return result;
    }

    T Trace() const {
        T result = T();
        for (int i = 0; i < GetSize(); ++i) {
            result = result + this->Get(i, i);
        }
        return result;
    }
};

#endif
