#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include "table.h"
#include "query_executor.h"

using namespace std;

class Database {
public:
    Database() = default;

    void load_from_file(const string& filepath);

    void save_to_file(const string& filepath);

    void execute(const string& query);

    unordered_map<string, shared_ptr<Table>>& get_tables();

    void set_tables(unordered_map<string, shared_ptr<Table>> new_tables);

private:
    unordered_map<string, shared_ptr<Table>> tables;
};

#endif // DATABASE_H
