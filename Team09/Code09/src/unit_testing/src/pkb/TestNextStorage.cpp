#include "NextStorage.h"
#include "catch.hpp"

//      procedure First {
//  1       read x;
//  2       read z;
//  3       call Second;
//  4       while (x == 1) {
//  5           x = 1 + x;
//  6           while (z == 2) {
//  7               x = x + z;}
//  8           x = 2 + x;} }
//
//      procedure Second {
//  9       x = 0;
// 10       i = 5;
// 11       while (i != 0) {
// 12           x = x + 2 * y;
// 13           call Third;
// 14           if (x == 1) then {
// 15               x = x + 1; }
//              else {
// 16               z = 1;}
// 17           print z;
// 18           print y;
// 19           x = x * y + z;}
// 20       i = i - 1;}

class TestNextStorage {
  private:
    std::unordered_map<int, std::set<int>> sampleAdjacencyList{
        {1, std::set<int>{2}},   {2, std::set<int>{3}},       {3, std::set<int>{4}},       {4, std::set<int>{5}},
        {5, std::set<int>{6}},   {6, std::set<int>{7, 8}},    {7, std::set<int>{6}},       {8, std::set<int>{4}},

        {9, std::set<int>{10}},  {10, std::set<int>{11}},     {11, std::set<int>{12, 20}}, {12, std::set<int>{13}},
        {13, std::set<int>{14}}, {14, std::set<int>{15, 16}}, {15, std::set<int>{17}},     {16, std::set<int>{17}},
        {17, std::set<int>{18}}, {18, std::set<int>{19}},     {19, std::set<int>{11}}};

  protected:
    NextStorage *nStore = NULL;

  public:
    TestNextStorage() : nStore(NextStorage::getInstance()) {
        nStore->setProcedureRange(1, 8);
        nStore->setProcedureRange(9, 20);

        nStore->setAdjacencyListMap(sampleAdjacencyList);
        nStore->setReverseAdjacencyListMap(sampleAdjacencyList);
        nStore->setupCache();

        nStore->setStmtToWhileParent(5, 4);
        nStore->setStmtToWhileParent(6, 4);
        nStore->setStmtToWhileParent(8, 4);

        nStore->setStmtToType(1, READ);
        nStore->setStmtToType(2, READ);
        nStore->setStmtToType(3, CALL);
        nStore->setStmtToType(4, WHILE_STMT);
        nStore->setStmtToType(5, ASSIGN);
        nStore->setStmtToType(6, WHILE_STMT);
        nStore->setStmtToType(7, ASSIGN);
        nStore->setStmtToType(8, ASSIGN);
        nStore->setStmtToType(9, ASSIGN);
        nStore->setStmtToType(10, ASSIGN);
        nStore->setStmtToType(11, WHILE_STMT);
        nStore->setStmtToType(12, ASSIGN);
        nStore->setStmtToType(13, CALL);
        nStore->setStmtToType(14, IF_STMT);
        nStore->setStmtToType(15, ASSIGN);
        nStore->setStmtToType(16, ASSIGN);
        nStore->setStmtToType(17, PRINT);
        nStore->setStmtToType(18, PRINT);
        nStore->setStmtToType(19, ASSIGN);
        nStore->setStmtToType(20, ASSIGN);
    }
};

TEST_CASE_METHOD(TestNextStorage, "Next Storage: Test set & get Next(*) "
                                  "relation w/ 2 constants") {

    REQUIRE(nStore->isNext(1, 2, false));
    REQUIRE(nStore->isNext(19, 11, false));
    REQUIRE(nStore->isNext(6, 8, false));
    REQUIRE(nStore->isNext(1, 8, true));
    REQUIRE(nStore->isNext(9, 20, true));
    REQUIRE(nStore->isNext(15, 16, true));
    REQUIRE(nStore->isNext(15, 17, true));

    REQUIRE(nStore->isNext(16, 15, true));

    REQUIRE_FALSE(nStore->isNext(1, 3, false));
    REQUIRE_FALSE(nStore->isNext(8, 9, false));
    REQUIRE_FALSE(nStore->isNext(8, 9, true));

    REQUIRE_FALSE(nStore->isNext(0, 1, false));
    REQUIRE_FALSE(nStore->isNext(99, 100, true));

    nStore->clear();
}

TEST_CASE_METHOD(TestNextStorage, "Next Storage: Test set & get Next(*) "
                                  "relation w/ 1 constant and 1 syn") {

    REQUIRE(nStore->getNextStatements(8, STMT) == std::unordered_set<int>{4});
    REQUIRE(nStore->getNextStatements(14, STMT) == std::unordered_set<int>{15, 16});
    REQUIRE(nStore->getNextStatementsT(1, STMT) == std::unordered_set<int>{2, 3, 4, 5, 6, 7, 8});
    REQUIRE(nStore->getNextStatementsT(1, CALL) == std::unordered_set<int>{3});
    REQUIRE(nStore->getNextStatementsT(1, IF_STMT).empty());

    REQUIRE(nStore->getPreviousStatements(8, STMT) == std::unordered_set<int>{6});
    REQUIRE(nStore->getPreviousStatements(17, STMT) == std::unordered_set<int>{15, 16});
    REQUIRE(nStore->getPreviousStatementsT(20, STMT) == std::unordered_set<int>{9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    REQUIRE(nStore->getPreviousStatementsT(20, CALL) == std::unordered_set<int>{13});

    REQUIRE(nStore->getNextStatements(0, STMT).empty());
    REQUIRE(nStore->getNextStatementsT(1000, STMT).empty());
    REQUIRE(nStore->getPreviousStatements(1000, STMT).empty());
    REQUIRE(nStore->getPreviousStatementsT(-1, STMT).empty());

    nStore->clear();
}

TEST_CASE_METHOD(TestNextStorage, "Next Storage: Test set & get Next(*) "
                                  "relation w/ 1 constant and 1 wildcard") {

    REQUIRE(nStore->hasNext(8));
    REQUIRE(nStore->hasNext(19));
    REQUIRE(nStore->hasPrevious(7));
    REQUIRE(nStore->hasPrevious(19));

    REQUIRE_FALSE(nStore->hasNext(20));
    REQUIRE_FALSE(nStore->hasPrevious(1));
    REQUIRE_FALSE(nStore->hasPrevious(9));

    REQUIRE_FALSE(nStore->hasNext(1000));
    REQUIRE_FALSE(nStore->hasNext(-1));
    REQUIRE_FALSE(nStore->hasPrevious(-1));
    REQUIRE_FALSE(nStore->hasPrevious(1000));

    nStore->clear();
}

TEST_CASE_METHOD(TestNextStorage, "Next Storage: Test set & get Next(*) "
                                  "relation w/ 1 syn and 1 wildcard") {

    REQUIRE(nStore->hasNext(STMT));
    REQUIRE(nStore->hasNext(WHILE_STMT));
    REQUIRE(nStore->hasPrevious(IF_STMT));
    REQUIRE(nStore->hasPrevious(STMT));

    nStore->clear();
}

TEST_CASE_METHOD(TestNextStorage, "Next Storage: Test set & get Next(*) "
                                  "relation w/ 2 syn") {

    std::unordered_set<int> setOfCallStmts{3, 13};
    REQUIRE(nStore->getNextRelationPairings(setOfCallStmts, STMT) ==
            IntPairResultSet{std::make_pair(3, 4), std::make_pair(13, 14)});
    REQUIRE(nStore->getNextRelationPairingsT(setOfCallStmts, STMT) ==
            IntPairResultSet{std::make_pair(3, 4), std::make_pair(3, 5), std::make_pair(3, 6), std::make_pair(3, 7),
                             std::make_pair(3, 8), std::make_pair(13, 14), std::make_pair(13, 15), std::make_pair(13, 16),
                             std::make_pair(13, 17), std::make_pair(13, 18), std::make_pair(13, 19), std::make_pair(13, 20),
                             std::make_pair(13, 11), std::make_pair(13, 12), std::make_pair(13, 13)});
    REQUIRE(nStore->getNextRelationPairingsT(setOfCallStmts, PRINT) ==
            IntPairResultSet{std::make_pair(13, 17), std::make_pair(13, 18)});

    std::unordered_set<int> setOfReadStmts{1, 2};
    REQUIRE(nStore->getNextRelationPairingsT(setOfReadStmts, STMT) ==
            IntPairResultSet{std::make_pair(1, 2), std::make_pair(1, 3), std::make_pair(1, 4), std::make_pair(1, 5),
                             std::make_pair(1, 6), std::make_pair(1, 7), std::make_pair(1, 8), std::make_pair(2, 3),
                             std::make_pair(2, 4), std::make_pair(2, 5), std::make_pair(2, 6), std::make_pair(2, 7),
                             std::make_pair(2, 8)});
    REQUIRE(nStore->getNextRelationPairingsT(setOfReadStmts, ASSIGN) ==
            IntPairResultSet{std::make_pair(1, 5), std::make_pair(1, 7), std::make_pair(1, 8), std::make_pair(2, 5),
                             std::make_pair(2, 7), std::make_pair(2, 8)});

    nStore->clear();
}

TEST_CASE_METHOD(TestNextStorage, "Next Storage: Test NextStarCache for Next r/s with 2 stmt#") {

    REQUIRE(nStore->isNext(9, 20, true));
    REQUIRE(nStore->isNext(10, 20, true)); // Debug to check that ans is already cached and BFS should not be performed.
    nStore->clear();
}
