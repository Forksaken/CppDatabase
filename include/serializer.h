#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <fstream>
#include "table.h"
#include "exceptions.h"

using namespace std;

class Serializer {
public:
    void save(const unordered_map<string, shared_ptr<Table>>& tables, ostream& out);

    unordered_map<string, shared_ptr<Table>> load(istream& file);
};

#endif // SERIALIZER_H
