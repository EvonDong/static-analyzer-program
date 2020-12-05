#include "GeneralStorage.h"

GeneralStorage *GeneralStorage::inst_ = NULL;

GeneralStorage *GeneralStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new GeneralStorage();
    }
    return inst_;
}

void GeneralStorage::setVar(std::string varName) {
    varSet.emplace(varName);
}

void GeneralStorage::storeStmtObj(Statement stmtObj) {
    stmtNumToStmtObjMap.emplace(stmtObj.stmtNum, stmtObj);

    stmtSet.emplace(stmtObj.stmtNum);

    auto itr = stmtTypeToStmtNumMap.find(stmtObj.type);
    if (itr != stmtTypeToStmtNumMap.end()) {
        itr->second.emplace(stmtObj.stmtNum);
    } else {
        std::unordered_set<int> stmtNumMap{stmtObj.stmtNum};
        stmtTypeToStmtNumMap.emplace(stmtObj.type, stmtNumMap);
    }
}

void GeneralStorage::setProcedure(std::string procName, int firstStmt, int lastStmt) {
    procedureSet.emplace(procName);
    procedureRangeMap.emplace(procName, std::make_pair(firstStmt, lastStmt));
}

void GeneralStorage::setVarToStmtNum(std::string varName, int stmtNum) {
    auto stmtTypeToStmtNumItr = varToStmtNumMap.find(varName);
    if (stmtTypeToStmtNumItr == varToStmtNumMap.end()) {
        std::unordered_set<int> stmtSet{stmtNum};
        varToStmtNumMap.emplace(varName, stmtSet);

    } else {
        stmtTypeToStmtNumItr->second.emplace(stmtNum);
    }
}

void GeneralStorage::setConstant(int constantVal) {
    constantsSet.emplace(constantVal);
}

std::unordered_set<std::string> GeneralStorage::getAllVars() {
    return this->varSet;
}

std::unordered_set<std::string> GeneralStorage::getAllProcedures() {
    return this->procedureSet;
}

bool GeneralStorage::isValidStatementNum(int stmtNum, std::string stmtType) {
    auto stmtNumToStmtObjMapItr = stmtNumToStmtObjMap.find(stmtNum);
    if (stmtNumToStmtObjMapItr != stmtNumToStmtObjMap.end()) {
        return stmtType == STMT ? true : stmtNumToStmtObjMapItr->second.type == stmtType;
    }
    return false;
}

Statement GeneralStorage::getStmtObjByStmtNum(int stmtNum) {
    if (stmtNumToStmtObjMap.find(stmtNum) != stmtNumToStmtObjMap.end()) {
        return stmtNumToStmtObjMap.at(stmtNum);
    } else {
        return Statement{stmtNum, INVALID, std::unordered_set<std::string>{}, std::unordered_set<int>{}};
    }
}

std::unordered_set<int> GeneralStorage::getStmtNumByStmtType(std::string entType) {
    if (entType == STMT) {
        return stmtSet;
    } else {
        if (stmtTypeToStmtNumMap.find(entType) == stmtTypeToStmtNumMap.end()) {
            std::unordered_set<int> emptySet;
            return emptySet;
        }

        return stmtTypeToStmtNumMap.at(entType);
    }
}

std::unordered_set<int> GeneralStorage::getStmtsByVar(std::string varName) {
    auto stmtTypeToStmtNumItr = varToStmtNumMap.find(varName);
    if (stmtTypeToStmtNumItr != varToStmtNumMap.end()) {
        return varToStmtNumMap.at(varName);
    }

    std::unordered_set<int> emptySet;
    return emptySet;
}

std::unordered_set<std::string> GeneralStorage::getVarByStmtNum(int stmtNum) {
    auto itr = stmtNumToStmtObjMap.find(stmtNum);

    if (itr == stmtNumToStmtObjMap.end()) {
        std::unordered_set<std::string> emptySet;
        return emptySet;
    }

    return itr->second.varSet;
}

std::string GeneralStorage::getStmtTypeByStmtNum(int stmtNum) {
    if (stmtNumToStmtObjMap.find(stmtNum) == stmtNumToStmtObjMap.end()) {
        return std::string{INVALID};
    }
    return stmtNumToStmtObjMap.at(stmtNum).type;
}

std::unordered_set<int> GeneralStorage::getAllConstants() {
    return constantsSet;
}

std::pair<int, int> GeneralStorage::getProcedureRange(std::string procName) {
    if (procedureRangeMap.find(procName) != procedureRangeMap.end()) {
        return procedureRangeMap[procName];
    } else {
        std::pair<int, int> empty;
        return empty;
    }
}

std::string GeneralStorage::getProcedureOfCurrStmt(int stmtNum) {
    for (auto &procItr : procedureRangeMap) {
        if (procItr.second.first <= stmtNum && stmtNum <= procItr.second.second) {
            return procItr.first;
        }
    }

    return std::string{};
}

void GeneralStorage::clear() {
    varSet.clear();
    stmtSet.clear();
    stmtNumToStmtObjMap.clear();
    procedureSet.clear();
    stmtTypeToStmtNumMap.clear();
    varToStmtNumMap.clear();
    constantsSet.clear();
    procedureRangeMap.clear();
}
