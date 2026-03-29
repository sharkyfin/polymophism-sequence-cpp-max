#ifndef HEAP_CLEANER_H
#define HEAP_CLEANER_H

template <class T>
class HeapCleaner {
private:
    T* ptr;

public:
    explicit HeapCleaner(T* p = nullptr) : ptr(p) {}

    ~HeapCleaner() {
        delete ptr;
    }

    T* Get() const {
        return ptr;
    }

    T*& Ref() {
        return ptr;
    }

    T* operator->() const {
        return ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    void Reset(T* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }

    T* Release() {
        T* out = ptr;
        ptr = nullptr;
        return out;
    }

private:
    HeapCleaner(const HeapCleaner&) = delete;
    HeapCleaner& operator=(const HeapCleaner&) = delete;
};

#endif
