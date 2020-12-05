#pragma once

#include "constants.h"

#include <algorithm>
#include <iterator>
#include <list>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class NextStorage {
  private:
    static NextStorage *inst_;

    std::unordered_map<int, std::set<int>> adjacencyListMap;
    std::unordered_map<int, std::set<int>> reverseAdjacencyListMap;

    std::unordered_map<int, std::string> stmtToTypeMap; // keep a separate copy here instead of making multiple calls
                                                        // to GeneralStorage which incurs overhead

    std::unordered_map<int, std::set<int>> stmtToWhileParentMap;
    std::vector<std::pair<int, int>> procedureRangeList;

    std::vector<bool> nextStarBoolCacheState;

    // nextStarBoolCache is mimics a 2D boolean array. Vector is used instead for efficiency as memory space is contiguous
    // although the tradeoff would be that it is not as simple to iterate.
    // For example arr[i][j] is equivalent to vector[i * COL_SIZE + j]. Since we are trying to mimic a 2D array where each entry
    // represents if there is a Next* relation between statements i and j, hence COL_SIZE == ROW_SIZE == total number of
    // statements.
    std::vector<bool> nextStarBoolCache;

    bool validateReachableUsingReverseBFS(int, int);

    std::pair<bool, bool> checkObviousNextStarRs(int, int);

    std::unordered_set<int> findAllImmediateNextNodes(int, std::string, std::unordered_map<int, std::set<int>> &);
    std::unordered_set<int> findAllReachableNodesUsingBFS(int);

    void updateNextStarMapCache(int, int);

  public:
    static NextStorage *getInstance();

    void setupCache();
    void populateNextStarBoolCache();
    void populateNextStarBoolCache(int);

    void setAdjacencyListMap(std::unordered_map<int, std::set<int>> &);
    void setReverseAdjacencyListMap(std::unordered_map<int, std::set<int>> &);
    void setProcedureRange(int, int);
    void setStmtToType(int, std::string);
    void setStmtToWhileParent(int, int);

    bool hasNext(int);
    bool hasNext(std::string);

    bool hasPrevious(int);
    bool hasPrevious(std::string);
    bool isNext(int, int, bool);

    std::unordered_set<int> getNextStatements(int, std::string);
    std::unordered_set<int> getNextStatementsT(int, std::string);
    std::unordered_set<int> getPreviousStatements(int, std::string);
    std::unordered_set<int> getPreviousStatementsT(int, std::string);

    std::unordered_set<int> getNextStatements(std::string);
    std::unordered_set<int> getPreviousStatements(std::string);

    std::unordered_set<int> getNextClosestStatement(int s);

    IntPairResultSet getNextRelationPairings(std::unordered_set<int> &, std::string);
    IntPairResultSet getNextRelationPairingsT(std::unordered_set<int> &, std::string);

    const std::unordered_map<int, std::set<int>> *getNextAdjMap();
    const std::unordered_map<int, std::set<int>> *getReverseAdjMap();

    void clear();
    void resetCache();
};
