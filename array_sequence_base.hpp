#ifndef ARRAY_SEQUENCE_BASE_H
#define ARRAY_SEQUENCE_BASE_H

#include "dynamic_array.hpp"
#include "sequence.hpp"

template <class T>
class ArraySequenceBase : public Sequence<T> {
protected:
    DynamicArray<T> data;

    virtual ArraySequenceBase<T>* Instance() = 0;

    Sequence<T>* AppendInternal(T item) {
        data.PushBack(item);
        return this;
    }

    Sequence<T>* PrependInternal(T item) {
        int oldSize = data.GetSize();
        data.Resize(oldSize + 1);

        for (int i = oldSize; i > 0; --i) {
            data.Set(i, data.Get(i - 1));
        }
        data.Set(0, item);
        return this;
    }

    Sequence<T>* InsertAtInternal(T item, int index) {
        int length = data.GetSize();
        if (index < 0 || index > length) {
            throw IndexOutOfRangeException("ArraySequenceBase: index out of range in InsertAt");
        }

        if (index == length) {
            return AppendInternal(item);
        }

        data.Resize(length + 1);
        for (int i = length; i > index; --i) {
            data.Set(i, data.Get(i - 1));
        }
        data.Set(index, item);
        return this;
    }

public:
    ArraySequenceBase() : data(0) {}

    ArraySequenceBase(T* items, int count) : data(items, count) {}

    ArraySequenceBase(const ArraySequenceBase<T>& other) : data(other.data) {}

    const T& GetFirst() const override {
        if (data.GetSize() == 0) {
            throw EmptyCollectionException("ArraySequenceBase: sequence is empty");
        }
        return data.Get(0);
    }

    const T& GetLast() const override {
        if (data.GetSize() == 0) {
            throw EmptyCollectionException("ArraySequenceBase: sequence is empty");
        }
        return data.Get(data.GetSize() - 1);
    }

    const T& Get(int index) const override {
        return data.Get(index);
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= data.GetSize()) {
            throw IndexOutOfRangeException("ArraySequenceBase: invalid subsequence range");
        }

        HeapCleaner<Sequence<T>> result(this->CreateEmpty());
        for (int i = startIndex; i <= endIndex; ++i) {
            this->AppendToResult(result.Ref(), data.Get(i));
        }

        return result.Release();
    }

    int GetLength() const override {
        return data.GetSize();
    }

    IEnumerator<T>* GetEnumerator() const override {
        return data.GetEnumerator();
    }

    Sequence<T>* Append(T item) override {
        ArraySequenceBase<T>* target = Instance();
        HeapCleaner<ArraySequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->AppendInternal(item);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* Prepend(T item) override {
        ArraySequenceBase<T>* target = Instance();
        HeapCleaner<ArraySequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->PrependInternal(item);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* InsertAt(T item, int index) override {
        ArraySequenceBase<T>* target = Instance();
        HeapCleaner<ArraySequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->InsertAtInternal(item, index);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* Concat(Sequence<T>* list) const override {
        if (list == nullptr) {
            throw InvalidArgumentException("ArraySequenceBase: null pointer in Concat");
        }

        HeapCleaner<Sequence<T>> result(this->Clone());
        HeapCleaner<IEnumerator<T>> enumerator(list->GetEnumerator());

        while (enumerator->MoveNext()) {
            this->AppendToResult(result.Ref(), enumerator->Current());
        }

        return result.Release();
    }
};

#endif
