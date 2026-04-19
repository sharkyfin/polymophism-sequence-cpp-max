#ifndef DEQUE_H
#define DEQUE_H

#include "segmented_buffer.hpp"
#include "exceptions.hpp"
#include "ienumerator.hpp"

template <class T>
class Deque {
private:
    static const int defaultMapSize = 8;
    static const int defaultSegmentSize = 8;

    SegmentedBuffer<T> buffer;
    int firstSegment;
    int firstIndex;
    int length;

    int GetMapSize() const {
        return buffer.GetSegmentCount();
    }

    int GetSegmentSize() const {
        return buffer.GetSegmentSize();
    }

    int GetUsedSegmentCount() const {
        if (length == 0) {
            return 1;
        }

        return (firstIndex + length - 1) / GetSegmentSize() + 1;
    }

    int GetSegmentIndex(int logicalIndex) const {
        return firstSegment + (firstIndex + logicalIndex) / GetSegmentSize();
    }

    int GetSegmentOffset(int logicalIndex) const {
        return (firstIndex + logicalIndex) % GetSegmentSize();
    }

    T& GetElementAtLogicalIndex(int logicalIndex) {
        return buffer.GetSegment(GetSegmentIndex(logicalIndex))[GetSegmentOffset(logicalIndex)];
    }

    const T& GetElementAtLogicalIndex(int logicalIndex) const {
        return buffer.GetSegment(GetSegmentIndex(logicalIndex))[GetSegmentOffset(logicalIndex)];
    }

    void AllocateSegment(int segmentIndex) {
        buffer.AllocateSegment(segmentIndex);
    }

    void ClearSegment(int segmentIndex) {
        buffer.ClearSegment(segmentIndex);
    }

    void SetEmptyState(int newMapSize) {
        if (newMapSize <= 0) {
            throw InvalidArgumentException("Deque: invalid map size");
        }

        int newSegmentSize = (buffer.GetSegmentSize() > 0) ? buffer.GetSegmentSize() : defaultSegmentSize;
        SegmentedBuffer<T> newBuffer(newMapSize, newSegmentSize);
        buffer.Swap(newBuffer);
        firstSegment = newMapSize / 2;
        firstIndex = GetSegmentSize() / 2;
        length = 0;
        AllocateSegment(firstSegment);
    }

    void InitializeFilledStorage(int itemCount, const T& value, int newSegmentSize) {
        if (itemCount < 0) {
            throw InvalidArgumentException("Deque: negative filled storage size");
        }
        if (newSegmentSize <= 0) {
            throw InvalidArgumentException("Deque: invalid segment size");
        }

        int neededSegments = (itemCount == 0) ? 1 : (itemCount - 1) / newSegmentSize + 1;
        SegmentedBuffer<T> newBuffer(neededSegments, newSegmentSize);

        for (int segment = 0; segment < neededSegments; ++segment) {
            newBuffer.AllocateSegment(segment);
        }

        for (int i = 0; i < itemCount; ++i) {
            int targetSegmentIndex = i / newSegmentSize;
            int segmentOffset = i % newSegmentSize;
            newBuffer.GetSegment(targetSegmentIndex)[segmentOffset] = value;
        }

        buffer.Swap(newBuffer);
        firstSegment = 0;
        firstIndex = 0;
        length = itemCount;
    }

    void CheckIndex(int index) const {
        if (index < 0 || index >= length) {
            throw IndexOutOfRangeException("Deque: index out of range");
        }
    }

    void GrowSegmentMap() {
        int usedSegmentCount = GetUsedSegmentCount();
        int newMapSize = (GetMapSize() > 0) ? GetMapSize() * 2 : defaultMapSize;
        while (newMapSize <= usedSegmentCount + 1) {
            newMapSize *= 2;
        }

        SegmentedBuffer<T> newBuffer(newMapSize, GetSegmentSize());

        int newFirstSegment = (newMapSize - usedSegmentCount) / 2;
        for (int i = 0; i < usedSegmentCount; ++i) {
            newBuffer.GetSegment(newFirstSegment + i).Swap(buffer.GetSegment(firstSegment + i));
        }

        buffer.Swap(newBuffer);
        firstSegment = newFirstSegment;
    }

    void EnsureMapForAppend() {
        int logicalLastElementIndex = length;
        while (GetSegmentIndex(logicalLastElementIndex) >= GetMapSize()) {
            GrowSegmentMap();
        }

        AllocateSegment(GetSegmentIndex(logicalLastElementIndex));
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
        buffer.Swap(other.buffer);

        int tempFirstSegment = firstSegment;
        firstSegment = other.firstSegment;
        other.firstSegment = tempFirstSegment;

        int tempFirstIndex = firstIndex;
        firstIndex = other.firstIndex;
        other.firstIndex = tempFirstIndex;

        int tempLength = length;
        length = other.length;
        other.length = tempLength;
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

    Deque() : buffer(), firstSegment(0), firstIndex(0), length(0) {
        SetEmptyState(defaultMapSize);
    }

    Deque(const T* items, int count) : buffer(), firstSegment(0), firstIndex(0), length(0) {
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
        : buffer(), firstSegment(0), firstIndex(0), length(0) {
        InitializeFilledStorage(count, value, customSegmentSize);
    }

    Deque(const Deque<T>& other) = default;

    Deque<T>& operator=(const Deque<T>& other) = default;

    const T& GetFirst() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        return buffer.GetSegment(firstSegment)[firstIndex];
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

    void Append(const T& item) {
        EnsureMapForAppend();

        int targetSegmentIndex = firstSegment;
        int segmentOffset = firstIndex;
        if (length > 0) {
            targetSegmentIndex = GetSegmentIndex(length);
            segmentOffset = GetSegmentOffset(length);
        }

        buffer.GetSegment(targetSegmentIndex)[segmentOffset] = item;
        ++length;
    }

    void Prepend(const T& item) {
        EnsureMapForPrepend();

        if (length > 0) {
            if (firstIndex == 0) {
                --firstSegment;
                firstIndex = GetSegmentSize() - 1;
            } else {
                --firstIndex;
            }
        }

        buffer.GetSegment(firstSegment)[firstIndex] = item;
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

        int oldLength = length;

        if (index < oldLength - index) {
            T firstValue = GetFirst();
            Prepend(firstValue);

            for (int i = 0; i < index; ++i) {
                GetElementAtLogicalIndex(i) = GetElementAtLogicalIndex(i + 1);
            }
        } else {
            T lastValue = GetLast();
            Append(lastValue);

            for (int i = oldLength; i > index; --i) {
                GetElementAtLogicalIndex(i) = GetElementAtLogicalIndex(i - 1);
            }
        }

        GetElementAtLogicalIndex(index) = item;
    }

    void PopFront() {
        if (length == 0) {
            throw EmptyCollectionException("Deque: PopFront on empty deque");
        }

        if (length == 1) {
            SetEmptyState(GetMapSize());
            return;
        }

        int oldFirstSegment = firstSegment;
        ++firstIndex;
        if (firstIndex == GetSegmentSize()) {
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
