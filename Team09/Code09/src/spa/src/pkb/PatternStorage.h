#pragma once

#include <locale>
#include <regex>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "constants.h"

class PatternStorage {
  private:
    static PatternStorage *inst_;
    std::unordered_map<int, std::pair<std::string, std::string>> stmtNumsToVarAndPostfixMap;
    std::unordered_map<int, std::string> stmtNumsToPostfixMap;
    std::unordered_map<std::string, std::unordered_set<int>> lhsVarToStmtsMap;
    std::unordered_set<int> checkAndReturnMatchingStatements(std::unordered_set<int> stmtList, std::string pattern, bool noLeft,
                                                             bool noRight);
    std::unordered_map<std::string, std::unordered_set<int>> varToIfStmtsMap;
    std::unordered_map<std::string, std::unordered_set<int>> varToWhileStmtsMap;
    std::unordered_map<int, std::unordered_set<std::string>> ifStmtToVarMap;
    std::unordered_map<int, std::unordered_set<std::string>> whileStmtToVarMap;

    IntStringPairResultSet ifStmtToVarPairSet;
    IntStringPairResultSet whileStmtToVarPairSet;

    int preced(char ch);
    void checkPostfixValidity(std::string postfix);
    void checkPatternValidity(std::string pattern);

  public:
    static PatternStorage *getInstance();
    void setStmtToPostfix(int stmtLine, std::string lhsVar, std::string pattern);
    std::unordered_set<int> getPatternMatchStatement(std::string pattern, bool noLeft, bool noRight);
    std::unordered_set<int> getPatternAndVarMatchStatement(std::string lhsVar, std::string pattern, bool noLeft, bool noRight);
    std::string convertInfixToPostfix(std::string infix);
    void setConditionPattern(int stmtNum, std::unordered_set<std::string> varsUsed, std::string designEnt);
    std::unordered_set<int> getVarMatchConditionStatements(std::string var, std::string designEnt);
    std::unordered_set<int> getConditionStatementsWithAnyVar(std::string designEnt);

    IntStringPairResultSet getVarMatchConditionStatementPairs(std::string designEnt);

    void clear();
};
