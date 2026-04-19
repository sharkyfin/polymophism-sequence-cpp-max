#ifndef SEGMENTED_BUFFER_H
#define SEGMENTED_BUFFER_H

#include "dynamic_array.hpp"
#include "exceptions.hpp"

template <class T>
class SegmentedBuffer {
private:
    DynamicArray<DynamicArray<T>> segments;
    int segmentSize;

    void CheckSegmentIndex(int segmentIndex) const {
        if (segmentIndex < 0 || segmentIndex >= segments.GetSize()) {
            throw IndexOutOfRangeException("SegmentedBuffer: segment index out of range");
        }
    }

public:
    SegmentedBuffer() : segments(), segmentSize(0) {}

    SegmentedBuffer(int segmentCount, int newSegmentSize) : segments(), segmentSize(0) {
        if (segmentCount < 0) {
            throw InvalidArgumentException("SegmentedBuffer: negative segment count");
        }
        if (newSegmentSize <= 0) {
            throw InvalidArgumentException("SegmentedBuffer: invalid segment size");
        }

        DynamicArray<DynamicArray<T>> newSegments(segmentCount);
        segments.Swap(newSegments);
        segmentSize = newSegmentSize;
    }

    int GetSegmentCount() const {
        return segments.GetSize();
    }

    int GetSegmentSize() const {
        return segmentSize;
    }

    bool HasSegment(int segmentIndex) const {
        CheckSegmentIndex(segmentIndex);
        return segments[segmentIndex].GetSize() > 0;
    }

    void AllocateSegment(int segmentIndex) {
        CheckSegmentIndex(segmentIndex);
        if (segments[segmentIndex].GetSize() == 0) {
            DynamicArray<T> newSegment(segmentSize);
            segments[segmentIndex].Swap(newSegment);
        }
    }

    void ClearSegment(int segmentIndex) {
        CheckSegmentIndex(segmentIndex);
        if (segments[segmentIndex].GetSize() > 0) {
            DynamicArray<T> emptySegment;
            segments[segmentIndex].Swap(emptySegment);
        }
    }

    DynamicArray<T>& GetSegment(int segmentIndex) {
        CheckSegmentIndex(segmentIndex);
        return segments[segmentIndex];
    }

    const DynamicArray<T>& GetSegment(int segmentIndex) const {
        CheckSegmentIndex(segmentIndex);
        return segments[segmentIndex];
    }

    T* GetSegmentData(int segmentIndex) {
        CheckSegmentIndex(segmentIndex);
        if (segments[segmentIndex].GetSize() == 0) {
            throw InvalidArgumentException("SegmentedBuffer: segment is not allocated");
        }
        return segments[segmentIndex].Data();
    }

    const T* GetSegmentData(int segmentIndex) const {
        CheckSegmentIndex(segmentIndex);
        if (segments[segmentIndex].GetSize() == 0) {
            throw InvalidArgumentException("SegmentedBuffer: segment is not allocated");
        }
        return segments[segmentIndex].Data();
    }

    void Swap(SegmentedBuffer<T>& other) {
        segments.Swap(other.segments);

        int tempSegmentSize = segmentSize;
        segmentSize = other.segmentSize;
        other.segmentSize = tempSegmentSize;
    }
};

#endif
