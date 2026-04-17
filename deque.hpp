#ifndef DEQUE_H
#define DEQUE_H

#include "dynamic_array.hpp"
#include "exceptions.hpp"
#include "ienumerator.hpp"

template <class T>
class Deque {
private:
    static const int defaultMapSize = 8;
    static const int defaultSegmentSize = 8;

    DynamicArray<DynamicArray<T>> segments;
    int firstSegment;
    int firstIndex;
    int length;
    int segmentSize;
    bool rowSegmentedStorage;

    int GetMapSize() const {
        return segments.GetSize();
    }

    int GetUsedSegmentCount() const {
        if (length == 0) {
            return 1;
        }

        return (firstIndex + length - 1) / segmentSize + 1;
    }

    int GetSegmentIndex(int logicalIndex) const {
        return firstSegment + (firstIndex + logicalIndex) / segmentSize;
    }

    int GetSegmentOffset(int logicalIndex) const {
        return (firstIndex + logicalIndex) % segmentSize;
    }

    T& GetElementAtLogicalIndex(int logicalIndex) {
        return segments[GetSegmentIndex(logicalIndex)][GetSegmentOffset(logicalIndex)];
    }

    const T& GetElementAtLogicalIndex(int logicalIndex) const {
        return segments[GetSegmentIndex(logicalIndex)][GetSegmentOffset(logicalIndex)];
    }

    void AllocateSegment(int segmentIndex) {
        if (segments[segmentIndex].GetSize() == 0) {
            DynamicArray<T> newSegment(segmentSize);
            segments[segmentIndex].Swap(newSegment);
        }
    }

    void ClearSegment(int segmentIndex) {
        if (segments[segmentIndex].GetSize() > 0) {
            DynamicArray<T> emptySegment;
            segments[segmentIndex].Swap(emptySegment);
        }
    }

    void SetEmptyState(int newMapSize) {
        if (newMapSize <= 0) {
            throw InvalidArgumentException("Deque: invalid map size");
        }

        DynamicArray<DynamicArray<T>> newSegments(newMapSize);

        segments.Swap(newSegments);
        firstSegment = newMapSize / 2;
        firstIndex = segmentSize / 2;
        length = 0;
        rowSegmentedStorage = false;
        AllocateSegment(firstSegment);
    }

    void InitializeFixedStorage(int itemCount, const T& value, int newSegmentSize,
                                bool newRowSegmentedStorage) {
        if (itemCount < 0) {
            throw InvalidArgumentException("Deque: negative fixed storage size");
        }
        if (newSegmentSize <= 0) {
            throw InvalidArgumentException("Deque: invalid segment size");
        }

        int neededSegments = (itemCount == 0) ? 1 : (itemCount - 1) / newSegmentSize + 1;
        DynamicArray<DynamicArray<T>> newSegments(neededSegments);

        for (int segment = 0; segment < neededSegments; ++segment) {
            DynamicArray<T> newSegment(newSegmentSize);
            newSegments[segment].Swap(newSegment);
        }

        for (int i = 0; i < itemCount; ++i) {
            int targetSegmentIndex = i / newSegmentSize;
            int segmentOffset = i % newSegmentSize;
            newSegments[targetSegmentIndex][segmentOffset] = value;
        }

        segments.Swap(newSegments);
        segmentSize = newSegmentSize;
        firstSegment = 0;
        firstIndex = 0;
        length = itemCount;
        rowSegmentedStorage = newRowSegmentedStorage;
    }

    void CheckIndex(int index) const {
        if (index < 0 || index >= length) {
            throw IndexOutOfRangeException("Deque: index out of range");
        }
    }

    void CheckMatrixRowIndex(int row) const {
        if (!rowSegmentedStorage || firstSegment != 0 || firstIndex != 0 ||
            segmentSize <= 0 || length % segmentSize != 0) {
            throw InvalidArgumentException("Deque: storage is not row-segmented");
        }

        int rowCount = length / segmentSize;
        if (row < 0 || row >= rowCount) {
            throw IndexOutOfRangeException("Deque: matrix row index out of range");
        }
    }

    void GrowSegmentMap() {
        int usedSegmentCount = GetUsedSegmentCount();
        int newMapSize = (GetMapSize() > 0) ? GetMapSize() * 2 : defaultMapSize;
        while (newMapSize <= usedSegmentCount + 1) {
            newMapSize *= 2;
        }

        DynamicArray<DynamicArray<T>> newSegments(newMapSize);

        int newFirstSegment = (newMapSize - usedSegmentCount) / 2;
        for (int i = 0; i < usedSegmentCount; ++i) {
            newSegments[newFirstSegment + i].Swap(segments[firstSegment + i]);
        }

        segments.Swap(newSegments);
        firstSegment = newFirstSegment;
    }

    void EnsureMapForAppend() {
        while (GetSegmentIndex(length) >= GetMapSize()) {
            GrowSegmentMap();
        }

        AllocateSegment(GetSegmentIndex(length));
    }

    void EnsureMapForPrepend() {
        if (firstIndex > 0) {
            return;
        }

        while (firstSegment == 0) {
            GrowSegmentMap();
        }

        AllocateSegment(firstSegment - 1);
    }

    void Swap(Deque<T>& other) {
        segments.Swap(other.segments);

        int tempFirstSegment = firstSegment;
        firstSegment = other.firstSegment;
        other.firstSegment = tempFirstSegment;

        int tempFirstIndex = firstIndex;
        firstIndex = other.firstIndex;
        other.firstIndex = tempFirstIndex;

        int tempLength = length;
        length = other.length;
        other.length = tempLength;

        int tempSegmentSize = segmentSize;
        segmentSize = other.segmentSize;
        other.segmentSize = tempSegmentSize;

        bool tempRowSegmentedStorage = rowSegmentedStorage;
        rowSegmentedStorage = other.rowSegmentedStorage;
        other.rowSegmentedStorage = tempRowSegmentedStorage;
    }

public:
    class Enumerator : public IEnumerator<T> {
    private:
        const Deque<T>* deque;
        int index;
        bool started;

    public:
        explicit Enumerator(const Deque<T>* owner)
            : deque(owner), index(-1), started(false) {}

        bool MoveNext() override {
            if (!started) {
                index = 0;
                started = true;
            } else {
                ++index;
            }

            return index < deque->GetLength();
        }

        T Current() const override {
            return CurrentRef();
        }

        const T& CurrentRef() const override {
            if (!started || index < 0 || index >= deque->GetLength()) {
                throw EmptyCollectionException("Deque::Enumerator: no current element");
            }

            return deque->Get(index);
        }

        void Reset() override {
            index = -1;
            started = false;
        }
    };

    Deque() : segments(0), firstSegment(0), firstIndex(0), length(0),
              segmentSize(defaultSegmentSize), rowSegmentedStorage(false) {
        SetEmptyState(defaultMapSize);
    }

    Deque(T* items, int count) : segments(0), firstSegment(0), firstIndex(0), length(0),
                                 segmentSize(defaultSegmentSize), rowSegmentedStorage(false) {
        if (count < 0) {
            throw InvalidArgumentException("Deque: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("Deque: null items pointer with positive count");
        }

        SetEmptyState(defaultMapSize);
        for (int i = 0; i < count; ++i) {
            Append(items[i]);
        }
    }

    Deque(int count, const T& value, int customSegmentSize)
        : segments(0), firstSegment(0), firstIndex(0), length(0),
          segmentSize(defaultSegmentSize), rowSegmentedStorage(false) {
        InitializeFixedStorage(count, value, customSegmentSize, false);
    }

    Deque(const Deque<T>& other) = default;

    Deque<T>& operator=(const Deque<T>& other) = default;

    static Deque<T> CreateMatrixStorage(int rowCount, int colCount, const T& value = T()) {
        Deque<T> result;
        result.InitializeFixedStorage(rowCount * colCount, value, colCount, true);
        return result;
    }

    static Deque<T> CreateVectorStorage(int count, const T& value = T()) {
        if (count < 0) {
            throw InvalidArgumentException("Deque: negative filled storage size");
        }
        if (count == 0) {
            return Deque<T>();
        }

        Deque<T> result;
        int filledSegmentSize = (count > defaultSegmentSize) ? count : defaultSegmentSize;
        result.InitializeFixedStorage(count, value, filledSegmentSize, false);
        return result;
    }

    const T& GetFirst() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        return segments[firstSegment][firstIndex];
    }

    const T& GetLast() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        int logicalIndex = length - 1;
        return GetElementAtLogicalIndex(logicalIndex);
    }

    T& Get(int index) {
        CheckIndex(index);
        return GetElementAtLogicalIndex(index);
    }

    const T& Get(int index) const {
        CheckIndex(index);
        return GetElementAtLogicalIndex(index);
    }

    int GetLength() const {
        return length;
    }

    T* GetMatrixRowPointer(int row) {
        CheckMatrixRowIndex(row);
        return segments[row].Data();
    }

    const T* GetMatrixRowPointer(int row) const {
        CheckMatrixRowIndex(row);
        return segments[row].Data();
    }

    T& operator[](int index) {
        return Get(index);
    }

    const T& operator[](int index) const {
        return Get(index);
    }

    void Set(int index, const T& item) {
        Get(index) = item;
    }

    void SwapElements(int first, int second) {
        CheckIndex(first);
        CheckIndex(second);

        if (first == second) {
            return;
        }

        T temporary = Get(first);
        Set(first, Get(second));
        Set(second, temporary);
    }

    void SwapRowsMatrix(int firstRow, int secondRow) {
        CheckMatrixRowIndex(firstRow);
        CheckMatrixRowIndex(secondRow);

        if (firstRow == secondRow) {
            return;
        }

        segments[firstRow].Swap(segments[secondRow]);
    }

    void Append(const T& item) {
        rowSegmentedStorage = false;
        EnsureMapForAppend();

        int targetSegmentIndex = firstSegment;
        int segmentOffset = firstIndex;
        if (length > 0) {
            targetSegmentIndex = GetSegmentIndex(length);
            segmentOffset = GetSegmentOffset(length);
        }

        segments[targetSegmentIndex][segmentOffset] = item;
        ++length;
    }

    void Prepend(const T& item) {
        rowSegmentedStorage = false;
        EnsureMapForPrepend();

        if (length > 0) {
            if (firstIndex == 0) {
                --firstSegment;
                firstIndex = segmentSize - 1;
            } else {
                --firstIndex;
            }
        }

        segments[firstSegment][firstIndex] = item;
        ++length;
    }

    void InsertAt(const T& item, int index) {
        if (index < 0 || index > length) {
            throw IndexOutOfRangeException("Deque: index out of range in InsertAt");
        }

        if (index == 0) {
            Prepend(item);
            return;
        }

        if (index == length) {
            Append(item);
            return;
        }

        Deque<T> newDeque;
        for (int i = 0; i < index; ++i) {
            newDeque.Append(Get(i));
        }
        newDeque.Append(item);
        for (int i = index; i < length; ++i) {
            newDeque.Append(Get(i));
        }

        Swap(newDeque);
    }

    void PopFront() {
        if (length == 0) {
            throw EmptyCollectionException("Deque: PopFront on empty deque");
        }

        rowSegmentedStorage = false;

        if (length == 1) {
            SetEmptyState(GetMapSize());
            return;
        }

        int oldFirstSegment = firstSegment;
        ++firstIndex;
        if (firstIndex == segmentSize) {
            firstIndex = 0;
            ++firstSegment;
            ClearSegment(oldFirstSegment);
        }

        --length;
    }

    void PopBack() {
        if (length == 0) {
            throw EmptyCollectionException("Deque: PopBack on empty deque");
        }

        rowSegmentedStorage = false;

        if (length == 1) {
            SetEmptyState(GetMapSize());
            return;
        }

        int oldLastSegment = GetSegmentIndex(length - 1);
        --length;
        int newLastSegment = GetSegmentIndex(length - 1);
        if (oldLastSegment != newLastSegment) {
            ClearSegment(oldLastSegment);
        }
    }

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(this);
    }
};

#endif
