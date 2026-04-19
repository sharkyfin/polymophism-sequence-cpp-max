#ifndef RECTANGULAR_MATRIX_H
#define RECTANGULAR_MATRIX_H

#include <iostream>

#include "exceptions.hpp"
#include "segmented_buffer.hpp"
#include "vector.hpp"

template <class T>
class RectangularMatrix {
private:
    int rows;
    int cols;
    SegmentedBuffer<T> data;

    static SegmentedBuffer<T> CreateStorage(int rowCount, int colCount, const T& value) {
        if (rowCount <= 0 || colCount <= 0) {
            throw InvalidArgumentException("RectangularMatrix: matrix sizes must be positive");
        }

        SegmentedBuffer<T> storage(rowCount, colCount);
        for (int row = 0; row < rowCount; ++row) {
            storage.AllocateSegment(row);
            for (int col = 0; col < colCount; ++col) {
                storage.GetSegment(row)[col] = value;
            }
        }

        return storage;
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
        return data.GetSegment(row)[col];
    }

    void Set(int row, int col, const T& value) {
        CheckIndex(row, col);
        data.GetSegment(row)[col] = value;
    }

    T& operator()(int row, int col) {
        CheckIndex(row, col);
        return data.GetSegment(row)[col];
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
        CheckRow(row);
        return data.GetSegmentData(row);
    }

    const T* GetRowPointer(int row) const {
        CheckRow(row);
        return data.GetSegmentData(row);
    }

    void Fill(const T& value) {
        for (int row = 0; row < rows; ++row) {
            T* rowData = data.GetSegmentData(row);
            for (int col = 0; col < cols; ++col) {
                rowData[col] = value;
            }
        }
    }

    void SwapRows(int first, int second) {
        CheckRow(first);
        CheckRow(second);
        if (first == second) {
            return;
        }

        data.GetSegment(first).Swap(data.GetSegment(second));
    }

    Vector<T> GetRow(int row) const {
        CheckRow(row);

        Vector<T> result(cols, T());
        for (int col = 0; col < cols; ++col) {
            result[col] = (*this)[row][col];
        }

        return result;
    }

    Vector<T> GetColumn(int col) const {
        CheckColumn(col);

        Vector<T> result(rows, T());
        for (int row = 0; row < rows; ++row) {
            result[row] = Get(row, col);
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
