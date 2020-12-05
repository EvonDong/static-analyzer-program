#pragma once

#include "constants.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

class UseStorage {
  private:
    static UseStorage *inst_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<int>>> varToStmtUsesMap;
    std::unordered_map<int, std::unordered_set<std::string>> stmtNumsToVarUsesMap;
    std::unordered_map<std::string, std::unordered_set<std::string>> procToVarUsesMap;
    std::unordered_map<std::string, std::unordered_set<std::string>> varToProcUsesMap;
    std::unordered_map<std::string, std::unordered_set<int>> stmtTypeToStmtNum;

  public:
    static UseStorage *getInstance();

    void setVarToStmt(int stmtLine, std::string designEnt, std::string varName);
    void setStmtToVar(int stmtLine, std::string varName);
    void setProcToVar(std::string proc, std::string varName);
    void setVarToProc(std::string proc, std::string varName);
    void setStmtTypeToStmt(std::string designEnt, int stmtLine);

    bool isUses(int stmtLine, std::string varName);
    bool isUsesByEnt(std::string stmtEnt, std::string varName);
    bool isUsesByProc(std::string procName, std::string varName);

    std::unordered_set<int> getUsesStatement(std::string stmtEnt, std::string varName);
    std::unordered_set<std::string> getUsesProc(std::string varName);
    std::unordered_set<std::string> getUsesProc();
    std::unordered_set<std::string> getUsesVariablesByStmt(int stmtLine);
    std::unordered_set<std::string> getUsesVariableByProc(std::string);
    std::unordered_set<int> getAllUsesStatements(std::string stmtEnt);

    StringPairResultSet getAllProcUsesVarRelationPair();
    IntStringPairResultSet getAllStmtUsesVarRelationPair(std::string stmtEnt);

    void clear();
};
