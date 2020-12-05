#pragma once

#include "GeneralStorage.h"
#include "ModifyStorage.h"
#include "NextStorage.h"
#include "ParentStorage.h"
#include "UseStorage.h"
#include "constants.h"

#include <list>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef std::unordered_map<std::string, std::set<int>> LastModifiedMap;
typedef std::unordered_set<std::pair<int, int>, intPairHash, intPairComparator> IntPairResultSet;

class AffectStorage {
  private:
    static AffectStorage *inst_;
    static NextStorage *nextStore;
    static ModifyStorage *modStore;
    static UseStorage *useStore;
    static GeneralStorage *genStore;
    static ParentStorage *parentStore;

    std::unordered_map<int, std::unordered_set<int>> affectsMap;
    std::unordered_map<int, std::unordered_set<int>> affectedByMap;
    std::unordered_map<int, std::unordered_set<int>> affectsStarMap;
    std::unordered_map<int, std::unordered_set<int>> affectedByStarMap;

    const std::unordered_map<int, std::set<int>> *cfgPtr = nullptr;
    const std::unordered_map<int, std::set<int>> *reversedCfgPtr = nullptr;
    bool isAllAffectsRelationPopulated = false;
    bool isAllAffectsStarRelationPopulated = false;

    std::vector<std::pair<int, int>> procedureRangeList;

    void traverseCFG(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt);
    void traverseAfterNonWhileIfStmt(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt);
    void traverseOnlyPath(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt);
    void traverseOutOfWhile(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt);
    void addToLastModifiedMap(LastModifiedMap *lastModifiedMap, std::string var, int s);
    void addLastModified(LastModifiedMap &lastModifiedMap, std::string var, int s);
    void traverseAfterWhileStmt(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt);
    void traverseAfterIfStmt(int s, LastModifiedMap &lastModifiedMap, std::unordered_set<int> &visited, int terminalStmt);
    void setAffectsForVarUsed(int s, LastModifiedMap &lastModifiedMap);
    void updateVarModified(int s, LastModifiedMap &lastModifiedMap);
    void removeVarModified(int s, LastModifiedMap *lastModifiedMap);
    void DFSConstructAffectingList(int end, int curr, std::unordered_set<int> &visited, bool isStart,
                                   std::unordered_set<int> &affectingList, std::unordered_set<std::string> &modifiedList);
    void DFSConstructAffectedList(int start, int curr, std::unordered_set<int> &visited, bool isStart,
                                  std::unordered_set<int> &affectedList);
    void populateAffectsStarInfo();
    void addAffectsStarRelation(int s1, int s2);

    int findMergeStmt(int ifStmt);

    bool isEndOfProcedure(int stmt);
    bool isAllowedToExitWhileLoop(LastModifiedMap &lastModifiedMap, LastModifiedMap &beforeLmm);
    bool contains(std::unordered_set<std::string> &modifiedInStmt, std::unordered_set<std::string> &usedInStmt);
    bool isValidPathUsingDFS(int start, int end, int curr, std::unordered_set<int> &visited, bool isStart);
    bool isLastModifiedChanged(int s, int start);
    bool isAnyValidPathUsingDFS(int start, int curr, std::unordered_set<int> &visited, bool isStart);
    bool isAffectAllowed(int s1, int s2, bool isTransitiveCheck);
    bool checkAffectsMap(int s, int s2);

    LastModifiedMap merge(LastModifiedMap &map1, LastModifiedMap &map2);

    std::unordered_set<int> getStmtsThatAffectsIfAffectedByMapPopulated(int s);
    std::unordered_set<int> getStmtsAffectedByIfAffectsMapPopulated(int s);
    std::unordered_set<int> getAllReachableNodes(int startNode, std::unordered_map<int, std::unordered_set<int>> &traversalMap);

  public:
    static AffectStorage *getInstance();

    void setupAffectStorage();
    void setProcedureRange(int startNode, int endNode);
    void setAffects(int s1, int s2);
    void setAffectedBy(int affectedStmt, int stmt);
    void populateAffectsInfo();

    // to use to unit test the populated Affects Map
    bool isAffectsFromPopulation(int s1, int s2);

    // populates all affects relation
    IntPairResultSet getAffectsRelationPairing();
    IntPairResultSet getAffectsRelationPairingT();
    // IntPairResultSet getAffectedByRelationPairing(std::unordered_set<int>& assignStmts);

    // Affects(stmt, stmt)
    bool isAffects(int s1, int s2);

    // Affects(_,_)
    bool containsAffectRelation();

    // Affects(stmt, _)
    bool isStmtAffectingAnother(int s);

    // get all stmts that affects stmt s Affects(_, s)
    std::unordered_set<int> getStmtsThatAffects(int s);

    // get stmts that is affected by stmt s Affects(s, _)
    std::unordered_set<int> getStmtsAffectedBy(int s);

    // get all stmts that are affected - assign a; Select a such that Affects(_, a)
    std::unordered_set<int> getStmtsThatAffects();

    // get stmts that affects - assign a; Select a such that Affects(a, _)
    std::unordered_set<int> getStmtsAffectedBy();

    // Affects*(stmt, stmt)
    bool isAffectsStar(int s1, int s2);

    // get all stmts that affects stmt s Affects*(_, s)
    std::unordered_set<int> getStmtsThatAffectsT(int s);

    // get stmts that is affected by stmt s Affects*(s, _)
    std::unordered_set<int> getStmtsAffectedByT(int s);

    void clear();
    void resetCache();
};
