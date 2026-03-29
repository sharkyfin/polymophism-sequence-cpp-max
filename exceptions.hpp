#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>

class LabException {
protected:
    std::string message;

public:
    explicit LabException(const std::string& msg) : message(msg) {}
    virtual ~LabException() = default;

    const std::string& GetMessage() const {
        return message;
    }
};

class IndexOutOfRangeException : public LabException {
public:
    explicit IndexOutOfRangeException(const std::string& msg)
        : LabException(msg) {}
};

class InvalidArgumentException : public LabException {
public:
    explicit InvalidArgumentException(const std::string& msg)
        : LabException(msg) {}
};

class EmptyCollectionException : public LabException {
public:
    explicit EmptyCollectionException(const std::string& msg)
        : LabException(msg) {}
};

#endif
