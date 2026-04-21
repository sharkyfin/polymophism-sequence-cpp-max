#ifndef TESTS_H
#define TESTS_H

#include <string>

template <class T>
class Sequence;

template <class T>
class Deque;

template <class T>
class RectangularMatrix;

template <class T>
class Vector;

struct TestEntry {
    const char* name;
    void (*func)();
};

void Assert(bool condition, const std::string& message);
void AssertIntSequenceEquals(const Sequence<int>* sequence,
                             const int* expected,
                             int expectedLength,
                             const std::string& messagePrefix);
void AssertStringSequenceEquals(const Sequence<std::string>* sequence,
                                const std::string* expected,
                                int expectedLength,
                                const std::string& messagePrefix);
void AssertDoubleSequenceNear(const Sequence<double>* sequence,
                              const double* expected,
                              int expectedLength,
                              double epsilon,
                              const std::string& messagePrefix);
void AssertDoubleNear(double actual,
                      double expected,
                      double epsilon,
                      const std::string& message);
void AssertDoubleDequeNear(const Deque<double>& deque,
                           const double* expected,
                           int expectedLength,
                           double epsilon,
                           const std::string& messagePrefix);
void AssertDoubleVectorNear(const Vector<double>& vector,
                           const double* expected,
                           int expectedLength,
                           double epsilon,
                           const std::string& messagePrefix);
void AssertMatrixNear(const RectangularMatrix<double>& matrix,
                      const double* expected,
                      int expectedRows,
                      int expectedCols,
                      double epsilon,
                      const std::string& messagePrefix);
void AssertIntDequeEquals(const Deque<int>& deque,
                          const int* expected,
                          int expectedLength,
                          const std::string& messagePrefix);

bool RunTestEntries(const TestEntry* tests, int testCount, int& passed, int& failed);
bool RunMatrixTests(int& passed, int& failed);
bool RunAllTests();

#endif
