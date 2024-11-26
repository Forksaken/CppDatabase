#include "table.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

Table::Table(const string& name) : name(name) {}

void Table::add_column(const Column& column) {
    if (columns.find(column.get_name()) != columns.end()) {
        throw runtime_error("Column already exists: " + column.get_name());
    }
    columns[column.get_name()] = column;
}

vector<Column> Table::get_columns() const {
    vector<Column> columns_list;
    for (const auto& [name, column] : columns) {
        columns_list.push_back(column);
    }
    return columns_list;
}

bool Table::has_column(const string& column_name) const {
    return columns.find(column_name) != columns.end();
}

const Column& Table::get_column(const string& column_name) const {
    auto it = columns.find(column_name);
    if (it == columns.end()) {
        throw runtime_error("Column not found: " + column_name);
    }
    return it->second;
}

void Table::insert_row(Row& row) {
    for (auto& [name, column] : columns) {
        if (!row.has_value(name)) {
            if (column.is_autoincrement()) {
                row.set_value(name, column.get_next_autoincrement_value());
            } else if (column.has_default()) {
                row.set_value(name, column.get_default_value());
            } else {
                throw runtime_error("Missing value for column: " + name);
            }
        }
    }
    rows.push_back(row);
}

std::vector<Row> Table::select(std::function<bool(const Row&)> condition) {
    std::vector<Row> result;
    for (const auto& row : rows) {
        if (condition(row)) {
            result.push_back(row);
        }
    }
    return result;
}

void Table::print_table() const {
    if (columns.empty()) {
        std::cout << "The table is empty." << std::endl;
        return;
    }

    for (const auto& [name, _] : columns) {
        std::cout << std::setw(15) << std::left << name;
    }
    std::cout << std::endl;

    std::cout << std::string(columns.size() * 15, '-') << std::endl;

    for (const auto& row : rows) {
        for (const auto& [name, _] : columns) {
            const auto& value = row.get_value(name);
            std::visit([](const auto& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::vector<unsigned char>>) {
                    std::cout << std::setw(15) << std::left << "[BLOB]";
                } else {
                    std::cout << std::setw(15) << std::left << val;
                }
            }, value);
        }
        std::cout << std::endl;
    }
}

const std::vector<Row>& Table::get_rows() const {
    return rows;
}

vector<Column> Table::get_column_definitions() const {
    vector<Column> column_definitions;
    for (const auto& [name, column] : columns) {
        column_definitions.push_back(column);
    }
    return column_definitions;
}