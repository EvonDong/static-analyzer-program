#pragma once

#include "catch.hpp"

#include "UseStorage.h"

TEST_CASE("Use Storage: getUsesStatement Test with DesignEnt and VarName") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToStmt(1, ASSIGN, "apple");
    uStore->setVarToStmt(2, WHILE_STMT, "orange");

    REQUIRE(uStore->getUsesStatement(ASSIGN, "apple").size() == 1);
    REQUIRE(uStore->getUsesStatement(WHILE_STMT, "orange").size() == 1);
    REQUIRE(uStore->getUsesStatement(WHILE_STMT, "apple").size() == 0);
    REQUIRE(uStore->getUsesStatement(ASSIGN, "orange").size() == 0);
    uStore->clear();
}

TEST_CASE("Use Storage: getUsesProc Test with VarName") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToProc("proc", "apple");
    uStore->setVarToProc("proc1", "orange");

    REQUIRE(uStore->getUsesProc("orange").size() == 1);
    REQUIRE(uStore->getUsesProc("apple").size() == 1);
    REQUIRE(uStore->getUsesProc("carrot").size() == 0);

    uStore->clear();
}

TEST_CASE("Use Storage: getUsesProc Test (all proc)") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToProc("proc", "apple");
    uStore->setVarToProc("proc", "h");
    uStore->setVarToProc("proc1", "orange");
    uStore->setProcToVar("proc1", "orange");
    uStore->setProcToVar("proc", "h");
    uStore->setProcToVar("proc", "apple");

    REQUIRE(uStore->getUsesProc().size() == 2);

    uStore->clear();
}

TEST_CASE("Use Storage: getUsesVarByProc Test") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToProc("proc", "apple");
    uStore->setVarToProc("proc", "h");
    uStore->setVarToProc("proc1", "orange");
    uStore->setProcToVar("proc1", "orange");
    uStore->setProcToVar("proc", "h");
    uStore->setProcToVar("proc", "apple");

    REQUIRE(uStore->getUsesVariableByProc("proc").size() == 2);
    REQUIRE(uStore->getUsesVariableByProc("proc1").size() == 1);

    uStore->clear();
}

TEST_CASE("Use Storage: getUsesVariablesByStmt Test with stmt#") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setStmtToVar(1, "apple");
    uStore->setStmtToVar(2, "orange");
    std::unordered_set<std::string> expectedUsesVarByStmt1{"apple"};
    std::unordered_set<std::string> expectedUsesVarByStmt2{"orange"};

    REQUIRE(uStore->getUsesVariablesByStmt(1) == expectedUsesVarByStmt1);
    REQUIRE(uStore->getUsesVariablesByStmt(2) == expectedUsesVarByStmt2);

    uStore->setStmtToVar(1, "apple");
    uStore->setStmtToVar(1, "banana");
    expectedUsesVarByStmt1.emplace("banana");
    REQUIRE(uStore->getUsesVariablesByStmt(1) == expectedUsesVarByStmt1);
    uStore->clear();
}

TEST_CASE("Use Storage: isUses Test with stmt#, varName") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToStmt(1, ASSIGN, "apple");
    uStore->setVarToStmt(2, READ, "orange");

    uStore->setStmtToVar(1, "apple");
    uStore->setStmtToVar(2, "orange");

    REQUIRE(uStore->isUses(1, "apple"));
    REQUIRE(uStore->isUses(2, "orange"));
    REQUIRE(!uStore->isUses(2, "carrot"));

    uStore->clear();
}

TEST_CASE("UseStorage: isUses Test with DesignEnt, varName") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToStmt(1, ASSIGN, "apple");
    uStore->setVarToStmt(2, WHILE_STMT, "orange");

    uStore->setStmtToVar(1, "apple");
    uStore->setStmtToVar(2, "orange");

    REQUIRE(uStore->isUsesByEnt(ASSIGN, "apple"));
    REQUIRE(uStore->isUsesByEnt(WHILE_STMT, "orange"));
    REQUIRE(!uStore->isUsesByEnt(ASSIGN, "orange"));

    uStore->clear();
}

TEST_CASE("Use Storage: isUses Test with procName, varName") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setVarToProc("proc", "apple");
    uStore->setVarToProc("proc1", "orange");

    uStore->setProcToVar("proc", "apple");
    uStore->setProcToVar("proc1", "orange");

    REQUIRE(uStore->isUsesByProc("proc", "apple"));
    REQUIRE(uStore->isUsesByProc("proc1", "orange"));
    REQUIRE(!uStore->isUsesByProc("proc", "orange"));

    uStore->clear();
}

TEST_CASE("Use Storage: getAllUsesStmt Test with DesignEnt stmt") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setStmtToVar(1, "apple");
    uStore->setStmtToVar(5, "orange");

    uStore->setVarToStmt(1, IF_STMT, "apple");
    uStore->setVarToStmt(5, IF_STMT, "orange");

    uStore->setStmtTypeToStmt(IF_STMT, 1);
    uStore->setStmtTypeToStmt(IF_STMT, 5);

    uStore->setVarToProc("proc", "apple");
    uStore->setVarToProc("proc1", "orange");

    uStore->setProcToVar("proc", "apple");
    uStore->setProcToVar("proc1", "orange");

    REQUIRE(uStore->getAllUsesStatements(IF_STMT).size() == 2);
    REQUIRE(uStore->getAllUsesStatements(STMT).size() == 2);

    uStore->clear();
}

TEST_CASE("Use Storage: getAllProcUsesVarRelationPair + getAllStmtUsesVarRelationPair Test get all Uses relations") {
    UseStorage *uStore = UseStorage::getInstance();
    uStore->setStmtToVar(1, "apple");
    uStore->setStmtToVar(5, "orange");

    uStore->setVarToStmt(1, IF_STMT, "apple");
    uStore->setVarToStmt(5, IF_STMT, "orange");

    uStore->setStmtTypeToStmt(IF_STMT, 1);
    uStore->setStmtTypeToStmt(IF_STMT, 5);

    uStore->setVarToProc("proc", "apple");
    uStore->setVarToProc("proc1", "orange");

    uStore->setProcToVar("proc", "apple");
    uStore->setProcToVar("proc1", "orange");

    REQUIRE(uStore->getAllProcUsesVarRelationPair() ==
            StringPairResultSet{std::make_pair("proc", "apple"), std::make_pair("proc1", "orange")});
    REQUIRE(uStore->getAllStmtUsesVarRelationPair(STMT) ==
            IntStringPairResultSet{std::make_pair(1, "apple"), std::make_pair(5, "orange")});
    REQUIRE(uStore->getAllStmtUsesVarRelationPair(IF_STMT) ==
            IntStringPairResultSet{std::make_pair(1, "apple"), std::make_pair(5, "orange")});
    REQUIRE(uStore->getAllStmtUsesVarRelationPair(PRINT).empty());

    uStore->clear();
}
