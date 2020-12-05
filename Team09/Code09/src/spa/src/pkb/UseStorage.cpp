#include "UseStorage.h"

UseStorage *UseStorage::inst_ = NULL;

UseStorage *UseStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new UseStorage();
    }
    return inst_;
}

void UseStorage::setStmtToVar(int stmtLine, std::string varName) {
    if (stmtNumsToVarUsesMap.find(stmtLine) != stmtNumsToVarUsesMap.end()) {
        stmtNumsToVarUsesMap.at(stmtLine).emplace(varName);
    } else {
        std::unordered_set<std::string> varList;
        varList.emplace(varName);
        stmtNumsToVarUsesMap.emplace(stmtLine, varList);
    }
}

void UseStorage::setVarToStmt(int stmtLine, std::string designEnt, std::string varName) {
    if (varToStmtUsesMap.find(varName) != varToStmtUsesMap.end()) {
        std::unordered_map<std::string, std::unordered_set<int>> &designEntToStmt = varToStmtUsesMap.at(varName);
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
        varToStmtUsesMap.emplace(varName, designEntToStmt);
    }
}

void UseStorage::setProcToVar(std::string proc, std::string varName) {
    if (procToVarUsesMap.find(proc) != procToVarUsesMap.end()) {
        procToVarUsesMap.at(proc).emplace(varName);
    } else {
        std::unordered_set<std::string> varList;
        varList.emplace(varName);
        procToVarUsesMap.emplace(proc, varList);
    }
}

void UseStorage::setVarToProc(std::string proc, std::string varName) {
    if (varToProcUsesMap.find(varName) != varToProcUsesMap.end()) {
        varToProcUsesMap.at(varName).emplace(proc);
    } else {
        std::unordered_set<std::string> procList;
        procList.emplace(proc);
        varToProcUsesMap.emplace(varName, procList);
    }
}

void UseStorage::setStmtTypeToStmt(std::string designEnt, int stmtLine) {
    if (stmtTypeToStmtNum.find(designEnt) != stmtTypeToStmtNum.end()) {
        stmtTypeToStmtNum.at(designEnt).emplace(stmtLine);
    } else {
        std::unordered_set<int> stmtSet{stmtLine};
        stmtTypeToStmtNum.emplace(designEnt, stmtSet);
    }
}

bool UseStorage::isUses(int stmtLine, std::string varName) {
    if (stmtNumsToVarUsesMap.find(stmtLine) != stmtNumsToVarUsesMap.end()) {
        if (stmtNumsToVarUsesMap.at(stmtLine).find(varName) != stmtNumsToVarUsesMap.at(stmtLine).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool UseStorage::isUsesByEnt(std::string stmtEnt, std::string varName) {
    if (varToStmtUsesMap.find(varName) != varToStmtUsesMap.end()) {
        if (varToStmtUsesMap.at(varName).find(stmtEnt) != varToStmtUsesMap.at(varName).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool UseStorage::isUsesByProc(std::string procName, std::string varName) {
    if (varToProcUsesMap.find(varName) != varToProcUsesMap.end()) {
        if (varToProcUsesMap.at(varName).find(procName) != varToProcUsesMap.at(varName).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

std::unordered_set<int> UseStorage::getUsesStatement(std::string stmtEnt, std::string varName) {
    if (varToStmtUsesMap.find(varName) != varToStmtUsesMap.end()) {
        std::unordered_map<std::string, std::unordered_set<int>> &designEntToStmts = varToStmtUsesMap.at(varName);
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

// unordered_set<int> UseStorage::getUsesStatement(string varName)
//{
//	if (varToStmtUsesMap.find(varName) != varToStmtUsesMap.end())
//	{
//		unordered_map<DESIGN_ENT, unordered_set<int>> &designEntToStmts = varToStmtUsesMap.at(varName);
//		unordered_set<int> result;
//		for (auto& itr : designEntToStmts) {
//			result.insert(itr.second.begin(), itr.second.end());
//		}
//		return result;
//	}
//	else
//	{
//		unordered_set<int> empty;
//		return empty;
//	}
//}

std::unordered_set<std::string> UseStorage::getUsesProc(std::string varName) {
    if (varToProcUsesMap.find(varName) != varToProcUsesMap.end()) {
        return varToProcUsesMap.at(varName);
    } else {
        std::unordered_set<std::string> empty;
        return empty;
    }
}

std::unordered_set<std::string> UseStorage::getUsesProc() {
    std::unordered_set<std::string> allModifiesProc;
    for (const auto &itr : procToVarUsesMap) {
        allModifiesProc.insert(itr.first);
    }
    return allModifiesProc;
}

std::unordered_set<std::string> UseStorage::getUsesVariablesByStmt(int stmtLine) {
    if (stmtNumsToVarUsesMap.find(stmtLine) != stmtNumsToVarUsesMap.end()) {
        return stmtNumsToVarUsesMap.at(stmtLine);
    } else {
        std::unordered_set<std::string> empty;
        return empty;
    }
}

std::unordered_set<std::string> UseStorage::getUsesVariableByProc(std::string procName) {
    if (procToVarUsesMap.find(procName) != procToVarUsesMap.end()) {
        return procToVarUsesMap.at(procName);
    } else {
        std::unordered_set<std::string> empty;
        return empty;
    }
}

std::unordered_set<int> UseStorage::getAllUsesStatements(std::string designEnt) {
    if (designEnt == STMT) {
        std::unordered_set<int> allUsesStatements;
        for (auto &itr : stmtTypeToStmtNum) {
            allUsesStatements.insert(itr.second.begin(), itr.second.end());
        }
        return allUsesStatements;
    } else {
        if (stmtTypeToStmtNum.find(designEnt) != stmtTypeToStmtNum.end()) {
            return stmtTypeToStmtNum.at(designEnt);
        } else {
            std::unordered_set<int> empty;
            return empty;
        }
    }
}

StringPairResultSet UseStorage::getAllProcUsesVarRelationPair() {
    StringPairResultSet resultSet;
    auto procToVarUsesMapItr = procToVarUsesMap.begin();
    while (procToVarUsesMapItr != procToVarUsesMap.end()) {
        for (auto usedVar : procToVarUsesMapItr->second) {
            resultSet.emplace(procToVarUsesMapItr->first, usedVar);
        }

        procToVarUsesMapItr++;
    }

    return resultSet;
}

IntStringPairResultSet UseStorage::getAllStmtUsesVarRelationPair(std::string stmtEnt) {
    IntStringPairResultSet resultSet;

    if (stmtEnt != STMT) {
        auto varToStmtUsesMapItr = varToStmtUsesMap.begin();
        while (varToStmtUsesMapItr != varToStmtUsesMap.end()) {
            if (varToStmtUsesMapItr->second.find(stmtEnt) == varToStmtUsesMapItr->second.end()) {
                varToStmtUsesMapItr++;
                continue;
            }

            for (auto stmtLine : varToStmtUsesMapItr->second[stmtEnt]) {
                resultSet.emplace(stmtLine, varToStmtUsesMapItr->first);
            }
            varToStmtUsesMapItr++;
        }
    } else {
        auto stmtNumsToVarUsesMapItr = stmtNumsToVarUsesMap.begin();
        while (stmtNumsToVarUsesMapItr != stmtNumsToVarUsesMap.end()) {
            for (auto varUsed : stmtNumsToVarUsesMapItr->second) {
                resultSet.emplace(stmtNumsToVarUsesMapItr->first, varUsed);
            }
            stmtNumsToVarUsesMapItr++;
        }
    }

    return resultSet;
}

void UseStorage::clear() {
    varToStmtUsesMap.clear();
    stmtNumsToVarUsesMap.clear();
    procToVarUsesMap.clear();
    varToProcUsesMap.clear();
    stmtTypeToStmtNum.clear();
}
