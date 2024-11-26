#include "serializer.h"
#include <sstream>
#include <vector>

void Serializer::save(const unordered_map<string, shared_ptr<Table>>& tables, ostream& out) {
    size_t table_count = tables.size();
    out.write(reinterpret_cast<const char*>(&table_count), sizeof(table_count));

    for (const auto& [table_name, table] : tables) {
        size_t name_length = table_name.size();
        out.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
        out.write(table_name.c_str(), name_length);

        auto columns = table->get_columns();
        size_t column_count = columns.size();
        out.write(reinterpret_cast<const char*>(&column_count), sizeof(column_count));

        for (const auto& column : columns) {
            size_t column_name_length = column.get_name().size();
            out.write(reinterpret_cast<const char*>(&column_name_length), sizeof(column_name_length));
            out.write(column.get_name().c_str(), column_name_length);
        }

        const auto& rows = table->get_rows();
        size_t row_count = rows.size();
        out.write(reinterpret_cast<const char*>(&row_count), sizeof(row_count));

        for (const auto& row : rows) {
            for (const auto& column : columns) {
                const auto& value = row.get_value(column.get_name());
                std::visit([&out](const auto& val) {
                    if constexpr (std::is_same_v<decltype(val), int32_t>) {
                        out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                    } else if constexpr (std::is_same_v<decltype(val), bool>) {
                        out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                    } else if constexpr (std::is_same_v<decltype(val), string>) {
                        size_t len = val.size();
                        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                        out.write(val.c_str(), len);
                    } else if constexpr (std::is_same_v<decltype(val), vector<uint8_t>>) {
                        size_t len = val.size();
                        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                        out.write(reinterpret_cast<const char*>(val.data()), len);
                    }
                }, value);
            }
        }
    }
}


unordered_map<string, shared_ptr<Table>> Serializer::load(istream& in) {
    unordered_map<string, shared_ptr<Table>> tables;

    size_t table_count;
    in.read(reinterpret_cast<char*>(&table_count), sizeof(table_count));

    for (size_t i = 0; i < table_count; ++i) {
        size_t name_length;
        in.read(reinterpret_cast<char*>(&name_length), sizeof(name_length));

        string table_name(name_length, '\0');
        in.read(&table_name[0], name_length);

        shared_ptr<Table> table = make_shared<Table>(table_name);

        size_t column_count;
        in.read(reinterpret_cast<char*>(&column_count), sizeof(column_count));
        for (size_t j = 0; j < column_count; ++j) {
            size_t column_name_length;
            in.read(reinterpret_cast<char*>(&column_name_length), sizeof(column_name_length));

            string column_name(column_name_length, '\0');
            in.read(&column_name[0], column_name_length);

            table->add_column(Column(column_name, DataType::INT32));
        }

        size_t row_count;
        in.read(reinterpret_cast<char*>(&row_count), sizeof(row_count));

        for (size_t k = 0; k < row_count; ++k) {
            Row row;
            for (size_t col = 0; col < column_count; ++col) {
                ValueType value;
                in.read(reinterpret_cast<char*>(&value), sizeof(value));
                row.set_value(table->get_columns()[col].get_name(), value);
            }
            table->insert_row(row);
        }

        tables[table_name] = table;
    }

    return tables;
}