#ifndef MUTABLE_ARRAY_SEQUENCE_H
#define MUTABLE_ARRAY_SEQUENCE_H

#include "sequence/array_sequence_base.hpp"

template <class T>
class MutableArraySequence : public ArraySequenceBase<T> {
protected:
    ArraySequenceBase<T>* Instance() override {
        return this;
    }

public:
    MutableArraySequence() : ArraySequenceBase<T>() {}

    MutableArraySequence(T* items, int count) : ArraySequenceBase<T>(items, count) {}

    MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }
};

#endif
