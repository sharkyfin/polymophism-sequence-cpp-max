#ifndef VECTOR_H
#define VECTOR_H

#include "deque.hpp"

template <class T>
class Vector {
private:
    Deque<T> data;

    static Deque<T> CreateStorage(int size, const T& value) {
        return Deque<T>::CreateVectorStorage(size, value);
    }

public:
    Vector() : data() {}

    explicit Vector(int size) : data(CreateStorage(size, T())) {}

    Vector(int size, const T& value) : data(CreateStorage(size, value)) {}

    Vector(const T* items, int count) : data(CreateStorage(count, T())) {
        for (int i = 0; i < count; ++i) {
            data.Set(i, items[i]);
        }
    }

    int GetSize() const {
        return data.GetLength();
    }

    T& Get(int index) {
        return data.Get(index);
    }

    const T& Get(int index) const {
        return data.Get(index);
    }

    void Set(int index, const T& value) {
        data.Set(index, value);
    }

    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }

    void Fill(const T& value) {
        for (int i = 0; i < GetSize(); ++i) {
            data.Set(i, value);
        }
    }

    void SwapElements(int first, int second) {
        data.SwapElements(first, second);
    }

    Deque<T>& AsDeque() {
        return data;
    }

    const Deque<T>& AsDeque() const {
        return data;
    }
};

#endif
