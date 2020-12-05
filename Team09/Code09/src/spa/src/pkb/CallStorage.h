#pragma once

#include "constants.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

typedef std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> StringPairResultSet;

class CallStorage {
  private:
    static CallStorage *inst_;
    std::unordered_map<std::string, std::unordered_set<std::string>> callsStarMap;
    std::unordered_map<std::string, std::unordered_set<std::string>> callsMap;
    std::unordered_map<std::string, std::unordered_set<std::string>> calledMap;
    std::unordered_map<int, std::string> stmtToProcCallMap; // stmt number to corresponding call statement procedure
    std::unordered_map<std::string, std::unordered_set<int>> callerProcToCallStmtMap; // caller procedure to its corresponding call statements
    std::unordered_map<std::string, std::unordered_set<int>>
        calleeProcToCallStmtMap; // callee proc to corresponding call statement where it is called

    std::unordered_map<std::string, std::unordered_set<std::string>> calledStarMap;
    StringPairResultSet callsProcToProcPairSet;
    StringPairResultSet callsStarProcToProcPairSet;

    void DFSUtil(std::string curr, std::unordered_set<int> &adj);

  public:
    static CallStorage *getInstance();
    void setCalls(std::string caller, std::string callee, int stmtNumOfCallee);
    void setCallsStar(std::string caller, std::string callee);

    bool isCalls(std::string caller, std::string callee);

    bool isCallsStar(std::string caller, std::string callee);

    bool isProcCaller(std::string proc);
    bool isProcCallee(std::string proc);
    bool isProgramCaller();

    void populateCallsStarMap();

    std::unordered_set<std::string> getAllCallsStar(std::string caller = "");
    std::unordered_set<std::string> getAllCalls(std::string caller = "");
    std::unordered_set<std::string> getAllCalledStarBy(std::string callee = "");
    std::unordered_set<std::string> getAllCalledBy(std::string callee = "");
    std::unordered_set<int> getAllCallsStmt(std::string proc = "");
    std::unordered_set<int> getStatementsThatCalls(std::string proc);

    StringPairResultSet getAllCallsRelationPair();
    StringPairResultSet getAllCallsStarRelationPair();
    IntStringPairResultSet getAllCallStmtWithProcPair();

    std::string getProcedureCalledAtStmt(int stmt);

    void clear();
};
