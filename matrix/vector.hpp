#ifndef VECTOR_H
#define VECTOR_H

#include "core/dynamic_array.hpp"

template <class T>
class Vector {
private:
    DynamicArray<T> data;

public:
    Vector() : data() {}

    explicit Vector(int size) : data(size) {}

    Vector(int size, const T& value) : data(size) {
        for (int i = 0; i < size; ++i) {
            data[i] = value;
        }
    }

    Vector(const T* items, int count) : data(count) {
        if (count < 0) {
            throw InvalidArgumentException("Vector: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("Vector: null items pointer with positive count");
        }

        for (int i = 0; i < count; ++i) {
            data[i] = items[i];
        }
    }

    int GetSize() const {
        return data.GetSize();
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
            data[i] = value;
        }
    }

    void SwapElements(int first, int second) {
        T temporary = data.Get(first);
        data.Set(first, data.Get(second));
        data.Set(second, temporary);
    }
};

#endif
