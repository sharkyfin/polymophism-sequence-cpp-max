#ifndef DEQUE_H
#define DEQUE_H

#include <climits>

#include "dynamic_array.hpp"
#include "exceptions.hpp"
#include "ienumerator.hpp"

template <class T>
class Deque {
private:
    static const int defaultMapSize = 8;
    static const int defaultSegmentSize = 8;

    DynamicArray<T*> segments;
    int firstSegment;
    int firstIndex;
    int length;
    int segmentSize;
    bool rowSegmentedStorage;

    int MapSize() const {
        return segments.GetSize();
    }

    int UsedSegmentCount() const {
        if (length == 0) {
            return 1;
        }

        return (firstIndex + length - 1) / segmentSize + 1;
    }

    int SegmentIndex(int logicalIndex) const {
        return firstSegment + (firstIndex + logicalIndex) / segmentSize;
    }

    int IndexInSegment(int logicalIndex) const {
        return (firstIndex + logicalIndex) % segmentSize;
    }

    void AllocateSegment(int segmentIndex) {
        if (segments.Get(segmentIndex) == nullptr) {
            segments.Set(segmentIndex, new T[segmentSize]);
        }
    }

    void DeleteSegments() {
        for (int i = 0; i < MapSize(); ++i) {
            T* segment = segments.Get(i);
            if (segment != nullptr) {
                delete[] segment;
                segments.Set(i, nullptr);
            }
        }
    }

    void InitializeMap(int mapSize) {
        if (mapSize <= 0) {
            throw InvalidArgumentException("Deque: invalid map size");
        }

        DynamicArray<T*> newSegments(mapSize);
        for (int i = 0; i < mapSize; ++i) {
            newSegments.Set(i, nullptr);
        }

        segments.Swap(newSegments);
        firstSegment = mapSize / 2;
        firstIndex = segmentSize / 2;
        length = 0;
        rowSegmentedStorage = false;
        AllocateSegment(firstSegment);
    }

    void ResetEmptyState() {
        DeleteSegments();
        firstSegment = MapSize() / 2;
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
        DynamicArray<T*> newSegments(neededSegments);
        for (int i = 0; i < neededSegments; ++i) {
            newSegments.Set(i, nullptr);
        }

        try {
            for (int segment = 0; segment < neededSegments; ++segment) {
                newSegments.Set(segment, new T[newSegmentSize]);
            }

            for (int i = 0; i < itemCount; ++i) {
                int segmentIndex = i / newSegmentSize;
                int indexInSegment = i % newSegmentSize;
                newSegments.Get(segmentIndex)[indexInSegment] = value;
            }
        } catch (...) {
            for (int segment = 0; segment < neededSegments; ++segment) {
                T* allocatedSegment = newSegments.Get(segment);
                if (allocatedSegment != nullptr) {
                    delete[] allocatedSegment;
                    newSegments.Set(segment, nullptr);
                }
            }
            throw;
        }

        DeleteSegments();
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
        int usedSegmentCount = UsedSegmentCount();
        int newMapSize = (MapSize() > 0) ? MapSize() * 2 : defaultMapSize;
        while (newMapSize <= usedSegmentCount + 1) {
            newMapSize *= 2;
        }

        DynamicArray<T*> newSegments(newMapSize);
        for (int i = 0; i < newMapSize; ++i) {
            newSegments.Set(i, nullptr);
        }

        int newFirstSegment = (newMapSize - usedSegmentCount) / 2;
        for (int i = 0; i < usedSegmentCount; ++i) {
            newSegments.Set(newFirstSegment + i, segments.Get(firstSegment + i));
        }

        segments.Swap(newSegments);
        firstSegment = newFirstSegment;
    }

    void EnsureMapForAppend() {
        if (length == 0) {
            AllocateSegment(firstSegment);
            return;
        }

        while (SegmentIndex(length) >= MapSize()) {
            GrowSegmentMap();
        }

        AllocateSegment(SegmentIndex(length));
    }

    void EnsureMapForPrepend() {
        if (length == 0) {
            AllocateSegment(firstSegment);
            return;
        }

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
        InitializeMap(defaultMapSize);
    }

    Deque(T* items, int count) : segments(0), firstSegment(0), firstIndex(0), length(0),
                                 segmentSize(defaultSegmentSize), rowSegmentedStorage(false) {
        if (count < 0) {
            throw InvalidArgumentException("Deque: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("Deque: null items pointer with positive count");
        }

        try {
            InitializeMap(defaultMapSize);
            for (int i = 0; i < count; ++i) {
                Append(items[i]);
            }
        } catch (...) {
            DeleteSegments();
            throw;
        }
    }

    Deque(int count, const T& value, int customSegmentSize)
        : segments(0), firstSegment(0), firstIndex(0), length(0),
          segmentSize(defaultSegmentSize), rowSegmentedStorage(false) {
        InitializeFixedStorage(count, value, customSegmentSize, false);
    }

    Deque(const Deque<T>& other) : segments(0), firstSegment(0), firstIndex(0), length(0),
                                  segmentSize(defaultSegmentSize), rowSegmentedStorage(false) {
        try {
            if (other.rowSegmentedStorage) {
                InitializeFixedStorage(other.length, T(), other.segmentSize, true);
                for (int i = 0; i < other.length; ++i) {
                    Set(i, other.Get(i));
                }
            } else {
                segmentSize = other.segmentSize;
                InitializeMap(other.MapSize() > defaultMapSize ? other.MapSize() : defaultMapSize);
                firstIndex = other.firstIndex;
                for (int i = 0; i < other.length; ++i) {
                    Append(other.Get(i));
                }
            }
        } catch (...) {
            DeleteSegments();
            throw;
        }
    }

    Deque<T>& operator=(const Deque<T>& other) {
        if (this == &other) {
            return *this;
        }

        Deque<T> copy(other);
        Swap(copy);
        return *this;
    }

    ~Deque() {
        DeleteSegments();
    }

    static Deque<T> CreateForMatrix(int rowCount, int colCount, const T& value = T()) {
        if (rowCount <= 0 || colCount <= 0) {
            throw InvalidArgumentException("Deque: matrix sizes must be positive");
        }
        if (rowCount > INT_MAX / colCount) {
            throw InvalidArgumentException("Deque: matrix storage is too large");
        }

        Deque<T> result;
        result.InitializeFixedStorage(rowCount * colCount, value, colCount, true);
        return result;
    }

    static Deque<T> CreateFixed(int count, const T& value = T()) {
        if (count < 0) {
            throw InvalidArgumentException("Deque: negative fixed storage size");
        }
        if (count == 0) {
            return Deque<T>();
        }

        Deque<T> result;
        int fixedSegmentSize = (count > defaultSegmentSize) ? count : defaultSegmentSize;
        result.InitializeFixedStorage(count, value, fixedSegmentSize, false);
        return result;
    }

    const T& GetFirst() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        return segments.Get(firstSegment)[firstIndex];
    }

    const T& GetLast() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        int logicalIndex = length - 1;
        return segments.Get(SegmentIndex(logicalIndex))[IndexInSegment(logicalIndex)];
    }

    T& Get(int index) {
        CheckIndex(index);
        return segments.Get(SegmentIndex(index))[IndexInSegment(index)];
    }

    const T& Get(int index) const {
        CheckIndex(index);
        return segments.Get(SegmentIndex(index))[IndexInSegment(index)];
    }

    int GetLength() const {
        return length;
    }

    T* GetMatrixRowPointer(int row) {
        CheckMatrixRowIndex(row);
        return segments.Get(row);
    }

    const T* GetMatrixRowPointer(int row) const {
        CheckMatrixRowIndex(row);
        return segments.Get(row);
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

        T* temporary = segments.Get(firstRow);
        segments.Set(firstRow, segments.Get(secondRow));
        segments.Set(secondRow, temporary);
    }

    void Append(const T& item) {
        rowSegmentedStorage = false;
        EnsureMapForAppend();

        int segmentIndex = firstSegment;
        int indexInSegment = firstIndex;
        if (length > 0) {
            segmentIndex = SegmentIndex(length);
            indexInSegment = IndexInSegment(length);
        }

        segments.Get(segmentIndex)[indexInSegment] = item;
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

        segments.Get(firstSegment)[firstIndex] = item;
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
            ResetEmptyState();
            return;
        }

        int oldFirstSegment = firstSegment;
        ++firstIndex;
        if (firstIndex == segmentSize) {
            firstIndex = 0;
            ++firstSegment;
            delete[] segments.Get(oldFirstSegment);
            segments.Set(oldFirstSegment, nullptr);
        }

        --length;
    }

    void PopBack() {
        if (length == 0) {
            throw EmptyCollectionException("Deque: PopBack on empty deque");
        }

        rowSegmentedStorage = false;

        if (length == 1) {
            ResetEmptyState();
            return;
        }

        int oldLastSegment = SegmentIndex(length - 1);
        --length;
        int newLastSegment = SegmentIndex(length - 1);
        if (oldLastSegment != newLastSegment) {
            delete[] segments.Get(oldLastSegment);
            segments.Set(oldLastSegment, nullptr);
        }
    }

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(this);
    }
};

#endif
