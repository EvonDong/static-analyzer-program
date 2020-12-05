#include "FollowStorage.h"
#include "catch.hpp"

TEST_CASE("Follow Storage: PopulateFollowsStarMap + isFollowsStar Test") {
    FollowStorage *fStore = FollowStorage::getInstance();
    fStore->setFollows(1, 3);
    fStore->setFollows(2, 5);
    fStore->setFollows(3, 7);
    fStore->setFollows(5, 8);
    fStore->setFollows(7, 9);
    fStore->populateFollowsStarMap();

    REQUIRE(fStore->isFollowsStar(1, 7));
    REQUIRE(fStore->isFollowsStar(1, 3));
    REQUIRE(fStore->isFollowsStar(1, 9));
    REQUIRE(fStore->isFollowsStar(2, 5));
    REQUIRE(fStore->isFollowsStar(2, 8));
    REQUIRE(fStore->isFollowsStar(3, 7));
    REQUIRE(fStore->isFollowsStar(3, 9));
    REQUIRE(fStore->isFollowsStar(5, 8));
    REQUIRE(fStore->isFollowsStar(7, 9));

    REQUIRE(!fStore->isFollowsStar(3, 8));
    REQUIRE(!fStore->isFollowsStar(3, 10));
    REQUIRE(!fStore->isFollowsStar(5, 10));
    REQUIRE(!fStore->isFollowsStar(8, 9));

    fStore->clear();
}

TEST_CASE("Follow Storage: isFollows Test") {
    FollowStorage *fStore = FollowStorage::getInstance();
    fStore->setFollows(1, 3);
    fStore->setFollows(2, 5);
    fStore->setFollows(3, 7);
    fStore->setFollows(5, 8);
    fStore->setFollows(7, 9);
    fStore->populateFollowsStarMap();

    REQUIRE(fStore->isFollows(1, 3));
    REQUIRE(fStore->isFollows(2, 5));
    REQUIRE(fStore->isFollows(3, 7));
    REQUIRE(fStore->isFollows(5, 8));
    REQUIRE(fStore->isFollows(7, 9));

    REQUIRE(!fStore->isFollows(3, 10));
    REQUIRE(!fStore->isFollows(5, 10));
    REQUIRE(!fStore->isFollows(7, 8));

    fStore->clear();
}

TEST_CASE("Follow Storage: getAllFollowsStar Test") {
    FollowStorage *fStore = FollowStorage::getInstance();
    fStore->setFollows(1, 3);
    fStore->setFollows(2, 5);
    fStore->setFollows(3, 7);
    fStore->setFollows(5, 8);
    fStore->setFollows(7, 9);
    fStore->populateFollowsStarMap();

    REQUIRE(fStore->getAllFollowsStar(1) == std::unordered_set<int>{7, 3, 9});
    REQUIRE(fStore->getAllFollowsStar(2) == std::unordered_set<int>{5, 8});

    fStore->clear();
}

TEST_CASE("Follow Storage: getAllLeadingStar Test") {
    FollowStorage *fStore = FollowStorage::getInstance();
    fStore->setFollows(1, 3);
    fStore->setFollows(2, 5);
    fStore->setFollows(3, 7);
    fStore->setFollows(5, 8);
    fStore->setFollows(7, 9);
    fStore->populateFollowsStarMap();

    REQUIRE(fStore->getAllLeadingStar(7) == std::unordered_set<int>{1, 3});
    REQUIRE(fStore->getAllLeadingStar(8) == std::unordered_set<int>{2, 5});
    REQUIRE(fStore->getAllLeadingStar(9) == std::unordered_set<int>{1, 3, 7});

    fStore->clear();
}

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

TEST_CASE("Follow Storage: PopulateFollowsStarMap + isFollowsStar Test according to sample program") {
    FollowStorage *fStore = FollowStorage::getInstance();
    fStore->setFollows(2, 3);
    fStore->setFollows(4, 7);
    fStore->setFollows(5, 6);
    fStore->setFollows(8, 9);

    REQUIRE(fStore->isFollows(2, 3));
    REQUIRE(fStore->isFollows(4, 7));
    REQUIRE(fStore->isFollows(5, 6));
    REQUIRE(fStore->isFollows(8, 9));
    REQUIRE(!fStore->isFollows(2, 8));
    REQUIRE(!fStore->isFollows(3, 7));
    REQUIRE(!fStore->isFollows(3, 9));
    REQUIRE(!fStore->isFollows(5, 8));
    REQUIRE(!fStore->isFollows(7, 9));

    fStore->clear();
}

TEST_CASE("Follow Storage: Incorrect input") {
    FollowStorage *fStore = FollowStorage::getInstance();
    fStore->setFollows(3, 2);

    REQUIRE(!fStore->isFollows(3, 2));
    REQUIRE(!fStore->isFollows(3, 100));

    fStore->clear();
}
