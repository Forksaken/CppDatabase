#include "data_types.h"

bool DataTypeHelper::validate(const ValueType& value, DataType type) {
    switch (type) {
        case DataType::INT32:
            return holds_alternative<int32_t>(value);
        case DataType::BOOL:
            return holds_alternative<bool>(value);
        case DataType::STRING:
            return holds_alternative<string>(value);
        case DataType::BYTES:
            return holds_alternative<vector<uint8_t>>(value);
        default:
            return false;
    }
}

string DataTypeHelper::type_to_string(DataType type) {
    switch (type) {
        case DataType::INT32: return "int32";
        case DataType::BOOL: return "bool";
        case DataType::STRING: return "string";
        case DataType::BYTES: return "bytes";
        default: return "unknown";
    }
}
