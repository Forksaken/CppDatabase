#include <iostream>
#include <fstream>
#include "database.h"
#include "query_executor.h"
#include "serializer.h"

using namespace std;

void test_database() {
    Database db;
    QueryExecutor executor;
    Serializer serializer;

    try {
        cout << "Running: CREATE TABLE users ({autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)" << endl;
        executor.execute("CREATE TABLE users ({autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)", db.get_tables());

        cout << "Running: INSERT INTO users VALUES (true 0x123abc 'Alice' 1)" << endl;
        executor.execute("INSERT INTO users VALUES (true 0x123abc 'Alice' 1)", db.get_tables());

        cout << "Running: INSERT INTO users VALUES (false 0x789xyz 'Bob' 2)" << endl;
        executor.execute("INSERT INTO users VALUES (false 0x789xyz 'Bob' 2)", db.get_tables());

        cout << "Printing table 'users' after inserts:" << endl;
        auto tables = db.get_tables();
        if (tables.find("users") != tables.end()) {
            tables["users"]->print_table();
        } else {
            cout << "Table 'users' not found after inserts!" << endl;
        }

        cout << "Saving database to file..." << endl;
        ofstream save_file("database.dat", ios::binary);
        serializer.save(db.get_tables(), save_file);
        save_file.close();

        db.set_tables({});

        cout << "Loading database from file..." << endl;
        ifstream load_file("database.dat", ios::binary);
        db.set_tables(serializer.load(load_file));
        load_file.close();

        cout << "Printing table 'users' after load:" << endl;
        tables = db.get_tables();
        if (tables.find("users") != tables.end()) {
            tables["users"]->print_table();
        } else {
            cout << "Table 'users' not found after load!" << endl;
        }
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
}

int main() {
    cout << "Starting database tests..." << endl;
    test_database();
    cout << "All tests completed." << endl;

    return 0;
}
