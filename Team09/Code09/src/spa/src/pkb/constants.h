#pragma once

#include <string>
#include <unordered_set>

#include "Hasher.h"

typedef std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> IntStringPairResultSet;
typedef std::unordered_set<std::pair<int, int>, intPairHash, intPairComparator> IntPairResultSet;
typedef std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> StringPairResultSet;

const std::string STMT = "stmt";
const std::string STMT_NUM = "stmtNum";
const std::string READ = "read";
const std::string PRINT = "print";
const std::string CALL = "call";
const std::string WHILE_STMT = "while";
const std::string IF_STMT = "if";
const std::string ASSIGN = "assign";
const std::string INVALID = "invalid";
const std::string CONSTANT = "constant";
const std::string PROCEDURE = "procedure";
const std::string VARIABLE = "variable";
const std::string WILDCARD = "wildcard";
const std::string EXPRESSION = "expression";
const std::string IDENTIFIER = "identifier";
