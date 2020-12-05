#include "ParentStorage.h"
#include "catch.hpp"

/*
 * 1	while (a == b) {
 * 2		print a;
 * 3		if (x == 1) then {
 * 4			while (c == a) {
 * 5				count = 1 + 1;
 * 6				a = count * 2;
 *			}
 * 7			print a;
 *		} else {
 * 8			count = 2;
 * 9			a = count * 2 + b;
 *		}
 *	}
 */

TEST_CASE("Parent Storage: Setup") {
    ParentStorage *pStore = ParentStorage::getInstance();

    pStore->setParent(1, 2);
    pStore->setParent(1, 3);
    pStore->setParent(4, 5);
    pStore->setParent(4, 6);
    pStore->setParent(3, 4);
    pStore->setParent(3, 7);
    pStore->setParent(3, 8);
    pStore->setParent(3, 9);

    pStore->setParentStar(1, 2);
    pStore->setParentStar(1, 3);
    pStore->setParentStar(4, 5);
    pStore->setParentStar(4, 6);
    pStore->setParentStar(3, 4);
    pStore->setParentStar(3, 7);
    pStore->setParentStar(3, 8);
    pStore->setParentStar(3, 9);

    pStore->setParentStar(1, 4);
    pStore->setParentStar(1, 5);
    pStore->setParentStar(1, 6);
    pStore->setParentStar(1, 7);
    pStore->setParentStar(1, 8);
    pStore->setParentStar(1, 9);

    pStore->setParentStar(3, 5);
    pStore->setParentStar(3, 6);
}

TEST_CASE("Parent Storage: Test isParentStar and isParent") {
    ParentStorage *pStore = ParentStorage::getInstance();

    REQUIRE(pStore->isParent(1, 2));
    REQUIRE(pStore->isParent(4, 6));
    REQUIRE(pStore->isParent(3, 9));
    REQUIRE(!pStore->isParent(1, 5));
    REQUIRE(!pStore->isParent(3, 6));
    REQUIRE(!pStore->isParent(4, 8));
    REQUIRE(!pStore->isParent(2, 1));
    REQUIRE(!pStore->isParent(4, 2));

    REQUIRE(pStore->isParentStar(1, 2));
    REQUIRE(pStore->isParentStar(1, 6));
    REQUIRE(pStore->isParentStar(3, 5));
    REQUIRE(pStore->isParentStar(1, 8));
    REQUIRE(pStore->isParentStar(3, 9));
    REQUIRE(!pStore->isParentStar(4, 7));
    REQUIRE(!pStore->isParentStar(4, 7));
    REQUIRE(!pStore->isParentStar(4, 3));
}

TEST_CASE("Parent Storage: Test getAllNestedStatements") {
    ParentStorage *pStore = ParentStorage::getInstance();

    std::unordered_set<int> expectedNestedStmtsForLine1{2, 3}, expectedNestedStmtsForLine3{4, 7, 8, 9},
        expectedNestedStmtsForLine4{5, 6};

    REQUIRE(pStore->getAllNestedStatements(1) == expectedNestedStmtsForLine1);
    REQUIRE(pStore->getAllNestedStatements(3) == expectedNestedStmtsForLine3);
    REQUIRE(pStore->getAllNestedStatements(4) == expectedNestedStmtsForLine4);
    REQUIRE(pStore->getAllNestedStatements(7).empty());
}

TEST_CASE("Parent Storage: Test getAllNestedStatementsT") {
    ParentStorage *pStore = ParentStorage::getInstance();

    std::unordered_set<int> expectedNestedStmtsForLine1{2, 3, 4, 5, 6, 7, 8, 9}, expectedNestedStmtsForLine3{4, 5, 6, 7, 8, 9},
        expectedNestedStmtsForLine4{5, 6};

    REQUIRE(pStore->getAllNestedStatementsT(1) == expectedNestedStmtsForLine1);
    REQUIRE(pStore->getAllNestedStatementsT(3) == expectedNestedStmtsForLine3);
    REQUIRE(pStore->getAllNestedStatementsT(4) == expectedNestedStmtsForLine4);
    REQUIRE(pStore->getAllNestedStatementsT(7).empty());
}

TEST_CASE("Parent Storage: Test getParentStatement") {
    ParentStorage *pStore = ParentStorage::getInstance();

    std::unordered_set<int> expectedParentForLine7{3}, expectedNestedStmtsForLine5{4};

    REQUIRE(pStore->getParentStatement(1).empty());
    REQUIRE(pStore->getParentStatement(7) == expectedParentForLine7);
    REQUIRE(pStore->getParentStatement(5) == expectedNestedStmtsForLine5);
}

TEST_CASE("Parent Storage: Test getParentStatementsT") {
    ParentStorage *pStore = ParentStorage::getInstance();

    std::unordered_set<int> expectedParentForLine7{1, 3}, expectedNestedStmtsForLine5{1, 3, 4};

    REQUIRE(pStore->getParentStatementsT(1).empty());
    REQUIRE(pStore->getParentStatementsT(7) == expectedParentForLine7);
    REQUIRE(pStore->getParentStatementsT(5) == expectedNestedStmtsForLine5);
}

TEST_CASE("Parent Storage: Teardown") {
    ParentStorage *pStore = ParentStorage::getInstance();
    pStore->clear();
}
