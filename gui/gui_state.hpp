#ifndef GUI_STATE_HPP
#define GUI_STATE_HPP

#include <string>

#include "gui/sample_data.hpp"

enum MatrixViewMode {
    MatrixViewRectangular = 0,
    MatrixViewSquare,
    MatrixViewLowerTriangular,
    MatrixViewUpperTriangular,
    MatrixViewDiagonal
};

struct GuiState {
    Deque<int> deque;
    int dequeValue;

    int matrixMode;
    int rectRows;
    int rectCols;
    int matrixSize;
    int editRow;
    int editCol;
    double editValue;

    RectangularMatrix<double> rectangular;
    SquareMatrix<double> square;
    TriangularMatrix<double> lowerTriangular;
    TriangularMatrix<double> upperTriangular;
    DiagonalMatrix<double> diagonal;

    std::string status;
    bool statusIsError;

    GuiState()
        : deque(CreateSampleDeque()),
          dequeValue(21),
          matrixMode(MatrixViewRectangular),
          rectRows(3),
          rectCols(4),
          matrixSize(4),
          editRow(0),
          editCol(0),
          editValue(99.0),
          rectangular(CreateRectangularSample(rectRows, rectCols)),
          square(CreateSquareSample(matrixSize)),
          lowerTriangular(CreateTriangularSample(matrixSize, TriangleKind::Lower)),
          upperTriangular(CreateTriangularSample(matrixSize, TriangleKind::Upper)),
          diagonal(CreateDiagonalSample(matrixSize)),
          status("Ready"),
          statusIsError(false) {}
};

inline GuiState& State() {
    static GuiState state;
    return state;
}

inline void SetStatus(const std::string& message, bool isError = false) {
    GuiState& state = State();
    state.status = message;
    state.statusIsError = isError;
}

#endif
