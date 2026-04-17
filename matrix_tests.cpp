#include "tests.hpp"

#include "matrix_solvers.hpp"

void TestMatrixInvalidSizes() {
    RectangularMatrix<double> empty;
    Assert(empty.GetRows() == 0 && empty.GetCols() == 0,
           "RectangularMatrix: default constructor must create empty matrix");

    bool caught = false;
    try {
        RectangularMatrix<double> matrix(0, 3);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "RectangularMatrix: zero rows must throw");

    caught = false;
    try {
        RectangularMatrix<double> matrix(3, 0);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "RectangularMatrix: zero columns must throw");

    caught = false;
    try {
        RectangularMatrix<double> matrix(-1, 3);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "RectangularMatrix: negative rows must throw");

    caught = false;
    try {
        RectangularMatrix<double> matrix(3, -1);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "RectangularMatrix: negative columns must throw");

    caught = false;
    try {
        SquareMatrix<double> matrix(0);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "SquareMatrix: zero size must throw");

    caught = false;
    try {
        SquareMatrix<double> matrix(-1);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "SquareMatrix: negative size must throw");

    caught = false;
    try {
        DiagonalMatrix<double> matrix(0);
        (void)matrix;
    } catch (const InvalidArgumentException&) {
        caught = true;
    }
    Assert(caught, "DiagonalMatrix: zero size must throw");
}

void TestRectangularMatrixBasicOperations() {
    RectangularMatrix<double> matrix(2, 3, 0.0);
    matrix.Set(0, 0, 1.0);
    matrix.Set(0, 1, 2.0);
    matrix.Set(0, 2, 3.0);
    matrix.Set(1, 0, 4.0);
    matrix.Set(1, 1, 5.0);
    matrix.Set(1, 2, 6.0);

    Assert(matrix.GetRows() == 2, "RectangularMatrix: wrong row count");
    Assert(matrix.GetCols() == 3, "RectangularMatrix: wrong column count");
    AssertDoubleNear(matrix.Get(1, 2), 6.0, 1e-9, "RectangularMatrix: wrong Get");

    matrix[1][1] = 50.0;
    AssertDoubleNear(matrix[1][1], 50.0, 1e-9, "RectangularMatrix: operator[][] access failed");

    RectangularMatrix<double> copiedMatrix = matrix;
    AssertDoubleNear(copiedMatrix[1][1], 50.0, 1e-9, "RectangularMatrix: copied operator[][] access failed");

    Deque<double> row = matrix.GetRow(0);
    double expectedRow[3] = {1.0, 2.0, 3.0};
    AssertDoubleDequeNear(row, expectedRow, 3, 1e-9, "RectangularMatrix: row extraction");

    Deque<double> column = matrix.GetColumn(1);
    double expectedColumn[2] = {2.0, 50.0};
    AssertDoubleDequeNear(column, expectedColumn, 2, 1e-9, "RectangularMatrix: column extraction");

    matrix.SwapRows(0, 1);
    double expectedAfterSwap[6] = {4.0, 50.0, 6.0, 1.0, 2.0, 3.0};
    AssertMatrixNear(matrix, expectedAfterSwap, 2, 3, 1e-9, "RectangularMatrix: SwapRows");
}

void TestSquareAndSpecialMatrices() {
    SquareMatrix<double> identity = SquareMatrix<double>::Identity(3);
    AssertDoubleNear(identity.Trace(), 3.0, 1e-9, "SquareMatrix: wrong trace");

    TriangularMatrix<double> lower(3, TriangleKind::Lower);
    lower.SetInsideTriangle(0, 0, 1.0);
    lower.SetInsideTriangle(1, 0, 2.0);
    lower.SetInsideTriangle(1, 1, 3.0);
    AssertDoubleNear(lower.Get(0, 1), 0.0, 1e-9, "TriangularMatrix: value outside lower triangle");
    AssertDoubleNear(lower.Get(1, 0), 2.0, 1e-9, "TriangularMatrix: value inside lower triangle");

    bool caught = false;
    try {
        lower.SetInsideTriangle(0, 2, 10.0);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "TriangularMatrix: setting outside triangle must throw");

    SquareMatrix<double> almostUpper(2, 0.0);
    almostUpper.Set(0, 0, 1.0);
    almostUpper.Set(0, 1, 2.0);
    almostUpper.Set(1, 0, 1e-15);
    almostUpper.Set(1, 1, 3.0);
    TriangularMatrix<double> upperWithTinyOutsideValue(almostUpper, TriangleKind::Upper);
    AssertDoubleNear(upperWithTinyOutsideValue.Get(1, 0), 0.0, 1e-9,
                     "TriangularMatrix: tiny double value outside triangle must be treated as zero");

    DiagonalMatrix<double> diagonal(3);
    diagonal.SetDiagonal(0, 2.0);
    diagonal.SetDiagonal(1, 4.0);
    diagonal.SetDiagonal(2, 8.0);
    AssertDoubleNear(diagonal.Get(0, 1), 0.0, 1e-9, "DiagonalMatrix: off-diagonal value");
    AssertDoubleNear(diagonal.Get(2, 2), 8.0, 1e-9, "DiagonalMatrix: diagonal value");

    Deque<double> rightSide = Deque<double>::CreateVectorStorage(3, 0.0);
    rightSide.Set(0, 4.0);
    rightSide.Set(1, 8.0);
    rightSide.Set(2, 16.0);
    Deque<double> solution = SolveDiagonal(diagonal, rightSide);
    double expectedSolution[3] = {2.0, 2.0, 2.0};
    AssertDoubleDequeNear(solution, expectedSolution, 3, 1e-9, "DiagonalMatrix: SolveDiagonal");
}

void TestMatrixAlgorithms() {
    RectangularMatrix<double> left(2, 2, 0.0);
    left.Set(0, 0, 1.0);
    left.Set(0, 1, 2.0);
    left.Set(1, 0, 3.0);
    left.Set(1, 1, 4.0);

    RectangularMatrix<double> right(2, 2, 0.0);
    right.Set(0, 0, 2.0);
    right.Set(0, 1, 0.0);
    right.Set(1, 0, 1.0);
    right.Set(1, 1, 2.0);

    RectangularMatrix<double> product = Multiply(left, right);
    double expectedProduct[4] = {4.0, 4.0, 10.0, 8.0};
    AssertMatrixNear(product, expectedProduct, 2, 2, 1e-9, "MatrixAlgorithms: matrix multiply");

    Deque<double> vector = Deque<double>::CreateVectorStorage(2, 0.0);
    vector.Set(0, 1.0);
    vector.Set(1, 2.0);
    Deque<double> multipliedVector = Multiply(left, vector);
    double expectedVector[2] = {5.0, 11.0};
    AssertDoubleDequeNear(multipliedVector, expectedVector, 2, 1e-9,
                          "MatrixAlgorithms: matrix-vector multiply");
}

SquareMatrix<double> CreateSolverMatrix() {
    SquareMatrix<double> matrix(3, 0.0);
    matrix.Set(0, 0, 3.0);
    matrix.Set(0, 1, 2.0);
    matrix.Set(0, 2, -4.0);
    matrix.Set(1, 0, 2.0);
    matrix.Set(1, 1, 3.0);
    matrix.Set(1, 2, 3.0);
    matrix.Set(2, 0, 5.0);
    matrix.Set(2, 1, -3.0);
    matrix.Set(2, 2, 1.0);
    return matrix;
}

Deque<double> CreateKnownSolution() {
    Deque<double> solution = Deque<double>::CreateVectorStorage(3, 0.0);
    solution.Set(0, 3.0);
    solution.Set(1, 1.0);
    solution.Set(2, 2.0);
    return solution;
}

void TestMatrixSolversKnownSystem() {
    SquareMatrix<double> matrix = CreateSolverMatrix();
    Deque<double> expectedSolution = CreateKnownSolution();
    Deque<double> rightSide = Multiply(matrix, expectedSolution);

    Deque<double> gaussSolution = SolveGaussPartialPivot(matrix, rightSide);
    double expected[3] = {3.0, 1.0, 2.0};
    AssertDoubleDequeNear(gaussSolution, expected, 3, 1e-9,
                          "MatrixSolvers: Gauss partial pivot");

    LUDecompositionResult lu = LUDecompose(matrix);
    Deque<double> luSolution = SolveViaLU(lu, rightSide);
    AssertDoubleDequeNear(luSolution, expected, 3, 1e-9, "MatrixSolvers: LU solve");

    RectangularMatrix<double> luProduct = Multiply(lu.L.AsSquare(), lu.U.AsSquare());
    AssertDoubleNear(MaxAbsDifference(matrix, luProduct), 0.0, 1e-9,
                     "MatrixSolvers: A must equal L*U");
    AssertDoubleNear(ResidualNorm(matrix, luSolution, rightSide), 0.0, 1e-9,
                     "MatrixSolvers: LU residual");
}

void TestMatrixSolversErrors() {
    SquareMatrix<double> singular(2, 0.0);
    singular.Set(0, 0, 1.0);
    singular.Set(0, 1, 2.0);
    singular.Set(1, 0, 2.0);
    singular.Set(1, 1, 4.0);

    Deque<double> rightSide = Deque<double>::CreateVectorStorage(2, 0.0);
    rightSide.Set(0, 3.0);
    rightSide.Set(1, 6.0);

    bool caught = false;
    try {
        SolveGaussPartialPivot(singular, rightSide);
    } catch (const CalculationException&) {
        caught = true;
    }
    Assert(caught, "MatrixSolvers: singular system must throw");

    SquareMatrix<double> needsPivot(2, 0.0);
    needsPivot.Set(0, 0, 0.0);
    needsPivot.Set(0, 1, 1.0);
    needsPivot.Set(1, 0, 1.0);
    needsPivot.Set(1, 1, 1.0);

    caught = false;
    try {
        LUDecompose(needsPivot);
    } catch (const CalculationException&) {
        caught = true;
    }
    Assert(caught, "MatrixSolvers: LU without pivot must throw on zero pivot");
}

bool RunMatrixTests(int& passed, int& failed) {
    TestEntry tests[] = {
        {"Matrix: invalid sizes", TestMatrixInvalidSizes},
        {"RectangularMatrix: basic operations", TestRectangularMatrixBasicOperations},
        {"Square/special matrices", TestSquareAndSpecialMatrices},
        {"MatrixAlgorithms", TestMatrixAlgorithms},
        {"MatrixSolvers: known system", TestMatrixSolversKnownSystem},
        {"MatrixSolvers: errors", TestMatrixSolversErrors},
    };

    int testCount = static_cast<int>(sizeof(tests) / sizeof(tests[0]));
    return RunTestEntries(tests, testCount, passed, failed);
}
