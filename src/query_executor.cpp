#include "query_executor.h"
#include "data_types.h"
#include "exceptions.h"

#include <iostream>
#include <regex>

#include "database.h"

template <typename ValueType>
void print_variant(const ValueType& value) {
    std::visit([](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<unsigned char>>) {
            std::cout << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                std::cout << static_cast<int>(arg[i]);
                if (i + 1 < arg.size()) {
                    std::cout << ", ";
                }
            }
            std::cout << "]";
        } else {
            std::cout << arg;
        }
    }, value);
}

ValueType parse_value(const string& value) {
    if (value.front() == '\'' && value.back() == '\'') {
        return value.substr(1, value.size() - 2);
    }
    try {
        return stoi(value);
    } catch (...) {
        throw runtime_error("Invalid value: " + value);
    }
}

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

void QueryExecutor::execute(const string& query, unordered_map<string, shared_ptr<Table>>& tables) {
    regex create_table_regex(R"(CREATE\s+TABLE\s+(\w+)\s*\((.*)\))", regex::icase);
    smatch match;

    if (regex_match(query, match, create_table_regex)) {
        string table_name = match[1];
        string columns_str = match[2];

        shared_ptr<Table> table = make_shared<Table>(table_name);
        tables[table_name] = table;

        regex column_regex(R"(\s*(\{[^}]*\})?\s*(\w+)\s*:\s*(int32|bool|string|bytes)(\[(\d+)\])?(?:\s*=\s*(\S+))?)");
        auto begin = sregex_iterator(columns_str.begin(), columns_str.end(), column_regex);
        auto end = sregex_iterator();

        for (auto it = begin; it != end; ++it) {
            string column_attr = (*it)[1];
            string column_name = (*it)[2];
            string column_type_str = (*it)[3];
            string column_size_str = (*it)[4];
            string default_value_str = (*it)[6];

            DataType type;
            size_t size = 0;

            if (column_type_str == "int32") {
                type = DataType::INT32;
            } else if (column_type_str == "bool") {
                type = DataType::BOOL;
            } else if (column_type_str == "string") {
                type = DataType::STRING;
                if (!column_size_str.empty()) {
                    string size_str = column_size_str.substr(1, column_size_str.size() - 2);
                    size = stoi(size_str);
                }
            } else if (column_type_str == "bytes") {
                type = DataType::BYTES;
                if (!column_size_str.empty()) {
                    string size_str = column_size_str.substr(1, column_size_str.size() - 2);
                    size = stoi(size_str);
                }
            } else {
                throw runtime_error("Unsupported column type: " + column_type_str);
            }

            bool is_autoincrement = column_attr.find("autoincrement") != string::npos;
            bool is_unique = column_attr.find("unique") != string::npos;

            ValueType default_value;
            if (!default_value_str.empty()) {
                if (column_type_str == "bool") {
                    if (default_value_str == "true") {
                        default_value = true;
                    } else if (default_value_str == "false") {
                        default_value = false;
                    } else {
                        throw runtime_error("Invalid default value for BOOL column: " + default_value_str);
                    }
                } else if (column_type_str == "int32") {
                    default_value = stoi(default_value_str);
                } else if (column_type_str == "string") {
                    default_value = default_value_str;
                } else if (column_type_str == "bytes") {
                    if (default_value_str.substr(0, 2) == "0x") {
                        vector<uint8_t> bytes;
                        for (size_t i = 2; i < default_value_str.length(); i += 2) {
                            string byte_str = default_value_str.substr(i, 2);
                            unsigned char byte = (unsigned char)stoul(byte_str, nullptr, 16);
                            bytes.push_back(byte);
                        }
                        default_value = bytes;
                    } else {
                        throw runtime_error("Invalid default value for BYTES column: " + default_value_str);
                    }
                } else {
                    throw runtime_error("Unsupported default value format for column: " + column_name);
                }
            }

            table->add_column(Column(column_name, type, is_autoincrement, is_unique, size, default_value));
        }

        cout << "Table '" << table_name << "' created successfully." << endl;
        return;
    }

    regex insert_regex(R"(INSERT\s+INTO\s+(\w+)\s+VALUES\s*\((.*)\))", regex::icase);
    if (regex_match(query, match, insert_regex)) {
        string table_name = match[1];
        string values_str = match[2];

        auto table = tables[table_name];
        auto columns = table->get_columns();

        regex value_regex(R"(\s*(\S+)\s*)");
        auto value_begin = sregex_iterator(values_str.begin(), values_str.end(), value_regex);
        auto value_end = sregex_iterator();

        Row row;
        int idx = 0;
        for (auto it = value_begin; it != value_end; ++it) {
            const string& value_str = (*it)[1];
            const Column& column = columns[idx];

            ValueType value;

            switch (column.get_type()) {
                case DataType::INT32:
                    try {
                        value = stoi(value_str);
                    } catch (...) {
                        throw runtime_error("Invalid value for INT32 column: " + value_str);
                    }
                    break;
                case DataType::BOOL:
                    if (value_str == "true") {
                        value = true;
                    } else if (value_str == "false") {
                        value = false;
                    } else {
                        throw runtime_error("Invalid value for BOOL column: " + value_str);
                    }
                    break;
                case DataType::STRING:
                    value = value_str;
                    break;
                case DataType::BYTES:
                    if (value_str.substr(0, 2) == "0x") {
                        vector<uint8_t> bytes;
                        for (size_t i = 2; i < value_str.length(); i += 2) {
                            string byte_str = value_str.substr(i, 2);
                            unsigned char byte = (unsigned char)stoul(byte_str, nullptr, 16);
                            bytes.push_back(byte);
                        }
                        value = bytes;
                    } else {
                        throw runtime_error("Invalid value for BYTES column: " + value_str);
                    }
                    break;
                default:
                    throw runtime_error("Unsupported column type.");
            }

            if (!DataTypeHelper::validate(value, column.get_type())) {
                throw runtime_error("Type mismatch for column '" + column.get_name() + "'. Expected: " + DataTypeHelper::type_to_string(column.get_type()));
            }

            row.set_value(column.get_name(), value);
            ++idx;
        }

        table->insert_row(row);
        cout << "Row inserted into table '" << table_name << "'." << endl;
        return;
    }

    throw runtime_error("Invalid query: Unsupported query: " + query);
}



QueryResult QueryExecutor::handle_create(const string& query, unordered_map<string, shared_ptr<Table>>& tables) {
    regex create_regex(R"(create table (\w+)\s*\((.*)\))", regex::icase);
    smatch match;

    if (!regex_match(query, match, create_regex)) {
        throw InvalidQueryException("Malformed CREATE TABLE query: " + query);
    }

    string table_name = match[1];
    string columns_definition = match[2];

    if (tables.find(table_name) != tables.end()) {
        throw InvalidQueryException("Table already exists: " + table_name);
    }

    unordered_map<string, Column> columns;
    regex column_regex(R"(\{([^}]*)\}\s*(\w+)\s*:\s*(\w+)(?:\s*=\s*(.*))?)");
    auto col_begin = sregex_iterator(columns_definition.begin(), columns_definition.end(), column_regex);
    auto col_end = sregex_iterator();

    for (auto it = col_begin; it != col_end; ++it) {
        string attributes = (*it)[1];
        string column_name = (*it)[2];
        string type = (*it)[3];

        DataType data_type;

        if (type == "int32") data_type = DataType::INT32;
        else if (type == "bool") data_type = DataType::BOOL;
        else if (type.find("string") == 0) data_type = DataType::STRING;
        else if (type.find("bytes") == 0) data_type = DataType::BYTES;
        else throw InvalidQueryException("Unknown column type: " + type);

        columns[column_name] = Column(column_name, data_type);
    }

    auto table = make_shared<Table>(table_name);
    for (const auto& [name, column] : columns) {
        table->add_column(column);
    }

    tables[table_name] = table;
    cout << "Table '" << table_name << "' created successfully." << endl;
    return QueryResult(true);
}

QueryResult QueryExecutor::handle_insert(const string& query, unordered_map<string, shared_ptr<Table>>& tables) {
    regex insert_regex(R"(insert\s*\((.*)\)\s*to\s*(\w+))", regex::icase);
    smatch match;

    if (!regex_match(query, match, insert_regex)) {
        throw InvalidQueryException("Malformed INSERT query: " + query);
    }

    string values = match[1];
    string table_name = match[2];

    auto table_it = tables.find(table_name);
    if (table_it == tables.end()) {
        throw InvalidQueryException("Table not found: " + table_name);
    }

    shared_ptr<Table> table = table_it->second;
    Row row;

    regex value_regex(R"((\w+)\s*=\s*([^,]+))");
    auto value_begin = sregex_iterator(values.begin(), values.end(), value_regex);
    auto value_end = sregex_iterator();

    for (auto it = value_begin; it != value_end; ++it) {
        string column_name = (*it)[1];
        string value = (*it)[2];

        if (!table->has_column(column_name)) {
            throw InvalidQueryException("Column not found: " + column_name);
        }

        DataType column_type = table->get_column(column_name).get_type();
        ValueType parsed_value;

        if (column_type == DataType::INT32) parsed_value = stoi(value);
        else if (column_type == DataType::BOOL) parsed_value = (value == "true");
        else if (column_type == DataType::STRING) parsed_value = value.substr(1, value.size() - 2);
        else if (column_type == DataType::BYTES) parsed_value = vector<uint8_t>(value.begin(), value.end());
        else throw InvalidQueryException("Unsupported data type for column: " + column_name);

        row.set_value(column_name, parsed_value);
    }

    table->insert_row(row);
    return QueryResult(true);
}

QueryResult QueryExecutor::handle_select(const string& query, unordered_map<string, shared_ptr<Table>>& tables) {
    regex select_regex(R"(select\s+(.*)\s+from\s+(\w+)\s+where\s+(.*))", regex::icase);
    smatch match;

    if (!regex_match(query, match, select_regex)) {
        throw InvalidQueryException("Malformed SELECT query: " + query);
    }

    string columns = match[1];
    string table_name = match[2];
    string condition = match[3];

    auto table_it = tables.find(table_name);
    if (table_it == tables.end()) {
        throw InvalidQueryException("Table not found: " + table_name);
    }

    shared_ptr<Table> table = table_it->second;

    auto condition_func = [condition](const Row& row) -> bool {
        regex condition_regex(R"((\w+)\s*=\s*(\w+))", regex::icase);
        smatch cond_match;

        if (!regex_match(condition, cond_match, condition_regex)) {
            throw InvalidQueryException("Invalid condition: " + condition);
        }

        string column_name = cond_match[1];
        string value = cond_match[2];

        try {
            return std::get<string>(row.get_value(column_name)) == value;
        } catch (const std::exception& e) {
            throw runtime_error("Condition evaluation failed: " + string(e.what()));
        }
    };

    vector<Row> selected_rows = table->select(condition_func);

    for (const auto& row : selected_rows) {
        for (const auto& column : table->get_columns()) {
            cout << column.get_name() << ": ";
            print_variant(row.get_value(column.get_name()));
            cout << "\t";
        }
        cout << endl;
    }

    return QueryResult(true);
}
