#ifndef IMMUTABLE_DEQUE_SEQUENCE_H
#define IMMUTABLE_DEQUE_SEQUENCE_H

#include "deque/deque_sequence_base.hpp"

template <class T>
class ImmutableDequeSequence : public DequeSequenceBase<T> {
protected:
    DequeSequenceBase<T>* Instance() override {
        return new ImmutableDequeSequence<T>(*this);
    }

public:
    ImmutableDequeSequence() : DequeSequenceBase<T>() {}

    ImmutableDequeSequence(T* items, int count) : DequeSequenceBase<T>(items, count) {}

    ImmutableDequeSequence(const ImmutableDequeSequence<T>& other) : DequeSequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableDequeSequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableDequeSequence<T>();
    }
};

#endif
