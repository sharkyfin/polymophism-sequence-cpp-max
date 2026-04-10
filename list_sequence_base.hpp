#ifndef LIST_SEQUENCE_BASE_H
#define LIST_SEQUENCE_BASE_H

#include "linked_list.hpp"
#include "sequence.hpp"

template <class T>
class ListSequenceBase : public Sequence<T> {
protected:
    LinkedList<T> data;

    virtual ListSequenceBase<T>* Instance() = 0;

    Sequence<T>* AppendInternal(const T& item) {
        data.Append(item);
        return this;
    }

    Sequence<T>* PrependInternal(const T& item) {
        data.Prepend(item);
        return this;
    }

    Sequence<T>* InsertAtInternal(const T& item, int index) {
        data.InsertAt(item, index);
        return this;
    }

public:
    ListSequenceBase() : data() {}

    ListSequenceBase(T* items, int count) : data(items, count) {}

    ListSequenceBase(const ListSequenceBase<T>& other) : data(other.data) {}

    const T& GetFirst() const override {
        return data.GetFirst();
    }

    const T& GetLast() const override {
        return data.GetLast();
    }

    const T& Get(int index) const override {
        return data.Get(index);
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= data.GetLength()) {
            throw IndexOutOfRangeException("ListSequenceBase: invalid subsequence range");
        }

        HeapCleaner<Sequence<T>> result(this->CreateEmpty());
        HeapCleaner<IEnumerator<T>> enumerator(data.GetEnumerator(startIndex));

        int count = endIndex - startIndex + 1;
        for (int i = 0; i < count && enumerator->MoveNext(); ++i) {
            this->AppendToResult(result.Ref(), enumerator->CurrentRef());
        }

        return result.Release();
    }

    int GetLength() const override {
        return data.GetLength();
    }

    IEnumerator<T>* GetEnumerator() const override {
        return data.GetEnumerator();
    }

    Sequence<T>* Append(T item) override {
        ListSequenceBase<T>* target = Instance();
        HeapCleaner<ListSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->AppendInternal(item);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* Prepend(T item) override {
        ListSequenceBase<T>* target = Instance();
        HeapCleaner<ListSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->PrependInternal(item);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* InsertAt(T item, int index) override {
        ListSequenceBase<T>* target = Instance();
        HeapCleaner<ListSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->InsertAtInternal(item, index);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* Concat(Sequence<T>* list) const override {
        if (list == nullptr) {
            throw InvalidArgumentException("ListSequenceBase: null pointer in Concat");
        }

        HeapCleaner<Sequence<T>> result(this->Clone());
        HeapCleaner<IEnumerator<T>> enumerator(list->GetEnumerator());

        while (enumerator->MoveNext()) {
            this->AppendToResult(result.Ref(), enumerator->CurrentRef());
        }

        return result.Release();
    }
};

#endif
