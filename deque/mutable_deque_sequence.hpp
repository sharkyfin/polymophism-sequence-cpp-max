#ifndef MUTABLE_DEQUE_SEQUENCE_H
#define MUTABLE_DEQUE_SEQUENCE_H

#include "deque/deque_sequence_base.hpp"

template <class T>
class MutableDequeSequence : public DequeSequenceBase<T> {
protected:
    DequeSequenceBase<T>* Instance() override {
        return this;
    }

public:
    MutableDequeSequence() : DequeSequenceBase<T>() {}

    MutableDequeSequence(T* items, int count) : DequeSequenceBase<T>(items, count) {}

    MutableDequeSequence(const MutableDequeSequence<T>& other) : DequeSequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new MutableDequeSequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new MutableDequeSequence<T>();
    }
};

#endif
