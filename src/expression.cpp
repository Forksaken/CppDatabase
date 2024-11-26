#include "expression.h"
#include <sstream>
#include <stdexcept>

bool Expression::evaluate(const string& condition, const Row& row) {
    istringstream iss(condition);
    string column, op, value;

    iss >> column >> op >> value;

    if (row.get_value(column).index() == 0) {
        int32_t column_value = get<int32_t>(row.get_value(column));
        int32_t compare_value = stoi(value);

        if (op == "=") return column_value == compare_value;
        if (op == "<") return column_value < compare_value;
        if (op == ">") return column_value > compare_value;
        if (op == "<=") return column_value <= compare_value;
        if (op == ">=") return column_value >= compare_value;
        if (op == "!=") return column_value != compare_value;
    }

    throw runtime_error("Unsupported operation or data type in condition: " + condition);
}
