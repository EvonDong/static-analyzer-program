#pragma once

#include <string>
#include <unordered_set>

struct Statement {
    int stmtNum;
    std::string type;
    std::unordered_set<std::string> varSet;
    std::unordered_set<int> constSet;
};
