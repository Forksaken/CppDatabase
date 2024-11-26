#ifndef COLUMN_H
#define COLUMN_H

#include <string>
#include <vector>
#include <variant>
#include "data_types.h"

using namespace std;

class Column {
public:
    Column() = default;

    Column(const string& name, DataType type, size_t length = 0, bool autoincrement = false, bool unique = false, ValueType default_value = ValueType())
        : name(name), type(type), length(length), autoincrement(autoincrement), unique(unique), default_value(default_value) {}

    string get_name() const { return name; }
    DataType get_type() const { return type; }
    size_t get_length() const { return length; }
    bool is_autoincrement() const { return autoincrement; }
    ValueType get_next_autoincrement_value() { return autoincrement_value++; }
    bool is_unique() const { return unique; }
    ValueType get_default_value() const { return default_value; }
    bool has_default() const { return default_value.index() != variant_npos; }

private:
    string name;
    DataType type;
    size_t length;
    bool autoincrement;
    bool unique;
    ValueType default_value;
    int32_t autoincrement_value = 0;
};


#endif // COLUMN_H