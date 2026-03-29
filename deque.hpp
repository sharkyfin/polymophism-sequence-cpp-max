#ifndef DEQUE_H
#define DEQUE_H

#include "dynamic_array.hpp"
#include "exceptions.hpp"
#include "ienumerator.hpp"

template <class T>
class Deque {
private:
    DynamicArray<T> buffer;
    int head;
    int length;

    int Capacity() const {
        return buffer.GetSize();
    }

    int PhysicalIndex(int logicalIndex) const {
        return (head + logicalIndex) % Capacity();
    }

    void CheckIndex(int index) const {
        if (index < 0 || index >= length) {
            throw IndexOutOfRangeException("Deque: index out of range");
        }
    }

    void EnsureCapacity(int requiredCapacity) {
        if (requiredCapacity <= Capacity()) {
            return;
        }

        int newCapacity = (Capacity() > 0) ? Capacity() : 1;
        while (newCapacity < requiredCapacity) {
            newCapacity *= 2;
        }

        DynamicArray<T> newBuffer(newCapacity);
        for (int i = 0; i < length; ++i) {
            newBuffer.Set(i, Get(i));
        }

        buffer.Swap(newBuffer);
        head = 0;
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

    Deque() : buffer(0), head(0), length(0) {}

    Deque(T* items, int count) : buffer(0), head(0), length(0) {
        if (count < 0) {
            throw InvalidArgumentException("Deque: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("Deque: null items pointer with positive count");
        }

        if (count > 0) {
            DynamicArray<T> newBuffer(count);
            try {
                for (int i = 0; i < count; ++i) {
                    newBuffer.Set(i, items[i]);
                }
            } catch (...) {
                throw;
            }

            buffer.Swap(newBuffer);
            length = count;
        }
    }

    Deque(const Deque<T>& other)
        : buffer(other.buffer), head(other.head), length(other.length) {}

    Deque<T>& operator=(const Deque<T>& other) {
        if (this == &other) {
            return *this;
        }

        buffer = other.buffer;
        head = other.head;
        length = other.length;
        return *this;
    }

    const T& GetFirst() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        return buffer.Get(head);
    }

    const T& GetLast() const {
        if (length == 0) {
            throw EmptyCollectionException("Deque: deque is empty");
        }

        return buffer.Get(PhysicalIndex(length - 1));
    }

    const T& Get(int index) const {
        CheckIndex(index);
        return buffer.Get(PhysicalIndex(index));
    }

    int GetLength() const {
        return length;
    }

    void Append(T item) {
        EnsureCapacity(length + 1);

        if (length == 0) {
            head = 0;
            buffer.Set(0, item);
        } else {
            buffer.Set(PhysicalIndex(length), item);
        }

        ++length;
    }

    void Prepend(T item) {
        EnsureCapacity(length + 1);

        if (length == 0) {
            head = 0;
            buffer.Set(0, item);
        } else {
            head = (head - 1 + Capacity()) % Capacity();
            buffer.Set(head, item);
        }

        ++length;
    }

    void InsertAt(T item, int index) {
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

        EnsureCapacity(length + 1);

        DynamicArray<T> newBuffer(Capacity());
        for (int i = 0; i < index; ++i) {
            newBuffer.Set(i, Get(i));
        }
        newBuffer.Set(index, item);
        for (int i = index; i < length; ++i) {
            newBuffer.Set(i + 1, Get(i));
        }

        buffer.Swap(newBuffer);
        head = 0;
        ++length;
    }

    void PopFront() {
        if (length == 0) {
            throw EmptyCollectionException("Deque: PopFront on empty deque");
        }

        if (length == 1) {
            head = 0;
            length = 0;
            return;
        }

        head = (head + 1) % Capacity();
        --length;
    }

    void PopBack() {
        if (length == 0) {
            throw EmptyCollectionException("Deque: PopBack on empty deque");
        }

        --length;
        if (length == 0) {
            head = 0;
        }
    }

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(this);
    }
};

#endif
