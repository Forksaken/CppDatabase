#include "row.h"

#include <stdexcept>

void Row::set_value(const string& column_name, const ValueType& value) {
    values[column_name] = value;
}

ValueType Row::get_value(const string& column_name) const {
    auto it = values.find(column_name);
    if (it == values.end()) {
        throw runtime_error("Column value not found: " + column_name);
    }
    return it->second;
}

bool Row::has_value(const string& column_name) const {
    return values.find(column_name) != values.end();
}
