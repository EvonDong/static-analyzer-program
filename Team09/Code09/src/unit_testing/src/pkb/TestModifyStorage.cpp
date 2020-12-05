#pragma once

#include "ModifyStorage.h"
#include "catch.hpp"

TEST_CASE("Modify Storage: getModifiesStatement Test with DesignEnt and VarName") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    std::string de = "assign";
    mStore->setVarToStmt(1, de, "apple");
    std::string de2 = "read";
    mStore->setVarToStmt(2, de2, "orange");

    std::string de3 = "assign";
    std::string de4 = "read";
    REQUIRE(mStore->getModifiesStatement(de3, "apple").size() == 1);
    REQUIRE(mStore->getModifiesStatement(de4, "orange").size() == 1);
    REQUIRE(mStore->getModifiesStatement(de4, "apple").size() == 0);
    REQUIRE(mStore->getModifiesStatement(de3, "orange").size() == 0);

    mStore->clear();
}

TEST_CASE("Modify Storage: getModifiesProc Test with VarName") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setVarToProc("proc", "apple");
    mStore->setVarToProc("proc", "h");
    mStore->setVarToProc("proc1", "orange");
    mStore->setProcToVar("proc1", "orange");
    mStore->setProcToVar("proc", "h");
    mStore->setProcToVar("proc", "apple");

    REQUIRE(mStore->getModifiesProc("orange").size() == 1);
    REQUIRE(mStore->getModifiesProc("apple").size() == 1);
    REQUIRE(mStore->getModifiesProc("carrot").size() == 0);

    mStore->clear();
}

TEST_CASE("Modify Storage: getModifiesProc Test (all proc)") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setVarToProc("proc", "apple");
    mStore->setVarToProc("proc", "h");
    mStore->setVarToProc("proc1", "orange");
    mStore->setProcToVar("proc1", "orange");
    mStore->setProcToVar("proc", "h");
    mStore->setProcToVar("proc", "apple");

    REQUIRE(mStore->getModifiesProc().size() == 2);

    mStore->clear();
}

TEST_CASE("Modify Storage: getModifiesVarByProc Test") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setVarToProc("proc", "apple");
    mStore->setVarToProc("proc", "h");
    mStore->setVarToProc("proc1", "orange");
    mStore->setProcToVar("proc1", "orange");
    mStore->setProcToVar("proc", "h");
    mStore->setProcToVar("proc", "apple");

    REQUIRE(mStore->getModifiesVariableByProc("proc").size() == 2);
    REQUIRE(mStore->getModifiesVariableByProc("proc1").size() == 1);

    mStore->clear();
}

TEST_CASE("Modify Storage: getModifiesVariableByStmt Test with stmt#") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setStmtToVar(1, "apple");
    mStore->setStmtToVar(2, "orange");
    std::unordered_set<std::string> expectedModifiesVarByStmt1{"apple"};
    std::unordered_set<std::string> expectedModifiesVarByStmt2{"orange"};

    REQUIRE(mStore->getModifiesVariableByStmt(1) == expectedModifiesVarByStmt1);
    REQUIRE(mStore->getModifiesVariableByStmt(2) == expectedModifiesVarByStmt2);

    mStore->setStmtToVar(1, "apple");
    mStore->setStmtToVar(1, "banana");
    expectedModifiesVarByStmt1.emplace("banana");
    REQUIRE(mStore->getModifiesVariableByStmt(1) == expectedModifiesVarByStmt1);
    mStore->clear();
}

TEST_CASE("Modify Storage: isModifies Test with stmt#, varName") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    std::string de = "assign";
    mStore->setVarToStmt(1, de, "apple");
    std::string de2 = "read";
    mStore->setVarToStmt(2, de2, "orange");

    mStore->setStmtToVar(1, "apple");
    mStore->setStmtToVar(2, "orange");

    REQUIRE(mStore->isModifies(1, "apple"));
    REQUIRE(mStore->isModifies(2, "orange"));
    REQUIRE(!mStore->isModifies(2, "carrot"));

    mStore->clear();
}

TEST_CASE("Modify Storage: isModifies Test with DesignEnt, varName") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    std::string de = "assign";
    mStore->setVarToStmt(1, de, "apple");
    std::string de2 = "read";
    mStore->setVarToStmt(2, de2, "orange");

    mStore->setStmtToVar(1, "apple");
    mStore->setStmtToVar(2, "orange");

    std::string de3 = "assign";
    std::string de4 = "read";
    REQUIRE(mStore->isModifiesByEnt(de3, "apple"));
    REQUIRE(mStore->isModifiesByEnt(de4, "orange"));
    REQUIRE(!mStore->isModifiesByEnt(de3, "orange"));

    mStore->clear();
}

TEST_CASE("Modify Storage: isModifies Test with procName, varName") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setVarToProc("proc", "apple");
    mStore->setVarToProc("proc1", "orange");

    mStore->setProcToVar("proc", "apple");
    mStore->setProcToVar("proc1", "orange");

    REQUIRE(mStore->isModifiesByProc("proc", "apple"));
    REQUIRE(mStore->isModifiesByProc("proc1", "orange"));
    REQUIRE(!mStore->isModifiesByProc("proc", "orange"));

    mStore->clear();
}

TEST_CASE("Modify Storage: getAllModifiesStmt Test with DesignEnt stmt") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setStmtToVar(1, "apple");
    mStore->setStmtToVar(5, "orange");

    std::string de = "assign";

    mStore->setVarToStmt(1, de, "apple");
    mStore->setVarToStmt(5, de, "orange");

    std::string de2 = "assign";

    mStore->setStmtTypeToStmt(de2, 1);
    mStore->setStmtTypeToStmt(de2, 5);

    mStore->setVarToProc("proc", "apple");
    mStore->setVarToProc("proc1", "orange");

    mStore->setProcToVar("proc", "apple");
    mStore->setProcToVar("proc1", "orange");

    REQUIRE(mStore->getAllModifiesStatements(de2).size() == 2);
    REQUIRE(mStore->getAllModifiesStatements(de).size() == 2);

    mStore->clear();
}

TEST_CASE("Modify Storage: getAllProcModifiesVarRelationPair + "
          "getAllStmtModifiesVarRelationPair Test get all Modifies relation") {
    ModifyStorage *mStore = ModifyStorage::getInstance();
    mStore->setStmtToVar(1, "apple");
    mStore->setStmtToVar(5, "orange");

    mStore->setVarToStmt(1, ASSIGN, "apple");
    mStore->setVarToStmt(5, ASSIGN, "orange");

    mStore->setStmtTypeToStmt(ASSIGN, 1);
    mStore->setStmtTypeToStmt(ASSIGN, 5);

    mStore->setVarToProc("proc", "apple");
    mStore->setVarToProc("proc1", "orange");

    mStore->setProcToVar("proc", "apple");
    mStore->setProcToVar("proc1", "orange");

    REQUIRE(mStore->getAllProcModifiesVarRelationPair() ==
            StringPairResultSet{std::make_pair("proc", "apple"), std::make_pair("proc1", "orange")});
    REQUIRE(mStore->getAllStmtModifiesVarRelationPair(STMT) ==
            IntStringPairResultSet{std::make_pair(1, "apple"), std::make_pair(5, "orange")});
    REQUIRE(mStore->getAllStmtModifiesVarRelationPair(ASSIGN) ==
            IntStringPairResultSet{std::make_pair(1, "apple"), std::make_pair(5, "orange")});
    REQUIRE(mStore->getAllStmtModifiesVarRelationPair(PRINT).empty());

    mStore->clear();
}
