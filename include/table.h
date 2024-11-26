#ifndef TABLE_H
#define TABLE_H

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include "row.h"
#include "column.h"

using namespace std;

class Table {
public:
    Table(const string& name);

    void add_column(const Column& column);

    vector<Column> get_columns() const;

    bool has_column(const string& column_name) const;

    const Column& get_column(const string& column_name) const;

    void insert_row(Row& row);

    vector<Row> select(function<bool(const Row&)> condition);

    void print_table() const;

    const std::vector<Row>& get_rows() const;

    vector<Column> get_column_definitions() const;

private:
    string name;
    unordered_map<string, Column> columns;
    vector<Row> rows;
};

#endif // TABLE_H
