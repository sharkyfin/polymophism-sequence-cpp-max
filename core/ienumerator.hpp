#ifndef IENUMERATOR_H
#define IENUMERATOR_H

template <class T>
class IEnumerator {
public:
    virtual bool MoveNext() = 0;
    virtual T Current() const = 0;
    virtual const T& CurrentRef() const = 0;
    virtual void Reset() = 0;
    virtual ~IEnumerator() = default;
};

#endif
