#ifndef IMMUTABLE_ARRAY_SEQUENCE_H
#define IMMUTABLE_ARRAY_SEQUENCE_H

#include "sequence/array_sequence_base.hpp"

template <class T>
class ImmutableArraySequence : public ArraySequenceBase<T> {
protected:
    ArraySequenceBase<T>* Instance() override {
        return new ImmutableArraySequence<T>(*this);
    }

public:
    ImmutableArraySequence() : ArraySequenceBase<T>() {}

    ImmutableArraySequence(T* items, int count) : ArraySequenceBase<T>(items, count) {}

    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableArraySequence<T>();
    }
};

#endif
