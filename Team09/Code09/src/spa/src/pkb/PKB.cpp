#include "PKB.h"

FollowStorage *PKB::fStore = FollowStorage::getInstance();
UseStorage *PKB::uStore = UseStorage::getInstance();
ModifyStorage *PKB::mStore = ModifyStorage::getInstance();
ParentStorage *PKB::pStore = ParentStorage::getInstance();
PatternStorage *PKB::ptStore = PatternStorage::getInstance();
GeneralStorage *PKB::gStore = GeneralStorage::getInstance();
NextStorage *PKB::nStore = NextStorage::getInstance();
CallStorage *PKB::cStore = CallStorage::getInstance();
AffectStorage *PKB::aStore = AffectStorage::getInstance();

void PKB::addFollowsRelation(int leadingStmt, int followingStmt) {
    fStore->setFollows(leadingStmt, followingStmt);
}

void PKB::addFollowsStarRelation(int leadingStmt, int followingStmt) {
    fStore->setFollowsStar(leadingStmt, followingStmt);
}

void PKB::addParentRelation(int parentStmt, int childStmt) {
    pStore->setParent(parentStmt, childStmt);
}

void PKB::addParentStarRelation(int parentStmt, int childStmt) {
    pStore->setParentStar(parentStmt, childStmt);
}

void PKB::addStmtUsesRelation(int stmt, std::string designEnt, std::unordered_set<std::string> vars) {
    if (vars.empty()) {
        return;
    }

    for (const auto &varName : vars) {
        uStore->setVarToStmt(stmt, designEnt, varName);
        uStore->setStmtToVar(stmt, varName);
    }
    uStore->setStmtTypeToStmt(designEnt, stmt);
}

void PKB::addProcUsesRelation(std::string proc, std::unordered_set<std::string> vars) {
    for (const auto &varName : vars) {
        uStore->setProcToVar(proc, varName);
        uStore->setVarToProc(proc, varName);
    }
}

void PKB::addStmtModifiesRelation(int stmt, std::string designEnt, std::unordered_set<std::string> vars) {
    if (vars.empty()) {
        return;
    }

    for (const auto &varName : vars) {
        mStore->setStmtToVar(stmt, varName);
        mStore->setVarToStmt(stmt, designEnt, varName);
    }
    mStore->setStmtTypeToStmt(designEnt, stmt);
}

void PKB::addProcModifiesRelation(std::string proc, std::unordered_set<std::string> vars) {
    for (const auto &varName : vars) {
        mStore->setProcToVar(proc, varName);
        mStore->setVarToProc(proc, varName);
    }
}

void PKB::storeStatement(int stmtLine, std::string stmtType, std::unordered_set<std::string> setOfVars,
                         std::unordered_set<int> setOfConstants) {

    for (const auto &var : setOfVars) {
        gStore->setVarToStmtNum(var, stmtLine);
        gStore->setVar(var);
    }

    for (const auto &constantVal : setOfConstants) {
        gStore->setConstant(constantVal);
    }

    gStore->storeStmtObj(Statement{stmtLine, stmtType, setOfVars, setOfConstants});

    nStore->setStmtToType(stmtLine, stmtType);
    if (stmtType == WHILE_STMT) {
        std::unordered_set<int> allStmtsNestedInWhile = pStore->getAllNestedStatementsT(stmtLine);
        for (auto &nestedStmts : allStmtsNestedInWhile) {
            nStore->setStmtToWhileParent(nestedStmts, stmtLine);
        }
    }
}

void PKB::storeAssignPattern(int stmt, std::string lhs, std::string pattern) {
    ptStore->setStmtToPostfix(stmt, lhs, pattern);
}

void PKB::addConditionPattern(int stmtNum, std::unordered_set<std::string> varsUsed, std::string designEnt) {
    ptStore->setConditionPattern(stmtNum, varsUsed, designEnt);
}

void PKB::addNextAdjacencyList(std::unordered_map<int, std::set<int>> adjList) {
    nStore->setAdjacencyListMap(adjList);
    nStore->setReverseAdjacencyListMap(adjList);
    nStore->setupCache();
    aStore->setupAffectStorage();
}

void PKB::addProcInfo(std::string procname, int startLine, int endLine) {
    gStore->setProcedure(procname, startLine, endLine);
    nStore->setProcedureRange(startLine, endLine);
    aStore->setProcedureRange(startLine, endLine);
}

void PKB::addCallsRelation(std::string caller, std::string callee, int stmtLine) {
    cStore->setCalls(caller, callee, stmtLine);
}

void PKB::startPopulateCallsStarInfo() {
    cStore->populateCallsStarMap();

    // Perform a topo sort on all procedures.
    std::unordered_set<std::string> allProcedures = gStore->getAllProcedures();
    std::unordered_map<std::string, int> inDegree;
    for (auto &procedure : allProcedures) {
        int calledBySize = cStore->getAllCalledBy(procedure).size();
        inDegree.emplace(procedure, calledBySize > 0 ? calledBySize : 0);
    }

    std::queue<std::string> q;
    std::unordered_map<std::string, int>::iterator inDegreeItr = inDegree.begin();
    while (inDegreeItr != inDegree.end()) {
        if (inDegreeItr->second == 0) {
            q.push(inDegreeItr->first);
        }
        inDegreeItr++;
    }

    std::list<std::string> topoOrder;
    while (!q.empty()) {
        std::string currProc = q.front();
        q.pop();
        topoOrder.emplace_back(currProc);

        std::unordered_set<std::string> neighbours = cStore->getAllCalls(currProc);
        for (auto neighbour : neighbours) {
            if (--inDegree.at(neighbour) == 0) {
                q.push(neighbour);
            }
        }
    }

    // After completing the topo sort, using (topo) sorted list of procedures, in reverse order, i.e. begin from
    // procedure(s) that does not call any other procedures, and start adding Uses and Modifies variables to all other procedures
    // that calls this procedure.
    std::list<std::string>::reverse_iterator topoOrderReverseItr = topoOrder.rbegin();
    while (topoOrderReverseItr != topoOrder.rend()) {
        std::unordered_set<std::string> modifiesVars = mStore->getModifiesVariableByProc(*topoOrderReverseItr);
        std::unordered_set<std::string> usesVars = uStore->getUsesVariableByProc(*topoOrderReverseItr);
        for (auto caller : cStore->getAllCalledBy(*topoOrderReverseItr)) {
            this->addProcModifiesRelation(caller, modifiesVars);
            this->addProcUsesRelation(caller, usesVars);
        }

        topoOrderReverseItr++;
    }

    // Given that all procedures are updated with the correct Uses and Modifies variables, loop through all call
    // statements to update their Uses and NModifies variables in the PKB storage, based on the procedure that the call stmts
    // calls.
    for (auto callStmt : cStore->getAllCallsStmt()) {
        std::string proc = cStore->getProcedureCalledAtStmt(callStmt);
        std::unordered_set<std::string> modifiesVarList = mStore->getModifiesVariableByProc(proc);
        std::unordered_set<std::string> usesVarList = uStore->getUsesVariableByProc(proc);

        this->addStmtModifiesRelation(callStmt, CALL, modifiesVarList);
        this->addStmtUsesRelation(callStmt, CALL, usesVarList);

        // Need to update for all "while" and "if" statements the call statements are nested in as well.
        std::unordered_set<int> allParents = pStore->getParentStatementsT(callStmt);
        for (auto &parent : allParents) {
            std::string stmtType = gStore->getStmtTypeByStmtNum(parent);
            this->addStmtModifiesRelation(parent, stmtType, modifiesVarList);
            this->addStmtUsesRelation(parent, stmtType, usesVarList);
        }
    }
}

bool PKB::isFollows(int s1, int s2, bool isRecursive) {
    return isRecursive ? fStore->isFollowsStar(s1, s2) : fStore->isFollows(s1, s2);
}

bool PKB::isFollows(std::string entType, int s1, bool isRecursive) {
    std::unordered_set<int> leadingStmtsSet = isRecursive ? fStore->getAllLeadingStar(s1) : fStore->getAllLeading(s1);

    if (entType == STMT) {
        return !leadingStmtsSet.empty();
    }

    for (const auto &leadingStmt : leadingStmtsSet) {
        if (gStore->getStmtTypeByStmtNum(leadingStmt) == entType) {
            return true;
        }
    }

    return false;
}

bool PKB::isFollows(int s1, std::string entType, bool isRecursive) {
    std::unordered_set<int> followingStmtsSet = isRecursive ? fStore->getAllFollowsStar(s1) : fStore->getAllFollows(s1);

    if (entType == STMT) {
        return !followingStmtsSet.empty();
    }

    for (const auto &leadingStmt : followingStmtsSet) {
        if (gStore->getStmtTypeByStmtNum(leadingStmt) == entType) {
            return true;
        }
    }

    return false;
}

bool PKB::isFollows(std::string leadingStmtEntType, std::string followingStmtEntType, bool isRecursive) {
    std::unordered_set<int> leadingStmtSets = gStore->getStmtNumByStmtType(leadingStmtEntType);

    for (const auto &leadingStmt : leadingStmtSets) {
        std::unordered_set<int> followingStmtsSet =
            isRecursive ? fStore->getAllFollowsStar(leadingStmt) : fStore->getAllFollows(leadingStmt);

        if ((followingStmtEntType == STMT) && (!followingStmtsSet.empty())) {
            return true;
        }

        for (const auto followingStmt : followingStmtsSet) {
            if (gStore->getStmtTypeByStmtNum(followingStmt) == followingStmtEntType) {
                return true;
            }
        }
    }

    return false;
}

std::unordered_set<int> PKB::getFollowingStatements(int s1, std::string resultFilter) {
    if (resultFilter == STMT) {
        return fStore->getAllFollows(s1);
    } else {
        std::unordered_set<int> followingStmts = fStore->getAllFollows(s1);

        std::unordered_set<int> filteredStmtSet;
        filterSetByEntType(followingStmts, filteredStmtSet, resultFilter);
        return filteredStmtSet;
    }
}

std::unordered_set<int> PKB::getFollowingStatementsT(int s1, std::string resultFilter) {
    if (resultFilter == STMT) {
        return fStore->getAllFollowsStar(s1);
    } else {
        std::unordered_set<int> followingStmts = fStore->getAllFollowsStar(s1);

        std::unordered_set<int> filteredStmtSet;
        filterSetByEntType(followingStmts, filteredStmtSet, resultFilter);
        return filteredStmtSet;
    }
}

std::unordered_set<int> PKB::getFollowingStatements(std::string entType, std::string resultFilter) {
    std::unordered_set<int> anchorStmtSet = gStore->getStmtNumByStmtType(entType);
    std::unordered_set<int> allFollowingStmts;
    for (const auto &anchorStmt : anchorStmtSet) {
        std::unordered_set<int> followsStmtSet = fStore->getAllFollows(anchorStmt);
        if (resultFilter == STMT) {
            allFollowingStmts.insert(followsStmtSet.begin(), followsStmtSet.end());
        } else {
            filterSetByEntType(followsStmtSet, allFollowingStmts, resultFilter);
        }
    }

    return allFollowingStmts;
}

std::unordered_set<int> PKB::getFollowingStatementsT(std::string entType, std::string resultFilter) {
    std::unordered_set<int> anchorStmtSet = gStore->getStmtNumByStmtType(entType);

    std::unordered_set<int> allFollowingStmts;
    for (const auto &anchorStmt : anchorStmtSet) {
        std::unordered_set<int> followsStmtSet = fStore->getAllFollowsStar(anchorStmt);
        if (resultFilter == STMT) {
            allFollowingStmts.insert(followsStmtSet.begin(), followsStmtSet.end());
        } else {
            filterSetByEntType(followsStmtSet, allFollowingStmts, resultFilter);
        }
    }

    return allFollowingStmts;
}

std::unordered_set<int> PKB::getLeadingStatements(int s1, std::string resultFilter) {
    std::unordered_set<int> leadingStmtsSet = fStore->getAllLeading(s1);

    if (resultFilter == STMT) {
        return leadingStmtsSet;
    } else {
        std::unordered_set<int> filterStmtSet;
        filterSetByEntType(leadingStmtsSet, filterStmtSet, resultFilter);
        return filterStmtSet;
    }
}

std::unordered_set<int> PKB::getLeadingStatementsT(int s1, std::string resultFilter) {
    std::unordered_set<int> leadingStmtsSet = fStore->getAllLeadingStar(s1);

    if (resultFilter == STMT) {
        return leadingStmtsSet;
    } else {
        std::unordered_set<int> filterStmtSet;
        filterSetByEntType(leadingStmtsSet, filterStmtSet, resultFilter);
        return filterStmtSet;
    }
}

std::unordered_set<int> PKB::getLeadingStatements(std::string entType, std::string resultFilter) {
    std::unordered_set<int> followingStmtsSet = gStore->getStmtNumByStmtType(entType);

    std::unordered_set<int> allLeadingStmts;
    for (const auto &followingStmt : followingStmtsSet) {
        std::unordered_set<int> leadingStmtsSet = fStore->getAllLeading(followingStmt);
        if (resultFilter == STMT) {
            allLeadingStmts.insert(leadingStmtsSet.begin(), leadingStmtsSet.end());
        } else {
            filterSetByEntType(leadingStmtsSet, allLeadingStmts, resultFilter);
        }
    }

    return allLeadingStmts;
}

std::unordered_set<int> PKB::getLeadingStatementsT(std::string entType, std::string resultFilter) {
    std::unordered_set<int> followingStmtsSet = gStore->getStmtNumByStmtType(entType);

    std::unordered_set<int> allLeadingStmts;
    for (const auto &followingStmt : followingStmtsSet) {
        std::unordered_set<int> leadingStmtsSet = fStore->getAllLeadingStar(followingStmt);
        if (resultFilter == STMT) {
            allLeadingStmts.insert(leadingStmtsSet.begin(), leadingStmtsSet.end());
        } else {
            filterSetByEntType(leadingStmtsSet, allLeadingStmts, resultFilter);
        }
    }

    return allLeadingStmts;
}

IntPairResultSet PKB::getAllFollowsRelationPairings(std::string entType1, std::string entType2) {
    IntPairResultSet filteredResult;
    std::unordered_map<int, int> *followsMapPtr = fStore->getAllFollowsRelationPair();

    auto followsMapItr = followsMapPtr->begin();
    while (followsMapItr != followsMapPtr->end()) {
        bool matchesEntType1 = (entType1 == STMT) || (gStore->getStmtTypeByStmtNum(followsMapItr->first) == entType1);
        bool matchesEntType2 = (entType2 == STMT) || (gStore->getStmtTypeByStmtNum(followsMapItr->second) == entType2);
        if (matchesEntType1 && matchesEntType2) {
            filteredResult.emplace(followsMapItr->first, followsMapItr->second);
        }

        followsMapItr++;
    }

    return filteredResult;
}

IntPairResultSet PKB::getAllFollowsRelationPairingsT(std::string entType1, std::string entType2) {
    IntPairResultSet filteredResult;
    std::unordered_map<int, std::unordered_set<int>> *followsMapPtr = fStore->getAllFollowsRelationPairT();

    auto followsMapItr = followsMapPtr->begin();
    while (followsMapItr != followsMapPtr->end()) {

        bool matchesEntType1 = (entType1 == STMT) || (gStore->getStmtTypeByStmtNum(followsMapItr->first) == entType1);

        if (!matchesEntType1) {
            followsMapItr++;
            continue;
        }

        for (auto followingStmt : followsMapItr->second) {
            bool matchesEntType2 = (entType2 == STMT) || (gStore->getStmtTypeByStmtNum(followingStmt) == entType2);
            if (matchesEntType1 && matchesEntType2) {
                filteredResult.emplace(followsMapItr->first, followingStmt);
            }
        }

        followsMapItr++;
    }

    return filteredResult;
}

bool PKB::isParent(int s1, int s2, bool isRecursive) {
    return isRecursive ? pStore->isParentStar(s1, s2) : pStore->isParent(s1, s2);
}

bool PKB::isParent(int s1, std::string entType, bool isRecursive) {
    if (!isContainerStmt(gStore->getStmtTypeByStmtNum(s1))) {
        return false;
    }

    std::unordered_set<int> nestedStatements =
        isRecursive ? pStore->getAllNestedStatementsT(s1) : pStore->getAllNestedStatements(s1);

    if (entType == STMT) {
        return !nestedStatements.empty();
    } else {
        for (const auto &nestedStmt : nestedStatements) {
            if (gStore->getStmtTypeByStmtNum(nestedStmt) == entType) {
                return true;
            }
        }
    }

    return false;
}

bool PKB::isParent(std::string entType, int s1, bool isRecursive) {
    if (!isContainerStmt(entType)) {
        return false;
    }

    std::unordered_set<int> parentStmtSet = isRecursive ? pStore->getParentStatementsT(s1) : pStore->getParentStatement(s1);

    if (entType == STMT) {
        return !parentStmtSet.empty();
    } else {
        for (const auto &parentStmt : parentStmtSet) {
            if (gStore->getStmtTypeByStmtNum(parentStmt) == entType) {
                return true;
            }
        }
    }
    return false;
}

bool PKB::isParent(std::string parentEntType, std::string childEntType, bool isRecursive) {
    if (!isContainerStmt(parentEntType)) {
        return false;
    }

    std::unordered_set<int> allParentStmts =
        (parentEntType == STMT) ? pStore->getAllParentStmts() : gStore->getStmtNumByStmtType(parentEntType);

    for (const auto &parentStmt : allParentStmts) {
        std::unordered_set<int> nestedStatements =
            isRecursive ? pStore->getAllNestedStatementsT(parentStmt) : pStore->getAllNestedStatements(parentStmt);

        for (const auto childStmt : nestedStatements) {
            if (gStore->getStmtTypeByStmtNum(childStmt) == childEntType) {
                return true;
            }
        }
    }

    return false;
}

std::unordered_set<int> PKB::getParentStatements(int s1, std::string resultFilter) {
    if (!isContainerStmt(resultFilter)) {
        std::unordered_set<int> emptySet;
        return emptySet;
    }

    if (resultFilter == STMT) {
        return pStore->getParentStatement(s1);
    } else {
        std::unordered_set<int> parentStmts = pStore->getParentStatement(s1);

        std::unordered_set<int> filteredSet;
        filterSetByEntType(parentStmts, filteredSet, resultFilter);
        return filteredSet;
    }
}

std::unordered_set<int> PKB::getParentStatementsT(int s1, std::string resultFilter) {
    if (!isContainerStmt(resultFilter)) {
        std::unordered_set<int> emptySet;
        return emptySet;
    }

    if (resultFilter == STMT) {
        return pStore->getParentStatementsT(s1);
    } else {
        std::unordered_set<int> parentStmts = pStore->getParentStatementsT(s1);

        std::unordered_set<int> filteredSet;
        filterSetByEntType(parentStmts, filteredSet, resultFilter);
        return filteredSet;
    }
}

std::unordered_set<int> PKB::getParentStatements(std::string childEntType, std::string resultFilter) {
    if (!isContainerStmt(resultFilter)) {
        std::unordered_set<int> emptySet;
        return emptySet;
    }

    std::unordered_set<int> allParents;
    std::unordered_set<int> childStmtsRef = gStore->getStmtNumByStmtType(childEntType);
    for (const auto &childStmt : childStmtsRef) {
        std::unordered_set<int> parentStmts = pStore->getParentStatement(childStmt);

        if (resultFilter == STMT) {
            allParents.insert(parentStmts.begin(), parentStmts.end());
        } else {
            filterSetByEntType(parentStmts, allParents, resultFilter);
        }
    }

    return allParents;
}

std::unordered_set<int> PKB::getParentStatementsT(std::string childEntType, std::string resultFilter) {
    if (!isContainerStmt(resultFilter)) {
        std::unordered_set<int> emptySet;
        return emptySet;
    }

    std::unordered_set<int> allParents;
    std::unordered_set<int> childStmtsRef = gStore->getStmtNumByStmtType(childEntType);
    for (const auto &childStmt : childStmtsRef) {
        std::unordered_set<int> parentStmts = pStore->getParentStatementsT(childStmt);

        if (resultFilter == STMT) {
            allParents.insert(parentStmts.begin(), parentStmts.end());
        } else {
            filterSetByEntType(parentStmts, allParents, resultFilter);
        }
    }

    return allParents;
}

std::unordered_set<int> PKB::getChildrenStatements(int s1, std::string resultFilter) {
    if (resultFilter == STMT) {
        return pStore->getAllNestedStatements(s1);
    } else {
        std::unordered_set<int> nestStmtSetRef = pStore->getAllNestedStatements(s1);
        std::unordered_set<int> filteredStmtSet;
        filterSetByEntType(nestStmtSetRef, filteredStmtSet, resultFilter);

        return filteredStmtSet;
    }
}

std::unordered_set<int> PKB::getChildrenStatementsT(int s1, std::string resultFilter) {
    if (resultFilter == STMT) {
        return pStore->getAllNestedStatementsT(s1);
    } else {
        std::unordered_set<int> nestStmtSetRef = pStore->getAllNestedStatementsT(s1);
        std::unordered_set<int> filteredStmtSet;
        filterSetByEntType(nestStmtSetRef, filteredStmtSet, resultFilter);

        return filteredStmtSet;
    }
}

std::unordered_set<int> PKB::getChildrenStatements(std::string parentEntType, std::string resultFilter) {
    if (!isContainerStmt(parentEntType)) {
        std::unordered_set<int> emptySet;
        return emptySet;
    }

    std::unordered_set<int> allParentStmts =
        (parentEntType == STMT) ? pStore->getAllParentStmts() : gStore->getStmtNumByStmtType(parentEntType);

    std::unordered_set<int> allChildStmts;
    for (const auto &parentStmt : allParentStmts) {
        std::unordered_set<int> nestedStmts = pStore->getAllNestedStatements(parentStmt);
        if (resultFilter == STMT) {
            allChildStmts.insert(nestedStmts.begin(), nestedStmts.end());
        } else {
            filterSetByEntType(nestedStmts, allChildStmts, resultFilter);
        }
    }

    return allChildStmts;
}

std::unordered_set<int> PKB::getChildrenStatementsT(std::string parentEntType, std::string resultFilter) {
    if (!isContainerStmt(parentEntType)) {
        std::unordered_set<int> emptySet;
        return emptySet;
    }

    std::unordered_set<int> allParentStmts =
        (parentEntType == STMT) ? pStore->getAllParentStmts() : gStore->getStmtNumByStmtType(parentEntType);

    std::unordered_set<int> allChildStmts;
    for (const auto &parentStmt : allParentStmts) {
        std::unordered_set<int> nestedStmts = pStore->getAllNestedStatementsT(parentStmt);
        if (resultFilter == STMT) {
            allChildStmts.insert(nestedStmts.begin(), nestedStmts.end());
        } else {
            filterSetByEntType(nestedStmts, allChildStmts, resultFilter);
        }
    }

    return allChildStmts;
}

IntPairResultSet PKB::getAllParentRelationPairings(std::string entType1, std::string entType2) {
    IntPairResultSet filteredResult;

    if (!isContainerStmt(entType1)) {
        return filteredResult;
    }

    std::unordered_map<int, std::unordered_set<int>> *parentMapPtr = pStore->getAllParentRelationPair();

    auto parentMapItr = parentMapPtr->begin();
    while (parentMapItr != parentMapPtr->end()) {

        bool matchesEntType1 = (entType1 == STMT) || (gStore->getStmtTypeByStmtNum(parentMapItr->first) == entType1);

        if (!matchesEntType1) {
            parentMapItr++;
            continue;
        }

        for (auto nestedStmt : parentMapItr->second) {
            bool matchesEntType2 = (entType2 == STMT) || (gStore->getStmtTypeByStmtNum(nestedStmt) == entType2);
            if (matchesEntType1 && matchesEntType2) {
                filteredResult.emplace(parentMapItr->first, nestedStmt);
            }
        }

        parentMapItr++;
    }

    return filteredResult;
}

IntPairResultSet PKB::getAllParentRelationPairingsT(std::string entType1, std::string entType2) {
    IntPairResultSet filteredResult;

    if (!isContainerStmt(entType1)) {
        return filteredResult;
    }

    std::unordered_map<int, std::unordered_set<int>> *parentStarMapPtr = pStore->getAllParentStarRelationPair();

    auto parentStarMapItr = parentStarMapPtr->begin();
    while (parentStarMapItr != parentStarMapPtr->end()) {

        bool matchesEntType1 = (entType1 == STMT) || (gStore->getStmtTypeByStmtNum(parentStarMapItr->first) == entType1);

        if (!matchesEntType1) {
            parentStarMapItr++;
            continue;
        }

        for (auto nestedStmt : parentStarMapItr->second) {
            bool matchesEntType2 = (entType2 == STMT) || (gStore->getStmtTypeByStmtNum(nestedStmt) == entType2);
            if (matchesEntType1 && matchesEntType2) {
                filteredResult.emplace(parentStarMapItr->first, nestedStmt);
            }
        }

        parentStarMapItr++;
    }

    return filteredResult;
}

bool PKB::isModifies(int s1, std::string v) {
    return mStore->isModifies(s1, v);
}

bool PKB::isModifies(std::string proc, std::string v) {
    return mStore->isModifiesByProc(proc, v);
}

std::unordered_set<std::string> PKB::getModifiesVariables(int s1) {
    return mStore->getModifiesVariableByStmt(s1);
}

std::unordered_set<std::string> PKB::getModifiesVariables(std::string entType) {
    std::unordered_set<int> matchingStmts = gStore->getStmtNumByStmtType(entType);

    std::unordered_set<std::string> allModifiedVars;
    for (const auto &stmt : matchingStmts) {
        std::unordered_set<std::string> modifiedVars = mStore->getModifiesVariableByStmt(stmt);
        allModifiedVars.insert(modifiedVars.begin(), modifiedVars.end());
    }

    return allModifiedVars;
}

std::unordered_set<int> PKB::getModifiesStatement(std::string entType, std::string v) {
    return mStore->getModifiesStatement(entType, v);
}

std::unordered_set<int> PKB::getAllModifiesStatement(std::string entType) {
    return mStore->getAllModifiesStatements(entType);
}

std::unordered_set<std::string> PKB::getModifiesProc(std::string v) {
    return mStore->getModifiesProc(v);
}

std::unordered_set<std::string> PKB::getModifiesProc() {
    return mStore->getModifiesProc();
}

std::unordered_set<std::string> PKB::getModifiesVariableFromProc(std::string proc) {
    return mStore->getModifiesVariableByProc(proc);
}

StringPairResultSet PKB::getAllProcModifiesVarRelationPair() {
    return mStore->getAllProcModifiesVarRelationPair();
}

IntStringPairResultSet PKB::getAllStmtModifiesVarRelationPair(std::string stmtEnt) {
    return mStore->getAllStmtModifiesVarRelationPair(stmtEnt);
}

bool PKB::isUses(int s1, std::string v) {
    return uStore->isUses(s1, v);
}

bool PKB::isUses(std::string p, std::string v) {
    return uStore->isUsesByProc(p, v);
}

std::unordered_set<std::string> PKB::getUsesVariables(int s1) {
    return uStore->getUsesVariablesByStmt(s1);
}

std::unordered_set<std::string> PKB::getUsesVariables(std::string entType) {
    std::unordered_set<int> matchingStmts = gStore->getStmtNumByStmtType(entType);

    std::unordered_set<std::string> allUsedVars;
    for (const auto &stmt : matchingStmts) {
        std::unordered_set<std::string> varsUsed = uStore->getUsesVariablesByStmt(stmt);
        allUsedVars.insert(varsUsed.begin(), varsUsed.end());
    }

    return allUsedVars;
}

std::unordered_set<int> PKB::getAllUsesStatement(std::string entType) {
    return uStore->getAllUsesStatements(entType);
}

std::unordered_set<int> PKB::getUsesStatement(std::string entType, std::string v) {
    return uStore->getUsesStatement(entType, v);
}

std::unordered_set<std::string> PKB::getUsesProc(std::string v) {
    return uStore->getUsesProc(v);
}

std::unordered_set<std::string> PKB::getUsesProc() {
    return uStore->getUsesProc();
}

std::unordered_set<std::string> PKB::getUsesVariableFromProc(std::string proc) {
    return uStore->getUsesVariableByProc(proc);
}

StringPairResultSet PKB::getAllProcUsesVarRelationPair() {
    return uStore->getAllProcUsesVarRelationPair();
}

IntStringPairResultSet PKB::getAllStmtUsesVarRelationPair(std::string stmtEnt) {
    return uStore->getAllStmtUsesVarRelationPair(stmtEnt);
}

std::unordered_set<std::string> PKB::getAllVar() {
    return gStore->getAllVars();
}

std::unordered_set<std::string> PKB::getAllProc() {
    return gStore->getAllProcedures();
}

std::unordered_set<int> PKB::getAllStatements(std::string entType) {
    return gStore->getStmtNumByStmtType(entType);
}

std::unordered_set<std::string> PKB::getAllVarsInStatement(std::unordered_set<int> stmtSet) {
    std::unordered_set<std::string> allVars;
    for (const auto &stmtLine : stmtSet) {
        std::unordered_set<std::string> setRef = gStore->getVarByStmtNum(stmtLine);
        allVars.insert(setRef.begin(), setRef.end());
    }
    return allVars;
}

std::unordered_set<int> PKB::getStatmentNumberFromVars(std::unordered_set<std::string> varSet) {
    std::unordered_set<int> allStatements;
    for (const auto &var : varSet) {
        std::unordered_set<int> setRef = gStore->getStmtsByVar(var);
        allStatements.insert(setRef.begin(), setRef.end());
    }
    return allStatements;
}

std::unordered_set<int> PKB::getAllConstants() {
    return gStore->getAllConstants();
}

bool PKB::isValidStatementNum(int stmtNum, std::string designEnt) {
    return gStore->isValidStatementNum(stmtNum, designEnt);
}

std::unordered_set<int> PKB::getStatementsWithPattern(std::string rhsPattern, bool strictStart, bool strictEnd) {

    return ptStore->getPatternMatchStatement(rhsPattern, strictStart, strictEnd);
}

std::unordered_set<int> PKB::getStatementsWithPattern(std::string lhsVar, std::string rhsPattern, bool strictStart,
                                                      bool strictEnd) {

    return ptStore->getPatternAndVarMatchStatement(lhsVar, rhsPattern, strictStart, strictEnd);
}

std::unordered_set<int> PKB::getVarMatchConditionStatements(std::string var, std::string designEnt) {
    return ptStore->getVarMatchConditionStatements(var, designEnt);
}

std::unordered_set<int> PKB::getConditionStatementsWithAnyVar(std::string designEnt) {
    return ptStore->getConditionStatementsWithAnyVar(designEnt);
}

IntStringPairResultSet PKB::getVarMatchConditionStatementPairs(std::string designEnt) {
    return ptStore->getVarMatchConditionStatementPairs(designEnt);
}

bool PKB::hasNext(int s) {
    return nStore->hasNext(s);
}

bool PKB::hasNext(std::string designEnt) {
    return nStore->hasNext(designEnt);
}

bool PKB::hasPrevious(int s) {
    return nStore->hasPrevious(s);
}

bool PKB::hasPrevious(std::string designEnt) {
    return nStore->hasPrevious(designEnt);
}

bool PKB::isNext(int s1, int s2, bool isRecursive) {
    return nStore->isNext(s1, s2, isRecursive);
}

std::unordered_set<int> PKB::getNextStatements(int s, std::string resultFilter) {
    return nStore->getNextStatements(s, resultFilter);
}

std::unordered_set<int> PKB::getNextStatementsT(int s, std::string resultFilter) {
    return nStore->getNextStatementsT(s, resultFilter);
}

std::unordered_set<int> PKB::getPreviousStatements(int s, std::string resultFilter) {
    return nStore->getPreviousStatements(s, resultFilter);
}

std::unordered_set<int> PKB::getPreviousStatementsT(int s, std::string resultFilter) {
    return nStore->getPreviousStatementsT(s, resultFilter);
}

std::unordered_set<int> PKB::getNextStatements(std::string designEnt) {
    return nStore->getNextStatements(designEnt);
}

std::unordered_set<int> PKB::getPreviousStatements(std::string designEnt) {
    return nStore->getPreviousStatements(designEnt);
}

IntPairResultSet PKB::getNextRelationPairings(std::string designEnt1, std::string designEnt2) {
    std::unordered_set<int> filteredSearchSpace = gStore->getStmtNumByStmtType(designEnt1);
    return nStore->getNextRelationPairings(filteredSearchSpace, designEnt2);
}

IntPairResultSet PKB::getNextRelationPairingsT(std::string designEnt1, std::string designEnt2) {
    std::unordered_set<int> filteredSearchSpace = gStore->getStmtNumByStmtType(designEnt1);
    return nStore->getNextRelationPairingsT(filteredSearchSpace, designEnt2);
}

std::unordered_set<std::string> PKB::getProceduresThatCalls(std::string proc, bool isRecursive) {
    return isRecursive ? cStore->getAllCalledStarBy(proc) : cStore->getAllCalledBy(proc);
}

std::unordered_set<std::string> PKB::getProceduresCalledBy(std::string proc, bool isRecursive) {
    return isRecursive ? cStore->getAllCallsStar(proc) : cStore->getAllCalls(proc);
}

StringPairResultSet PKB::getCallsRelationPairings(bool isRecursive) {
    return isRecursive ? cStore->getAllCallsStarRelationPair() : cStore->getAllCallsRelationPair();
}

bool PKB::isCalls(std::string caller, std::string callee, bool isRecursive) {
    return isRecursive ? cStore->isCallsStar(caller, callee) : cStore->isCalls(caller, callee);
}

bool PKB::procedureIsCaller(std::string proc) {
    return cStore->isProcCaller(proc);
}

bool PKB::procedureIsCallee(std::string proc) {
    return cStore->isProcCallee(proc);
}

bool PKB::containsCallRelation() {
    return cStore->isProgramCaller();
}

std::string PKB::getProcedureCalledAtStmt(int stmtLine) {
    return cStore->getProcedureCalledAtStmt(stmtLine);
}

std::unordered_set<int> PKB::getStatementsThatCalls(std::string proc) {
    return cStore->getStatementsThatCalls(proc);
}

IntStringPairResultSet PKB::getAllCallStmtWithProcPair() {
    return cStore->getAllCallStmtWithProcPair();
}

bool PKB::containsAffectsRelation() {
    return true;
}

bool PKB::isAffects(int s1, int s2, bool isRecursive) {
    return isRecursive ? aStore->isAffectsStar(s1, s2) : aStore->isAffects(s1, s2);
}

std::unordered_set<int> PKB::getStatementsThatAffects() {
    return aStore->getStmtsThatAffects();
}

std::unordered_set<int> PKB::getStatementsThatAffects(int s1) {
    if (gStore->getStmtTypeByStmtNum(s1) != ASSIGN) {
        std::unordered_set<int> empty;
        return empty;
    }

    return aStore->getStmtsThatAffects(s1);
}

std::unordered_set<int> PKB::getStatementsThatAffectsT(int s1) {
    if (gStore->getStmtTypeByStmtNum(s1) != ASSIGN) {
        std::unordered_set<int> empty;
        return empty;
    }

    return aStore->getStmtsThatAffectsT(s1);
}

std::unordered_set<int> PKB::getStatementsAffectedBy() {
    return aStore->getStmtsAffectedBy();
}

std::unordered_set<int> PKB::getStatementsAffectedBy(int s1) {
    if (gStore->getStmtTypeByStmtNum(s1) != ASSIGN) {
        std::unordered_set<int> empty;
        return empty;
    }

    return aStore->getStmtsAffectedBy(s1);
}

std::unordered_set<int> PKB::getStatementsAffectedByT(int s1) {
    if (gStore->getStmtTypeByStmtNum(s1) != ASSIGN) {
        std::unordered_set<int> empty;
        return empty;
    }

    return aStore->getStmtsAffectedByT(s1);
}

IntPairResultSet PKB::getAffectsRelationPairings() {
    return aStore->getAffectsRelationPairing();
}

IntPairResultSet PKB::getAffectsRelationPairingsT() {
    return aStore->getAffectsRelationPairingT();
}

void PKB::clearCache() {
    nStore->resetCache();
    aStore->resetCache();
}

void PKB::clear() {
    fStore->clear();
    mStore->clear();
    uStore->clear();
    pStore->clear();
    ptStore->clear();
    gStore->clear();
    nStore->clear();
    cStore->clear();
    aStore->clear();
}

// Internal helper functions
void PKB::filterSetByEntType(std::unordered_set<int> &originalSet, std::unordered_set<int> &filteredSetDest, std::string filter) {
    std::copy_if(originalSet.begin(), originalSet.end(), std::inserter(filteredSetDest, filteredSetDest.begin()),
                 [&filter](int stmtLine) {
                     return (gStore->getStmtTypeByStmtNum(stmtLine) == filter);
                 });
}

bool PKB::isContainerStmt(std::string entType) {
    return ((entType == WHILE_STMT) || (entType == IF_STMT)) || (entType == STMT);
}
