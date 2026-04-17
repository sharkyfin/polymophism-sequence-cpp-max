#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "exceptions.hpp"
#include "ienumerator.hpp"

template <class T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;

    int RecommendedCapacity(int requiredSize) const {
        int newCapacity = (capacity > 0) ? capacity : 1;
        while (newCapacity < requiredSize) {
            newCapacity *= 2;
        }
        return newCapacity;
    }

    void Reallocate(int newCapacity) {
        T* newData = nullptr;
        if (newCapacity > 0) {
            newData = new T[newCapacity];
            int copyCount = (size < newCapacity) ? size : newCapacity;
            try {
                for (int i = 0; i < copyCount; ++i) {
                    newData[i] = data[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
        if (size > capacity) {
            size = capacity;
        }
    }

public:
    class Enumerator : public IEnumerator<T> {
    private:
        const DynamicArray<T>* array;
        int index;
        bool started;

    public:
        explicit Enumerator(const DynamicArray<T>* owner)
            : array(owner), index(-1), started(false) {}

        bool MoveNext() override {
            if (!started) {
                index = 0;
                started = true;
            } else {
                ++index;
            }
            return index < array->size;
        }

        T Current() const override {
            return CurrentRef();
        }

        const T& CurrentRef() const override {
            if (!started || index < 0 || index >= array->size) {
                throw EmptyCollectionException("DynamicArray::Enumerator: no current element");
            }
            return array->data[index];
        }

        void Reset() override {
            index = -1;
            started = false;
        }
    };

    DynamicArray() : data(nullptr), size(0), capacity(0) {}

    DynamicArray(T* items, int count) : data(nullptr), size(0), capacity(0) {
        if (count < 0) {
            throw InvalidArgumentException("DynamicArray: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("DynamicArray: null items pointer with positive count");
        }

        size = count;
        capacity = count;
        if (capacity > 0) {
            T* newData = new T[capacity];
            try {
                for (int i = 0; i < size; ++i) {
                    newData[i] = items[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
            data = newData;
        }
    }

    explicit DynamicArray(int newSize) : data(nullptr), size(0), capacity(0) {
        if (newSize < 0) {
            throw InvalidArgumentException("DynamicArray: negative size");
        }

        size = newSize;
        capacity = newSize;
        if (capacity > 0) {
            data = new T[capacity];
        }
    }

    DynamicArray(const DynamicArray<T>& other)
        : data(nullptr), size(other.size), capacity(other.capacity) {
        if (capacity > 0) {
            T* newData = new T[capacity];
            try {
                for (int i = 0; i < size; ++i) {
                    newData[i] = other.data[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
            data = newData;
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this == &other) {
            return *this;
        }

        T* newData = nullptr;
        if (other.capacity > 0) {
            newData = new T[other.capacity];
            try {
                for (int i = 0; i < other.size; ++i) {
                    newData[i] = other.data[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
        }

        delete[] data;
        data = newData;
        size = other.size;
        capacity = other.capacity;
        return *this;
    }

    ~DynamicArray() {
        delete[] data;
    }

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(this);
    }

    T& Get(int index) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("DynamicArray: index out of range");
        }
        return data[index];
    }

    const T& Get(int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("DynamicArray: index out of range");
        }
        return data[index];
    }

    int GetSize() const {
        return size;
    }

    int GetCapacity() const {
        return capacity;
    }

    T* Data() {
        return data;
    }

    const T* Data() const {
        return data;
    }

    T& operator[](int index) {
        return Get(index);
    }

    const T& operator[](int index) const {
        return Get(index);
    }

    void Set(int index, const T& value) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("DynamicArray: index out of range");
        }
        data[index] = value;
    }

    void Swap(DynamicArray<T>& other) {
        T* tempData = data;
        data = other.data;
        other.data = tempData;

        int tempSize = size;
        size = other.size;
        other.size = tempSize;

        int tempCapacity = capacity;
        capacity = other.capacity;
        other.capacity = tempCapacity;
    }

    void PushBack(const T& value) {
        if (size == capacity) {
            int newCapacity = (capacity == 0) ? 1 : capacity * 2;
            Reallocate(newCapacity);
        }

        data[size] = value;
        ++size;
    }

    void Resize(int newSize) {
        if (newSize < 0) {
            throw InvalidArgumentException("DynamicArray: negative size in Resize");
        }

        if (newSize > capacity) {
            Reallocate(RecommendedCapacity(newSize));
        }

        size = newSize;
    }
};

#endif
