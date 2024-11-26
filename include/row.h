#ifndef ROW_H
#define ROW_H

#include <string>
#include <unordered_map>
#include <variant>
#include "data_types.h"

using namespace std;

class Row {
public:
    void set_value(const string& column_name, const ValueType& value);

    ValueType get_value(const string& column_name) const;

    bool has_value(const string& column_name) const;

private:
    unordered_map<string, ValueType> values;
};

#endif // ROW_H
