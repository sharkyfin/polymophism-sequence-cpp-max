#include "tests.hpp"

#include <iostream>
#include <string>
#include <type_traits>

#include "dynamic_array.hpp"
#include "deque.hpp"
#include "immutable_array_sequence.hpp"
#include "immutable_deque_sequence.hpp"
#include "immutable_list_sequence.hpp"
#include "linked_list.hpp"
#include "matrix_solvers.hpp"
#include "mutable_array_sequence.hpp"
#include "mutable_deque_sequence.hpp"
#include "mutable_list_sequence.hpp"
#include "pair.hpp"
#include "sequence_utils.hpp"

class TestFailedException {
private:
    std::string message;

public:
    explicit TestFailedException(const std::string& msg) : message(msg) {}

    const std::string& GetMessage() const {
        return message;
    }
};

void Assert(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailedException(message);
    }
}

void AssertIntSequenceEquals(const Sequence<int>* sequence,
                             const int* expected,
                             int expectedLength,
                             const std::string& messagePrefix) {
    Assert(sequence != nullptr, messagePrefix + ": sequence is null");
    Assert(sequence->GetLength() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        Assert(sequence->Get(i) == expected[i],
               messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void AssertStringSequenceEquals(const Sequence<std::string>* sequence,
                                const std::string* expected,
                                int expectedLength,
                                const std::string& messagePrefix) {
    Assert(sequence != nullptr, messagePrefix + ": sequence is null");
    Assert(sequence->GetLength() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        Assert(sequence->Get(i) == expected[i],
               messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void AssertDoubleSequenceNear(const Sequence<double>* sequence,
                              const double* expected,
                              int expectedLength,
                              double epsilon,
                              const std::string& messagePrefix) {
    Assert(sequence != nullptr, messagePrefix + ": sequence is null");
    Assert(sequence->GetLength() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        double difference = sequence->Get(i) - expected[i];
        if (difference < 0) {
            difference = -difference;
        }

        Assert(difference <= epsilon,
               messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void AssertDoubleNear(double actual,
                      double expected,
                      double epsilon,
                      const std::string& message) {
    double difference = actual - expected;
    if (difference < 0) {
        difference = -difference;
    }

    Assert(difference <= epsilon, message);
}

void AssertDoubleDequeNear(const Deque<double>& deque,
                           const double* expected,
                           int expectedLength,
                           double epsilon,
                           const std::string& messagePrefix) {
    Assert(deque.GetLength() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        AssertDoubleNear(deque.Get(i), expected[i], epsilon,
                         messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void AssertDoubleVectorNear(const Vector<double>& vector,
                           const double* expected,
                           int expectedLength,
                           double epsilon,
                           const std::string& messagePrefix) {
    Assert(vector.GetSize() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        AssertDoubleNear(vector[i], expected[i], epsilon,
                         messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void AssertMatrixNear(const RectangularMatrix<double>& matrix,
                      const double* expected,
                      int expectedRows,
                      int expectedCols,
                      double epsilon,
                      const std::string& messagePrefix) {
    Assert(matrix.GetRows() == expectedRows, messagePrefix + ": wrong row count");
    Assert(matrix.GetCols() == expectedCols, messagePrefix + ": wrong column count");

    for (int row = 0; row < expectedRows; ++row) {
        for (int col = 0; col < expectedCols; ++col) {
            AssertDoubleNear(matrix.Get(row, col), expected[row * expectedCols + col], epsilon,
                             messagePrefix + ": wrong value");
        }
    }
}

void AssertIntDequeEquals(const Deque<int>& deque,
                          const int* expected,
                          int expectedLength,
                          const std::string& messagePrefix) {
    Assert(deque.GetLength() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        Assert(deque.Get(i) == expected[i],
               messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void TestDynamicArrayCreateAndGet() {
    int items[3] = {1, 2, 3};
    DynamicArray<int> arr(items, 3);

    Assert(arr.GetSize() == 3, "DynamicArray: wrong size after construction");
    Assert(arr.Get(0) == 1, "DynamicArray: wrong element at index 0");
    Assert(arr.Get(2) == 3, "DynamicArray: wrong element at index 2");
}

void TestDynamicArrayDefaultConstructorAndIndexOperator() {
    DynamicArray<int> arr;

    Assert(arr.GetSize() == 0, "DynamicArray: wrong size after default construction");
    Assert(arr.GetCapacity() == 0, "DynamicArray: wrong capacity after default construction");

    arr.PushBack(10);
    arr.PushBack(20);

    Assert(arr[0] == 10, "DynamicArray: wrong value from operator[]");
    arr[1] = 99;
    Assert(arr.Get(1) == 99, "DynamicArray: operator[] must allow mutation");
}

void TestDynamicArrayCopy() {
    int items[2] = {10, 20};
    DynamicArray<int> a(items, 2);
    DynamicArray<int> b(a);

    a.Set(0, 99);
    Assert(b.Get(0) == 10, "DynamicArray: copy is not deep");
}

void TestDynamicArrayResizeSetGet() {
    DynamicArray<int> arr(2);
    arr.Set(0, 5);
    arr.Set(1, 6);

    arr.Resize(4);
    arr.Set(2, 7);
    arr.Set(3, 8);

    Assert(arr.GetSize() == 4, "DynamicArray: wrong size after Resize up");
    Assert(arr.Get(0) == 5 && arr.Get(3) == 8, "DynamicArray: wrong values after Resize up");

    arr.Resize(1);
    Assert(arr.GetSize() == 1, "DynamicArray: wrong size after Resize down");
    Assert(arr.Get(0) == 5, "DynamicArray: wrong value after Resize down");
}

void TestDynamicArrayPushBack() {
    DynamicArray<int> arr(0);

    for (int i = 1; i <= 5; ++i) {
        arr.PushBack(i);
    }

    Assert(arr.GetSize() == 5, "DynamicArray: wrong size after PushBack");
    Assert(arr.Get(0) == 1 && arr.Get(4) == 5, "DynamicArray: wrong values after PushBack");
    Assert(arr.GetCapacity() >= arr.GetSize(), "DynamicArray: capacity must be >= size");
}

void TestDynamicArrayIndexErrors() {
    DynamicArray<int> arr(1);

    bool caught = false;
    try {
        arr.Get(1);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "DynamicArray: out-of-range Get must throw");
}

void TestDynamicArrayNestedStorage() {
    DynamicArray<DynamicArray<int>> rows(2);

    rows[0] = DynamicArray<int>(3);
    rows[1] = DynamicArray<int>(2);

    rows[0][0] = 1;
    rows[0][1] = 2;
    rows[0][2] = 3;
    rows.Get(1).Set(0, 4);
    rows.Get(1).Set(1, 5);

    Assert(rows.Get(0).GetSize() == 3, "DynamicArray nested: wrong first row size");
    Assert(rows.Get(1).GetSize() == 2, "DynamicArray nested: wrong second row size");
    Assert(rows[0][2] == 3, "DynamicArray nested: wrong value in first row");
    Assert(rows[1][1] == 5, "DynamicArray nested: wrong value in second row");

    DynamicArray<DynamicArray<int>> copy(rows);
    rows[0][0] = 99;
    Assert(copy[0][0] == 1, "DynamicArray nested: copy must be deep");

    DynamicArray<int> thirdRow(1);
    thirdRow[0] = 6;
    rows.PushBack(thirdRow);
    Assert(rows.GetSize() == 3, "DynamicArray nested: wrong outer size after PushBack");
    Assert(rows[2][0] == 6, "DynamicArray nested: wrong pushed row");
}

void TestDynamicArrayStringAndDoubleStorage() {
    DynamicArray<std::string> words;
    words.PushBack("alpha");
    words.PushBack("beta");
    words.Set(1, "gamma");

    Assert(words.GetSize() == 2, "DynamicArray typed: wrong string size");
    Assert(words[0] == "alpha", "DynamicArray typed: wrong first string");
    Assert(words[1] == "gamma", "DynamicArray typed: wrong second string");

    DynamicArray<double> numbers;
    numbers.PushBack(1.5);
    numbers.PushBack(2.25);
    numbers.PushBack(3.75);

    Assert(numbers.GetSize() == 3, "DynamicArray typed: wrong double size");
    Assert(numbers[0] == 1.5, "DynamicArray typed: wrong first double");
    Assert(numbers[2] == 3.75, "DynamicArray typed: wrong third double");
}

void TestLinkedListAppendPrependInsertGet() {
    LinkedList<int> list;
    list.Append(2);
    list.Prepend(1);
    list.InsertAt(3, 2);
    list.InsertAt(100, 1);

    Assert(list.GetLength() == 4, "LinkedList: wrong length");
    Assert(list.GetFirst() == 1, "LinkedList: wrong first element");
    Assert(list.GetLast() == 3, "LinkedList: wrong last element");
    Assert(list.Get(1) == 100, "LinkedList: wrong inserted element");
}

void TestLinkedListSubListAndConcat() {
    int itemsA[4] = {1, 2, 3, 4};
    int itemsB[2] = {5, 6};

    LinkedList<int> a(itemsA, 4);
    LinkedList<int> b(itemsB, 2);

    LinkedList<int>* sub = a.GetSubList(1, 3);
    Assert(sub->GetLength() == 3, "LinkedList: wrong sublist length");
    Assert(sub->Get(0) == 2 && sub->Get(2) == 4, "LinkedList: wrong sublist values");

    LinkedList<int>* c = a.Concat(&b);
    Assert(c->GetLength() == 6, "LinkedList: wrong concat length");
    Assert(c->Get(4) == 5 && c->Get(5) == 6, "LinkedList: wrong concat values");

    delete sub;
    delete c;
}

void TestDequeAppendPrependInsertGet() {
    Deque<int> deque;
    deque.Append(2);
    deque.Append(3);
    deque.Prepend(1);
    deque.InsertAt(100, 2);

    int expected[4] = {1, 2, 100, 3};
    AssertIntDequeEquals(deque, expected, 4, "Deque: append/prepend/insert/get");
    Assert(deque.GetFirst() == 1, "Deque: wrong first element");
    Assert(deque.GetLast() == 3, "Deque: wrong last element");
}

void TestDequeWrapAroundAndPop() {
    Deque<int> deque;
    deque.Append(1);
    deque.Append(2);
    deque.Append(3);
    deque.Append(4);

    deque.PopFront();
    deque.PopFront();
    deque.Append(5);
    deque.Append(6);

    int wrappedExpected[4] = {3, 4, 5, 6};
    AssertIntDequeEquals(deque, wrappedExpected, 4, "Deque: wrap-around");

    deque.PopBack();
    int afterPopBack[3] = {3, 4, 5};
    AssertIntDequeEquals(deque, afterPopBack, 3, "Deque: PopBack");

    deque.Prepend(2);
    int afterPrepend[4] = {2, 3, 4, 5};
    AssertIntDequeEquals(deque, afterPrepend, 4, "Deque: Prepend after wrap-around");
}

void TestDequeIndexAndEmptyErrors() {
    Deque<int> deque;

    bool caught = false;
    try {
        deque.GetFirst();
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "Deque: GetFirst on empty deque must throw");

    deque.Append(10);

    caught = false;
    try {
        deque.Get(1);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "Deque: out-of-range Get must throw");

    caught = false;
    try {
        deque.InsertAt(20, 2);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "Deque: out-of-range InsertAt must throw");
}

void TestDequeSegmentBoundaries() {
    Deque<int> deque;

    for (int i = 0; i < 20; ++i) {
        deque.Append(i);
    }

    int expectedAfterAppend[20];
    for (int i = 0; i < 20; ++i) {
        expectedAfterAppend[i] = i;
    }
    AssertIntDequeEquals(deque, expectedAfterAppend, 20, "Deque: append across segments");

    for (int i = 0; i < 9; ++i) {
        deque.PopFront();
    }
    for (int i = 20; i < 32; ++i) {
        deque.Append(i);
    }

    int expectedAfterMixed[23];
    for (int i = 0; i < 23; ++i) {
        expectedAfterMixed[i] = i + 9;
    }
    AssertIntDequeEquals(deque, expectedAfterMixed, 23, "Deque: append after segment pops");

    for (int i = 8; i >= 0; --i) {
        deque.Prepend(i);
    }

    int expectedAfterPrepend[32];
    for (int i = 0; i < 32; ++i) {
        expectedAfterPrepend[i] = i;
    }
    AssertIntDequeEquals(deque, expectedAfterPrepend, 32, "Deque: prepend across segments");
}

void TestDequeCopyAndInsertAcrossSegments() {
    Deque<int> deque;
    for (int i = 0; i < 18; ++i) {
        deque.Append(i);
    }

    for (int i = 0; i < 5; ++i) {
        deque.PopFront();
    }
    for (int i = 4; i >= 0; --i) {
        deque.Prepend(i);
    }

    deque.InsertAt(999, 10);

    int expected[19] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 999, 10, 11, 12, 13, 14, 15, 16, 17};
    AssertIntDequeEquals(deque, expected, 19, "Deque: insert across segments");

    Deque<int> copied(deque);
    AssertIntDequeEquals(copied, expected, 19, "Deque: copy across segments");

    Deque<int> assigned;
    assigned = deque;
    AssertIntDequeEquals(assigned, expected, 19, "Deque: assignment across segments");

    deque.InsertAt(555, 2);
    int expectedNearFront[20] = {0, 1, 555, 2, 3, 4, 5, 6, 7, 8, 9, 999, 10, 11, 12, 13, 14, 15, 16, 17};
    AssertIntDequeEquals(deque, expectedNearFront, 20, "Deque: insert near front across segments");
}

void TestDequeMutableAccessForMatrixStorage() {
    Deque<double> deque;
    deque.Append(1.0);
    deque.Append(2.0);
    deque.Append(3.0);

    deque.Set(1, 20.0);
    deque[2] = 30.0;
    deque.SwapElements(0, 2);

    double expected[3] = {30.0, 20.0, 1.0};
    AssertDoubleDequeNear(deque, expected, 3, 1e-9, "Deque mutable access");
}

void TestDequeMatrixStorageFactory() {
    Deque<int> storage = Deque<int>::CreateMatrixStorage(3, 4, 7);
    Assert(storage.GetLength() == 12, "Deque matrix storage: wrong length");

    for (int i = 0; i < storage.GetLength(); ++i) {
        Assert(storage.Get(i) == 7, "Deque matrix storage: wrong initial value");
    }

    for (int row = 0; row < 3; ++row) {
        int* rowPointer = storage.GetMatrixRowPointer(row);
        for (int col = 0; col < 4; ++col) {
            rowPointer[col] = row * 10 + col;
        }
    }

    Assert(storage.Get(6) == 12, "Deque matrix storage: row pointer access failed");

    storage.SwapRowsMatrix(0, 2);
    int expectedAfterRowSwap[12] = {20, 21, 22, 23, 10, 11, 12, 13, 0, 1, 2, 3};
    for (int i = 0; i < 12; ++i) {
        Assert(storage.Get(i) == expectedAfterRowSwap[i], "Deque matrix storage: row swap failed");
    }

    storage.Append(100);
    storage.Prepend(-1);
    Assert(storage.GetLength() == 14, "Deque matrix storage: append/prepend length failed");
    Assert(storage.Get(0) == -1, "Deque matrix storage: prepend failed");
    Assert(storage.Get(7) == 12, "Deque matrix storage: value moved incorrectly after prepend");
    Assert(storage.Get(13) == 100, "Deque matrix storage: append failed");

    bool caught = false;
    try {
        storage.GetMatrixRowPointer(0);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "Deque matrix storage: row pointer must be disabled after append/prepend");

    Deque<int> copied(storage);
    Assert(copied.GetLength() == storage.GetLength(), "Deque matrix storage: copy length failed");
    for (int i = 0; i < copied.GetLength(); ++i) {
        Assert(copied.Get(i) == storage.Get(i), "Deque matrix storage: copy value failed");
    }

    Deque<std::string> filledStorage = Deque<std::string>::CreateVectorStorage(3, std::string("item"));
    Assert(filledStorage.GetLength() == 3, "Deque vector storage: wrong length");
    Assert(filledStorage.Get(0) == "item" && filledStorage.Get(2) == "item",
           "Deque vector storage: wrong values");
    filledStorage.Set(1, "changed");
    Assert(filledStorage.Get(1) == "changed", "Deque vector storage: mutable access failed");

    Deque<int> emptyMatrixStorage = Deque<int>::CreateMatrixStorage(0, 4, 1);
    Assert(emptyMatrixStorage.GetLength() == 0, "Deque matrix storage: zero-row storage must be empty");

    caught = false;
    try {
        emptyMatrixStorage.GetMatrixRowPointer(0);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "Deque matrix storage: empty storage must reject row access");
}

void TestSequenceInterfaceSubsequenceAndConcat() {
    Sequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(10);
    seq->Append(20);
    seq->Append(30);

    Sequence<int>* sub = seq->GetSubsequence(1, 2);
    Assert(sub->GetLength() == 2, "Sequence: wrong subsequence length");
    Assert(sub->Get(0) == 20 && sub->Get(1) == 30, "Sequence: wrong subsequence values");

    Sequence<int>* con = seq->Concat(sub);
    Assert(con->GetLength() == 5, "Sequence: wrong concat length");
    Assert(con->Get(3) == 20 && con->Get(4) == 30, "Sequence: wrong concat values");

    delete seq;
    delete sub;
    delete con;
}

void TestMutableSemantics() {
    MutableArraySequence<int> seq;
    Sequence<int>* returned = seq.Append(1);

    Assert(returned == &seq, "MutableSequence: Append must return same object");
    Assert(seq.GetLength() == 1 && seq.Get(0) == 1, "MutableSequence: object not modified");
}

void TestImmutableArraySemantics() {
    ImmutableArraySequence<int> seq;
    Sequence<int>* s1 = seq.Append(10);

    Assert(seq.GetLength() == 0, "ImmutableArraySequence: original changed after Append");
    Assert(s1 != &seq, "ImmutableArraySequence: Append must return new object");
    Assert(s1->GetLength() == 1 && s1->Get(0) == 10, "ImmutableArraySequence: wrong new sequence");

    Sequence<int>* s2 = s1->Prepend(5);
    Assert(s1->GetLength() == 1, "ImmutableArraySequence: previous object changed after Prepend");
    Assert(s2->GetLength() == 2 && s2->Get(0) == 5 && s2->Get(1) == 10,
           "ImmutableArraySequence: wrong Prepend result");

    delete s1;
    delete s2;
}

void TestImmutableListSemantics() {
    ImmutableListSequence<int> seq;
    Sequence<int>* s1 = seq.Append(7);

    Assert(seq.GetLength() == 0, "ImmutableListSequence: original changed after Append");
    Assert(s1 != &seq, "ImmutableListSequence: Append must return new object");

    Sequence<int>* s2 = s1->InsertAt(9, 1);
    Assert(s1->GetLength() == 1, "ImmutableListSequence: previous object changed after InsertAt");
    Assert(s2->GetLength() == 2 && s2->Get(1) == 9, "ImmutableListSequence: wrong InsertAt result");

    delete s1;
    delete s2;
}

void TestMutableDequeSemantics() {
    MutableDequeSequence<int> seq;
    Sequence<int>* appended = seq.Append(1);
    Assert(appended == &seq, "MutableDequeSequence: Append must return same object");

    Sequence<int>* prepended = seq.Prepend(0);
    Assert(prepended == &seq, "MutableDequeSequence: Prepend must return same object");

    Sequence<int>* popped = seq.PopFront();
    Assert(popped == &seq, "MutableDequeSequence: PopFront must return same object");

    int expected[1] = {1};
    AssertIntSequenceEquals(&seq, expected, 1, "MutableDequeSequence semantics");
}

void TestImmutableDequeSemantics() {
    ImmutableDequeSequence<int> seq;
    Sequence<int>* s1 = seq.Append(10);

    Assert(seq.GetLength() == 0, "ImmutableDequeSequence: original changed after Append");
    Assert(s1 != &seq, "ImmutableDequeSequence: Append must return new object");

    ImmutableDequeSequence<int>* immutable = dynamic_cast<ImmutableDequeSequence<int>*>(s1);
    Assert(immutable != nullptr, "ImmutableDequeSequence: wrong runtime type");

    Sequence<int>* s2 = immutable->PopBack();
    Assert(s1->GetLength() == 1, "ImmutableDequeSequence: previous object changed after PopBack");
    Assert(s2->GetLength() == 0, "ImmutableDequeSequence: wrong PopBack result");

    delete s1;
    delete s2;
}

void TestImmutableSubsequence() {
    int items[4] = {4, 5, 6, 7};
    ImmutableArraySequence<int> seq(items, 4);

    Sequence<int>* sub = seq.GetSubsequence(1, 2);
    Assert(sub->GetLength() == 2, "ImmutableArraySequence: wrong subsequence length");
    Assert(sub->Get(0) == 5 && sub->Get(1) == 6, "ImmutableArraySequence: wrong subsequence values");
    Assert(seq.GetLength() == 4, "ImmutableArraySequence: original changed after GetSubsequence");

    delete sub;
}

void TestICollectionContract() {
    int items[3] = {11, 22, 33};
    ICollection<int>* collection = new MutableArraySequence<int>(items, 3);

    Assert(collection->GetCount() == 3, "ICollection: wrong count");
    Assert(collection->Get(1) == 22, "ICollection: wrong value from Get");

    ICollection<int>* clone = collection->Clone();
    Assert(clone->GetCount() == 3, "ICollection: wrong clone count");
    Assert(clone->Get(2) == 33, "ICollection: wrong clone value");

    delete collection;
    delete clone;
}

void TestOperatorIndex() {
    int items[3] = {7, 8, 9};
    MutableListSequence<int> seq(items, 3);

    Assert(seq[0] == 7, "operator[]: wrong value at index 0");
    Assert(seq[2] == 9, "operator[]: wrong value at index 2");

    bool caught = false;
    try {
        int unused = seq[3];
        (void)unused;
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "operator[]: out-of-range must throw");
}

void TestOperatorEqualAndNotEqual() {
    int itemsA[3] = {1, 2, 3};
    int itemsB[3] = {1, 2, 3};
    int itemsC[3] = {1, 2, 4};

    MutableArraySequence<int> a(itemsA, 3);
    ImmutableListSequence<int> b(itemsB, 3);
    MutableListSequence<int> c(itemsC, 3);

    Assert(a == b, "operator==: equal sequences must be equal");
    Assert(a != c, "operator!=: different sequences must be not equal");
}

void TestOperatorPlus() {
    int leftItems[2] = {1, 2};
    int rightItems[2] = {3, 4};

    MutableArraySequence<int> left(leftItems, 2);
    MutableListSequence<int> right(rightItems, 2);

    Sequence<int>* concat = left + right;
    int expectedConcat[4] = {1, 2, 3, 4};
    AssertIntSequenceEquals(concat, expectedConcat, 4, "operator+ sequence");

    Sequence<int>* plusElement = left + 9;
    int expectedPlusElement[3] = {1, 2, 9};
    AssertIntSequenceEquals(plusElement, expectedPlusElement, 3, "operator+ element");

    Assert(left.GetLength() == 2, "operator+: left side must not be changed");

    delete concat;
    delete plusElement;
}

void TestOperatorOutput() {
    int items[3] = {1, 2, 3};
    MutableArraySequence<int> seq(items, 3);

    std::ostream& output = (std::cout << seq);
    std::cout << "\n";
    Assert(&output == &std::cout, "operator<<: must return output stream reference");
}

void TestMap() {
    int items[3] = {1, 2, 3};
    MutableArraySequence<int> seq(items, 3);

    Sequence<int>* mapped = seq.Map([](int value) { return value * 3; });
    int expected[3] = {3, 6, 9};
    AssertIntSequenceEquals(mapped, expected, 3, "Map");
    Assert(seq.Get(0) == 1, "Map: source sequence must stay unchanged");

    delete mapped;
}

void TestReduce() {
    int items[4] = {1, 2, 3, 4};
    ImmutableArraySequence<int> seq(items, 4);

    int sum = seq.Reduce([](int left, int right) { return left + right; });
    Assert(sum == 10, "Reduce: wrong sum");
}

void TestFold() {
    int items[3] = {2, 3, 4};
    MutableArraySequence<int> seq(items, 3);

    int folded = seq.Fold(1, [](int acc, int value) { return acc * value; });
    Assert(folded == 24, "Fold: wrong result");
}

void TestWhereAndNoMatches() {
    int items[5] = {1, 2, 3, 4, 5};
    MutableListSequence<int> seq(items, 5);

    Sequence<int>* even = seq.Where([](int value) { return value % 2 == 0; });
    int expectedEven[2] = {2, 4};
    AssertIntSequenceEquals(even, expectedEven, 2, "Where even");

    Sequence<int>* none = seq.Where([](int value) { return value > 100; });
    Assert(none->GetLength() == 0, "Where: no matches should produce empty sequence");

    delete even;
    delete none;
}

void TestFind() {
    int items[4] = {5, 10, 15, 20};
    ImmutableListSequence<int> seq(items, 4);

    int found = seq.Find([](int value) { return value == 15; });
    int notFound = seq.Find([](int value) { return value == 99; });

    Assert(found == 2, "Find: wrong index for existing value");
    Assert(notFound == -1, "Find: must return -1 if no value found");
}

void TestIteration() {
    int items[4] = {1, 2, 3, 4};
    MutableArraySequence<int> arraySeq(items, 4);
    MutableListSequence<int> listSeq(items, 4);

    int arraySum = 0;
    IEnumerator<int>* arrayEnumerator = arraySeq.GetEnumerator();
    while (arrayEnumerator->MoveNext()) {
        arraySum += arrayEnumerator->Current();
    }
    delete arrayEnumerator;

    int listSum = 0;
    IEnumerator<int>* listEnumerator = listSeq.GetEnumerator();
    while (listEnumerator->MoveNext()) {
        listSum += listEnumerator->Current();
    }
    delete listEnumerator;

    Assert(arraySum == 10, "Iteration: wrong sum for array sequence");
    Assert(listSum == 10, "Iteration: wrong sum for list sequence");
}

void TestEnumeratorCurrentConstReference() {
    std::string items[2] = {"alpha", "beta"};
    MutableDequeSequence<std::string> seq(items, 2);
    HeapCleaner<IEnumerator<std::string>> enumerator(seq.GetEnumerator());

    static_assert(std::is_same<decltype(enumerator->Current()), std::string>::value,
                  "IEnumerator::Current must return T by value");
    static_assert(std::is_same<decltype(enumerator->CurrentRef()), const std::string&>::value,
                  "IEnumerator::CurrentRef must return const T&");

    Assert(enumerator->MoveNext(), "Enumerator Current const ref: MoveNext failed");
    std::string firstCopy = enumerator->Current();
    const std::string& first = enumerator->CurrentRef();
    Assert(firstCopy == "alpha", "Enumerator Current copy: wrong first value");
    Assert(first == "alpha", "Enumerator Current const ref: wrong first value");
}

void TestSplitWithoutDelimiters() {
    int items[3] = {1, 2, 3};
    MutableArraySequence<int> seq(items, 3);

    DynamicArray<Sequence<int>*> parts = seq.Split([](int value) { return value == 0; });
    Assert(parts.GetSize() == 1, "Split: without delimiters there must be one part");

    int expected[3] = {1, 2, 3};
    AssertIntSequenceEquals(parts.Get(0), expected, 3, "Split: wrong part content");

    delete parts.Get(0);
}

void TestSplitBoundaries() {
    int items[5] = {0, 1, 0, 2, 0};
    MutableArraySequence<int> seq(items, 5);

    DynamicArray<Sequence<int>*> parts = seq.Split([](int value) { return value == 0; }, false);
    Assert(parts.GetSize() == 2, "Split: wrong count when delimiter at start/end and keepEmpty=false");

    int expectedA[1] = {1};
    int expectedB[1] = {2};
    AssertIntSequenceEquals(parts.Get(0), expectedA, 1, "Split part 0");
    AssertIntSequenceEquals(parts.Get(1), expectedB, 1, "Split part 1");

    for (int i = 0; i < parts.GetSize(); ++i) {
        delete parts.Get(i);
    }

    DynamicArray<Sequence<int>*> partsWithEmpty = seq.Split([](int value) { return value == 0; }, true);
    Assert(partsWithEmpty.GetSize() == 4,
           "Split: wrong count when delimiter at start/end and keepEmpty=true");

    Assert(partsWithEmpty.Get(0)->GetLength() == 0, "Split keepEmpty: first part should be empty");
    Assert(partsWithEmpty.Get(1)->GetLength() == 1 && partsWithEmpty.Get(1)->Get(0) == 1,
           "Split keepEmpty: second part wrong");
    Assert(partsWithEmpty.Get(2)->GetLength() == 1 && partsWithEmpty.Get(2)->Get(0) == 2,
           "Split keepEmpty: third part wrong");
    Assert(partsWithEmpty.Get(3)->GetLength() == 0, "Split keepEmpty: last part should be empty");

    for (int i = 0; i < partsWithEmpty.GetSize(); ++i) {
        delete partsWithEmpty.Get(i);
    }
}

void TestSliceCountZero() {
    int items[3] = {4, 5, 6};
    MutableListSequence<int> seq(items, 3);

    Sequence<int>* sliced = seq.Slice(1, 0);
    int expected[3] = {4, 5, 6};
    AssertIntSequenceEquals(sliced, expected, 3, "Slice count=0");

    delete sliced;
}

void TestSliceRemoveAll() {
    int items[3] = {4, 5, 6};
    ImmutableArraySequence<int> seq(items, 3);

    Sequence<int>* sliced = seq.Slice(0, 3);
    Assert(sliced->GetLength() == 0, "Slice remove all: result must be empty");

    delete sliced;
}

void TestSliceWithReplacement() {
    int items[4] = {1, 2, 3, 4};
    MutableArraySequence<int> seq(items, 4);

    int replacementItems[2] = {9, 8};
    ImmutableListSequence<int> replacement(replacementItems, 2);

    Sequence<int>* sliced = seq.Slice(1, 2, &replacement);
    int expected[4] = {1, 9, 8, 4};
    AssertIntSequenceEquals(sliced, expected, 4, "Slice with replacement");

    delete sliced;
}

void TestZipDifferentLength() {
    int leftItems[3] = {1, 2, 3};
    int rightItems[2] = {10, 20};

    MutableArraySequence<int> left(leftItems, 3);
    MutableListSequence<int> right(rightItems, 2);

    Sequence<Pair<int, int>>* zipped = Zip(left, right);
    Assert(zipped->GetLength() == 2, "Zip: length must be min(len1, len2)");

    Pair<int, int> first = zipped->Get(0);
    Pair<int, int> second = zipped->Get(1);
    Assert(first.first == 1 && first.second == 10, "Zip: wrong first pair");
    Assert(second.first == 2 && second.second == 20, "Zip: wrong second pair");

    Pair<Sequence<int>*, Sequence<int>*> unzipped = Unzip(*zipped);
    int expectedFirst[2] = {1, 2};
    int expectedSecond[2] = {10, 20};
    AssertIntSequenceEquals(unzipped.first, expectedFirst, 2, "Unzip first");
    AssertIntSequenceEquals(unzipped.second, expectedSecond, 2, "Unzip second");

    delete zipped;
    delete unzipped.first;
    delete unzipped.second;
}

void TestUnzipEmpty() {
    MutableArraySequence<Pair<int, int>> zipped;
    Pair<Sequence<int>*, Sequence<int>*> unzipped = Unzip(zipped);

    Assert(unzipped.first->GetLength() == 0, "Unzip empty: first sequence must be empty");
    Assert(unzipped.second->GetLength() == 0, "Unzip empty: second sequence must be empty");

    delete unzipped.first;
    delete unzipped.second;
}

void TestReduceOnEmpty() {
    MutableArraySequence<int> empty;

    bool caught = false;
    try {
        int reduced = empty.Reduce([](int left, int right) { return left + right; });
        (void)reduced;
    } catch (const LabException&) {
        caught = true;
    }

    Assert(caught, "Reduce: empty sequence must throw");
}

void TestDequeSequenceSubsequenceAndConcat() {
    int items[4] = {1, 2, 3, 4};
    MutableDequeSequence<int> seq(items, 4);

    Sequence<int>* sub = seq.GetSubsequence(1, 3);
    int expectedSub[3] = {2, 3, 4};
    AssertIntSequenceEquals(sub, expectedSub, 3, "DequeSequence: subsequence");

    Sequence<int>* concat = seq.Concat(sub);
    int expectedConcat[7] = {1, 2, 3, 4, 2, 3, 4};
    AssertIntSequenceEquals(concat, expectedConcat, 7, "DequeSequence: concat");

    delete sub;
    delete concat;
}

void TestDequeSequenceAlgorithms() {
    int items[5] = {1, 2, 3, 4, 5};
    MutableDequeSequence<int> seq(items, 5);

    Sequence<int>* mapped = seq.Map([](int value) { return value * 2; });
    int expectedMapped[5] = {2, 4, 6, 8, 10};
    AssertIntSequenceEquals(mapped, expectedMapped, 5, "DequeSequence: map");

    Sequence<int>* filtered = seq.Where([](int value) { return value % 2 == 1; });
    int expectedFiltered[3] = {1, 3, 5};
    AssertIntSequenceEquals(filtered, expectedFiltered, 3, "DequeSequence: where");

    int reduced = seq.Reduce([](int left, int right) { return left + right; });
    Assert(reduced == 15, "DequeSequence: reduce");

    delete mapped;
    delete filtered;
}

void TestDequeSequencePopOperations() {
    int items[4] = {1, 2, 3, 4};
    MutableDequeSequence<int> mutableSeq(items, 4);
    Sequence<int>* same = mutableSeq.PopBack();
    Assert(same == &mutableSeq, "MutableDequeSequence: PopBack must return same object");

    int expectedMutable[3] = {1, 2, 3};
    AssertIntSequenceEquals(&mutableSeq, expectedMutable, 3, "MutableDequeSequence: PopBack result");

    ImmutableDequeSequence<int> immutableSeq(items, 4);
    Sequence<int>* popped = immutableSeq.PopFront();

    int expectedImmutable[3] = {2, 3, 4};
    AssertIntSequenceEquals(popped, expectedImmutable, 3, "ImmutableDequeSequence: PopFront result");
    Assert(immutableSeq.GetLength() == 4, "ImmutableDequeSequence: original changed after PopFront");

    delete popped;
}

void TestStringSequencesAcrossBackends() {
    std::string items[3] = {"red", "green", "blue"};
    MutableArraySequence<std::string> arraySeq(items, 3);
    ImmutableListSequence<std::string> listSeq(items, 3);
    MutableDequeSequence<std::string> dequeSeq(items, 3);

    Assert(arraySeq == listSeq, "String sequences: different backends must compare equal");
    Assert(dequeSeq == listSeq, "String sequences: deque backend must compare equal");

    Sequence<std::string>* mapped = dequeSeq.Map([](std::string value) { return value + "!"; });
    std::string expectedMapped[3] = {"red!", "green!", "blue!"};
    AssertStringSequenceEquals(mapped, expectedMapped, 3, "String sequences: map");

    Sequence<std::string>* filtered = arraySeq.Where([](const std::string& value) {
        return value.size() > 3;
    });
    std::string expectedFiltered[2] = {"green", "blue"};
    AssertStringSequenceEquals(filtered, expectedFiltered, 2, "String sequences: where");

    Sequence<std::string>* concat = dequeSeq.Concat(&listSeq);
    std::string expectedConcat[6] = {"red", "green", "blue", "red", "green", "blue"};
    AssertStringSequenceEquals(concat, expectedConcat, 6, "String sequences: concat");

    delete mapped;
    delete filtered;
    delete concat;
}

void TestDoubleDequeSequenceAlgorithms() {
    double items[4] = {1.5, 2.5, 3.5, 4.5};
    MutableDequeSequence<double> seq(items, 4);

    Sequence<double>* mapped = seq.Map([](double value) { return value + 0.5; });
    double expectedMapped[4] = {2.0, 3.0, 4.0, 5.0};
    AssertDoubleSequenceNear(mapped, expectedMapped, 4, 1e-9, "Double deque: map");

    Sequence<double>* filtered = seq.Where([](double value) { return value >= 3.0; });
    double expectedFiltered[2] = {3.5, 4.5};
    AssertDoubleSequenceNear(filtered, expectedFiltered, 2, 1e-9, "Double deque: where");

    double reduced = seq.Reduce([](double left, double right) { return left + right; });
    double expectedReduced = 12.0;
    double difference = reduced - expectedReduced;
    if (difference < 0) {
        difference = -difference;
    }
    Assert(difference <= 1e-9, "Double deque: reduce");

    Sequence<double>* sliced = seq.Slice(1, 2);
    double expectedSliced[2] = {1.5, 4.5};
    AssertDoubleSequenceNear(sliced, expectedSliced, 2, 1e-9, "Double deque: slice");

    delete mapped;
    delete filtered;
    delete sliced;
}

bool RunTestEntries(const TestEntry* tests, int testCount, int& passed, int& failed) {
    int initialFailed = failed;

    for (int i = 0; i < testCount; ++i) {
        try {
            tests[i].func();
            std::cout << "[PASS] " << tests[i].name << "\n";
            ++passed;
        } catch (const TestFailedException& e) {
            std::cout << "[FAIL] " << tests[i].name << " -> " << e.GetMessage() << "\n";
            ++failed;
        } catch (const LabException& e) {
            std::cout << "[FAIL] " << tests[i].name << " -> unexpected LabException: "
                      << e.GetMessage() << "\n";
            ++failed;
        } catch (...) {
            std::cout << "[FAIL] " << tests[i].name << " -> unknown exception\n";
            ++failed;
        }
    }

    return failed == initialFailed;
}

bool RunAllTests() {
    int passed = 0;
    int failed = 0;

    TestEntry sequenceTests[] = {
        {"DynamicArray: create/get", TestDynamicArrayCreateAndGet},
        {"DynamicArray: default constructor/index operator", TestDynamicArrayDefaultConstructorAndIndexOperator},
        {"DynamicArray: copy", TestDynamicArrayCopy},
        {"DynamicArray: resize/set/get", TestDynamicArrayResizeSetGet},
        {"DynamicArray: push_back", TestDynamicArrayPushBack},
        {"DynamicArray: index errors", TestDynamicArrayIndexErrors},
        {"DynamicArray: nested storage", TestDynamicArrayNestedStorage},
        {"DynamicArray: string/double storage", TestDynamicArrayStringAndDoubleStorage},
        {"LinkedList: append/prepend/insert/get", TestLinkedListAppendPrependInsertGet},
        {"LinkedList: sublist/concat", TestLinkedListSubListAndConcat},
        {"Deque: append/prepend/insert/get", TestDequeAppendPrependInsertGet},
        {"Deque: wrap-around/pop", TestDequeWrapAroundAndPop},
        {"Deque: index and empty errors", TestDequeIndexAndEmptyErrors},
        {"Deque: segment boundaries", TestDequeSegmentBoundaries},
        {"Deque: copy and insert across segments", TestDequeCopyAndInsertAcrossSegments},
        {"Deque: mutable access for matrix storage", TestDequeMutableAccessForMatrixStorage},
        {"Deque: matrix storage factory", TestDequeMatrixStorageFactory},
        {"Sequence: abstraction", TestSequenceInterfaceSubsequenceAndConcat},
        {"MutableSequence: semantics", TestMutableSemantics},
        {"ImmutableArraySequence: semantics", TestImmutableArraySemantics},
        {"ImmutableListSequence: semantics", TestImmutableListSemantics},
        {"MutableDequeSequence: semantics", TestMutableDequeSemantics},
        {"ImmutableDequeSequence: semantics", TestImmutableDequeSemantics},
        {"ImmutableSequence: subsequence", TestImmutableSubsequence},
        {"ICollection: contract", TestICollectionContract},
        {"Operators: []", TestOperatorIndex},
        {"Operators: == / !=", TestOperatorEqualAndNotEqual},
        {"Operators: +", TestOperatorPlus},
        {"Operators: <<", TestOperatorOutput},
        {"Map", TestMap},
        {"Reduce", TestReduce},
        {"Fold", TestFold},
        {"Where", TestWhereAndNoMatches},
        {"Find", TestFind},
        {"Iteration", TestIteration},
        {"Enumerator: Current/CurrentRef", TestEnumeratorCurrentConstReference},
        {"Split: no delimiters", TestSplitWithoutDelimiters},
        {"Split: boundaries", TestSplitBoundaries},
        {"Slice: count=0", TestSliceCountZero},
        {"Slice: remove all", TestSliceRemoveAll},
        {"Slice: replacement", TestSliceWithReplacement},
        {"Zip", TestZipDifferentLength},
        {"Unzip: empty", TestUnzipEmpty},
        {"Reduce: empty", TestReduceOnEmpty},
        {"DequeSequence: subsequence/concat", TestDequeSequenceSubsequenceAndConcat},
        {"DequeSequence: algorithms", TestDequeSequenceAlgorithms},
        {"DequeSequence: pop operations", TestDequeSequencePopOperations},
        {"String sequences: across backends", TestStringSequencesAcrossBackends},
        {"Double deque: algorithms", TestDoubleDequeSequenceAlgorithms},
    };

    int sequenceTestCount = static_cast<int>(sizeof(sequenceTests) / sizeof(sequenceTests[0]));
    RunTestEntries(sequenceTests, sequenceTestCount, passed, failed);
    RunMatrixTests(passed, failed);

    std::cout << "\nTests passed: " << passed << "\n";
    std::cout << "Tests failed: " << failed << "\n";

    return failed == 0;
}

#ifdef TESTS_MAIN
int main() {
    bool ok = RunAllTests();
    return ok ? 0 : 1;
}
#endif
