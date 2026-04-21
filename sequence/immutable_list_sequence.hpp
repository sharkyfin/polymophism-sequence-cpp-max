#ifndef IMMUTABLE_LIST_SEQUENCE_H
#define IMMUTABLE_LIST_SEQUENCE_H

#include "sequence/list_sequence_base.hpp"

template <class T>
class ImmutableListSequence : public ListSequenceBase<T> {
protected:
    ListSequenceBase<T>* Instance() override {
        return new ImmutableListSequence<T>(*this);
    }

public:
    ImmutableListSequence() : ListSequenceBase<T>() {}

    ImmutableListSequence(T* items, int count) : ListSequenceBase<T>(items, count) {}

    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableListSequence<T>();
    }
};

#endif
