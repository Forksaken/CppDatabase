#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <string>
#include <unordered_map>
#include <memory>

#include "database.h"
#include "table.h"

using namespace std;

class QueryResult {
public:
    QueryResult(bool success, const string& error = "")
        : success(success), error_message(error) {}

    bool is_ok() const { return success; }
    string get_error() const { return error_message; }

private:
    bool success;
    string error_message;
};

class QueryExecutor {
public:
    QueryExecutor() = default;

    void execute(const string& query, unordered_map<string, shared_ptr<Table>>& tables);

private:
    QueryResult handle_create(const string& query, unordered_map<string, shared_ptr<Table>>& tables);

    QueryResult handle_insert(const string& query, unordered_map<string, shared_ptr<Table>>& tables);

    QueryResult handle_select(const string &query, unordered_map<string, shared_ptr<Table>> &tables);
};

#endif // QUERY_EXECUTOR_H
