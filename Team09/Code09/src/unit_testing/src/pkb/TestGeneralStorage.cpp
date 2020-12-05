#include "GeneralStorage.h"
#include "catch.hpp"

TEST_CASE("General Storage: Test varSet") {
    GeneralStorage *gStore = GeneralStorage::getInstance();

    std::string mockVarNameOne{"x"};
    std::string mockVarNameTwo{"count"};

    std::unordered_set<std::string> expectedVarSet;

    gStore->setVar(mockVarNameOne);
    expectedVarSet.emplace(mockVarNameOne);
    REQUIRE(gStore->getAllVars() == expectedVarSet);

    gStore->setVar(mockVarNameTwo);
    expectedVarSet.emplace(mockVarNameTwo);
    REQUIRE(gStore->getAllVars() == expectedVarSet);

    gStore->setVar(mockVarNameOne);
    REQUIRE(gStore->getAllVars() == expectedVarSet);

    gStore->clear();
}

TEST_CASE("General Storage: Test varToStmtNumMap") {
    GeneralStorage *gStore = GeneralStorage::getInstance();

    std::string mockVarNameOne{"x"};
    std::string mockVarNameTwo{"count"};

    std::unordered_set<int> stmtSetOne{1, 4, 5}, stmtSetTwo{2, 3};

    gStore->setVarToStmtNum(mockVarNameOne, 1);
    gStore->setVarToStmtNum(mockVarNameOne, 4);
    gStore->setVarToStmtNum(mockVarNameOne, 5);

    REQUIRE(gStore->getStmtsByVar(mockVarNameOne) == stmtSetOne);

    gStore->setVarToStmtNum(mockVarNameOne, 4);
    gStore->setVarToStmtNum(mockVarNameTwo, 2);
    gStore->setVarToStmtNum(mockVarNameTwo, 3);
    REQUIRE(gStore->getStmtsByVar(mockVarNameOne) == stmtSetOne);
    REQUIRE(gStore->getStmtsByVar(mockVarNameTwo) == stmtSetTwo);

    gStore->clear();
}

TEST_CASE("General Storage: Test stmtNumToStmtObjMap and stmtTypeToStmtNumMap") {
    GeneralStorage *gStore = GeneralStorage::getInstance();

    Statement mockStatementStructOne{1, ASSIGN, std::unordered_set<std::string>{"count", "flag"}, std::unordered_set<int>{6}};
    Statement mockStatementStructTwo{2, WHILE_STMT, std::unordered_set<std::string>{"x", "y"}, std::unordered_set<int>{}};

    gStore->storeStmtObj({1, ASSIGN, std::unordered_set<std::string>{"count", "flag"}, std::unordered_set<int>{6}});
    gStore->storeStmtObj({2, WHILE_STMT, std::unordered_set<std::string>{"x", "y"}, std::unordered_set<int>{}});

    Statement s1 = gStore->getStmtObjByStmtNum(1);
    REQUIRE(s1.stmtNum == mockStatementStructOne.stmtNum);
    REQUIRE(s1.type == mockStatementStructOne.type);
    REQUIRE(s1.varSet == mockStatementStructOne.varSet);
    REQUIRE(s1.constSet == mockStatementStructOne.constSet);

    Statement s2 = gStore->getStmtObjByStmtNum(2);
    REQUIRE(s2.stmtNum == mockStatementStructTwo.stmtNum);
    REQUIRE(s2.type == mockStatementStructTwo.type);
    REQUIRE(s2.varSet == mockStatementStructTwo.varSet);
    REQUIRE(s2.constSet == mockStatementStructTwo.constSet);

    REQUIRE(gStore->getStmtObjByStmtNum(0).type == INVALID);

    REQUIRE(gStore->getVarByStmtNum(1) == std::unordered_set<std::string>{"count", "flag"});
    REQUIRE(gStore->getVarByStmtNum(2) == std::unordered_set<std::string>{"x", "y"});
    REQUIRE(gStore->getVarByStmtNum(100).empty());
    REQUIRE(gStore->getVarByStmtNum(-1).empty());

    REQUIRE(gStore->getStmtTypeByStmtNum(1) == ASSIGN);
    REQUIRE(gStore->getStmtTypeByStmtNum(2) == WHILE_STMT);
    REQUIRE(gStore->getStmtTypeByStmtNum(100) == INVALID);
    REQUIRE(gStore->getStmtTypeByStmtNum(-1) == INVALID);

    REQUIRE(gStore->isValidStatementNum(1, ASSIGN));
    REQUIRE(gStore->isValidStatementNum(2, WHILE_STMT));
    REQUIRE(gStore->isValidStatementNum(1, STMT));
    REQUIRE_FALSE(gStore->isValidStatementNum(1, WHILE_STMT));
    REQUIRE_FALSE(gStore->isValidStatementNum(2, ASSIGN));
    REQUIRE_FALSE(gStore->isValidStatementNum(100, STMT));
    REQUIRE_FALSE(gStore->isValidStatementNum(0, CALL));

    REQUIRE(gStore->getStmtNumByStmtType(ASSIGN) == std::unordered_set<int>{1});
    REQUIRE(gStore->getStmtNumByStmtType(WHILE_STMT) == std::unordered_set<int>{2});
    REQUIRE(gStore->getStmtNumByStmtType(IF_STMT).empty());

    gStore->clear();
}

TEST_CASE("General Storage: Test procedureSet and procedureRangeMap") {
    GeneralStorage *gStore = GeneralStorage::getInstance();

    gStore->setProcedure("MockProc1", 1, 10);
    gStore->setProcedure("MockProc2", 11, 20);
    gStore->setProcedure("MockProc3", 21, 30);

    REQUIRE(gStore->getAllProcedures() == std::unordered_set<std::string>{"MockProc1", "MockProc2", "MockProc3"});
    REQUIRE(gStore->getProcedureRange("MockProc1") == std::pair<int, int>{1, 10});
    REQUIRE(gStore->getProcedureRange("MockProc2") == std::pair<int, int>{11, 20});
    REQUIRE(gStore->getProcedureRange("MockProc3") == std::pair<int, int>{21, 30});
    REQUIRE(gStore->getProcedureRange("unknown") == std::pair<int, int>{0, 0});

    gStore->clear();
}
