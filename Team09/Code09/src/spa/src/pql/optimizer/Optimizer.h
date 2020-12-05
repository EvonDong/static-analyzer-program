#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "QueryEvaluator.h"
#include "QueryParser.h"

class Optimizer {
  public:
    static std::unordered_map<std::string, std::vector<QueryParser::Clause>>
    getOptimizedQuerySequence(std::vector<QueryParser::InputType> selectedEntites,
                              std::vector<QueryParser::Clause> suchThatClauses, std::vector<QueryParser::Clause> patternClauses,
                              std::vector<QueryParser::Clause> withClauses);

    struct Group {
        std::unordered_set<std::string> synUsed;
        // the number in clauses corresponds to the index in nonBooleanClauses vector
        std::unordered_set<size_t> clauses;
    };

    struct ClauseWithPoints {
        size_t clauseIndex;
        size_t points;

        bool operator<(const ClauseWithPoints &other) const;
    };

    /*
     * Description: Initialize adj list to group clauses into groups with common synonyms.
     */
    static void initializeAdjList(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList,
                                  std::vector<QueryParser::Clause> &nonBooleanClauses);

    /*
     * Description: Adds <synonym, clauseIndex> to adjList
     */
    static void insertToMap(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList, std::string var,
                            size_t clauseIndex);

    /*
     * Description: Group clauses by common synonyms using DFS.
     */
    static void DFS(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList, std::vector<Group> &groupedBySynonyms,
                    std::vector<QueryParser::Clause> &nonBooleanClauses);

    /*
     * Description: Helper for DFS
     */
    static void DFSHelper(std::unordered_map<std::string, std::unordered_set<size_t>> &adjList,
                          std::vector<QueryParser::Clause> &nonBooleanClauses, std::vector<bool> &visited,
                          std::unordered_set<size_t> &group, std::unordered_set<std::string> &synUsed, size_t curr);

    /*
     * Description: Sort groups of clauses.
     * Ranked via
     * 1. Without synonyms used in select-cl (should be added to booleanClause vector)
     * 2. Use sortOneGroup to sort each groups
     *
     * Returns: final non-boolean clauses sorted
     */
    static std::vector<QueryParser::Clause> sortClauses(std::vector<Group> &group, std::unordered_set<std::string> &selectedSyns,
                                                        std::vector<QueryParser::Clause> &booleanClauses,
                                                        std::vector<QueryParser::Clause> &nonBooleanClauses);

    /*
     * Description: Sort clauses in a (common-synonym) group.
     * Ranked via
     * 1. No. of synonyms (Less is better)
     * 2. Type of clause
     *   2a. with-cl -> Parent -> Calls -> Follow -> Next -> pattern -> Modifies -> Uses ->
     *       Parent* -> Call* -> Follow* -> Next* -> Affects -> Affects*
     *
     * Then perform modification of insertion sort to put clauses with common synonyms together
     */
    static std::vector<size_t> sortOneGroup(std::unordered_set<size_t> &group,
                                            std::vector<QueryParser::Clause> &nonBooleanClauses);

    /*
     * Description: Sort/Arrange clauses in a (common-synonym) group.
     * Does point 2. from Optimizer::sortOneGroup
     * 2. Type of clause
     *   2a. with-cl -> Parent -> Calls -> Follow -> Next -> pattern -> Modifies -> Uses ->
     *       Parent* -> Call* -> Follow* -> Next* -> Affects -> Affects*
     */
    static void sortByClauseType(std::vector<ClauseWithPoints> &clauses, std::vector<QueryParser::Clause> &nonBooleanClauses);

    static void sortByAlreadyEvaluatedClause(std::vector<ClauseWithPoints> &toSort,
                                             std::unordered_set<std::string> &currEvaluatedSyns,
                                             std::vector<QueryParser::Clause> &nonBooleanClauses,
                                             std::vector<size_t> &resultGroup);

    // ============================ HELPERS ===================================
    /*
     * Description: Get a list of synonyms used in a clause.
     */
    static std::unordered_set<std::string> getSynonymsFromClause(QueryParser::Clause &clause);

    /*
     * Description: Checks whether Clause::InputType is a synonym based on entity type.
     * Using Clause::InputType.entityType
     *
     * Example: Synonym used in clauses have specific entityType.
     * variable v; Select v such that Uses(2, v) and Follows(s, 2)
     * v will have entityType "variable", s will have entityType "stmt"
     */
    static bool checkEntityTypeIsSynonymType(std::string ent);

    /*
     * Description: Counts the number of synonyms used in a clause.
     * Maximum of 2
     */
    static int countSynonymsInClause(QueryParser::Clause &clause);

    /*
     * Description: Check if synonyms from synUsed (synonym used in a group) have any from the select-cl
     * OR Check if any std::string from synUsedInClause exists in selectedSet OR check if any common synonyms
     * Mainly to find out if the group do not return result in Select (all synonyms used have none in select-cl)
     */
    static bool hasCommonSynonyms(std::unordered_set<std::string> &selectedSet, std::unordered_set<std::string> &synUsedInClause);

    static int getTableSizeOfClause(QueryParser::Clause &clause);
};
