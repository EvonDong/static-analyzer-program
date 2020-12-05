#include "ParentStorage.h"

ParentStorage *ParentStorage::inst_ = NULL;

ParentStorage *ParentStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new ParentStorage();
    }
    return inst_;
}

void ParentStorage::setParent(int parentStmt, int childStmt) {
    if (parentStmt >= childStmt) {
        return;
    }

    auto itr = parentMap.find(parentStmt);
    if (itr != parentMap.end()) {
        itr->second.emplace(childStmt);
    } else {
        std::unordered_set<int> childStmtSet{childStmt};
        parentMap.emplace(parentStmt, childStmtSet);
    }

    reverseParentMap[childStmt] = parentStmt;

    parentSet.emplace(parentStmt);
}

void ParentStorage::setParentStar(int parentStmt, int childStmt) {
    if (parentStmt >= childStmt) {
        return;
    }

    auto parentMapItr = parentStarMap.find(parentStmt);
    if (parentMapItr != parentStarMap.end()) {
        parentMapItr->second.emplace(childStmt);
    } else {
        std::unordered_set<int> childStmtSet{childStmt};
        parentStarMap.emplace(parentStmt, childStmtSet);
    }

    auto reverseParentMapItr = reverseParentStarMap.find(childStmt);
    if (reverseParentMapItr != reverseParentStarMap.end()) {
        reverseParentMapItr->second.emplace(parentStmt);
    } else {
        std::unordered_set<int> parentStmtSet{parentStmt};
        reverseParentStarMap.emplace(childStmt, parentStmtSet);
    }

    parentSet.emplace(parentStmt);
}

bool ParentStorage::isParent(int parentStmt, int childStmt) {
    if (parentStmt >= childStmt) {
        return false;
    }

    const auto itr = parentMap.find(parentStmt);
    if (itr != parentMap.end()) {
        return itr->second.find(childStmt) != itr->second.end();
    }
    return false;
}

bool ParentStorage::isParentStar(int parentStmt, int childStmt) {
    if (parentStmt >= childStmt) {
        return false;
    }

    const auto itr = parentStarMap.find(parentStmt);
    return itr != parentStarMap.end() && itr->second.find(childStmt) != itr->second.end();
}

std::unordered_set<int> ParentStorage::getAllNestedStatements(int parentStmt) {
    const auto itr = parentMap.find(parentStmt);
    if (itr != parentMap.end()) {
        return itr->second;
    }
    std::unordered_set<int> emptySet;
    return emptySet;
}

std::unordered_set<int> ParentStorage::getAllNestedStatementsT(int parentStmt) {
    const auto itr = parentStarMap.find(parentStmt);
    if (itr != parentStarMap.end()) {
        return itr->second;
    }
    std::unordered_set<int> emptySet;
    return emptySet;
}

std::unordered_set<int> ParentStorage::getParentStatement(int childStmt) {
    const auto itr = reverseParentMap.find(childStmt);
    std::unordered_set<int> result;
    if (itr != reverseParentMap.end()) {
        result.emplace(itr->second);
    }
    return result;
}

std::unordered_set<int> ParentStorage::getParentStatementsT(int childStmt) {
    const auto itr = reverseParentStarMap.find(childStmt);
    if (itr != reverseParentStarMap.end()) {
        return itr->second;
    }

    std::unordered_set<int> emptySet;
    return emptySet;
}

std::unordered_set<int> ParentStorage::getAllParentStmts() {
    return parentSet;
}

std::unordered_map<int, std::unordered_set<int>> *ParentStorage::getAllParentRelationPair() {
    return &parentMap;
}

std::unordered_map<int, std::unordered_set<int>> *ParentStorage::getAllParentStarRelationPair() {
    return &parentStarMap;
}

void ParentStorage::clear() {
    parentMap.clear();
    parentStarMap.clear();
    reverseParentMap.clear();
    reverseParentStarMap.clear();
    parentSet.clear();
}
