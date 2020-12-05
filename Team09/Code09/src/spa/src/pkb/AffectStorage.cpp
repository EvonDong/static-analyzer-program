#include "AffectStorage.h"

AffectStorage *AffectStorage::inst_ = NULL;
NextStorage *AffectStorage::nextStore = NextStorage::getInstance();
ModifyStorage *AffectStorage::modStore = ModifyStorage::getInstance();
UseStorage *AffectStorage::useStore = UseStorage::getInstance();
GeneralStorage *AffectStorage::genStore = GeneralStorage::getInstance();
ParentStorage *AffectStorage::parentStore = ParentStorage::getInstance();

AffectStorage *AffectStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new AffectStorage();
    }

    return inst_;
}

void AffectStorage::setupAffectStorage() {
    if (cfgPtr == nullptr) {
        cfgPtr = nextStore->getNextAdjMap();
    }

    if (reversedCfgPtr == nullptr) {
        reversedCfgPtr = nextStore->getReverseAdjMap();
    }
}

// This method must be called in order of the procedures since it does not internally sort the ranges.
void AffectStorage::setProcedureRange(int startNode, int endNode) {
    procedureRangeList.emplace_back(startNode, endNode);
}

bool AffectStorage::isEndOfProcedure(int s) {
    return (cfgPtr->find(s) == cfgPtr->end());
}

void AffectStorage::traverseOnlyPath(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited,
                                     int terminalStmt) {
    int nextStmt = *std::begin(cfgPtr->at(s));
    traverseCFG(nextStmt, lastModifiedMap, visited, terminalStmt);
}

int AffectStorage::findMergeStmt(int ifStmt) {
    std::unordered_set<int> visited;
    std::list<int> queue;

    queue.push_back(ifStmt);

    while (!queue.empty()) {
        int currNode = queue.front();
        queue.pop_front();
        visited.emplace(currNode);

        std::unordered_map<int, std::set<int>>::const_iterator cfgItr = cfgPtr->find(currNode);
        // Current node has no neighbouring nodes.
        if (cfgItr == cfgPtr->end()) {
            continue;
        }

        std::set<int>::const_iterator currNeighbour = cfgItr->second.begin();
        std::set<int>::const_iterator end = cfgItr->second.end();
        for (; currNeighbour != end; currNeighbour++) {

            // Skip if you have already visited the neighbour node.
            if (visited.find(*currNeighbour) != visited.end()) {
                continue;
            }

            if (!parentStore->isParentStar(ifStmt, *currNeighbour)) {
                return *currNeighbour;
            } else {
                queue.push_back(*currNeighbour);
            }
        }
    }

    return -1;
}

void AffectStorage::traverseCFG(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt) {
    if (s == terminalStmt) {
        return;
    }

    std::string stmtType = genStore->getStmtTypeByStmtNum(s); // get statement type
    visited.emplace(s);                                       // mark statement as visited

    // std::cout << "curr s: " << s << "\n";

    if (stmtType == ASSIGN) {
        setAffectsForVarUsed(s, lastModifiedMap);
        updateVarModified(s, lastModifiedMap);
        traverseAfterNonWhileIfStmt(s, lastModifiedMap, visited, terminalStmt);
    } else if (stmtType == WHILE_STMT) {
        traverseAfterWhileStmt(s, lastModifiedMap, visited, terminalStmt);
    } else if (stmtType == IF_STMT) {
        traverseAfterIfStmt(s, lastModifiedMap, visited, terminalStmt);
    } else if (stmtType == READ || stmtType == CALL) {
        removeVarModified(s, &lastModifiedMap);
        traverseAfterNonWhileIfStmt(s, lastModifiedMap, visited, terminalStmt);
    } else {
        traverseAfterNonWhileIfStmt(s, lastModifiedMap, visited, terminalStmt);
    }
}

void AffectStorage::removeVarModified(int s, LastModifiedMap *lastModifiedMap) {
    std::unordered_set<std::string> varsModified = modStore->getModifiesVariableByStmt(s);
    if (varsModified.size() > 0) {
        for (auto &modifiedVar : varsModified) {
            lastModifiedMap->erase(modifiedVar);
        }
    }
}

/*
 * Description: Adds the affects relation for the used variable of the statement
 * according to the last modified map with the modified variable mapped
 * to the statement number(s).
 */
void AffectStorage::setAffectsForVarUsed(int s, LastModifiedMap &lastModifiedMap) {
    // find statements that modifies usedVar in this statement, s
    std::unordered_set<std::string> varsUsed = useStore->getUsesVariablesByStmt(s);
    for (auto &usedVar : varsUsed) {
        if (lastModifiedMap.find(usedVar) != lastModifiedMap.end()) {
            for (auto &modifiedStmt : lastModifiedMap.at(usedVar)) {
                setAffects(modifiedStmt, s);
                setAffectedBy(s, modifiedStmt);
            }
        }
    }
}

/*
 * Description: Updates the last modified map with the modified variable mapped
 * to the statement number.
 */
void AffectStorage::updateVarModified(int s, LastModifiedMap &lastModifiedMap) {
    // track the variable modified in this statement
    std::unordered_set<std::string> varsModified = modStore->getModifiesVariableByStmt(s);
    for (auto &modifiedVar : varsModified) {
        addLastModified(lastModifiedMap, modifiedVar, s);
    }
}

void AffectStorage::traverseAfterIfStmt(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited,
                                        int terminalStmt) {
    int *ifElseStmts = new int[2];
    int idx = 0;
    for (auto &stmt : cfgPtr->at(s)) {
        ifElseStmts[idx] = stmt;
        idx++;
    }
    LastModifiedMap branchLmm1 = lastModifiedMap;
    LastModifiedMap branchLmm2 = lastModifiedMap;

    int mergedStmt = findMergeStmt(s);

    // if statements cannot be the last statement so no need to check if end of statement
    traverseCFG(ifElseStmts[0], branchLmm1, visited, mergedStmt);
    traverseCFG(ifElseStmts[1], branchLmm2, visited, mergedStmt);
    lastModifiedMap = merge(branchLmm1, branchLmm2);
    traverseCFG(mergedStmt, lastModifiedMap, visited, terminalStmt);
}

void AffectStorage::traverseAfterWhileStmt(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited,
                                           int terminalStmt) {
    LastModifiedMap currBeforeLmm = lastModifiedMap;
    traverseCFG(s + 1, lastModifiedMap, visited, s);
    // check if can exit while loop
    while (!isAllowedToExitWhileLoop(lastModifiedMap, currBeforeLmm)) {
        // cannot exit while loop so traverse into next stmt in while loop
        lastModifiedMap = merge(currBeforeLmm, lastModifiedMap);
        currBeforeLmm = lastModifiedMap;
        traverseCFG(s + 1, lastModifiedMap, visited, s);
    }

    // can exit while loop
    lastModifiedMap = merge(currBeforeLmm, lastModifiedMap);
    traverseOutOfWhile(s, lastModifiedMap, visited, terminalStmt);
}

void AffectStorage::traverseOutOfWhile(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited,
                                       int terminalStmt) {
    if (cfgPtr->at(s).size() > 1) {
        for (auto &stmt : cfgPtr->at(s)) {
            if (stmt != s + 1) {
                traverseCFG(stmt, lastModifiedMap, visited, terminalStmt);
            }
        }
    }
}

void AffectStorage::traverseAfterNonWhileIfStmt(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited,
                                                int terminalStmt) {
    if (!isEndOfProcedure(s)) {
        traverseOnlyPath(s, lastModifiedMap, visited, terminalStmt);
    }
}

bool AffectStorage::isAllowedToExitWhileLoop(LastModifiedMap &lastModifiedMap, LastModifiedMap &beforeLmm) {
    if (lastModifiedMap.size() > beforeLmm.size()) {
        return false;
    }

    for (auto &modSet : lastModifiedMap) {
        auto beforeLmmItr = beforeLmm.find(modSet.first);
        if (beforeLmmItr == beforeLmm.end()) {
            return false;
        }
        if (modSet.second.size() > beforeLmmItr->second.size()) {
            return false;
        }
        for (auto &elm : modSet.second) {
            if (beforeLmmItr->second.find(elm) == beforeLmmItr->second.end()) {
                return false;
            }
        }
    }
    return true;
}

void AffectStorage::populateAffectsInfo() {
    // Skip if affectsMap is already populated.
    if (isAllAffectsRelationPopulated) {
        return;
    }

    int totalStmtNum = procedureRangeList.rbegin()->second;
    std::unordered_set<int> visited;
    for (int stmtNum = 1; stmtNum <= totalStmtNum; stmtNum++) {
        LastModifiedMap lastModifiedMap;
        if (visited.find(stmtNum) == visited.end()) {
            traverseCFG(stmtNum, lastModifiedMap, visited, -1);
        }
    }

    isAllAffectsRelationPopulated = true;
}

void AffectStorage::setAffects(int s1, int s2) {
    if (affectsMap.find(s1) != affectsMap.end()) {
        affectsMap.at(s1).emplace(s2);
    } else {
        std::unordered_set<int> stmts;
        stmts.emplace(s2);
        affectsMap.emplace(s1, stmts);
    }
}

void AffectStorage::setAffectedBy(int s1, int s2) {
    if (affectedByMap.find(s1) != affectedByMap.end()) {
        affectedByMap.at(s1).emplace(s2);
    } else {
        std::unordered_set<int> stmts;
        stmts.emplace(s2);
        affectedByMap.emplace(s1, stmts);
    }
}

// for testing the populates
bool AffectStorage::isAffectsFromPopulation(int s1, int s2) {
    if (affectsMap.find(s1) != affectsMap.end()) {
        return affectsMap.at(s1).find(s2) != affectsMap.at(s1).end();
    }
    return false;
}

IntPairResultSet AffectStorage::getAffectsRelationPairing() {
    populateAffectsInfo();

    IntPairResultSet result;
    auto affectsMapItr = affectsMap.begin();
    while (affectsMapItr != affectsMap.end()) {
        for (auto &affectedStmt : affectsMapItr->second) {
            result.emplace(affectsMapItr->first, affectedStmt);
        }
        affectsMapItr++;
    }
    return result;
}

IntPairResultSet AffectStorage::getAffectsRelationPairingT() {
    populateAffectsStarInfo();

    IntPairResultSet result;
    auto affectsStarMapItr = affectsStarMap.begin();
    while (affectsStarMapItr != affectsStarMap.end()) {
        for (auto &affectedStmt : affectsStarMapItr->second) {
            result.emplace(affectsStarMapItr->first, affectedStmt);
        }
        affectsStarMapItr++;
    }
    return result;
}

LastModifiedMap AffectStorage::merge(LastModifiedMap &map1, LastModifiedMap &map2) {
    LastModifiedMap *mergedMap = new LastModifiedMap;
    mergedMap->insert(map1.begin(), map1.end());
    for (auto &itr : map2) {
        for (auto &stmt : itr.second) {
            addToLastModifiedMap(mergedMap, itr.first, stmt);
        }
    }
    return *mergedMap;
}

void AffectStorage::addToLastModifiedMap(LastModifiedMap *lastModifiedMap, std::string var, int s) {
    if (lastModifiedMap->find(var) != lastModifiedMap->end()) {
        lastModifiedMap->at(var).insert(s);
    } else {
        std::set<int> stmts;
        stmts.insert(s);
        lastModifiedMap->emplace(var, stmts);
    }
}

void AffectStorage::addLastModified(LastModifiedMap &lastModifiedMap, std::string var, int s) {
    if (lastModifiedMap.find(var) != lastModifiedMap.end()) {
        lastModifiedMap.at(var).clear();
        lastModifiedMap.at(var).emplace(s);
    } else {
        std::set<int> stmts{s};
        lastModifiedMap.emplace(var, stmts);
    }
}

bool AffectStorage::containsAffectRelation() {
    if (!affectsMap.empty()) {
        return true;
    }

    int endStmt = (procedureRangeList.rbegin())->second;
    for (int stmt = 1; stmt <= endStmt; stmt++) {
        bool isAffects = isStmtAffectingAnother(stmt);
        if (isAffects) {
            return true;
        }
    }
    return false;
}

std::unordered_set<int> AffectStorage::getStmtsAffectedBy() {
    if (!isAllAffectsRelationPopulated) {
        populateAffectsInfo();
    }

    std::unordered_set<int> stmtsAffectedBy;

    for (auto &affectedByMapItr : affectedByMap) {
        stmtsAffectedBy.emplace(affectedByMapItr.first);
    }

    return stmtsAffectedBy;
}

std::unordered_set<int> AffectStorage::getStmtsAffectedBy(int s) {
    if (isAllAffectsRelationPopulated) {
        return getStmtsAffectedByIfAffectsMapPopulated(s);
    }
    std::string stmtType = genStore->getStmtTypeByStmtNum(s);
    if (stmtType != ASSIGN) {
        return std::unordered_set<int>{};
    }

    std::unordered_set<int> visited;
    std::unordered_set<int> affectedList;
    DFSConstructAffectedList(s, s, visited, true, affectedList);
    return affectedList;
}

std::unordered_set<int> AffectStorage::getStmtsAffectedByT(int s) {
    std::string stmtType = genStore->getStmtTypeByStmtNum(s);
    if (stmtType != ASSIGN) {
        return std::unordered_set<int>{};
    }

    // If all Affects* relation has been populated or if this clause was evaluated before.
    if (isAllAffectsStarRelationPopulated || affectsStarMap.find(s) != affectsStarMap.end()) {
        return affectsStarMap.at(s);
    }

    populateAffectsInfo();
    affectsStarMap.emplace(s, getAllReachableNodes(s, affectsMap));

    return affectsStarMap.at(s);
}

void AffectStorage::DFSConstructAffectingList(int end, int curr, std::unordered_set<int> &visited, bool isStart,
                                              std::unordered_set<int> &affectingList,
                                              std::unordered_set<std::string> &modifiedList) {
    std::unordered_set<std::string> usedList = useStore->getUsesVariablesByStmt(end);
    std::unordered_set<std::string> currModifiedList = modStore->getModifiesVariableByStmt(curr);
    std::unordered_set<std::string> currModifiedVars;
    std::string stmtType = genStore->getStmtTypeByStmtNum(curr);

    if (!isStart) {
        // if the curr stmt modifies at least a var
        if (stmtType == ASSIGN || stmtType == CALL || stmtType == READ) {

            for (std::string varModified : currModifiedList) {
                // check if end uses var modified and if what current is modifying has already been modified before
                if (usedList.find(varModified) == usedList.end() || modifiedList.find(varModified) != modifiedList.end()) {
                    continue;
                }

                // mark var as modified for read, call, assign stmts
                modifiedList.insert(varModified);
                currModifiedVars.insert(varModified);

                if (stmtType == ASSIGN) {
                    setAffects(curr, end);
                    setAffectedBy(end, curr);
                    affectingList.emplace(curr);
                }
            }
        }
    }

    // if all the var used by end is already found to be modified for this dfs branch then can return
    // reset the modified list to before the path was taken
    if (modifiedList == usedList) {
        for (std::string var : currModifiedVars) {
            modifiedList.erase(var);
        }
        return;
    }

    if (!isStart) {
        visited.insert(curr);
    }

    // end of the path and reset the modified list to before path was taken
    if (reversedCfgPtr->find(curr) == reversedCfgPtr->end()) {
        for (std::string var : currModifiedVars) {
            modifiedList.erase(var);
        }
        return;
    }

    for (auto &stmt : reversedCfgPtr->at(curr)) {
        if (visited.find(stmt) == visited.end()) {
            DFSConstructAffectingList(end, stmt, visited, false, affectingList, modifiedList);
        }
    }

    // reset the modified list and visited to before that path was taken
    for (std::string var : currModifiedVars) {
        modifiedList.erase(var);
    }
    visited.erase(curr);
    return;
}

std::unordered_set<int> AffectStorage::getStmtsThatAffects() {
    if (!isAllAffectsRelationPopulated) {
        populateAffectsInfo();
    }

    std::unordered_set<int> stmtsThatAffects;

    for (auto &affectsMapItr : affectsMap) {
        stmtsThatAffects.emplace(affectsMapItr.first);
    }

    return stmtsThatAffects;
}

std::unordered_set<int> AffectStorage::getStmtsThatAffects(int s) {
    if (isAllAffectsRelationPopulated) {
        return getStmtsThatAffectsIfAffectedByMapPopulated(s);
    }

    std::string stmtType = genStore->getStmtTypeByStmtNum(s);
    if (stmtType != ASSIGN) {
        return std::unordered_set<int>{};
    }

    std::unordered_set<int> visited;
    std::unordered_set<int> affectingList;
    std::unordered_set<std::string> modifiedList;
    DFSConstructAffectingList(s, s, visited, true, affectingList, modifiedList);
    return affectingList;
}

std::unordered_set<int> AffectStorage::getStmtsThatAffectsT(int s) {
    std::string stmtType = genStore->getStmtTypeByStmtNum(s);
    if (stmtType != ASSIGN) {
        return std::unordered_set<int>{};
    }

    // If all Affects* relation has been populated or if this clause was evaluated before.
    if (isAllAffectsStarRelationPopulated || affectedByStarMap.find(s) != affectedByStarMap.end()) {
        return affectedByStarMap.at(s);
    }

    populateAffectsInfo();
    affectedByStarMap.emplace(s, getAllReachableNodes(s, affectedByMap));

    return affectedByStarMap.at(s);
}

void AffectStorage::DFSConstructAffectedList(int start, int curr, std::unordered_set<int> &visited, bool isStart,
                                             std::unordered_set<int> &affectedList) {
    if (!isStart) {
        // start to curr has affect relation (both are assign) -> return true
        if (isAffectAllowed(start, curr, false)) {
            setAffects(start, curr);
            setAffectedBy(curr, start);
            affectedList.insert(curr);
        }
        // if curr is a read or call stmt, breaks the valid path -> return false
        // both are assign but curr is not affected or not read or call stmt so can continue traversal without returning anything
        if (isLastModifiedChanged(curr, start)) {
            return;
        }
    }

    if (!isStart) {
        visited.insert(curr);
    }

    // end of path
    if (cfgPtr->find(curr) == cfgPtr->end()) {
        return;
    }

    for (int stmt : cfgPtr->at(curr)) {
        if (visited.find(stmt) == visited.end()) {
            DFSConstructAffectedList(start, stmt, visited, false, affectedList);
        }
    }
    return;
}

bool AffectStorage::isStmtAffectingAnother(int s) {
    std::string stmtType = genStore->getStmtTypeByStmtNum(s);
    if (stmtType != ASSIGN) {
        return false;
    }
    std::unordered_set<int> visited;
    return isAnyValidPathUsingDFS(s, s, visited, true);
}

bool AffectStorage::isAnyValidPathUsingDFS(int start, int curr, std::unordered_set<int> &visited, bool isStart) {
    if (!isStart) {
        // start to curr has affect relation (both are assign) -> return true
        if (isAffectAllowed(start, curr, false)) {
            setAffects(start, curr);
            setAffectedBy(curr, start);
            return true;
        }

        // if curr is a read or call stmt, breaks the valid path -> return false
        // both are assign but curr is not affected or not read or call stmt so can continue traversal without returning anything
        if (isLastModifiedChanged(curr, start)) {
            return false;
        }
    }

    // can visit the start again in cfgPtr later when it just started traversal
    if (!isStart) {
        visited.insert(curr);
    }

    // end of the path -> return false
    if (cfgPtr->find(curr) == cfgPtr->end()) {
        return false;
    }

    for (auto &stmt : cfgPtr->at(curr)) {
        if (visited.find(stmt) == visited.end()) {
            bool isAnyValidPathFound = isAnyValidPathUsingDFS(start, stmt, visited, false);
            if (isAnyValidPathFound) {
                return true;
            }
        }
    }

    // reach the end of the cfgPtr without any valid paths, remove all the inserted values in visited for next start stmt
    visited.erase(curr);
    return false;
}

bool AffectStorage::isAffects(int s1, int s2) {
    // check if has been populated previously
    if (checkAffectsMap(s1, s2)) {
        return true;
    } else {
        if (isAllAffectsRelationPopulated) {
            return false;
        }
    }

    if (!isAffectAllowed(s1, s2, false)) {
        return false;
    }
    std::unordered_set<int> visited;
    return isValidPathUsingDFS(s1, s2, s1, visited, true);
}

bool AffectStorage::isAffectsStar(int s1, int s2) {
    if (!isAffectAllowed(s1, s2, true)) {
        return false;
    }

    // Check if Affect* relation has already been computed.
    if (isAllAffectsStarRelationPopulated) {
        auto affectedStmtsItr = affectsStarMap.find(s1);
        if (affectedStmtsItr != affectsStarMap.end()) {
            return affectedStmtsItr->second.find(s2) != affectedStmtsItr->second.end();
        }
    }

    // Check if info has already been evaluated and cache
    auto affectedStmtsItr = affectsStarMap.find(s1);
    if (affectedStmtsItr != affectsStarMap.end()) {
        if (affectedStmtsItr->second.find(s2) != affectedStmtsItr->second.end()) {
            return true;
        }
    }

    auto affectedByStarMapItr = affectedByStarMap.find(s2);
    if (affectedByStarMapItr != affectedByStarMap.end()) {
        if (affectedByStarMapItr->second.find(s1) != affectedByStarMapItr->second.end()) {
            return true;
        }
    }

    populateAffectsInfo();

    // Perform BFS from s1 to s2
    std::unordered_set<int> visited;
    std::list<int> queue;
    queue.push_back(s1);
    while (!queue.empty()) {
        int currNode = queue.front();
        queue.pop_front();
        visited.emplace(currNode);

        auto affectsMapItr = affectsMap.find(currNode);
        // Current node has no neighbouring nodes.
        if (affectsMapItr == affectsMap.end()) {
            continue;
        }

        auto currNeighbour = affectsMapItr->second.begin();
        auto end = affectsMapItr->second.end();
        for (; currNeighbour != end; currNeighbour++) {
            // TODO: Add separate caching if needed.
            if (*currNeighbour == s2) {
                return true;
            }

            if (visited.find(*currNeighbour) == visited.end()) {
                queue.push_back(*currNeighbour);
            }
        }
    }

    return false;
}

bool AffectStorage::isValidPathUsingDFS(int start, int end, int curr, std::unordered_set<int> &visited, bool isStart) {
    if (!isStart) {
        if (curr == end) {
            setAffects(start, end);
            setAffectedBy(end, start);
            return true;
        }
        if (isLastModifiedChanged(curr, start)) {
            return false;
        }
    }

    // can visit the start again in cfgPtr later when it just started traversal
    if (!isStart) {
        visited.insert(curr);
    }

    // end of the path -> return false
    if (cfgPtr->find(curr) == cfgPtr->end()) {
        return false;
    }

    for (auto &stmt : cfgPtr->at(curr)) {
        if (visited.find(stmt) == visited.end()) {
            bool isValidPathFound = isValidPathUsingDFS(start, end, stmt, visited, false);
            if (isValidPathFound) {
                return true;
            }
        }
    }
    return false;
}

bool AffectStorage::isLastModifiedChanged(int s, int start) {
    std::string stmtType = genStore->getStmtTypeByStmtNum(s);
    if (stmtType == ASSIGN || stmtType == READ || stmtType == CALL) {
        std::unordered_set<std::string> modifiedVar = modStore->getModifiesVariableByStmt(start);
        std::unordered_set<std::string> modifiedInCurrent = modStore->getModifiesVariableByStmt(s);
        return contains(modifiedVar, modifiedInCurrent);
    }
    return false;
}

bool AffectStorage::isAffectAllowed(int s1, int s2, bool isTransitiveCheck) {
    if (genStore->getStmtTypeByStmtNum(s1) != ASSIGN || genStore->getStmtTypeByStmtNum(s2) != ASSIGN) {
        return false;
    }

    // Check if s1 and s2 are in the same procedure.
    for (auto &range : procedureRangeList) {
        bool isS1InProc = range.first <= s1 < range.second;
        bool isS2InProc = range.first <= s2 < range.second;
        if (isS1InProc != isS2InProc) {
            return false;
        }
    }

    if (!isTransitiveCheck) {
        // Check if s1 modifies something that s2 uses
        std::unordered_set<std::string> modifiedInStmt = modStore->getModifiesVariableByStmt(s1);
        std::unordered_set<std::string> usedInStmt = useStore->getUsesVariablesByStmt(s2);
        return contains(modifiedInStmt, usedInStmt);
    }

    return true;
}

// check if modifiedInStmt contains anything in usedInStmt
bool AffectStorage::contains(std::unordered_set<std::string> &modifiedInStmt, std::unordered_set<std::string> &usedInStmt) {
    for (auto &modifiedVar : modifiedInStmt) {
        if (usedInStmt.find(modifiedVar) != usedInStmt.end()) {
            return true;
        }
    }
    return false;
}

bool AffectStorage::checkAffectsMap(int s1, int s2) {
    if (!affectsMap.empty()) {
        if (affectsMap.find(s1) != affectsMap.end()) {
            return affectsMap.at(s1).find(s2) != affectsMap.at(s1).end();
        }
        return false;
    }
    return false;
}

std::unordered_set<int> AffectStorage::getStmtsThatAffectsIfAffectedByMapPopulated(int s) {
    if (affectedByMap.find(s) != affectedByMap.end()) {
        return affectedByMap.at(s);
    } else {
        return std::unordered_set<int>{};
    }
}

std::unordered_set<int> AffectStorage::getStmtsAffectedByIfAffectsMapPopulated(int s) {
    if (affectsMap.find(s) != affectsMap.end()) {
        return affectsMap.at(s);
    } else {
        return std::unordered_set<int>{};
    }
}

void AffectStorage::populateAffectsStarInfo() {
    if (isAllAffectsStarRelationPopulated) {
        return;
    }

    populateAffectsInfo();

    std::unordered_map<int, std::unordered_set<int>>::iterator affectsMapItr = affectsMap.begin();

    // Perform BFS for each assignment statement
    while (affectsMapItr != affectsMap.end()) {
        int currAssignmentStmt = affectsMapItr->first;

        for (auto reachableNode : getAllReachableNodes(currAssignmentStmt, affectsMap)) {
            addAffectsStarRelation(currAssignmentStmt, reachableNode);
        }

        affectsMapItr++;
    }

    isAllAffectsStarRelationPopulated = true;
}

void AffectStorage::addAffectsStarRelation(int s1, int s2) {
    auto affectsStarMapItr = affectsStarMap.find(s1);
    if (affectsStarMapItr != affectsStarMap.end()) {
        affectsStarMapItr->second.emplace(s2);
    } else {
        affectsStarMap.emplace(s1, std::unordered_set<int>{s2});
    }

    auto affectedByStarMapItr = affectedByStarMap.find(s2);
    if (affectedByStarMapItr != affectedByStarMap.end()) {
        affectedByStarMapItr->second.emplace(s1);
    } else {
        affectedByStarMap.emplace(s2, std::unordered_set<int>{s1});
    }
}

std::unordered_set<int> AffectStorage::getAllReachableNodes(int startNode,
                                                            std::unordered_map<int, std::unordered_set<int>> &traversalMap) {

    std::unordered_set<int> reachableNodes;

    std::unordered_set<int> visited;
    std::list<int> queue;

    queue.push_back(startNode);

    while (!queue.empty()) {
        int currNode = queue.front();
        queue.pop_front();
        visited.emplace(currNode);

        auto traversalMapItr = traversalMap.find(currNode);
        // Current node has no neighbouring nodes.
        if (traversalMapItr == traversalMap.end()) {
            continue;
        }

        auto currNeighbour = traversalMapItr->second.begin();
        auto end = traversalMapItr->second.end();
        for (; currNeighbour != end; currNeighbour++) {
            reachableNodes.emplace(*currNeighbour);

            if (visited.find(*currNeighbour) == visited.end()) {
                queue.push_back(*currNeighbour);
            }
        }
    }

    return reachableNodes;
}

void AffectStorage::clear() {
    affectsMap.clear();
    affectedByMap.clear();
    affectsStarMap.clear();
    affectedByStarMap.clear();

    procedureRangeList.clear();

    cfgPtr = nullptr;
    reversedCfgPtr = nullptr;

    isAllAffectsRelationPopulated = false;
    isAllAffectsStarRelationPopulated = false;
}

void AffectStorage::resetCache() {
    affectsMap.clear();
    affectedByMap.clear();
    affectsStarMap.clear();
    affectedByStarMap.clear();

    isAllAffectsRelationPopulated = false;
    isAllAffectsStarRelationPopulated = false;
}
