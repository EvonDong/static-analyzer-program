#pragma once

#include "StatementStruct.h"
#include "constants.h"

#include <unordered_map>

class GeneralStorage {
  private:
    static GeneralStorage *inst_;
    std::unordered_set<std::string> varSet;
    std::unordered_set<int> stmtSet;
    std::unordered_set<int> constantsSet;
    std::unordered_map<int, Statement> stmtNumToStmtObjMap;
    std::unordered_set<std::string> procedureSet;
    std::unordered_map<std::string, std::unordered_set<int>> stmtTypeToStmtNumMap;
    std::unordered_map<std::string, std::unordered_set<int>> varToStmtNumMap;
    std::unordered_map<std::string, std::pair<int, int>> procedureRangeMap;

  public:
    static GeneralStorage *getInstance();

    void setConstant(int);
    void setVar(std::string);
    void storeStmtObj(Statement);
    void setProcedure(std::string, int, int);
    void setVarToStmtNum(std::string, int);

    bool isValidStatementNum(int, std::string);

    std::string getProcedureOfCurrStmt(int);
    std::unordered_set<std::string> getAllVars();
    std::unordered_set<std::string> getAllProcedures();

    Statement getStmtObjByStmtNum(int);

    std::unordered_set<int> getStmtNumByStmtType(std::string entType = STMT);
    std::unordered_set<int> getStmtsByVar(std::string);
    std::unordered_set<std::string> getVarByStmtNum(int);
    std::string getStmtTypeByStmtNum(int);
    std::unordered_set<int> getAllConstants();
    std::pair<int, int> getProcedureRange(std::string);

    void clear();
};
