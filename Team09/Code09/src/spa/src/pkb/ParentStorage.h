#pragma once

#include <unordered_map>
#include <unordered_set>

class ParentStorage {
  private:
    static ParentStorage *inst_;
    std::unordered_map<int, std::unordered_set<int>> parentMap;
    std::unordered_map<int, std::unordered_set<int>> parentStarMap;
    std::unordered_set<int> parentSet;

    // Maps stmts to their direct parent stmt if present
    std::unordered_map<int, int> reverseParentMap;

    // Maps stmts to all their direct and indirect parents if present
    std::unordered_map<int, std::unordered_set<int>> reverseParentStarMap;

  public:
    static ParentStorage *getInstance();

    void setParent(int, int);
    void setParentStar(int, int);

    bool isParent(int, int);
    bool isParentStar(int, int);

    std::unordered_set<int> getAllNestedStatements(int);
    std::unordered_set<int> getAllNestedStatementsT(int); // T stands for transitive i.e. Parent*
    std::unordered_set<int> getParentStatement(int);
    std::unordered_set<int> getParentStatementsT(int);
    std::unordered_set<int> getAllParentStmts();

    std::unordered_map<int, std::unordered_set<int>> *getAllParentRelationPair();
    std::unordered_map<int, std::unordered_set<int>> *getAllParentStarRelationPair();

    void clear();
};
