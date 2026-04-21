#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>

#include "core/dynamic_array.hpp"
#include "core/heap_cleaner.hpp"
#include "core/icollection.hpp"
#include "core/ienumerator.hpp"

template <class T>
class Sequence : public ICollection<T> {
protected:
    void AppendToResult(Sequence<T>*& sequence, const T& item) const {
        Sequence<T>* next = sequence->Append(item);
        if (next != sequence) {
            delete sequence;
            sequence = next;
        }
    }

public:
    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;
    virtual const T& Get(int index) const override = 0;

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    virtual int GetLength() const = 0;

    int GetCount() const override {
        return GetLength();
    }

    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;

    virtual Sequence<T>* Concat(Sequence<T>* list) const = 0;
    virtual Sequence<T>* Clone() const override = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;
    virtual IEnumerator<T>* GetEnumerator() const = 0;

    virtual ~Sequence() = default;

    const T& operator[](int index) const {
        return Get(index);
    }

    template <class Mapper>
    Sequence<T>* Map(Mapper mapper) const {
        HeapCleaner<Sequence<T>> result(CreateEmpty());
        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator());

        while (enumerator->MoveNext()) {
            AppendToResult(result.Ref(), mapper(enumerator->CurrentRef()));
        }

        return result.Release();
    }

    template <class Predicate>
    Sequence<T>* Where(Predicate predicate) const {
        HeapCleaner<Sequence<T>> result(CreateEmpty());
        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator());

        while (enumerator->MoveNext()) {
            const T& value = enumerator->CurrentRef();
            if (predicate(value)) {
                AppendToResult(result.Ref(), value);
            }
        }

        return result.Release();
    }

    template <class Reducer>
    T Reduce(Reducer reducer) const {
        if (GetLength() == 0) {
            throw EmptyCollectionException("Sequence: Reduce on empty sequence");
        }

        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator());
        if (!enumerator->MoveNext()) {
            throw EmptyCollectionException("Sequence: Reduce on empty sequence");
        }

        T result = enumerator->CurrentRef();
        while (enumerator->MoveNext()) {
            result = reducer(result, enumerator->CurrentRef());
        }

        return result;
    }

    template <class TResult, class Folder>
    TResult Fold(TResult initialValue, Folder folder) const {
        TResult result = initialValue;
        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator());

        while (enumerator->MoveNext()) {
            result = folder(result, enumerator->CurrentRef());
        }

        return result;
    }

    template <class Predicate>
    int Find(Predicate predicate) const {
        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator());

        int index = 0;
        while (enumerator->MoveNext()) {
            if (predicate(enumerator->CurrentRef())) {
                return index;
            }
            ++index;
        }

        return -1;
    }

    template <class Predicate>
    DynamicArray<Sequence<T>*> Split(Predicate isDelimiter, bool keepEmpty = false) const {
        DynamicArray<Sequence<T>*> parts(0);
        HeapCleaner<Sequence<T>> currentPart(CreateEmpty());
        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator());

        try {
            while (enumerator->MoveNext()) {
                const T& value = enumerator->CurrentRef();
                if (isDelimiter(value)) {
                    if (keepEmpty || currentPart->GetLength() > 0) {
                        int oldSize = parts.GetSize();
                        parts.Resize(oldSize + 1);
                        parts.Set(oldSize, currentPart.Release());
                    }
                    currentPart.Reset(CreateEmpty());
                } else {
                    AppendToResult(currentPart.Ref(), value);
                }
            }

            if (keepEmpty || currentPart->GetLength() > 0) {
                int oldSize = parts.GetSize();
                parts.Resize(oldSize + 1);
                parts.Set(oldSize, currentPart.Release());
            }

            return parts;
        } catch (...) {
            for (int i = 0; i < parts.GetSize(); ++i) {
                delete parts.Get(i);
            }
            throw;
        }
    }

    Sequence<T>* Slice(int index, int count) const {
        return Slice(index, count, nullptr);
    }

    Sequence<T>* Slice(int index, int count, const Sequence<T>* replacement) const {
        int length = GetLength();
        if (index < 0 || count < 0 || index > length || index + count > length) {
            throw IndexOutOfRangeException("Sequence: invalid range in Slice");
        }

        HeapCleaner<Sequence<T>> result(CreateEmpty());
        HeapCleaner<IEnumerator<T>> sourceEnumerator(GetEnumerator());
        HeapCleaner<IEnumerator<T>> replacementEnumerator;

        bool insertedReplacement = false;
        int position = 0;

        while (sourceEnumerator->MoveNext()) {
            if (!insertedReplacement && position == index) {
                if (replacement != nullptr) {
                    replacementEnumerator.Reset(replacement->GetEnumerator());
                    while (replacementEnumerator->MoveNext()) {
                        AppendToResult(result.Ref(), replacementEnumerator->CurrentRef());
                    }
                    replacementEnumerator.Reset();
                }
                insertedReplacement = true;
            }

            const T& value = sourceEnumerator->CurrentRef();
            if (position < index || position >= index + count) {
                AppendToResult(result.Ref(), value);
            }
            ++position;
        }

        if (!insertedReplacement && replacement != nullptr) {
            replacementEnumerator.Reset(replacement->GetEnumerator());
            while (replacementEnumerator->MoveNext()) {
                AppendToResult(result.Ref(), replacementEnumerator->CurrentRef());
            }
        }

        return result.Release();
    }
};

template <class T>
bool operator==(const Sequence<T>& left, const Sequence<T>& right) {
    if (left.GetLength() != right.GetLength()) {
        return false;
    }

    HeapCleaner<IEnumerator<T>> leftEnumerator(left.GetEnumerator());
    HeapCleaner<IEnumerator<T>> rightEnumerator(right.GetEnumerator());

    while (true) {
        bool hasLeft = leftEnumerator->MoveNext();
        bool hasRight = rightEnumerator->MoveNext();

        if (hasLeft != hasRight) {
            return false;
        }

        if (!hasLeft) {
            break;
        }

        if (leftEnumerator->CurrentRef() != rightEnumerator->CurrentRef()) {
            return false;
        }
    }

    return true;
}

template <class T>
bool operator!=(const Sequence<T>& left, const Sequence<T>& right) {
    return !(left == right);
}

template <class T>
Sequence<T>* operator+(const Sequence<T>& left, const Sequence<T>& right) {
    HeapCleaner<Sequence<T>> rightCopy(right.Clone());
    return left.Concat(rightCopy.Get());
}

template <class T>
Sequence<T>* operator+(const Sequence<T>& sequence, T item) {
    HeapCleaner<Sequence<T>> result(sequence.Clone());
    Sequence<T>* next = result->Append(item);
    if (next != result.Get()) {
        result.Reset(next);
    }
    return result.Release();
}

template <class T>
std::ostream& operator<<(std::ostream& output, const Sequence<T>& sequence) {
    output << "[";

    HeapCleaner<IEnumerator<T>> enumerator(sequence.GetEnumerator());
    bool first = true;
    while (enumerator->MoveNext()) {
        if (!first) {
            output << ", ";
        }
        output << enumerator->CurrentRef();
        first = false;
    }

    output << "]";
    return output;
}

#endif
