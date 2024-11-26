#include "database.h"

#include <iostream>

#include "serializer.h"

void Database::load_from_file(const string& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filepath);
    }

    Serializer serializer;
    tables = serializer.load(file);
    file.close();
}

void Database::save_to_file(const string& filepath) {
    ofstream file(filepath, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filepath);
    }

    Serializer serializer;
    serializer.save(tables, file);
    file.close();
}

void Database::execute(const string& query) {
    QueryExecutor executor;
    return executor.execute(query, tables);
}

unordered_map<string, shared_ptr<Table>>& Database::get_tables() {
    return tables;
}


void Database::set_tables(unordered_map<string, shared_ptr<Table>> new_tables) {
    tables = std::move(new_tables);
    cout << "Tables set in database: ";
    for (const auto& [name, _] : tables) {
        cout << name << " ";
    }
    cout << endl;
}


