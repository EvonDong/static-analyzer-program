#include "ModifyStorage.h"

ModifyStorage *ModifyStorage::inst_ = NULL;

ModifyStorage *ModifyStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new ModifyStorage();
    }
    return inst_;
}

void ModifyStorage::setStmtToVar(int stmtLine, std::string varName) {
    if (stmtNumsToVarModifiesMap.find(stmtLine) != stmtNumsToVarModifiesMap.end()) {
        stmtNumsToVarModifiesMap.at(stmtLine).emplace(varName);
    } else {
        std::unordered_set<std::string> varList;
        varList.emplace(varName);
        stmtNumsToVarModifiesMap.emplace(stmtLine, varList);
    }
}

void ModifyStorage::setVarToStmt(int stmtLine, std::string designEnt, std::string varName) {
    if (varToStmtModifiesMap.find(varName) != varToStmtModifiesMap.end()) {
        std::unordered_map<std::string, std::unordered_set<int>> &designEntToStmt = varToStmtModifiesMap.at(varName);
        if (designEntToStmt.find(designEnt) != designEntToStmt.end()) {
            designEntToStmt.at(designEnt).emplace(stmtLine);
        } else {
            std::unordered_set<int> stmtList;
            stmtList.emplace(stmtLine);
            designEntToStmt.emplace(designEnt, stmtList);
        }
    } else {
        std::unordered_set<int> stmtList;
        stmtList.emplace(stmtLine);
        std::unordered_map<std::string, std::unordered_set<int>> designEntToStmt;
        designEntToStmt.emplace(designEnt, stmtList);
        varToStmtModifiesMap.emplace(varName, designEntToStmt);
    }
}

void ModifyStorage::setProcToVar(std::string proc, std::string varName) {
    if (procToVarModifiesMap.find(proc) != procToVarModifiesMap.end()) {
        procToVarModifiesMap.at(proc).emplace(varName);
    } else {
        std::unordered_set<std::string> varList;
        varList.emplace(varName);
        procToVarModifiesMap.emplace(proc, varList);
    }
}

void ModifyStorage::setVarToProc(std::string proc, std::string varName) {
    if (varToProcModifiesMap.find(varName) != varToProcModifiesMap.end()) {
        varToProcModifiesMap.at(varName).emplace(proc);
    } else {
        std::unordered_set<std::string> procList;
        procList.emplace(proc);
        varToProcModifiesMap.emplace(varName, procList);
    }
}

void ModifyStorage::setStmtTypeToStmt(std::string designEnt, int stmtLine) {
    if (stmtTypeToStmtNum.find(designEnt) != stmtTypeToStmtNum.end()) {
        stmtTypeToStmtNum.at(designEnt).emplace(stmtLine);
    } else {
        std::unordered_set<int> stmtSet{stmtLine};
        stmtTypeToStmtNum.emplace(designEnt, stmtSet);
    }
}

bool ModifyStorage::isModifies(int stmtLine, std::string varName) {
    if (stmtNumsToVarModifiesMap.find(stmtLine) != stmtNumsToVarModifiesMap.end()) {
        if (stmtNumsToVarModifiesMap.at(stmtLine).find(varName) != stmtNumsToVarModifiesMap.at(stmtLine).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool ModifyStorage::isModifiesByEnt(std::string stmtEnt, std::string varName) {
    if (varToStmtModifiesMap.find(varName) != varToStmtModifiesMap.end()) {
        if (varToStmtModifiesMap.at(varName).find(stmtEnt) != varToStmtModifiesMap.at(varName).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool ModifyStorage::isModifiesByProc(std::string procName, std::string varName) {
    if (varToProcModifiesMap.find(varName) != varToProcModifiesMap.end()) {
        if (varToProcModifiesMap.at(varName).find(procName) != varToProcModifiesMap.at(varName).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

std::unordered_set<int> ModifyStorage::getModifiesStatement(std::string stmtEnt, std::string varName) {
    if (varToStmtModifiesMap.find(varName) != varToStmtModifiesMap.end()) {
        std::unordered_map<std::string, std::unordered_set<int>> &designEntToStmts = varToStmtModifiesMap.at(varName);
        if (stmtEnt == STMT) {
            std::unordered_set<int> allStatements;
            for (const auto itr : designEntToStmts) {
                allStatements.insert(itr.second.begin(), itr.second.end());
            }
            return allStatements;
        } else if (designEntToStmts.find(stmtEnt) != designEntToStmts.end()) {
            return designEntToStmts.at(stmtEnt);
        } else {
            std::unordered_set<int> empty;
            return empty;
        }
    } else {
        std::unordered_set<int> empty;
        return empty;
    }
}

std::unordered_set<std::string> ModifyStorage::getModifiesProc(std::string varName) {
    if (varToProcModifiesMap.find(varName) != varToProcModifiesMap.end()) {
        return varToProcModifiesMap.at(varName);
    } else {
        std::unordered_set<std::string> empty;
        return empty;
    }
}

std::unordered_set<std::string> ModifyStorage::getModifiesProc() {
    std::unordered_set<std::string> allModifiesProc;
    for (const auto &itr : procToVarModifiesMap) {
        allModifiesProc.insert(itr.first);
    }
    return allModifiesProc;
}

std::unordered_set<std::string> ModifyStorage::getModifiesVariableByStmt(int stmtLine) {
    if (stmtNumsToVarModifiesMap.find(stmtLine) != stmtNumsToVarModifiesMap.end()) {
        return stmtNumsToVarModifiesMap.at(stmtLine);
    } else {
        std::unordered_set<std::string> empty;
        return empty;
    }
}

std::unordered_set<std::string> ModifyStorage::getModifiesVariableByProc(std::string procName) {
    if (procToVarModifiesMap.find(procName) != procToVarModifiesMap.end()) {
        return procToVarModifiesMap.at(procName);
    } else {
        std::unordered_set<std::string> empty;
        return empty;
    }
}

std::unordered_set<int> ModifyStorage::getAllModifiesStatements(std::string designEnt) {
    if (designEnt == STMT) {
        std::unordered_set<int> allModifiesStmt;
        for (auto &itr : stmtTypeToStmtNum) {
            allModifiesStmt.insert(itr.second.begin(), itr.second.end());
        }
        return allModifiesStmt;
    } else {
        if (stmtTypeToStmtNum.find(designEnt) != stmtTypeToStmtNum.end()) {
            return stmtTypeToStmtNum.at(designEnt);
        } else {
            std::unordered_set<int> empty;
            return empty;
        }
    }
}

StringPairResultSet ModifyStorage::getAllProcModifiesVarRelationPair() {
    StringPairResultSet resultSet;
    auto procToVarModifiesMapItr = procToVarModifiesMap.begin();
    while (procToVarModifiesMapItr != procToVarModifiesMap.end()) {
        for (auto modifiedVar : procToVarModifiesMapItr->second) {
            resultSet.emplace(procToVarModifiesMapItr->first, modifiedVar);
        }

        procToVarModifiesMapItr++;
    }

    return resultSet;
}

IntStringPairResultSet ModifyStorage::getAllStmtModifiesVarRelationPair(std::string stmtEnt) {
    IntStringPairResultSet resultSet;

    if (stmtEnt != STMT) {
        auto varToStmtModifiesMapItr = varToStmtModifiesMap.begin();
        while (varToStmtModifiesMapItr != varToStmtModifiesMap.end()) {
            if (varToStmtModifiesMapItr->second.find(stmtEnt) == varToStmtModifiesMapItr->second.end()) {
                varToStmtModifiesMapItr++;
                continue;
            }

            for (auto stmtLine : varToStmtModifiesMapItr->second[stmtEnt]) {
                resultSet.emplace(stmtLine, varToStmtModifiesMapItr->first);
            }
            varToStmtModifiesMapItr++;
        }
    } else {
        auto stmtNumsToVarModifiesMapItr = stmtNumsToVarModifiesMap.begin();
        while (stmtNumsToVarModifiesMapItr != stmtNumsToVarModifiesMap.end()) {
            for (auto varModified : stmtNumsToVarModifiesMapItr->second) {
                resultSet.emplace(stmtNumsToVarModifiesMapItr->first, varModified);
            }
            stmtNumsToVarModifiesMapItr++;
        }
    }

    return resultSet;
}

void ModifyStorage::clear() {
    varToStmtModifiesMap.clear();
    stmtNumsToVarModifiesMap.clear();
    procToVarModifiesMap.clear();
    varToProcModifiesMap.clear();
    stmtTypeToStmtNum.clear();
}
