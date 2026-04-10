#ifndef DEQUE_SEQUENCE_BASE_H
#define DEQUE_SEQUENCE_BASE_H

#include "deque.hpp"
#include "sequence.hpp"

template <class T>
class DequeSequenceBase : public Sequence<T> {
protected:
    Deque<T> data;

    virtual DequeSequenceBase<T>* Instance() = 0;

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

    Sequence<T>* PopFrontInternal() {
        data.PopFront();
        return this;
    }

    Sequence<T>* PopBackInternal() {
        data.PopBack();
        return this;
    }

public:
    DequeSequenceBase() : data() {}

    DequeSequenceBase(T* items, int count) : data(items, count) {}

    DequeSequenceBase(const DequeSequenceBase<T>& other) : data(other.data) {}

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
        int length = data.GetLength();
        if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= length) {
            throw IndexOutOfRangeException("DequeSequenceBase: invalid subsequence range");
        }

        HeapCleaner<Sequence<T>> result(this->CreateEmpty());
        for (int i = startIndex; i <= endIndex; ++i) {
            this->AppendToResult(result.Ref(), data.Get(i));
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
        DequeSequenceBase<T>* target = Instance();
        HeapCleaner<DequeSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->AppendInternal(item);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* Prepend(T item) override {
        DequeSequenceBase<T>* target = Instance();
        HeapCleaner<DequeSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->PrependInternal(item);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* InsertAt(T item, int index) override {
        DequeSequenceBase<T>* target = Instance();
        HeapCleaner<DequeSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->InsertAtInternal(item, index);
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* Concat(Sequence<T>* list) const override {
        if (list == nullptr) {
            throw InvalidArgumentException("DequeSequenceBase: null pointer in Concat");
        }

        HeapCleaner<Sequence<T>> result(this->Clone());
        HeapCleaner<IEnumerator<T>> enumerator(list->GetEnumerator());

        while (enumerator->MoveNext()) {
            this->AppendToResult(result.Ref(), enumerator->CurrentRef());
        }

        return result.Release();
    }

    Sequence<T>* PopFront() {
        DequeSequenceBase<T>* target = Instance();
        HeapCleaner<DequeSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->PopFrontInternal();
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }

    Sequence<T>* PopBack() {
        DequeSequenceBase<T>* target = Instance();
        HeapCleaner<DequeSequenceBase<T>> targetGuard((target == this) ? nullptr : target);

        Sequence<T>* result = target->PopBackInternal();
        if (result == target) {
            targetGuard.Release();
        }
        return result;
    }
};

#endif
