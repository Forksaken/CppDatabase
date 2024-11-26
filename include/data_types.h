#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>
#include <variant>
#include <string>
#include <vector>

using namespace std;

enum class DataType { INT32, BOOL, STRING, BYTES };

using ValueType = variant<int32_t, bool, string, vector<uint8_t>>;

class DataTypeHelper {
public:
    static bool validate(const ValueType& value, DataType type);

    static string type_to_string(DataType type);
};

#endif // DATA_TYPES_H
