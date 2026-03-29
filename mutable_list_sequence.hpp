#ifndef MUTABLE_LIST_SEQUENCE_H
#define MUTABLE_LIST_SEQUENCE_H

#include "list_sequence_base.hpp"

template <class T>
class MutableListSequence : public ListSequenceBase<T> {
protected:
    ListSequenceBase<T>* Instance() override {
        return this;
    }

public:
    MutableListSequence() : ListSequenceBase<T>() {}

    MutableListSequence(T* items, int count) : ListSequenceBase<T>(items, count) {}

    MutableListSequence(const MutableListSequence<T>& other) : ListSequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new MutableListSequence<T>();
    }
};

#endif
