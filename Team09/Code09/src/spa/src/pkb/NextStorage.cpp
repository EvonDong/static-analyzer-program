#include "NextStorage.h"

NextStorage *NextStorage::inst_ = NULL;

NextStorage *NextStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new NextStorage();
    }

    return inst_;
}

void NextStorage::setAdjacencyListMap(std::unordered_map<int, std::set<int>> &mappings) {
    adjacencyListMap.insert(mappings.begin(), mappings.end());
}

void NextStorage::setReverseAdjacencyListMap(std::unordered_map<int, std::set<int>> &mappings) {

    std::unordered_map<int, std::set<int>>::iterator itr = mappings.begin();
    while (itr != mappings.end()) {
        for (auto callee : itr->second) {
            if (reverseAdjacencyListMap.find(callee) != reverseAdjacencyListMap.end()) {
                reverseAdjacencyListMap.at(callee).emplace(itr->first);
            } else {
                std::set<int> callerSet{itr->first};
                reverseAdjacencyListMap.emplace(callee, callerSet);
            }
        }

        itr++;
    }
}

void NextStorage::setupCache() {
    const int totalStmtNum = (procedureRangeList.rbegin())->second;
    nextStarBoolCache.reserve(totalStmtNum * totalStmtNum);

    // initialise with default values
    for (int index = 0; index < (totalStmtNum * totalStmtNum); index++) {
        nextStarBoolCache.push_back(false);
    }

    for (auto &_ : procedureRangeList) {
        nextStarBoolCacheState.push_back(false);
    }
}

void NextStorage::setProcedureRange(int startLine, int endLine) {
    procedureRangeList.emplace_back(startLine, endLine);
}

void NextStorage::setStmtToType(int stmtLine, std::string stmtType) {
    stmtToTypeMap.emplace(stmtLine, stmtType);
}

void NextStorage::setStmtToWhileParent(int childStmt, int whileParent) {
    std::unordered_map<int, std::set<int>>::iterator itr = stmtToWhileParentMap.find(childStmt);
    if (itr != stmtToWhileParentMap.end()) {
        stmtToWhileParentMap.at(childStmt).emplace(whileParent);
    } else {
        std::set<int> whileParentSet{whileParent};
        stmtToWhileParentMap.emplace(childStmt, whileParentSet);
    }
}

bool NextStorage::hasNext(int s1) {
    std::unordered_map<int, std::set<int>>::const_iterator itr = adjacencyListMap.find(s1);
    if (itr != adjacencyListMap.end()) {
        return !(itr->second.empty());
    }
    return false;
}

bool NextStorage::hasNext(std::string stmtType) {
    std::unordered_map<int, std::string>::iterator itr = stmtToTypeMap.begin();
    while (itr != stmtToTypeMap.end()) {
        if (stmtType == STMT || itr->second == stmtType) {
            if (hasNext(itr->first)) {
                return true;
            }
        }
        itr++;
    }
    return false;
}

bool NextStorage::hasPrevious(int s1) {
    std::unordered_map<int, std::set<int>>::const_iterator itr = reverseAdjacencyListMap.find(s1);
    if (itr != reverseAdjacencyListMap.end()) {
        return !(itr->second.empty());
    }
    return false;
}

bool NextStorage::hasPrevious(std::string stmtType) {
    std::unordered_map<int, std::string>::iterator itr = stmtToTypeMap.begin();
    while (itr != stmtToTypeMap.end()) {
        if (stmtType == STMT || itr->second == stmtType) {
            if (hasPrevious(itr->first)) {
                return true;
            }
        }
        itr++;
    }
    return false;
}

bool NextStorage::isNext(int s1, int s2, bool isRecursive) {
    if (!isRecursive) {
        std::unordered_map<int, std::set<int>>::const_iterator itr = adjacencyListMap.find(s1);
        if (itr != adjacencyListMap.end()) {
            return itr->second.find(s2) != itr->second.end();
        } else {
            return false;
        }
    } else {
        auto res = checkObviousNextStarRs(s1, s2);
        if (res.first) {
            return res.second;
        }

        return validateReachableUsingReverseBFS(s1, s2);
    }
}

std::unordered_set<int> NextStorage::getNextStatements(std::string stmtType) {
    std::unordered_set<int> nextStatements;

    for (auto &itr : reverseAdjacencyListMap) {
        if (stmtType == STMT || stmtToTypeMap.at(itr.first) == stmtType) {
            nextStatements.emplace(itr.first);
        }
    }
    return nextStatements;
}

std::unordered_set<int> NextStorage::getPreviousStatements(std::string stmtType) {
    std::unordered_set<int> previousStatements;

    for (auto &itr : adjacencyListMap) {
        if (stmtType == STMT || stmtToTypeMap.at(itr.first) == stmtType) {
            previousStatements.emplace(itr.first);
        }
    }
    return previousStatements;
}

std::unordered_set<int> NextStorage::getPreviousStatements(int s, std::string stmtTypeFilter) {
    return findAllImmediateNextNodes(s, stmtTypeFilter, this->reverseAdjacencyListMap);
}

std::unordered_set<int> NextStorage::getPreviousStatementsT(int s, std::string stmtTypeFilter) {
    std::unordered_set<int> reachableNodes;
    const int totalStmtNum = (procedureRangeList.rbegin())->second;

    // handle invalid stmt nums
    if (s > totalStmtNum || s < 1) {
        return reachableNodes;
    }

    // Get procedure range that this statement belongs in
    int procRangeIndex{0};
    std::pair<int, int> procRange;
    for (; procRangeIndex <= procedureRangeList.size(); procRangeIndex++) {
        procRange = procedureRangeList.at(procRangeIndex);
        if (procRange.first <= s && s <= procRange.second) {
            break;
        }
    }

    populateNextStarBoolCache(procRangeIndex);

    for (int neigbourNum = procRange.first; neigbourNum <= procRange.second; neigbourNum++) {
        if (nextStarBoolCache[(neigbourNum - 1) * totalStmtNum + s - 1] &&
            (stmtTypeFilter == STMT || stmtToTypeMap.at(neigbourNum) == stmtTypeFilter)) {
            reachableNodes.emplace(neigbourNum);
        }
    }

    return reachableNodes;
}

std::unordered_set<int> NextStorage::getNextStatements(int s, std::string stmtTypeFilter) {
    return findAllImmediateNextNodes(s, stmtTypeFilter, this->adjacencyListMap);
}

std::unordered_set<int> NextStorage::getNextStatementsT(int s, std::string stmtTypeFilter) {

    std::unordered_set<int> reachableNodes;
    const int totalStmtNum = (procedureRangeList.rbegin())->second;

    // handle invalid stmt nums
    if (s > totalStmtNum || s < 1) {
        return reachableNodes;
    }

    // Get procedure range that this statement belongs in
    int procRangeIndex{0};
    std::pair<int, int> procRange;
    for (; procRangeIndex <= procedureRangeList.size(); procRangeIndex++) {
        procRange = procedureRangeList.at(procRangeIndex);
        if (procRange.first <= s && s <= procRange.second) {
            break;
        }
    }

    populateNextStarBoolCache(procRangeIndex);

    for (int neighbourNum = procRange.first; neighbourNum <= procRange.second; neighbourNum++) {
        if (nextStarBoolCache[(s - 1) * totalStmtNum + neighbourNum - 1] &&
            (stmtTypeFilter == STMT || stmtToTypeMap.at(neighbourNum) == stmtTypeFilter)) {
            reachableNodes.emplace(neighbourNum);
        }
    }

    return reachableNodes;
}

std::unordered_set<int> NextStorage::getNextClosestStatement(int s) {
    if (adjacencyListMap.find(s) != adjacencyListMap.end()) {
        int nextStmt = *adjacencyListMap[s].begin(); // Since the adjacency list is already a sorted set.
                                                     // Hence the first stmt is the closest statement
        return std::unordered_set<int>{nextStmt};
    } else {
        return std::unordered_set<int>{};
    }
}

IntPairResultSet NextStorage::getNextRelationPairings(std::unordered_set<int> &lhsStmtSet, std::string rhsStmtType) {
    IntPairResultSet nextRelationPairingSet;

    std::unordered_set<int>::iterator lhsStmtSetItr = lhsStmtSet.begin();
    while (lhsStmtSetItr != lhsStmtSet.end()) {
        std::unordered_map<int, std::set<int>>::const_iterator adjacencyListMapItr = adjacencyListMap.find(*lhsStmtSetItr);
        if (adjacencyListMapItr != adjacencyListMap.end()) {
            for (auto neighbour : adjacencyListMapItr->second) {
                if (rhsStmtType == STMT || stmtToTypeMap.at(neighbour) == rhsStmtType) {
                    nextRelationPairingSet.emplace(adjacencyListMapItr->first, neighbour);
                }
            }
        }

        lhsStmtSetItr++;
    }

    return nextRelationPairingSet;
}

IntPairResultSet NextStorage::getNextRelationPairingsT(std::unordered_set<int> &lhsStmtSet, std::string rhsStmtType) {
    IntPairResultSet nextRelationPairingSet;

    populateNextStarBoolCache();

    const int totalStmtNum = (procedureRangeList.rbegin())->second;
    for (auto &lhsStmt : lhsStmtSet) {
        std::pair<int, int> range;
        for (auto &procRange : procedureRangeList) {
            if (procRange.first <= lhsStmt && lhsStmt <= procRange.second) {
                range = procRange;
                break;
            }
        }

        for (int j = range.first; j <= range.second; j++)
            if (nextStarBoolCache[(lhsStmt - 1) * totalStmtNum + j - 1] &&
                (rhsStmtType == STMT || stmtToTypeMap.at(j) == rhsStmtType)) {
                nextRelationPairingSet.emplace(lhsStmt, j);
            }
    }

    return nextRelationPairingSet;
}

std::pair<bool, bool> NextStorage::checkObviousNextStarRs(int s1, int s2) {
    // 1: Check if s1 and s2 are valid stmtLines
    // i.e. within the range of number of stmt lines in the SIMPLE program.
    int endingStatementLine = (procedureRangeList.rbegin())->second;
    if (s1 > endingStatementLine || s2 > endingStatementLine) {
        return std::make_pair(true, false);
    }

    // 2: Check if the result has been evaluated and cached
    bool hasAlreadyFoundNextStarRelation = nextStarBoolCache[(s1 - 1) * endingStatementLine + s2 - 1];
    if (hasAlreadyFoundNextStarRelation) {
        return std::make_pair(true, true);
    }

    // 3: If s1 and s2 are the same stmtLine, check if s1 is a while
    // stmt or s1(i.e. s2) is nested in a while statement.
    if (s1 == s2) {
        bool isWhileStatementType = stmtToTypeMap.at(s1) == WHILE_STMT;
        bool isNestedInWhileStatement = (stmtToWhileParentMap.find(s1) != stmtToWhileParentMap.end());
        return std::make_pair(true, isWhileStatementType || isNestedInWhileStatement);
    }

    // 4: Check if s1 and s2 are in the same procedure since a Next r/s
    // must be between 2 stmts within the same procedure.
    for (auto &range : procedureRangeList) {
        bool isS1InProc = range.first <= s1 < range.second;
        bool isS2InProc = range.first <= s2 < range.second;

        if (isS1InProc != isS2InProc) {
            // Either s1 or s2 belongs to this procedure range whilst the other
            // is not, meaning that they are not in the same procedure, hence an
            // invalid Next relationship.
            return std::make_pair(true, false);
        } else if (isS1InProc && isS2InProc) {
            // Both s1 and s2 belongs to this procedure range,
            // hence a valid Next relationship; proceed with checking they are nested in the same while loop. We can
            // infer that there is a path from one statement to another if they are nested in the same while loop.
            std::unordered_map<int, std::set<int>>::iterator itr1, itr2;
            itr1 = stmtToWhileParentMap.find(s1);
            itr2 = stmtToWhileParentMap.find(s2);
            if (itr1 != stmtToWhileParentMap.end() && itr2 != stmtToWhileParentMap.end()) {
                std::set<int> s1WhileParent = itr1->second;
                std::set<int> s2WhileParent = itr2->second;
                std::set<int> intersection;
                std::set_intersection(s1WhileParent.begin(), s1WhileParent.end(), s2WhileParent.begin(), s2WhileParent.end(),
                                      std::inserter(intersection, intersection.begin()));

                bool areNestedInSameWhile = !intersection.empty();
                return std::make_pair(areNestedInSameWhile, areNestedInSameWhile);
            }

            // Both s1 and s2 belong to the same procedure but does not belong to the same while loop, therefore we
            // cannot infer if there is a path between them and need to perform BFS instead.
            return std::make_pair(false, false);
        }
    }
}

bool NextStorage::validateReachableUsingReverseBFS(int startNode, int endNode) {
    std::unordered_set<int> visited;
    std::list<int> queue;

    queue.push_back(endNode);

    while (!queue.empty()) {
        int currNode = queue.front();
        queue.pop_front();
        visited.emplace(currNode);

        std::unordered_map<int, std::set<int>>::iterator reverseAdjListMapItr = reverseAdjacencyListMap.find(currNode);
        // Current node has no neighbouring nodes.
        if (reverseAdjListMapItr == reverseAdjacencyListMap.end()) {
            continue;
        }

        std::set<int>::iterator currNeighbour = reverseAdjListMapItr->second.begin();
        std::set<int>::iterator end = reverseAdjListMapItr->second.end();
        for (; currNeighbour != end; currNeighbour++) {
            updateNextStarMapCache(*currNeighbour, endNode);

            // Skip if you have already visited the neighbour node.
            if (visited.find(*currNeighbour) != visited.end()) {
                continue;
            }

            if (startNode == *currNeighbour) {
                return true;
            } else {
                queue.push_back(*currNeighbour);
            }

            // The adjacency list is sorted in ascending order. Therefore when there is branching in the path and if the
            // startNode i.e. the node to traverse to, is smaller than the node in the first branch, the second branch
            // can be ignored. For example if startNode is 2 and there is branch in the path to nodes 4 and 9, this
            // means that node 9 can be ignored since there is no way node 2 lies in the path from 5 to 9.
            if (startNode < *currNeighbour) {
                break;
            }
        }
    }
    return false;
}

std::unordered_set<int> NextStorage::findAllImmediateNextNodes(int startNode, std::string stmtTypeFilter,
                                                               std::unordered_map<int, std::set<int>> &targetAdjacencyListMap) {

    std::unordered_map<int, std::set<int>>::iterator itr = targetAdjacencyListMap.find(startNode);
    if (itr == targetAdjacencyListMap.end()) {
        std::unordered_set<int> empty;
        return empty;
    } else {
        std::unordered_set<int> filteredResult;
        for (auto stmtItr : itr->second) {
            if (stmtTypeFilter == STMT || stmtToTypeMap.at(stmtItr) == stmtTypeFilter) {
                filteredResult.emplace(stmtItr);
            }
        }
        return filteredResult;
    }
}

void NextStorage::updateNextStarMapCache(int s1, int s2) {
    int s1ZeroBased{s1 - 1}, s2ZeroBased{s2 - 1}, totalStmts{procedureRangeList.rbegin()->second};
    nextStarBoolCache[s1ZeroBased * totalStmts + s2ZeroBased];
}

void NextStorage::populateNextStarBoolCache() {
    for (int index = 0; index < procedureRangeList.size(); index++) {
        populateNextStarBoolCache(index);
    }
}

void NextStorage::populateNextStarBoolCache(int procRangeIndex) {
    if (nextStarBoolCacheState.at(procRangeIndex)) {
        return;
    }

    const int totalStmtNum{procedureRangeList.rbegin()->second};

    std::pair<int, int> procedureRange = procedureRangeList.at(procRangeIndex);
    for (int lhsStmt = procedureRange.first; lhsStmt <= procedureRange.second; lhsStmt++) {
        for (auto reachableNode : findAllReachableNodesUsingBFS(lhsStmt)) {
            nextStarBoolCache[(lhsStmt - 1) * totalStmtNum + reachableNode - 1] = true;
        }
    }

    nextStarBoolCacheState.at(procRangeIndex) = true;
}

void NextStorage::clear() {
    adjacencyListMap.clear();
    reverseAdjacencyListMap.clear();
    stmtToTypeMap.clear();
    procedureRangeList.clear();
    stmtToWhileParentMap.clear();

    nextStarBoolCache.clear();
    nextStarBoolCacheState.clear();
}

void NextStorage::resetCache() {
    std::fill(nextStarBoolCache.begin(), nextStarBoolCache.end(), false);
    std::fill(nextStarBoolCacheState.begin(), nextStarBoolCacheState.end(), false);
}

std::unordered_set<int> NextStorage::findAllReachableNodesUsingBFS(int startNode) {

    std::unordered_set<int> reachableNodes;
    std::unordered_set<int> visited;
    std::list<int> queue;

    queue.push_back(startNode);

    while (!queue.empty()) {
        int currNode = queue.front();
        queue.pop_front();
        visited.emplace(currNode);

        std::unordered_map<int, std::set<int>>::iterator itr = adjacencyListMap.find(currNode);
        // Current node has no neighbouring nodes.
        if (itr == adjacencyListMap.end()) {
            continue;
        }

        std::set<int>::iterator currNeighbour = itr->second.begin();
        std::set<int>::iterator end = itr->second.end();
        for (; currNeighbour != end; currNeighbour++) {
            reachableNodes.emplace(*currNeighbour);

            if (visited.find(*currNeighbour) == visited.end()) {
                queue.push_back(*currNeighbour);
            }
        }
    }
    return reachableNodes;
}

const std::unordered_map<int, std::set<int>> *NextStorage::getNextAdjMap() {
    return &adjacencyListMap;
}
const std::unordered_map<int, std::set<int>> *NextStorage::getReverseAdjMap() {
    return &reverseAdjacencyListMap;
}
