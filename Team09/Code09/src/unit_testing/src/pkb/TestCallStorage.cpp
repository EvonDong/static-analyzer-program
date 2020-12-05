#include "CallStorage.h"
#include "catch.hpp"

TEST_CASE("Call Storage: PopulateCallsStarMap + isCallsStar Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc1", "proc3", 1);
    cStore->setCalls("proc2", "proc5", 5);
    cStore->setCalls("proc3", "proc7", 15);
    cStore->setCalls("proc5", "proc8", 20);
    cStore->setCalls("proc7", "proc9", 35);
    cStore->setCalls("proc1", "proc5", 7);
    cStore->populateCallsStarMap();

    REQUIRE(cStore->isCallsStar("proc1", "proc7"));
    REQUIRE(cStore->isCallsStar("proc1", "proc3"));
    REQUIRE(cStore->isCallsStar("proc1", "proc5"));
    REQUIRE(cStore->isCallsStar("proc1", "proc8"));

    REQUIRE(cStore->isCallsStar("proc1", "proc9"));
    REQUIRE(cStore->isCallsStar("proc2", "proc5"));
    REQUIRE(cStore->isCallsStar("proc2", "proc8"));
    REQUIRE(cStore->isCallsStar("proc3", "proc7"));
    REQUIRE(cStore->isCallsStar("proc3", "proc9"));
    REQUIRE(cStore->isCallsStar("proc5", "proc8"));
    REQUIRE(cStore->isCallsStar("proc7", "proc9"));

    REQUIRE(!cStore->isCallsStar("proc3", "proc8"));
    REQUIRE(!cStore->isCallsStar("proc3", "proc10"));
    REQUIRE(!cStore->isCallsStar("proc5", "proc10"));
    REQUIRE(!cStore->isCallsStar("proc8", "proc9"));

    cStore->clear();
}

TEST_CASE("Call Storage: isCalls Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc1", "proc3", 1);
    cStore->setCalls("proc2", "proc5", 5);
    cStore->setCalls("proc3", "proc7", 15);
    cStore->setCalls("proc5", "proc8", 20);
    cStore->setCalls("proc7", "proc9", 25);

    REQUIRE(cStore->isCalls("proc1", "proc3"));
    REQUIRE(cStore->isCalls("proc2", "proc5"));
    REQUIRE(cStore->isCalls("proc3", "proc7"));
    REQUIRE(cStore->isCalls("proc5", "proc8"));
    REQUIRE(cStore->isCalls("proc7", "proc9"));

    REQUIRE(!cStore->isCalls("proc3", "proc10"));
    REQUIRE(!cStore->isCalls("proc5", "proc10"));
    REQUIRE(!cStore->isCalls("proc7", "proc8"));

    cStore->clear();
}

TEST_CASE("Call Storage: getAllCallsStar Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc1", "proc3", 2);
    cStore->setCalls("proc2", "proc5", 5);
    cStore->setCalls("proc3", "proc7", 15);
    cStore->setCalls("proc5", "proc8", 20);
    cStore->setCalls("proc7", "proc9", 30);
    cStore->populateCallsStarMap();

    REQUIRE(cStore->getAllCallsStar("proc1") == std::unordered_set<std::string>{"proc7", "proc3", "proc9"});
    REQUIRE(cStore->getAllCallsStar("proc2") == std::unordered_set<std::string>{"proc5", "proc8"});

    cStore->clear();
}

TEST_CASE("Call Storage: getAllLeadingStar Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc1", "proc3", 5);
    cStore->setCalls("proc2", "proc5", 10);
    cStore->setCalls("proc3", "proc7", 15);
    cStore->setCalls("proc5", "proc8", 20);
    cStore->setCalls("proc7", "proc9", 25);
    cStore->populateCallsStarMap();

    REQUIRE(cStore->getAllCalledStarBy("proc7") == std::unordered_set<std::string>{"proc1", "proc3"});
    REQUIRE(cStore->getAllCalledStarBy("proc8") == std::unordered_set<std::string>{"proc2", "proc5"});
    REQUIRE(cStore->getAllCalledStarBy("proc9") == std::unordered_set<std::string>{"proc1", "proc3", "proc7"});

    cStore->clear();
}

TEST_CASE("Call Storage: PopulateCallsStarMap + isCallsStar Test according to sample program") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc2", "proc3", 5);
    cStore->setCalls("proc4", "proc7", 10);
    cStore->setCalls("proc5", "proc6", 15);
    cStore->setCalls("proc8", "proc9", 20);
    cStore->setCalls("proc6", "proc10", 23);
    cStore->setCalls("proc3", "proc11", 25);
    cStore->setCalls("proc10", "proc11", 30);

    cStore->populateCallsStarMap();

    REQUIRE(cStore->isCalls("proc2", "proc3"));
    REQUIRE(cStore->isCalls("proc4", "proc7"));
    REQUIRE(cStore->isCalls("proc5", "proc6"));
    REQUIRE(cStore->isCalls("proc8", "proc9"));
    REQUIRE(!cStore->isCalls("proc2", "proc8"));

    REQUIRE(!cStore->isCalls("proc3", "proc7"));
    REQUIRE(!cStore->isCalls("proc3", "proc9"));
    REQUIRE(!cStore->isCalls("proc5", "proc8"));
    REQUIRE(!cStore->isCalls("proc7", "proc9"));

    REQUIRE(cStore->isCallsStar("proc5", "proc10"));
    REQUIRE(cStore->isCallsStar("proc5", "proc11"));
    REQUIRE(cStore->isCallsStar("proc6", "proc11"));

    REQUIRE(cStore->isCallsStar("proc2", "proc11"));

    cStore->clear();
}

TEST_CASE("Call Storage: Incorrect input") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc3", "proc2", 5);

    REQUIRE(!cStore->isCalls("proc2", "proc3"));
    REQUIRE(!cStore->isCalls("proc3", "proc100"));

    cStore->clear();
}

TEST_CASE("Call Storage: isProcCaller isProcCallee Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc1", "proc3", 5);
    cStore->setCalls("proc2", "proc5", 10);
    cStore->setCalls("proc3", "proc7", 15);
    cStore->setCalls("proc5", "proc8", 20);
    cStore->setCalls("proc7", "proc9", 25);
    cStore->populateCallsStarMap();

    REQUIRE(cStore->isProcCaller("proc1"));
    REQUIRE(cStore->isProcCaller("proc5"));
    REQUIRE(cStore->isProcCaller("proc7"));

    REQUIRE(!cStore->isProcCaller("proc9"));
    REQUIRE(!cStore->isProcCaller("proc8"));

    REQUIRE(cStore->isProcCallee("proc3"));
    REQUIRE(cStore->isProcCallee("proc5"));
    REQUIRE(cStore->isProcCallee("proc7"));

    REQUIRE(!cStore->isProcCallee("proc1"));
    REQUIRE(!cStore->isProcCallee("proc2"));

    REQUIRE(cStore->isProgramCaller());

    cStore->clear();
}

TEST_CASE("Call Storage: isProgramCaller Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc3", "proc2", 5);

    REQUIRE(cStore->isProgramCaller());

    cStore->clear();
}

TEST_CASE("Call Storage: getAllCallsRelationPair getAllCallsStarRelationPair Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("proc1", "proc3", 5);
    cStore->setCalls("proc2", "proc5", 10);
    cStore->setCalls("proc3", "proc7", 15);
    cStore->setCalls("proc5", "proc8", 20);
    cStore->setCalls("proc7", "proc9", 25);
    cStore->populateCallsStarMap();

    std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> ans;
    ans.emplace(std::pair<std::string, std::string>{"proc1", "proc3"});
    ans.emplace(std::pair<std::string, std::string>{"proc2", "proc5"});
    ans.emplace(std::pair<std::string, std::string>{"proc3", "proc7"});
    ans.emplace(std::pair<std::string, std::string>{"proc5", "proc8"});
    ans.emplace(std::pair<std::string, std::string>{"proc7", "proc9"});

    std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> result =
        cStore->getAllCallsRelationPair();
    REQUIRE(ans == result);

    result = cStore->getAllCallsStarRelationPair();
    ans.emplace(std::pair<std::string, std::string>{"proc1", "proc7"});
    ans.emplace(std::pair<std::string, std::string>{"proc1", "proc9"});
    ans.emplace(std::pair<std::string, std::string>{"proc3", "proc9"});
    ans.emplace(std::pair<std::string, std::string>{"proc2", "proc8"});
    REQUIRE(ans == result);

    cStore->clear();
}

TEST_CASE("Call Storage: PopulateCallsStarMap + isCallsStar Test according to prog3 program") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("TestA", "transportation", 1);
    cStore->setCalls("TestA", "transportation", 10);
    cStore->setCalls("TestA", "oneCall", 11);
    cStore->setCalls("TestA", "procedure", 14);
    cStore->setCalls("TestA", "oneCall", 16);

    cStore->setCalls("TestB", "TestA", 20);
    cStore->setCalls("TestB", "transportation", 32);
    cStore->setCalls("TestB", "procedure", 35);
    cStore->setCalls("TestB", "country", 37);
    cStore->setCalls("TestB", "procedure", 43);
    cStore->setCalls("TestB", "procedure", 49);
    cStore->setCalls("TestB", "TestA", 58);
    cStore->setCalls("TestB", "country", 59);
    cStore->setCalls("TestB", "procedure", 64);
    cStore->setCalls("TestB", "TenWhileLoop", 66);
    cStore->setCalls("TestB", "transportation", 69);

    cStore->setCalls("TestC", "fruits", 71);
    cStore->setCalls("TestC", "procedure", 72);
    cStore->setCalls("TestC", "fruits", 75);
    cStore->setCalls("TestC", "transportation", 80);
    cStore->setCalls("TestC", "TestA", 83);
    cStore->setCalls("TestC", "TenWhileLoop", 85);
    cStore->setCalls("TestC", "fruits", 86);
    cStore->setCalls("TestC", "fruits", 88);
    cStore->setCalls("TestC", "oneCall", 90);
    cStore->setCalls("TestC", "TenWhileLoop", 92);

    cStore->setCalls("TenWhileLoop", "country", 94);
    cStore->setCalls("TenWhileLoop", "colours", 96);
    cStore->setCalls("TenWhileLoop", "Pr0c3dUr3", 100);
    cStore->setCalls("TenWhileLoop", "Pr0c3dUr3", 103);
    cStore->setCalls("TenWhileLoop", "colours", 109);
    cStore->setCalls("TenWhileLoop", "colours", 113);
    cStore->setCalls("TenWhileLoop", "colours", 119);
    cStore->setCalls("TenWhileLoop", "colours", 122);
    cStore->setCalls("TenWhileLoop", "country", 130);
    cStore->setCalls("TenWhileLoop", "Pr0c3dUr3", 132);
    cStore->setCalls("TenWhileLoop", "colours", 135);
    cStore->setCalls("TenWhileLoop", "Pr0c3dUr3", 137);
    cStore->setCalls("TenWhileLoop", "country", 145);
    cStore->setCalls("TenWhileLoop", "country", 146);
    cStore->setCalls("TenWhileLoop", "country", 149);
    cStore->setCalls("TenWhileLoop", "Pr0c3dUr3", 152);

    cStore->setCalls("procedure", "fruits", 159);
    cStore->setCalls("procedure", "fruits", 164);
    cStore->setCalls("procedure", "fruits", 167);
    cStore->setCalls("procedure", "oneCall", 169);
    cStore->setCalls("procedure", "oneCall", 175);
    cStore->setCalls("procedure", "oneCall", 178);
    cStore->setCalls("procedure", "oneCall", 183);
    cStore->setCalls("procedure", "fruits", 187);

    cStore->setCalls("fruits", "Pr0c3dUr3", 201);
    cStore->setCalls("fruits", "country", 218);
    cStore->setCalls("fruits", "TenWhileLoop", 224);
    cStore->setCalls("fruits", "colours", 237);
    cStore->setCalls("fruits", "oneCall", 251);
    cStore->populateCallsStarMap();

    REQUIRE(cStore->isCalls("fruits", "Pr0c3dUr3"));
    REQUIRE(cStore->isCalls("fruits", "TenWhileLoop"));
    REQUIRE(cStore->isCalls("fruits", "country"));
    REQUIRE(cStore->isCalls("fruits", "oneCall"));
    REQUIRE(!cStore->isCalls("fruits", "transportation"));
    REQUIRE(!cStore->isCalls("TestB", "oneCall"));

    cStore->clear();
}

TEST_CASE("Call Storage: PopulateCallsStarMap + getAllProcWithCallStmt Test") {
    CallStorage *cStore = CallStorage::getInstance();
    cStore->setCalls("TestA", "transportation", 1);
    cStore->setCalls("TestA", "transportation", 10);

    cStore->setCalls("TestB", "TestA", 20);
    cStore->setCalls("TestB", "transportation", 32);
    cStore->setCalls("TestB", "procedure", 35);

    cStore->setCalls("TestC", "fruits", 71);
    cStore->setCalls("TestC", "procedure", 72);
    cStore->setCalls("TestC", "fruits", 75);
    cStore->setCalls("TestC", "transportation", 80);

    cStore->setCalls("TenWhileLoop", "country", 94);

    cStore->setCalls("procedure", "fruits", 159);

    cStore->setCalls("fruits", "oneCall", 251);
    cStore->populateCallsStarMap();

    REQUIRE(cStore->getAllCallStmtWithProcPair() ==
            IntStringPairResultSet{std::make_pair(1, "transportation"), std::make_pair(10, "transportation"),
                                   std::make_pair(20, "TestA"), std::make_pair(32, "transportation"),
                                   std::make_pair(35, "procedure"), std::make_pair(71, "fruits"), std::make_pair(72, "procedure"),
                                   std::make_pair(75, "fruits"), std::make_pair(80, "transportation"),
                                   std::make_pair(94, "country"), std::make_pair(159, "fruits"), std::make_pair(251, "oneCall")});

    cStore->clear();
}
