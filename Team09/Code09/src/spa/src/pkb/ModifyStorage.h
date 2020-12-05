#pragma once

#include "constants.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

class ModifyStorage {
  private:
    static ModifyStorage *inst_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<int>>> varToStmtModifiesMap;
    std::unordered_map<int, std::unordered_set<std::string>> stmtNumsToVarModifiesMap;
    std::unordered_map<std::string, std::unordered_set<std::string>> procToVarModifiesMap;
    std::unordered_map<std::string, std::unordered_set<std::string>> varToProcModifiesMap;
    std::unordered_map<std::string, std::unordered_set<int>> stmtTypeToStmtNum;

  public:
    static ModifyStorage *getInstance();

    void setVarToStmt(int stmtLine, std::string designEnt, std::string varName);
    void setStmtToVar(int stmtLine, std::string varName);
    void setProcToVar(std::string proc, std::string varName);
    void setVarToProc(std::string proc, std::string varName);
    void setStmtTypeToStmt(std::string designEnt, int stmtLine);

    bool isModifies(int stmtLine, std::string varName);
    bool isModifiesByEnt(std::string stmtEnt, std::string varName);
    bool isModifiesByProc(std::string procName, std::string varName);

    std::unordered_set<int> getModifiesStatement(std::string stmtEnt, std::string varName);
    std::unordered_set<std::string> getModifiesProc(std::string varName);
    std::unordered_set<std::string> getModifiesProc();
    std::unordered_set<std::string> getModifiesVariableByStmt(int stmtLine);
    std::unordered_set<std::string> getModifiesVariableByProc(std::string procName);
    std::unordered_set<int> getAllModifiesStatements(std::string stmtEnt);

    StringPairResultSet getAllProcModifiesVarRelationPair();
    IntStringPairResultSet getAllStmtModifiesVarRelationPair(std::string stmtEnt);

    void clear();
};
