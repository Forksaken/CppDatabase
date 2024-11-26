#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

using namespace std;

class InvalidQueryException : public runtime_error {
public:
    explicit InvalidQueryException(const string& message)
        : runtime_error("Invalid query: " + message) {}
};

class ConstraintViolationException : public runtime_error {
public:
    explicit ConstraintViolationException(const string& message)
        : runtime_error("Constraint violation: " + message) {}
};

class SerializationException : public runtime_error {
public:
    explicit SerializationException(const string& message)
        : runtime_error("Serialization error: " + message) {}
};

#endif // EXCEPTIONS_H
