#include "Optimizer.h"

bool Optimizer::ClauseWithPoints::operator<(const ClauseWithPoints &other) const {
    return points < other.points;
}

std::unordered_map<std::string, std::vector<QueryParser::Clause>> Optimizer::getOptimizedQuerySequence(
    std::vector<QueryParser::InputType> selectedEntities, std::vector<QueryParser::Clause> suchThatClauses,
    std::vector<QueryParser::Clause> patternClauses, std::vector<QueryParser::Clause> withClauses) {

    std::unordered_map<std::string, std::vector<QueryParser::Clause>> returnMap;
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::vector<Group> groupedBySynonyms;

    std::unordered_set<std::string> usedSynonyms;
    for (QueryParser::InputType selectedEntity : selectedEntities) {
        std::string stripped = QueryEvaluator::parseAttributeInput(selectedEntity.var)[0];
        usedSynonyms.insert(stripped);
    }

    // TODO: use isbooleanclause instead
    // categorize into nonBoolean/boolean
    for (QueryParser::Clause suchThat : suchThatClauses) {
        if (QueryEvaluator::isRelationBetweenIdentifiers(suchThat)) {
            booleanClauses.push_back(suchThat);
        } else {
            nonBooleanClauses.push_back(suchThat);
        }
    }

    for (QueryParser::Clause pattern : patternClauses) {
        if (QueryEvaluator::isRelationBetweenIdentifiers(pattern)) {
            booleanClauses.push_back(pattern);
        } else {
            nonBooleanClauses.push_back(pattern);
        }
    }

    for (QueryParser::Clause with : withClauses) {
        if (QueryEvaluator::isRelationBetweenIdentifiers(with)) {
            booleanClauses.push_back(with);
        } else {
            nonBooleanClauses.push_back(with);
        }
    }

    initializeAdjList(adjList, nonBooleanClauses);
    DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    std::vector<QueryParser::Clause> newNonBooleanClauses =
        sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    returnMap.insert({"booleanClauses", booleanClauses});
    returnMap.insert({"nonBooleanClauses", newNonBooleanClauses});

    return returnMap;
}

void Optimizer::initializeAdjList(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList,
                                  std::vector<QueryParser::Clause> &nonBooleanClauses) {
    for (size_t i = 0; i < nonBooleanClauses.size(); i++) {
        QueryParser::Clause &curr = nonBooleanClauses[i];

        std::unordered_set<std::string> synList = getSynonymsFromClause(curr);

        for (std::string syn : synList) {
            insertToMap(adjList, syn, i);
        }
    }
}

void Optimizer::insertToMap(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList, std::string var,
                            size_t clauseIndex) {
    auto it = adjList.find(var);
    if (it == adjList.end()) {
        std::unordered_set<size_t> newSet{clauseIndex};
        adjList.emplace(var, newSet);
    } else {
        it->second.insert(clauseIndex);
    }
}

void Optimizer::DFS(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList, std::vector<Group> &groupedBySynonyms,
                    std::vector<QueryParser::Clause> &nonBooleanClauses) {
    std::vector<bool> visited;
    for (auto &c : nonBooleanClauses) {
        visited.push_back(false);
    }

    for (auto i = 0; i < nonBooleanClauses.size(); i++) {
        if (!visited[i]) {
            std::unordered_set<std::string> synUsed;
            std::unordered_set<size_t> clauses;
            DFSHelper(adjList, nonBooleanClauses, visited, clauses, synUsed, i);
            Group newGroup{synUsed, clauses};
            groupedBySynonyms.push_back(newGroup);
        }
    }
}

void Optimizer::DFSHelper(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList,
                          std::vector<QueryParser::Clause> &nonBooleanClauses, std::vector<bool> &visited,
                          std::unordered_set<size_t> &group, std::unordered_set<std::string> &synUsed, size_t curr) {
    visited[curr] = true;
    group.insert(curr);
    std::unordered_set<std::string> synList = getSynonymsFromClause(nonBooleanClauses[curr]);

    for (std::string syn : synList) {
        synUsed.insert(syn);
        std::unordered_set<size_t> &neighbours = adjList.find(syn)->second;
        for (size_t neighbour : neighbours) {
            if (!visited[neighbour]) {
                DFSHelper(adjList, nonBooleanClauses, visited, group, synUsed, neighbour);
            }
        }
    }
}

std::vector<QueryParser::Clause> Optimizer::sortClauses(std::vector<Group> &groups, std::unordered_set<std::string> &selectedSyns,
                                                        std::vector<QueryParser::Clause> &booleanClauses,
                                                        std::vector<QueryParser::Clause> &nonBooleanClauses) {

    std::vector<QueryParser::Clause> newNonBooleanClauses;

    for (Group &currGroup : groups) {
        // sort each group first
        std::vector<size_t> sortedCurrGroup = sortOneGroup(currGroup.clauses, nonBooleanClauses);

        // if syn is BOOLEAN, then all clauses are nonBoolean
        if (selectedSyns.find("BOOLEAN") != selectedSyns.end() || hasCommonSynonyms(selectedSyns, currGroup.synUsed)) {
            // add clauses from group into non boolean clauses
            for (size_t cl : sortedCurrGroup) {
                newNonBooleanClauses.push_back(nonBooleanClauses[cl]);
            }
        } else {
            // add clauses from group into boolean clauses
            for (size_t cl : sortedCurrGroup) {
                booleanClauses.push_back(nonBooleanClauses[cl]);
            }
        }
    }
    return newNonBooleanClauses;
}

std::vector<size_t> Optimizer::sortOneGroup(std::unordered_set<size_t> &group,
                                            std::vector<QueryParser::Clause> &nonBooleanClauses) {

    std::vector<size_t> resultGroup;
    std::vector<ClauseWithPoints> combined;

    for (size_t cl : group) {
        QueryParser::Clause &clause = nonBooleanClauses[cl];
        combined.push_back({cl, 0});
    }

    sortByClauseType(combined, nonBooleanClauses);

    std::unordered_set<std::string> currEvaluatedSyns;
    sortByAlreadyEvaluatedClause(combined, currEvaluatedSyns, nonBooleanClauses, resultGroup);

    return resultGroup;
}

void Optimizer::sortByClauseType(std::vector<ClauseWithPoints> &clauses, std::vector<QueryParser::Clause> &nonBooleanClauses) {
    // to do: get the table size for each clause inside nonBooleanClauses and store it in above function.
    // then sort according to the size, and return

    // can be used for unit-debugging
    //
    // const std::unordered_map<std::string, size_t> clauseTypePoints{
    //    {"with", 0}, {"suchThat", 0}, {"assignPattern", 3}, {"ifPattern", 3}, {"whilePattern", 3}};

    // const std::unordered_map<std::string, size_t> relationTypePoints{
    //

    //    {"Parent", 1},  {"Follows", 2},  {"Calls", 4},   {"Modifies", 5},   {"Uses", 6}, {"Calls*", 7},
    //    {"Parent*", 8}, {"Follows*", 9}, {"Next", 10}, {"Affects", 11}, {"Affects*", 12}, {"Next*", 13}};

    // for (ClauseWithPoints &clause : clauses) {
    //    QueryParser::Clause &currClause = nonBooleanClauses[clause.clauseIndex];

    //    clause.points += clauseTypePoints.find(currClause.clauseType)->second;

    //    if (currClause.clauseType == "suchThat") {
    //        clause.points += relationTypePoints.find(currClause.clauseVariable)->second;
    //    }
    //}

    for (ClauseWithPoints &clause : clauses) {
        QueryParser::Clause &currClause = nonBooleanClauses[clause.clauseIndex];
        clause.points += getTableSizeOfClause(currClause);
    }
    std::sort(clauses.begin(), clauses.end());
}

void Optimizer::sortByAlreadyEvaluatedClause(std::vector<ClauseWithPoints> &toSort,
                                             std::unordered_set<std::string> &currEvaluatedSyns,
                                             std::vector<QueryParser::Clause> &nonBooleanClauses,
                                             std::vector<size_t> &resultGroup) {

    // insertion sort like to find nearest clause that has synonyms that have already been evaluated
    for (auto i = 0; i < toSort.size(); i++) {
        size_t currClauseIndex = toSort[i].clauseIndex;

        if (currClauseIndex == -1) {
            // already inserted, can skip to next
            continue;
        }

        QueryParser::Clause &currClause = nonBooleanClauses[currClauseIndex];

        std::unordered_set<std::string> usedSyns = getSynonymsFromClause(currClause);

        // worse case scenario where the current clause will be moved to the back
        int limit = toSort.size() - (i + 1);
        while (!hasCommonSynonyms(currEvaluatedSyns, usedSyns) && limit != 0) {
            limit--;
            // like bubbleSort, if nothing was moved in front, means nothing was found and can break from loop
            bool moved = false;

            // no common syns currently, find possible link and insert
            for (auto j = i + 1; j < toSort.size(); j++) {
                size_t currSearchClauseIndex = toSort[j].clauseIndex;
                if (currSearchClauseIndex == -1) {
                    // already inserted before, can skip to next
                    continue;
                }
                QueryParser::Clause &currSearchClause = nonBooleanClauses[currSearchClauseIndex];
                // if (currSearchClause.clauseType == "affects" || currSearchClause.clauseType == "affects*") {
                //    // don't move affects/affects*
                //    // can remove since we split affects into another list
                //    continue;
                //}

                std::unordered_set<std::string> usedSynsInSearch = getSynonymsFromClause(currSearchClause);
                if (hasCommonSynonyms(currEvaluatedSyns, usedSynsInSearch)) {
                    // if curr search clause can link the LHS
                    resultGroup.push_back(currSearchClauseIndex);
                    currEvaluatedSyns.insert(usedSynsInSearch.begin(), usedSynsInSearch.end());
                    // set index to -1 so wont be inserted again
                    toSort[j].clauseIndex = -1;
                    moved = true;
                    break;
                }
            }

            if (!moved) {
                break;
            }
        }
        // just add to result group because if no "link" clause could be found, just add anyway
        resultGroup.push_back(currClauseIndex);
        currEvaluatedSyns.insert(usedSyns.begin(), usedSyns.end());
    }
}

// ============================= HELPERS ============================================

std::unordered_set<std::string> Optimizer::getSynonymsFromClause(QueryParser::Clause &clause) {
    std::unordered_set<std::string> varList;
    QueryParser::InputType &first = clause.firstVar;
    QueryParser::InputType &second = clause.secondVar;

    // pattern clause
    if (clause.clauseType == "assignPattern" || clause.clauseType == "ifPattern" || clause.clauseType == "whilePattern") {
        varList.insert(clause.clauseVariable);
    }

    if (checkEntityTypeIsSynonymType(first.entityType)) {
        if (clause.clauseType == "with") {
            // need to split the .
            std::string var = QueryEvaluator::parseAttributeInput(first.var)[0];
            varList.insert(var);
        } else {
            varList.insert(first.var);
        }
    }

    if (checkEntityTypeIsSynonymType(second.entityType)) {
        if (clause.clauseType == "with") {
            // need to split the .
            std::string var = QueryEvaluator::parseAttributeInput(second.var)[0];
            varList.insert(var);
        } else {
            varList.insert(second.var);
        }
    }
    return varList;
}

bool Optimizer::checkEntityTypeIsSynonymType(std::string ent) {
    return (ent == "stmt" || ent == "read" || ent == "print" || ent == "call" || ent == "procedure" || ent == "while" ||
            ent == "if" || ent == "assign" || ent == "variable" || ent == "prog_line" || ent == "constant" || ent == "stmtLst");
}

int Optimizer::countSynonymsInClause(QueryParser::Clause &clause) {
    int count = 0;
    QueryParser::InputType &first = clause.firstVar;
    QueryParser::InputType &second = clause.secondVar;

    // pattern clause already has 1 synonym
    if (clause.clauseType == "assignPattern" || clause.clauseType == "ifPattern" || clause.clauseType == "whilePattern") {
        count += 1;
    }

    if (checkEntityTypeIsSynonymType(first.entityType)) {
        count += 1;
    }

    if (checkEntityTypeIsSynonymType(second.entityType)) {
        count += 1;
    }

    return count;
}

bool Optimizer::hasCommonSynonyms(std::unordered_set<std::string> &selectedSyns,
                                  std::unordered_set<std::string> &synUsedInClause) {
    for (std::string s : synUsedInClause) {
        if (selectedSyns.find(s) != selectedSyns.end()) {
            return true;
        }
    }
    return false;
}

int Optimizer::getTableSizeOfClause(QueryParser::Clause &clause) {
    return QueryEvaluator::getTableFromClauseHelper(clause).getCardinality();
}