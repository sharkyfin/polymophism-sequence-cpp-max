#ifndef DEQUE_H
#define DEQUE_H

#include "core/segmented_buffer.hpp"
#include "core/exceptions.hpp"
#include "core/ienumerator.hpp"

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

    int GetCircularSegmentIndex(int segmentIndex) const {
        int mapSize = GetMapSize();
        int circularSegmentIndex = segmentIndex % mapSize;
        if (circularSegmentIndex < 0) {
            circularSegmentIndex += mapSize;
        }

        return circularSegmentIndex;
    }

    int GetLinearOffset(int logicalIndex) const {
        return firstIndex + logicalIndex;
    }

    int GetUsedSegmentCount() const {
        if (length == 0) {
            return 1;
        }

        int lastLogicalIndex = length - 1;
        int lastLinearOffset = GetLinearOffset(lastLogicalIndex);
        int lastUsedSegmentOffset = lastLinearOffset / GetSegmentSize();
        return lastUsedSegmentOffset + 1;
    }

    int GetSegmentIndex(int logicalIndex) const {
        int linearOffset = GetLinearOffset(logicalIndex);
        int segmentOffsetFromFront = linearOffset / GetSegmentSize();
        int physicalSegmentIndex = firstSegment + segmentOffsetFromFront;
        return GetCircularSegmentIndex(physicalSegmentIndex);
    }

    int GetSegmentOffset(int logicalIndex) const {
        int linearOffset = GetLinearOffset(logicalIndex);
        return linearOffset % GetSegmentSize();
    }

    T& GetElementAtLogicalIndex(int logicalIndex) {
        int segmentIndex = GetSegmentIndex(logicalIndex);
        int segmentOffset = GetSegmentOffset(logicalIndex);
        return buffer.GetSegment(segmentIndex)[segmentOffset];
    }

    const T& GetElementAtLogicalIndex(int logicalIndex) const {
        int segmentIndex = GetSegmentIndex(logicalIndex);
        int segmentOffset = GetSegmentOffset(logicalIndex);
        return buffer.GetSegment(segmentIndex)[segmentOffset];
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
        firstSegment = 0;
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

        int requiredSegmentCount = (itemCount == 0) ? 1 : (itemCount - 1) / newSegmentSize + 1;
        SegmentedBuffer<T> newBuffer(requiredSegmentCount, newSegmentSize);

        for (int segmentIndex = 0; segmentIndex < requiredSegmentCount; ++segmentIndex) {
            newBuffer.AllocateSegment(segmentIndex);
        }

        for (int itemIndex = 0; itemIndex < itemCount; ++itemIndex) {
            int targetSegmentIndex = itemIndex / newSegmentSize;
            int targetSegmentOffset = itemIndex % newSegmentSize;
            newBuffer.GetSegment(targetSegmentIndex)[targetSegmentOffset] = value;
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
        int expandedMapSize = (GetMapSize() > 0) ? GetMapSize() * 2 : defaultMapSize;
        while (expandedMapSize <= usedSegmentCount + 1) {
            expandedMapSize *= 2;
        }

        SegmentedBuffer<T> expandedBuffer(expandedMapSize, GetSegmentSize());

        int centeredFirstSegment = (expandedMapSize - usedSegmentCount) / 2;
        for (int segmentOffset = 0; segmentOffset < usedSegmentCount; ++segmentOffset) {
            int oldPhysicalSegmentIndex = GetCircularSegmentIndex(firstSegment + segmentOffset);
            int newPhysicalSegmentIndex = centeredFirstSegment + segmentOffset;
            expandedBuffer.GetSegment(newPhysicalSegmentIndex).Swap(
                buffer.GetSegment(oldPhysicalSegmentIndex));
        }

        buffer.Swap(expandedBuffer);
        firstSegment = centeredFirstSegment;
    }

    void EnsureMapForAppend() {
        bool appendNeedsNewSegment = length > 0 && GetSegmentOffset(length) == 0;
        while (appendNeedsNewSegment && GetUsedSegmentCount() == GetMapSize()) {
            GrowSegmentMap();
        }

        int appendSegmentIndex = GetSegmentIndex(length);
        AllocateSegment(appendSegmentIndex);
    }

    void EnsureMapForPrepend() {
        bool prependNeedsNewSegment = length > 0 && firstIndex == 0;
        if (!prependNeedsNewSegment) {
            return;
        }

        while (GetUsedSegmentCount() == GetMapSize()) {
            GrowSegmentMap();
        }

        int prependSegmentIndex = GetCircularSegmentIndex(firstSegment - 1);
        AllocateSegment(prependSegmentIndex);
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

        int writeSegmentIndex = firstSegment;
        int writeSegmentOffset = firstIndex;
        if (length > 0) {
            writeSegmentIndex = GetSegmentIndex(length);
            writeSegmentOffset = GetSegmentOffset(length);
        }

        buffer.GetSegment(writeSegmentIndex)[writeSegmentOffset] = item;
        ++length;
    }

    void Prepend(const T& item) {
        EnsureMapForPrepend();

        if (length > 0) {
            if (firstIndex == 0) {
                firstSegment = GetCircularSegmentIndex(firstSegment - 1);
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
        int distanceToFront = index;
        int distanceToBack = oldLength - index;

        if (distanceToFront < distanceToBack) {
            T firstValue = GetFirst();
            Prepend(firstValue);

            for (int logicalIndex = 0; logicalIndex < index; ++logicalIndex) {
                GetElementAtLogicalIndex(logicalIndex) =
                    GetElementAtLogicalIndex(logicalIndex + 1);
            }
        } else {
            T lastValue = GetLast();
            Append(lastValue);

            for (int logicalIndex = oldLength; logicalIndex > index; --logicalIndex) {
                GetElementAtLogicalIndex(logicalIndex) =
                    GetElementAtLogicalIndex(logicalIndex - 1);
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

        int leavingSegmentIndex = firstSegment;
        ++firstIndex;
        if (firstIndex == GetSegmentSize()) {
            firstIndex = 0;
            firstSegment = GetCircularSegmentIndex(firstSegment + 1);
            ClearSegment(leavingSegmentIndex);
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

        int removedSegmentIndex = GetSegmentIndex(length - 1);
        --length;
        int remainingLastSegmentIndex = GetSegmentIndex(length - 1);
        if (removedSegmentIndex != remainingLastSegmentIndex) {
            ClearSegment(removedSegmentIndex);
        }
    }

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(this);
    }
};

#endif
