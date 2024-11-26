#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <unordered_map>
#include <functional>
#include "data_types.h"
#include "row.h"

using namespace std;

class Expression {
public:
    static bool evaluate(const string& condition, const Row& row);
};

#endif // EXPRESSION_H
