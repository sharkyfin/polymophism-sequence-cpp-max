#ifndef RECTANGULAR_MATRIX_H
#define RECTANGULAR_MATRIX_H

#include <iostream>

#include "deque.hpp"
#include "exceptions.hpp"

template <class T>
class RectangularMatrix {
private:
    int rows;
    int cols;
    Deque<T> data;

    static Deque<T> CreateStorage(int rowCount, int colCount, const T& value) {
        if (rowCount <= 0 || colCount <= 0) {
            throw InvalidArgumentException("RectangularMatrix: matrix sizes must be positive");
        }

        return Deque<T>::CreateMatrixStorage(rowCount, colCount, value);
    }

    int Index(int row, int col) const {
        return row * cols + col;
    }

    void CheckRow(int row) const {
        if (row < 0 || row >= rows) {
            throw IndexOutOfRangeException("RectangularMatrix: row index out of range");
        }
    }

    void CheckColumn(int col) const {
        if (col < 0 || col >= cols) {
            throw IndexOutOfRangeException("RectangularMatrix: column index out of range");
        }
    }

    void CheckIndex(int row, int col) const {
        CheckRow(row);
        CheckColumn(col);
    }

public:
    RectangularMatrix() : rows(0), cols(0), data() {}

    RectangularMatrix(int rowCount, int colCount)
        : rows(rowCount), cols(colCount), data(CreateStorage(rowCount, colCount, T())) {}

    RectangularMatrix(int rowCount, int colCount, const T& value)
        : rows(rowCount), cols(colCount), data(CreateStorage(rowCount, colCount, value)) {}

    int GetRows() const {
        return rows;
    }

    int GetCols() const {
        return cols;
    }

    const T& Get(int row, int col) const {
        CheckIndex(row, col);
        return data.Get(Index(row, col));
    }

    void Set(int row, int col, const T& value) {
        CheckIndex(row, col);
        data.Set(Index(row, col), value);
    }

    T& operator()(int row, int col) {
        CheckIndex(row, col);
        return data[Index(row, col)];
    }

    const T& operator()(int row, int col) const {
        return Get(row, col);
    }

    T* operator[](int row) {
        return GetRowPointer(row);
    }

    const T* operator[](int row) const {
        return GetRowPointer(row);
    }

    T* GetRowPointer(int row) {
        return data.GetMatrixRowPointer(row);
    }

    const T* GetRowPointer(int row) const {
        return data.GetMatrixRowPointer(row);
    }

    void Fill(const T& value) {
        for (int i = 0; i < data.GetLength(); ++i) {
            data.Set(i, value);
        }
    }

    void SwapRows(int first, int second) {
        if (first == second) {
            return;
        }

        data.SwapRowsMatrix(first, second);
    }

    Deque<T> GetRow(int row) const {
        CheckRow(row);

        Deque<T> result = Deque<T>::CreateVectorStorage(cols, T());
        for (int col = 0; col < cols; ++col) {
            result.Set(col, (*this)[row][col]);
        }

        return result;
    }

    Deque<T> GetColumn(int col) const {
        CheckColumn(col);

        Deque<T> result = Deque<T>::CreateVectorStorage(rows, T());
        for (int row = 0; row < rows; ++row) {
            result.Set(row, Get(row, col));
        }

        return result;
    }
};

template <class T>
std::ostream& operator<<(std::ostream& output, const RectangularMatrix<T>& matrix) {
    for (int row = 0; row < matrix.GetRows(); ++row) {
        output << "[";
        for (int col = 0; col < matrix.GetCols(); ++col) {
            if (col > 0) {
                output << " ";
            }
            output << matrix[row][col];
        }
        output << "]";
        if (row + 1 < matrix.GetRows()) {
            output << "\n";
        }
    }

    return output;
}

#endif
