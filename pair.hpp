#ifndef PAIR_H
#define PAIR_H

#include <iostream>

template <class T1, class T2>
class Pair {
public:
    T1 first;
    T2 second;

    Pair() : first(), second() {}

    Pair(const T1& firstValue, const T2& secondValue)
        : first(firstValue), second(secondValue) {}

    bool operator==(const Pair<T1, T2>& other) const {
        return first == other.first && second == other.second;
    }

    bool operator!=(const Pair<T1, T2>& other) const {
        return !(*this == other);
    }
};

template <class T1, class T2>
std::ostream& operator<<(std::ostream& output, const Pair<T1, T2>& pair) {
    output << "(" << pair.first << ", " << pair.second << ")";
    return output;
}

#endif
