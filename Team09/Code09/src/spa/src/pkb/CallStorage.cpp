#include "CallStorage.h"

CallStorage *CallStorage::inst_ = NULL;

CallStorage *CallStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new CallStorage();
    }
    return inst_;
}

void CallStorage::setCalls(std::string caller, std::string callee, int stmtNumOfCallee) {
    if (callsMap.find(caller) != callsMap.end()) {
        callsMap.at(caller).emplace(callee);
    } else {
        std::unordered_set<std::string> procList;
        procList.emplace(callee);
        callsMap.emplace(caller, procList);
    }
    if (calledMap.find(callee) != calledMap.end()) {
        calledMap.at(callee).emplace(caller);
    } else {
        std::unordered_set<std::string> procList;
        procList.emplace(caller);
        calledMap.emplace(callee, procList);
    }

    callsProcToProcPairSet.emplace(std::pair<std::string, std::string>{caller, callee});

    stmtToProcCallMap[stmtNumOfCallee] = callee;

    if (callerProcToCallStmtMap.find(caller) != callerProcToCallStmtMap.end()) {
        callerProcToCallStmtMap.at(caller).emplace(stmtNumOfCallee);
    } else {
        std::unordered_set<int> stmtList;
        stmtList.emplace(stmtNumOfCallee);
        callerProcToCallStmtMap.emplace(caller, stmtList);
    }

    if (calleeProcToCallStmtMap.find(callee) != calleeProcToCallStmtMap.end()) {
        calleeProcToCallStmtMap.at(callee).emplace(stmtNumOfCallee);
    } else {
        std::unordered_set<int> stmtSet{stmtNumOfCallee};
        calleeProcToCallStmtMap.emplace(callee, stmtSet);
    }
}

void CallStorage::setCallsStar(std::string caller, std::string callee) {
    if (callsStarMap.find(caller) != callsStarMap.end()) {
        callsStarMap.at(caller).emplace(callee);
    } else {
        std::unordered_set<std::string> procList{callee};
        callsStarMap.emplace(caller, procList);
    }

    if (calledStarMap.find(callee) != calledStarMap.end()) {
        calledStarMap.at(callee).emplace(caller);
    } else {
        std::unordered_set<std::string> procList{caller};
        calledStarMap.emplace(callee, procList);
    }
    callsStarProcToProcPairSet.emplace(std::pair<std::string, std::string>{caller, callee});
}

bool CallStorage::isCalls(std::string caller, std::string callee) {
    if (callsMap.find(caller) != callsMap.end()) {
        return callsMap.at(caller).find(callee) != callsMap.at(caller).end();
    } else {
        return false;
    }
}

bool CallStorage::isCallsStar(std::string caller, std::string callee) {
    if (callsStarMap.find(caller) != callsStarMap.end()) {
        if (callsStarMap.at(caller).find(callee) != callsStarMap.at(caller).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

std::unordered_set<std::string> CallStorage::getAllCallsStar(std::string proc) {
    std::unordered_set<std::string> resultForWildcard;
    if (proc.empty()) {
        for (auto &proc : callsStarMap) {
            resultForWildcard.emplace(proc.first);
        }
        return resultForWildcard;
    }
    if (callsStarMap.find(proc) != callsStarMap.end()) {
        return callsStarMap.at(proc);
    }
    std::unordered_set<std::string> empty;
    return empty;
}

std::unordered_set<std::string> CallStorage::getAllCalls(std::string proc) {
    std::unordered_set<std::string> resultForWildcard;
    if (proc.empty()) {
        for (auto &proc : callsMap) {
            resultForWildcard.emplace(proc.first);
        }
        return resultForWildcard;
    }
    if (callsMap.find(proc) != callsMap.end()) {
        return callsMap.at(proc);
    }
    std::unordered_set<std::string> empty;
    return empty;
}

std::unordered_set<std::string> CallStorage::getAllCalledStarBy(std::string proc) {
    std::unordered_set<std::string> resultForWildcard;
    if (proc.empty()) {
        for (auto &proc : calledStarMap) {
            resultForWildcard.emplace(proc.first);
        }
        return resultForWildcard;
    }
    if (calledStarMap.find(proc) != calledStarMap.end()) {
        return calledStarMap.at(proc);
    }
    std::unordered_set<std::string> empty;
    return empty;
}

std::unordered_set<std::string> CallStorage::getAllCalledBy(std::string proc) {
    std::unordered_set<std::string> resultForWildcard;
    if (proc.empty()) {
        for (auto &proc : calledMap) {
            resultForWildcard.emplace(proc.first);
        }
        return resultForWildcard;
    }
    if (calledMap.find(proc) != calledMap.end()) {
        return calledMap.at(proc);
    }
    std::unordered_set<std::string> empty;
    return empty;
}

std::unordered_set<int> CallStorage::getAllCallsStmt(std::string proc) {
    std::unordered_set<int> resultForWildcard;
    if (proc.empty()) {
        for (auto &stmtList : callerProcToCallStmtMap) {
            resultForWildcard.insert(stmtList.second.begin(), stmtList.second.end());
        }
        return resultForWildcard;
    }
    if (callerProcToCallStmtMap.find(proc) != callerProcToCallStmtMap.end()) {
        return callerProcToCallStmtMap.at(proc);
    }
    std::unordered_set<int> empty;
    return empty;
}

void CallStorage::DFSUtil(std::string curr, std::unordered_set<int> &adj) {
    for (auto &v : adj) {
        if (stmtToProcCallMap.find(v) != stmtToProcCallMap.end()) {
            std::string proc = stmtToProcCallMap.at(v);
            setCallsStar(curr, proc);
            if (callerProcToCallStmtMap.find(proc) != callerProcToCallStmtMap.end()) {
                std::unordered_set<int> stmtList = callerProcToCallStmtMap.at(proc);
                DFSUtil(curr, stmtList);
            }
        }
    }
}

/*
 * Description: Populates the callsStarMap
 * based on the callsMap.
 */
void CallStorage::populateCallsStarMap() {

    std::unordered_map<std::string, std::unordered_set<int>>::iterator itr;
    for (itr = callerProcToCallStmtMap.begin(); itr != callerProcToCallStmtMap.end(); ++itr) {
        DFSUtil((*itr).first, (*itr).second);
    }
}

bool CallStorage::isProcCaller(std::string proc) {
    return (callsMap.find(proc) != callsMap.end());
}

bool CallStorage::isProcCallee(std::string proc) {
    return (calledMap.find(proc) != calledMap.end());
}

bool CallStorage::isProgramCaller() {
    return (callsMap.size() > 0);
}

StringPairResultSet CallStorage::getAllCallsRelationPair() {
    return callsProcToProcPairSet;
}

StringPairResultSet CallStorage::getAllCallsStarRelationPair() {
    return callsStarProcToProcPairSet;
}

std::string CallStorage::getProcedureCalledAtStmt(int stmt) {
    if (stmtToProcCallMap.find(stmt) != stmtToProcCallMap.end()) {
        return stmtToProcCallMap.at(stmt);
    } else {
        return "";
    }
}

std::unordered_set<int> CallStorage::getStatementsThatCalls(std::string proc) {
    if (calleeProcToCallStmtMap.find(proc) != calleeProcToCallStmtMap.end()) {
        return calleeProcToCallStmtMap.at(proc);
    }
    std::unordered_set<int> empty;
    return empty;
}

IntStringPairResultSet CallStorage::getAllCallStmtWithProcPair() {
    IntStringPairResultSet resultSet;
    for (auto &mapItr : stmtToProcCallMap) {
        resultSet.emplace(mapItr.first, mapItr.second);
    }
    return resultSet;
}

void CallStorage::clear() {
    callsStarMap.clear();
    callsMap.clear();
    calledMap.clear();
    stmtToProcCallMap.clear();
    callerProcToCallStmtMap.clear();
    calleeProcToCallStmtMap.clear();

    calledStarMap.clear();
    callsProcToProcPairSet.clear();
    callsStarProcToProcPairSet.clear();
}
