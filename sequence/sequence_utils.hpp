#ifndef SEQUENCE_UTILS_H
#define SEQUENCE_UTILS_H

#include "core/heap_cleaner.hpp"
#include "sequence/mutable_array_sequence.hpp"
#include "core/pair.hpp"

template <class T1, class T2>
Sequence<Pair<T1, T2>>* Zip(const Sequence<T1>& first, const Sequence<T2>& second) {
    HeapCleaner<MutableArraySequence<Pair<T1, T2>>> result(new MutableArraySequence<Pair<T1, T2>>());
    HeapCleaner<IEnumerator<T1>> firstEnumerator(first.GetEnumerator());
    HeapCleaner<IEnumerator<T2>> secondEnumerator(second.GetEnumerator());

    while (true) {
        bool hasFirst = firstEnumerator->MoveNext();
        bool hasSecond = secondEnumerator->MoveNext();
        if (!hasFirst || !hasSecond) {
            break;
        }

        result->Append(Pair<T1, T2>(firstEnumerator->CurrentRef(), secondEnumerator->CurrentRef()));
    }

    return result.Release();
}

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Pair<T1, T2>>& zipped) {
    HeapCleaner<MutableArraySequence<T1>> first(new MutableArraySequence<T1>());
    HeapCleaner<MutableArraySequence<T2>> second(new MutableArraySequence<T2>());
    HeapCleaner<IEnumerator<Pair<T1, T2>>> enumerator(zipped.GetEnumerator());

    while (enumerator->MoveNext()) {
        const Pair<T1, T2>& value = enumerator->CurrentRef();
        first->Append(value.first);
        second->Append(value.second);
    }

    return Pair<Sequence<T1>*, Sequence<T2>*>(first.Release(), second.Release());
}

#endif
