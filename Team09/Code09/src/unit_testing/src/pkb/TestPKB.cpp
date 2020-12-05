#include "PKB.h"
#include "catch.hpp"

/*
 * 		procedure main {
 *  1		count = 0;
 *  2		cenX = 0;
 *  3		cenY = 0;
 *  4		while ((x != 0) && (y != 0)) {
 *  5			count = count + 1;
 *  6			if (cenX == cenY) then {
 *  7				print count;
 * 				} else {
 *  8				a = 1;
 *  9				count = a;
 * 10				while (cenX < cenY) {
 * 11					read cenX;
 * 12					read cenY;
 * 13					cenX = cenX + 1;
 *					}
 * 14				count = count + 1;
 *				}
 * 			}
 * 15		if (count == 0) then {
 * 16			flag = 1;
 * 			} else {
 * 17			cenX = cenX / count;
 * 18			cenY = cenY / count;
 * 			}
 * 19		normSq = cenX * cenX + cenY * cenY;
 * 20		print normSq;
 * 21       call first;
 * 		}
 *
 *   	procedure printResults {
 * 22		print flag;
 * 23		print count;
 *		}
 *      procedure first {
 * 24       while (count == 1) {
 * 25           call second;
 *          }
 *      }
 *   	procedure second {
 * 26		x = c + d;
 *		}
 *      procedure affect {
 * 27       while (a == c) {
 * 28          normSq = a;
 * 29          a = b;
 * 30          while (a == c) {
 * 31              b = c;
 * 32              while (a == c) {
 * 33                  cenX = normSq;
 * 34                  c = d;
 * 35                  d = e;
 * 36                  e = f;        }} }
 * 37       cenY = cenX;   }
 */

class TestPKB {
  private:
    static bool isInitialized;

    std::unordered_map<int, std::set<int>> mockAdjacencyList{
        {1, std::set<int>{2}},       {2, std::set<int>{3}},       {3, std::set<int>{4}},       {4, std::set<int>{5, 15}},
        {5, std::set<int>{6}},       {6, std::set<int>{7, 8}},    {7, std::set<int>{4}},       {8, std::set<int>{9}},
        {9, std::set<int>{10}},      {10, std::set<int>{11, 14}}, {11, std::set<int>{12}},     {12, std::set<int>{13}},
        {13, std::set<int>{10}},     {14, std::set<int>{4}},      {15, std::set<int>{16, 17}}, {16, std::set<int>{19}},
        {17, std::set<int>{18}},     {18, std::set<int>{19}},     {19, std::set<int>{20}},     {20, std::set<int>{21}},
        {22, std::set<int>{23}},     {24, std::set<int>{25}},     {25, std::set<int>{24}},     {27, std::set<int>{28, 37}},
        {28, std::set<int>{29}},     {29, std::set<int>{30}},     {30, std::set<int>{31, 27}}, {31, std::set<int>{32}},
        {32, std::set<int>{33, 30}}, {33, std::set<int>{34}},     {34, std::set<int>{35}},     {35, std::set<int>{36}},
        {36, std::set<int>{32}}};

  protected:
    PKB &pkb = PKB::Instance();

  public:
    TestPKB() {
        if (!isInitialized) {
            isInitialized = true;

            CHECK_NOTHROW(pkb.addProcInfo("main", 1, 21));
            CHECK_NOTHROW(pkb.addProcInfo("printResults", 22, 23));
            CHECK_NOTHROW(pkb.addProcInfo("first", 24, 25));
            CHECK_NOTHROW(pkb.addProcInfo("second", 26, 26));
            CHECK_NOTHROW(pkb.addProcInfo("affect", 27, 37));

            CHECK_NOTHROW(pkb.addFollowsRelation(1, 2));
            CHECK_NOTHROW(pkb.addFollowsRelation(2, 3));
            CHECK_NOTHROW(pkb.addFollowsRelation(3, 4));
            CHECK_NOTHROW(pkb.addFollowsRelation(5, 6));
            CHECK_NOTHROW(pkb.addFollowsRelation(8, 9));
            CHECK_NOTHROW(pkb.addFollowsRelation(9, 10));
            CHECK_NOTHROW(pkb.addFollowsRelation(11, 12));
            CHECK_NOTHROW(pkb.addFollowsRelation(12, 13));
            CHECK_NOTHROW(pkb.addFollowsRelation(10, 14));
            CHECK_NOTHROW(pkb.addFollowsRelation(4, 15));
            CHECK_NOTHROW(pkb.addFollowsRelation(17, 18));
            CHECK_NOTHROW(pkb.addFollowsRelation(15, 19));
            CHECK_NOTHROW(pkb.addFollowsRelation(19, 20));
            CHECK_NOTHROW(pkb.addFollowsRelation(20, 21));
            CHECK_NOTHROW(pkb.addFollowsRelation(22, 23));
            CHECK_NOTHROW(pkb.addFollowsRelation(27, 37));
            CHECK_NOTHROW(pkb.addFollowsRelation(28, 29));
            CHECK_NOTHROW(pkb.addFollowsRelation(29, 30));
            CHECK_NOTHROW(pkb.addFollowsRelation(31, 32));
            CHECK_NOTHROW(pkb.addFollowsRelation(33, 34));
            CHECK_NOTHROW(pkb.addFollowsRelation(34, 35));
            CHECK_NOTHROW(pkb.addFollowsRelation(35, 36));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 2));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 3));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 4));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(5, 6));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(8, 9));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(9, 10));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(11, 12));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(12, 13));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(10, 14));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(4, 15));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(17, 18));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(15, 19));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(19, 20));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(20, 21));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(22, 23));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 3));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 4));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 15));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 19));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 20));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 21));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 4));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 15));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 19));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 20));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 21));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 15));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 19));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 20));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 21));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(4, 19));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(4, 20));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(4, 21));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(15, 20));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(15, 21));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(8, 10));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(8, 14));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(9, 14));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(11, 13));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(27, 37));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(28, 29));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(28, 30));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(29, 30));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(33, 34));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(33, 35));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(33, 36));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(34, 35));
            CHECK_NOTHROW(pkb.addFollowsStarRelation(34, 36));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(35, 36));

            CHECK_NOTHROW(pkb.addFollowsStarRelation(31, 32));

            // Populate Parent/Parent* relationships
            CHECK_NOTHROW(pkb.addParentRelation(4, 5));
            CHECK_NOTHROW(pkb.addParentRelation(4, 6));

            CHECK_NOTHROW(pkb.addParentRelation(6, 7));
            CHECK_NOTHROW(pkb.addParentRelation(6, 8));
            CHECK_NOTHROW(pkb.addParentRelation(6, 9));
            CHECK_NOTHROW(pkb.addParentRelation(6, 10));
            CHECK_NOTHROW(pkb.addParentRelation(6, 14));

            CHECK_NOTHROW(pkb.addParentRelation(10, 11));
            CHECK_NOTHROW(pkb.addParentRelation(10, 12));
            CHECK_NOTHROW(pkb.addParentRelation(10, 13));

            CHECK_NOTHROW(pkb.addParentRelation(15, 16));
            CHECK_NOTHROW(pkb.addParentRelation(15, 17));
            CHECK_NOTHROW(pkb.addParentRelation(15, 18));

            CHECK_NOTHROW(pkb.addParentRelation(24, 25));

            CHECK_NOTHROW(pkb.addParentRelation(27, 28));
            CHECK_NOTHROW(pkb.addParentRelation(27, 29));
            CHECK_NOTHROW(pkb.addParentRelation(27, 30));

            CHECK_NOTHROW(pkb.addParentRelation(30, 31));
            CHECK_NOTHROW(pkb.addParentRelation(30, 32));

            CHECK_NOTHROW(pkb.addParentRelation(32, 33));
            CHECK_NOTHROW(pkb.addParentRelation(32, 34));
            CHECK_NOTHROW(pkb.addParentRelation(32, 35));
            CHECK_NOTHROW(pkb.addParentRelation(32, 36));

            CHECK_NOTHROW(pkb.addParentStarRelation(4, 5));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 6));

            CHECK_NOTHROW(pkb.addParentStarRelation(6, 7));
            CHECK_NOTHROW(pkb.addParentStarRelation(6, 8));
            CHECK_NOTHROW(pkb.addParentStarRelation(6, 9));
            CHECK_NOTHROW(pkb.addParentStarRelation(6, 10));
            CHECK_NOTHROW(pkb.addParentStarRelation(6, 14));

            CHECK_NOTHROW(pkb.addParentStarRelation(10, 11));
            CHECK_NOTHROW(pkb.addParentStarRelation(10, 12));
            CHECK_NOTHROW(pkb.addParentStarRelation(10, 13));

            CHECK_NOTHROW(pkb.addParentStarRelation(15, 16));
            CHECK_NOTHROW(pkb.addParentStarRelation(15, 17));
            CHECK_NOTHROW(pkb.addParentStarRelation(15, 18));

            CHECK_NOTHROW(pkb.addParentStarRelation(4, 7));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 8));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 9));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 10));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 14));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 11));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 12));
            CHECK_NOTHROW(pkb.addParentStarRelation(4, 13));

            CHECK_NOTHROW(pkb.addParentStarRelation(6, 11));
            CHECK_NOTHROW(pkb.addParentStarRelation(6, 12));
            CHECK_NOTHROW(pkb.addParentStarRelation(6, 13));

            CHECK_NOTHROW(pkb.addParentStarRelation(24, 25));

            CHECK_NOTHROW(pkb.addParentStarRelation(27, 28));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 29));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 30));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 31));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 32));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 33));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 34));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 35));
            CHECK_NOTHROW(pkb.addParentStarRelation(27, 36));

            CHECK_NOTHROW(pkb.addParentStarRelation(30, 31));
            CHECK_NOTHROW(pkb.addParentStarRelation(30, 32));
            CHECK_NOTHROW(pkb.addParentStarRelation(30, 33));
            CHECK_NOTHROW(pkb.addParentStarRelation(30, 34));
            CHECK_NOTHROW(pkb.addParentStarRelation(30, 35));
            CHECK_NOTHROW(pkb.addParentStarRelation(30, 36));

            CHECK_NOTHROW(pkb.addParentStarRelation(32, 33));
            CHECK_NOTHROW(pkb.addParentStarRelation(32, 34));
            CHECK_NOTHROW(pkb.addParentStarRelation(32, 35));
            CHECK_NOTHROW(pkb.addParentStarRelation(32, 36));

            // Populate Uses relationships
            CHECK_NOTHROW(
                pkb.addStmtUsesRelation(4, WHILE_STMT, std::unordered_set<std::string>{"x", "y", "cenX", "cenY", "count", "a"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(5, ASSIGN, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(6, IF_STMT, std::unordered_set<std::string>{"cenX", "cenY", "count", "a"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(7, PRINT, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(9, ASSIGN, std::unordered_set<std::string>{"a"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(10, WHILE_STMT, std::unordered_set<std::string>{"cenX", "cenY"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(13, ASSIGN, std::unordered_set<std::string>{"cenX"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(14, ASSIGN, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(15, IF_STMT, std::unordered_set<std::string>{"cenX", "cenY", "count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(6, IF_STMT, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(17, ASSIGN, std::unordered_set<std::string>{"cenX", "count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(18, ASSIGN, std::unordered_set<std::string>{"cenY", "count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(19, ASSIGN, std::unordered_set<std::string>{"cenX", "cenY"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(20, PRINT, std::unordered_set<std::string>{"normSq"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(22, PRINT, std::unordered_set<std::string>{"flag"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(23, PRINT, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(24, WHILE_STMT, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(26, ASSIGN, std::unordered_set<std::string>{"c", "d"}));
            CHECK_NOTHROW(
                pkb.addStmtUsesRelation(27, WHILE_STMT, std::unordered_set<std::string>{"a", "b", "c", "d", "e", "f", "normSq"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(28, ASSIGN, std::unordered_set<std::string>{"a"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(29, ASSIGN, std::unordered_set<std::string>{"b"}));
            CHECK_NOTHROW(
                pkb.addStmtUsesRelation(30, WHILE_STMT, std::unordered_set<std::string>{"a", "c", "d", "e", "f", "normSq"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(31, ASSIGN, std::unordered_set<std::string>{"c"}));
            CHECK_NOTHROW(
                pkb.addStmtUsesRelation(32, WHILE_STMT, std::unordered_set<std::string>{"a", "c", "d", "e", "f", "normSq"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(33, ASSIGN, std::unordered_set<std::string>{"normSq"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(34, ASSIGN, std::unordered_set<std::string>{"d"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(35, ASSIGN, std::unordered_set<std::string>{"e"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(36, ASSIGN, std::unordered_set<std::string>{"f"}));
            CHECK_NOTHROW(pkb.addStmtUsesRelation(37, ASSIGN, std::unordered_set<std::string>{"cenX"}));

            CHECK_NOTHROW(pkb.addProcUsesRelation(
                "main", std::unordered_set<std::string>{"x", "y", "cenX", "cenY", "count", "a", "normSq"}));
            CHECK_NOTHROW(pkb.addProcUsesRelation("printResults", std::unordered_set<std::string>{"flag", "count"}));
            CHECK_NOTHROW(pkb.addProcUsesRelation("second", std::unordered_set<std::string>{"c", "d"}));
            CHECK_NOTHROW(pkb.addProcUsesRelation("first", std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addProcUsesRelation(
                "affect", std::unordered_set<std::string>{"a", "c", "b", "normSq", "cenX", "d", "e", "f"}));

            // Populate Modifies relationships
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(1, ASSIGN, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(2, ASSIGN, std::unordered_set<std::string>{"cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(3, ASSIGN, std::unordered_set<std::string>{"cenY"}));
            CHECK_NOTHROW(
                pkb.addStmtModifiesRelation(4, WHILE_STMT, std::unordered_set<std::string>{"count", "a", "cenX", "cenY"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(5, ASSIGN, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(6, IF_STMT, std::unordered_set<std::string>{"count", "a", "cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(8, ASSIGN, std::unordered_set<std::string>{"a"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(9, ASSIGN, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(10, WHILE_STMT, std::unordered_set<std::string>{"cenX", "cenY"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(11, READ, std::unordered_set<std::string>{"cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(12, READ, std::unordered_set<std::string>{"cenY"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(13, ASSIGN, std::unordered_set<std::string>{"cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(14, ASSIGN, std::unordered_set<std::string>{"count"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(15, IF_STMT, std::unordered_set<std::string>{"flag", "cenX", "cenY"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(16, ASSIGN, std::unordered_set<std::string>{"flag"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(17, ASSIGN, std::unordered_set<std::string>{"cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(18, ASSIGN, std::unordered_set<std::string>{"cenY"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(19, ASSIGN, std::unordered_set<std::string>{"normSq"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(26, ASSIGN, std::unordered_set<std::string>{"x"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(
                27, WHILE_STMT, std::unordered_set<std::string>{"normSq", "a", "b", "c", "d", "e", "cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(28, ASSIGN, std::unordered_set<std::string>{"normSq"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(29, ASSIGN, std::unordered_set<std::string>{"a"}));
            CHECK_NOTHROW(
                pkb.addStmtModifiesRelation(30, WHILE_STMT, std::unordered_set<std::string>{"b", "c", "d", "e", "cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(31, ASSIGN, std::unordered_set<std::string>{"b"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(32, WHILE_STMT, std::unordered_set<std::string>{"c", "d", "e", "cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(33, ASSIGN, std::unordered_set<std::string>{"cenX"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(34, ASSIGN, std::unordered_set<std::string>{"c"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(35, ASSIGN, std::unordered_set<std::string>{"d"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(36, ASSIGN, std::unordered_set<std::string>{"e"}));
            CHECK_NOTHROW(pkb.addStmtModifiesRelation(37, ASSIGN, std::unordered_set<std::string>{"cenY"}));

            CHECK_NOTHROW(pkb.addProcModifiesRelation(
                "main", std::unordered_set<std::string>{"normSq", "flag", "cenX", "cenY", "count", "a"}));
            CHECK_NOTHROW(pkb.addProcModifiesRelation("second", std::unordered_set<std::string>{"x"}));
            CHECK_NOTHROW(pkb.addProcModifiesRelation(
                "affect", std::unordered_set<std::string>{"normSq", "a", "b", "c", "d", "e", "cenY", "cenX"}));

            // Populate General Storage with Statements
            CHECK_NOTHROW(pkb.storeStatement(1, ASSIGN, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{0}));
            CHECK_NOTHROW(pkb.storeStatement(2, ASSIGN, std::unordered_set<std::string>{"cenX"}, std::unordered_set<int>{0}));
            CHECK_NOTHROW(pkb.storeStatement(3, ASSIGN, std::unordered_set<std::string>{"cenY"}, std::unordered_set<int>{0}));
            CHECK_NOTHROW(
                pkb.storeStatement(4, WHILE_STMT, std::unordered_set<std::string>{"x", "y"}, std::unordered_set<int>{0}));
            CHECK_NOTHROW(pkb.storeStatement(5, ASSIGN, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{1}));
            CHECK_NOTHROW(
                pkb.storeStatement(6, IF_STMT, std::unordered_set<std::string>{"cenX", "cenY"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(7, PRINT, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(8, ASSIGN, std::unordered_set<std::string>{"a"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(9, ASSIGN, std::unordered_set<std::string>{"count", "a"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(10, WHILE_STMT, std::unordered_set<std::string>{"cenX", "cenY"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(11, READ, std::unordered_set<std::string>{"cenX"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(12, READ, std::unordered_set<std::string>{"cenY"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(13, ASSIGN, std::unordered_set<std::string>{"cenX"}, std::unordered_set<int>{1}));
            CHECK_NOTHROW(pkb.storeStatement(14, ASSIGN, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{1}));
            CHECK_NOTHROW(pkb.storeStatement(15, IF_STMT, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{0}));
            CHECK_NOTHROW(pkb.storeStatement(16, ASSIGN, std::unordered_set<std::string>{"flag"}, std::unordered_set<int>{1}));
            CHECK_NOTHROW(
                pkb.storeStatement(17, ASSIGN, std::unordered_set<std::string>{"cenX", "count"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(18, ASSIGN, std::unordered_set<std::string>{"cenY", "count"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(19, ASSIGN, std::unordered_set<std::string>{"normSq", "cenY", "cenX"},
                                             std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(20, PRINT, std::unordered_set<std::string>{"normSq"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(21, CALL, std::unordered_set<std::string>{}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(22, PRINT, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(23, PRINT, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(24, WHILE_STMT, std::unordered_set<std::string>{"count"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(25, CALL, std::unordered_set<std::string>{}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(26, ASSIGN, std::unordered_set<std::string>{"x", "c", "d"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(27, WHILE_STMT, std::unordered_set<std::string>{"a", "c"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(28, ASSIGN, std::unordered_set<std::string>{"a", "normSq"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(29, ASSIGN, std::unordered_set<std::string>{"a", "b"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(30, WHILE_STMT, std::unordered_set<std::string>{"a", "c"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(31, ASSIGN, std::unordered_set<std::string>{"c", "b"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(32, WHILE_STMT, std::unordered_set<std::string>{"a", "c"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(33, ASSIGN, std::unordered_set<std::string>{"cenX", "normSq"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(34, ASSIGN, std::unordered_set<std::string>{"c", "d"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(35, ASSIGN, std::unordered_set<std::string>{"d", "e"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(pkb.storeStatement(36, ASSIGN, std::unordered_set<std::string>{"e", "f"}, std::unordered_set<int>{}));
            CHECK_NOTHROW(
                pkb.storeStatement(37, ASSIGN, std::unordered_set<std::string>{"cenX", "cenY"}, std::unordered_set<int>{}));

            // Populate Pattern Storage with assignment statement patterns
            CHECK_NOTHROW(pkb.storeAssignPattern(1, "count", "0"));
            CHECK_NOTHROW(pkb.storeAssignPattern(2, "cenX", "0"));
            CHECK_NOTHROW(pkb.storeAssignPattern(3, "cenY", "0"));
            CHECK_NOTHROW(pkb.storeAssignPattern(5, "count", "count + 1"));
            CHECK_NOTHROW(pkb.storeAssignPattern(8, "a", "1"));
            CHECK_NOTHROW(pkb.storeAssignPattern(9, "count", "a"));
            CHECK_NOTHROW(pkb.storeAssignPattern(13, "cenX", "cenX + 1"));
            CHECK_NOTHROW(pkb.storeAssignPattern(14, "count", "count + 1"));
            CHECK_NOTHROW(pkb.storeAssignPattern(16, "flag", "1"));
            CHECK_NOTHROW(pkb.storeAssignPattern(17, "cenX", "cenX / count"));
            CHECK_NOTHROW(pkb.storeAssignPattern(18, "cenY", "cenY / count"));
            CHECK_NOTHROW(pkb.storeAssignPattern(19, "normSq", "((cenX * cenX) + cenY * (cenY))"));
            CHECK_NOTHROW(pkb.storeAssignPattern(26, "x", "c + d"));
            CHECK_NOTHROW(pkb.storeAssignPattern(28, "normSq", "a"));
            CHECK_NOTHROW(pkb.storeAssignPattern(29, "a", "b"));
            CHECK_NOTHROW(pkb.storeAssignPattern(31, "b", "c"));
            CHECK_NOTHROW(pkb.storeAssignPattern(33, "c", "normSq"));
            CHECK_NOTHROW(pkb.storeAssignPattern(34, "d", "d"));
            CHECK_NOTHROW(pkb.storeAssignPattern(35, "e", "e"));
            CHECK_NOTHROW(pkb.storeAssignPattern(36, "f", "f"));
            CHECK_NOTHROW(pkb.storeAssignPattern(37, "cenY", "cenX"));

            // Populate Patten Storage with conditional statement patterns
            CHECK_NOTHROW(pkb.addConditionPattern(4, std::unordered_set<std::string>{"x", "y"}, WHILE_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(6, std::unordered_set<std::string>{"cenX", "cenY"}, IF_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(10, std::unordered_set<std::string>{"cenX", "cenY"}, WHILE_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(15, std::unordered_set<std::string>{"count"}, IF_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(24, std::unordered_set<std::string>{"count"}, WHILE_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(27, std::unordered_set<std::string>{"a", "c"}, WHILE_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(30, std::unordered_set<std::string>{"a", "c"}, WHILE_STMT));
            CHECK_NOTHROW(pkb.addConditionPattern(32, std::unordered_set<std::string>{"a", "c"}, WHILE_STMT));

            // Populate Next Storage with adjacency list
            CHECK_NOTHROW(pkb.addNextAdjacencyList(mockAdjacencyList));

            // Populate Calls Storage with Calls relation
            CHECK_NOTHROW(pkb.addCallsRelation("main", "first", 21));
            CHECK_NOTHROW(pkb.addCallsRelation("first", "second", 25));

            CHECK_NOTHROW(pkb.startPopulateCallsStarInfo());
        }
    }
};

bool TestPKB::isInitialized = false;

/* Helper method to compare the custom unordered sets used in PKB e.g. IntPairResultSet.
 * Prints out the differences (missing and extra) between the actual and the expected set.
 * */
// template <typename T, typename S, typename U>
// void REQUIRE_SET_MATCH(std::unordered_set<T, S, U> actual, std::unordered_set<T, S, U> expected) {
//    if (actual != expected) {
//        std::cout << "======== Only in actual set ========" << std::endl;
//        for (auto &item : actual) {
//            if (expected.find(item) == expected.end()) {
//                std::cout << "[ " << item.first << ", " << item.second << " ]" << std::endl;
//            }
//        }
//        std::cout << "======== Only in expected set ========" << std::endl;
//        for (auto &item : expected) {
//            if (actual.find(item) == actual.end()) {
//                std::cout << "[ " << item.first << ", " << item.second << " ]" << std::endl;
//            }
//        }
//        REQUIRE(false);
//    }
//    REQUIRE(true);
//}

TEST_CASE_METHOD(TestPKB, "PKB: Test Follows Storage") {
    // Use case: Follows/Follows*(stmtNum, stmtNum)
    REQUIRE(pkb.isFollows(1, 2) == true);
    REQUIRE(pkb.isFollows(11, 12) == true);
    REQUIRE(pkb.isFollows(16, 17) == false);
    REQUIRE(pkb.isFollows(20, 2) == false);
    REQUIRE(pkb.isFollows(0, 1) == false);

    REQUIRE(pkb.isFollows(2, 4, true) == true);
    REQUIRE(pkb.isFollows(1, 19, true) == true);
    REQUIRE(pkb.isFollows(9, 12, true) == false);
    REQUIRE(pkb.isFollows(5, 19, true) == false);
    REQUIRE(pkb.isFollows(2, 1, true) == false);
    REQUIRE(pkb.isFollows(0, 2, true) == false);

    REQUIRE(pkb.isFollows(ASSIGN, 2) == true);
    REQUIRE(pkb.isFollows(PRINT, 23) == true);
    REQUIRE(pkb.isFollows(WHILE_STMT, 19, true) == true);
    REQUIRE(pkb.isFollows(ASSIGN, 22, true) == false);
    REQUIRE(pkb.isFollows(ASSIGN, 7, true) == false);

    REQUIRE(pkb.isFollows(2, ASSIGN) == true);
    REQUIRE(pkb.isFollows(22, PRINT) == true);
    REQUIRE(pkb.isFollows(1, WHILE_STMT, true) == true);
    REQUIRE(pkb.isFollows(4, READ, true) == false);
    REQUIRE(pkb.isFollows(19, ASSIGN, true) == false);

    REQUIRE(pkb.isFollows(WHILE_STMT, ASSIGN) == true);
    REQUIRE(pkb.isFollows(WHILE_STMT, ASSIGN, true) == true);
    REQUIRE(pkb.isFollows(WHILE_STMT, PRINT) == false);
    REQUIRE(pkb.isFollows(WHILE_STMT, PRINT, true) == true);
    REQUIRE(pkb.isFollows(WHILE_STMT, PRINT, false) == false);

    REQUIRE(pkb.getFollowingStatements(1) == std::unordered_set<int>{2});
    REQUIRE(pkb.getFollowingStatements(1, ASSIGN) == std::unordered_set<int>{2});
    REQUIRE(pkb.getFollowingStatements(1, STMT) == std::unordered_set<int>{2});
    REQUIRE(pkb.getFollowingStatements(1, READ).empty());
    REQUIRE(pkb.getFollowingStatementsT(23).empty());
    REQUIRE(pkb.getFollowingStatementsT(100).empty());

    REQUIRE(pkb.getFollowingStatementsT(1) == std::unordered_set<int>{2, 3, 4, 15, 19, 20, 21});
    REQUIRE(pkb.getFollowingStatementsT(1, WHILE_STMT) == std::unordered_set<int>{4});
    REQUIRE(pkb.getFollowingStatementsT(1, STMT) == std::unordered_set<int>{2, 3, 4, 15, 19, 20, 21});
    REQUIRE(pkb.getFollowingStatementsT(1, READ).empty());
    REQUIRE(pkb.getFollowingStatementsT(100).empty());
    REQUIRE(pkb.getFollowingStatementsT(100, READ).empty());

    REQUIRE(pkb.getFollowingStatements(WHILE_STMT) == std::unordered_set<int>{14, 15, 37});
    REQUIRE(pkb.getFollowingStatements(WHILE_STMT, ASSIGN) == std::unordered_set<int>{14, 37});
    REQUIRE(pkb.getFollowingStatements(PRINT) == std::unordered_set<int>{21, 23});
    REQUIRE(pkb.getFollowingStatements(PRINT, ASSIGN).empty());

    REQUIRE(pkb.getFollowingStatementsT(WHILE_STMT) == std::unordered_set<int>{14, 15, 19, 20, 21, 37});
    REQUIRE(pkb.getFollowingStatementsT(WHILE_STMT, ASSIGN) == std::unordered_set<int>{14, 19, 37});
    REQUIRE(pkb.getFollowingStatementsT(PRINT) == std::unordered_set<int>{21, 23});
    REQUIRE(pkb.getFollowingStatementsT(PRINT, ASSIGN).empty());

    REQUIRE(pkb.getLeadingStatements(15) == std::unordered_set<int>{4});
    REQUIRE(pkb.getLeadingStatements(15, WHILE_STMT) == std::unordered_set<int>{4});
    REQUIRE(pkb.getLeadingStatements(15, READ).empty());
    REQUIRE(pkb.getLeadingStatements(1).empty());
    REQUIRE(pkb.getLeadingStatements(100).empty());

    REQUIRE(pkb.getLeadingStatementsT(15) == std::unordered_set<int>{1, 2, 3, 4});
    REQUIRE(pkb.getLeadingStatementsT(15, WHILE_STMT) == std::unordered_set<int>{4});
    REQUIRE(pkb.getLeadingStatementsT(15, READ).empty());
    REQUIRE(pkb.getLeadingStatementsT(1).empty());
    REQUIRE(pkb.getLeadingStatementsT(100).empty());

    REQUIRE(pkb.getLeadingStatements(PRINT) == std::unordered_set<int>{19, 22});
    REQUIRE(pkb.getLeadingStatements(PRINT, ASSIGN) == std::unordered_set<int>{19});
    REQUIRE(pkb.getLeadingStatements(PRINT, WHILE_STMT).empty());

    REQUIRE(pkb.getLeadingStatementsT(PRINT) == std::unordered_set<int>{1, 2, 3, 4, 15, 19, 22});
    REQUIRE(pkb.getLeadingStatementsT(PRINT, ASSIGN) == std::unordered_set<int>{1, 2, 3, 19});
    REQUIRE(pkb.getLeadingStatementsT(READ, ASSIGN).empty());

    REQUIRE(pkb.getAllFollowsRelationPairings(STMT, STMT) ==
            IntPairResultSet{std::make_pair(1, 2),   std::make_pair(2, 3),   std::make_pair(3, 4),   std::make_pair(4, 15),
                             std::make_pair(5, 6),   std::make_pair(8, 9),   std::make_pair(9, 10),  std::make_pair(10, 14),
                             std::make_pair(11, 12), std::make_pair(12, 13), std::make_pair(15, 19), std::make_pair(17, 18),
                             std::make_pair(19, 20), std::make_pair(20, 21), std::make_pair(22, 23), std::make_pair(27, 37),
                             std::make_pair(28, 29), std::make_pair(29, 30), std::make_pair(31, 32), std::make_pair(33, 34),
                             std::make_pair(34, 35), std::make_pair(35, 36)});
    REQUIRE(pkb.getAllFollowsRelationPairings(PRINT, PRINT) == IntPairResultSet{std::make_pair(22, 23)});
    REQUIRE(pkb.getAllFollowsRelationPairings(CALL, PRINT).empty());

    REQUIRE(pkb.getAllFollowsRelationPairingsT(ASSIGN, ASSIGN) ==
            IntPairResultSet{std::make_pair(1, 2), std::make_pair(1, 3), std::make_pair(1, 19), std::make_pair(2, 3),
                             std::make_pair(2, 19), std::make_pair(3, 19), std::make_pair(8, 9), std::make_pair(8, 14),
                             std::make_pair(9, 14), std::make_pair(17, 18), std::make_pair(28, 29), std::make_pair(33, 34),
                             std::make_pair(33, 35), std::make_pair(33, 36), std::make_pair(34, 35), std::make_pair(34, 36),
                             std::make_pair(35, 36)});
    REQUIRE(pkb.getAllFollowsRelationPairingsT(PRINT, PRINT) == IntPairResultSet{std::make_pair(22, 23)});
    REQUIRE(pkb.getAllFollowsRelationPairingsT(CALL, PRINT).empty());
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Parent Storage") {
    REQUIRE(pkb.isParent(4, 5) == true);
    REQUIRE(pkb.isParent(4, 6) == true);
    REQUIRE(pkb.isParent(4, 11) == false);
    REQUIRE(pkb.isParent(5, 4) == false);
    REQUIRE(pkb.isParent(100, 4) == false);

    REQUIRE(pkb.isParent(4, 5, true) == true);
    REQUIRE(pkb.isParent(4, 11, true) == true);
    REQUIRE(pkb.isParent(6, 12, true) == true);
    REQUIRE(pkb.isParent(11, 4, true) == false);
    REQUIRE(pkb.isParent(4, 20, true) == false);
    REQUIRE(pkb.isParent(100, 4, true) == false);

    REQUIRE(pkb.isParent(4, IF_STMT) == true);
    REQUIRE(pkb.isParent(4, READ) == false);
    REQUIRE(pkb.isParent(100, ASSIGN) == false);
    REQUIRE(pkb.isParent(2, READ) == false);

    REQUIRE(pkb.isParent(4, IF_STMT, true) == true);
    REQUIRE(pkb.isParent(4, READ, true) == true);
    REQUIRE(pkb.isParent(100, ASSIGN, true) == false);
    REQUIRE(pkb.isParent(2, READ, true) == false);

    REQUIRE(pkb.isParent(WHILE_STMT, 11) == true);
    REQUIRE(pkb.isParent(IF_STMT, 18) == true);
    REQUIRE(pkb.isParent(ASSIGN, 4) == false);
    REQUIRE(pkb.isParent(IF_STMT, 100) == false);

    REQUIRE(pkb.isParent(WHILE_STMT, 11, true) == true);
    REQUIRE(pkb.isParent(IF_STMT, 13, true) == true);
    REQUIRE(pkb.isParent(ASSIGN, 4, true) == false);
    REQUIRE(pkb.isParent(IF_STMT, 100, true) == false);

    REQUIRE(pkb.isParent(WHILE_STMT, ASSIGN) == true);
    REQUIRE(pkb.isParent(IF_STMT, ASSIGN) == true);
    REQUIRE(pkb.isParent(IF_STMT, READ) == false);
    REQUIRE(pkb.isParent(ASSIGN, ASSIGN) == false);
    REQUIRE(pkb.isParent(READ, IF_STMT) == false);

    REQUIRE(pkb.isParent(WHILE_STMT, ASSIGN, true) == true);
    REQUIRE(pkb.isParent(IF_STMT, ASSIGN, true) == true);
    REQUIRE(pkb.isParent(IF_STMT, READ, true) == true);
    REQUIRE(pkb.isParent(ASSIGN, ASSIGN, true) == false);
    REQUIRE(pkb.isParent(READ, IF_STMT, true) == false);

    REQUIRE(pkb.getParentStatements(13) == std::unordered_set<int>{10});
    REQUIRE(pkb.getParentStatements(13, WHILE_STMT) == std::unordered_set<int>{10});
    REQUIRE(pkb.getParentStatements(13, IF_STMT).empty());
    REQUIRE(pkb.getParentStatements(1).empty());
    REQUIRE(pkb.getParentStatements(100).empty());

    REQUIRE(pkb.getParentStatementsT(13) == std::unordered_set<int>{4, 6, 10});
    REQUIRE(pkb.getParentStatementsT(13, WHILE_STMT) == std::unordered_set<int>{4, 10});
    REQUIRE(pkb.getParentStatementsT(13, IF_STMT) == std::unordered_set<int>{6});
    REQUIRE(pkb.getParentStatementsT(13, ASSIGN).empty());
    REQUIRE(pkb.getParentStatementsT(1).empty());
    REQUIRE(pkb.getParentStatementsT(100).empty());

    REQUIRE(pkb.getParentStatements(PRINT) == std::unordered_set<int>{6});
    REQUIRE(pkb.getParentStatements(READ) == std::unordered_set<int>{10});
    REQUIRE(pkb.getParentStatements(READ, IF_STMT).empty());
    REQUIRE(pkb.getParentStatements(READ, ASSIGN).empty());

    REQUIRE(pkb.getParentStatementsT(READ) == std::unordered_set<int>{4, 6, 10});
    REQUIRE(pkb.getParentStatementsT(READ, WHILE_STMT) == std::unordered_set<int>{4, 10});
    REQUIRE(pkb.getParentStatementsT(WHILE_STMT, WHILE_STMT) == std::unordered_set<int>{4, 27, 30});
    REQUIRE(pkb.getParentStatementsT(WHILE_STMT, ASSIGN).empty());
    REQUIRE(pkb.getParentStatementsT(READ, ASSIGN).empty());

    REQUIRE(pkb.getChildrenStatements(4) == std::unordered_set<int>{5, 6});
    REQUIRE(pkb.getChildrenStatements(4, IF_STMT) == std::unordered_set<int>{6});
    REQUIRE(pkb.getChildrenStatements(4, PRINT).empty());
    REQUIRE(pkb.getChildrenStatements(1).empty());
    REQUIRE(pkb.getChildrenStatements(100).empty());

    REQUIRE(pkb.getChildrenStatementsT(4) == std::unordered_set<int>{5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
    REQUIRE(pkb.getChildrenStatementsT(4, ASSIGN) == std::unordered_set<int>{5, 8, 9, 13, 14});
    REQUIRE(pkb.getChildrenStatementsT(4, PRINT) == std::unordered_set<int>{7});
    REQUIRE(pkb.getChildrenStatementsT(1).empty());
    REQUIRE(pkb.getChildrenStatementsT(100).empty());

    REQUIRE(pkb.getChildrenStatements(IF_STMT) == std::unordered_set<int>{7, 8, 9, 10, 14, 16, 17, 18});
    REQUIRE(pkb.getChildrenStatements(IF_STMT, PRINT) == std::unordered_set<int>{7});
    REQUIRE(pkb.getChildrenStatements(IF_STMT, READ).empty());
    REQUIRE(pkb.getChildrenStatements(READ, WHILE_STMT).empty());

    REQUIRE(pkb.getChildrenStatementsT(IF_STMT) == std::unordered_set<int>{7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18});
    REQUIRE(pkb.getChildrenStatementsT(IF_STMT, PRINT) == std::unordered_set<int>{7});
    REQUIRE(pkb.getChildrenStatementsT(IF_STMT, READ) == std::unordered_set<int>{11, 12});
    REQUIRE(pkb.getChildrenStatementsT(READ, WHILE_STMT).empty());

    REQUIRE(pkb.getAllParentRelationPairings(STMT, STMT) ==
            IntPairResultSet{std::make_pair(4, 5),   std::make_pair(4, 6),   std::make_pair(6, 7),   std::make_pair(6, 8),
                             std::make_pair(6, 9),   std::make_pair(6, 10),  std::make_pair(6, 14),  std::make_pair(10, 11),
                             std::make_pair(10, 12), std::make_pair(10, 13), std::make_pair(15, 16), std::make_pair(15, 17),
                             std::make_pair(15, 18), std::make_pair(24, 25), std::make_pair(27, 28), std::make_pair(27, 29),
                             std::make_pair(27, 30), std::make_pair(30, 31), std::make_pair(30, 32), std::make_pair(32, 33),
                             std::make_pair(32, 34), std::make_pair(32, 35), std::make_pair(32, 36)});
    REQUIRE(pkb.getAllParentRelationPairings(IF_STMT, ASSIGN) ==
            IntPairResultSet{std::make_pair(6, 8), std::make_pair(6, 9), std::make_pair(6, 14), std::make_pair(15, 16),
                             std::make_pair(15, 17), std::make_pair(15, 18)});
    REQUIRE(pkb.getAllParentRelationPairings(READ, STMT).empty());

    REQUIRE(pkb.getAllParentRelationPairingsT(WHILE_STMT, ASSIGN) ==
            IntPairResultSet{std::make_pair(4, 5),   std::make_pair(4, 8),   std::make_pair(4, 9),   std::make_pair(4, 13),
                             std::make_pair(4, 14),  std::make_pair(10, 13), std::make_pair(27, 28), std::make_pair(27, 29),
                             std::make_pair(27, 31), std::make_pair(27, 33), std::make_pair(27, 34), std::make_pair(27, 35),
                             std::make_pair(27, 36), std::make_pair(30, 31), std::make_pair(30, 33), std::make_pair(30, 34),
                             std::make_pair(30, 35), std::make_pair(30, 36), std::make_pair(32, 33), std::make_pair(32, 34),
                             std::make_pair(32, 35), std::make_pair(32, 36)});
    REQUIRE(pkb.getAllParentRelationPairingsT(WHILE_STMT, WHILE_STMT) ==
            IntPairResultSet{std::make_pair(4, 10), std::make_pair(27, 30), std::make_pair(27, 32), std::make_pair(30, 32)});
    REQUIRE(pkb.getAllParentRelationPairingsT(CALL, PRINT).empty());
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Modifies Storage") {
    REQUIRE(pkb.isModifies(19, "normSq") == true);
    REQUIRE(pkb.isModifies(1, "count") == true);
    REQUIRE(pkb.isModifies(10, "cenX") == true);
    REQUIRE(pkb.isModifies(4, "cenX") == true);
    REQUIRE(pkb.isModifies(10, "cenY") == true);
    REQUIRE(pkb.isModifies(19, "cenY") == false);
    REQUIRE(pkb.isModifies(100, "cenY") == false);
    REQUIRE(pkb.isModifies(21, "flag") == false);

    REQUIRE(pkb.isModifies("main", "cenX") == true);
    REQUIRE(pkb.isModifies("main", "flag") == true);
    REQUIRE(pkb.isModifies("printResults", "flag") == false);
    REQUIRE(pkb.isModifies("randomProc", "cenY") == false);

    REQUIRE(pkb.getModifiesVariables(19) == std::unordered_set<std::string>{"normSq"});
    REQUIRE(pkb.getModifiesVariables(4) == std::unordered_set<std::string>{"count", "a", "cenX", "cenY"});
    REQUIRE(pkb.getModifiesVariables(7).empty());
    REQUIRE(pkb.getModifiesVariables(100).empty());

    REQUIRE(pkb.getModifiesVariables(WHILE_STMT) ==
            std::unordered_set<std::string>{"count", "a", "cenX", "cenY", "x", "b", "c", "d", "e", "normSq"});
    REQUIRE(pkb.getModifiesVariables(IF_STMT) == std::unordered_set<std::string>{"count", "a", "cenX", "cenY", "flag"});
    REQUIRE(pkb.getModifiesVariables(PRINT).empty());
    REQUIRE(pkb.getModifiesVariables(100).empty());

    REQUIRE(pkb.getAllModifiesStatement(STMT) == std::unordered_set<int>{1,  2,  3,  4,  5,  6,  8,  9,  10, 11, 12,
                                                                         13, 14, 15, 16, 17, 18, 19, 21, 24, 25, 26,
                                                                         27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37});
    REQUIRE(pkb.getAllModifiesStatement(IF_STMT) == std::unordered_set<int>{6, 15});
    REQUIRE(pkb.getAllModifiesStatement(PRINT).empty());

    REQUIRE(pkb.getModifiesStatement(STMT, "count") == std::unordered_set<int>{1, 4, 5, 6, 9, 14});
    REQUIRE(pkb.getModifiesStatement(IF_STMT, "flag") == std::unordered_set<int>{15});
    REQUIRE(pkb.getModifiesStatement(PRINT, "count").empty());
    REQUIRE(pkb.getModifiesStatement(ASSIGN, "random").empty());

    REQUIRE(pkb.getModifiesProc("cenX") == std::unordered_set<std::string>{"main", "affect"});
    REQUIRE(pkb.getModifiesProc("unknown").empty());

    REQUIRE(pkb.getModifiesProc() == std::unordered_set<std::string>{"main", "first", "second", "affect"});

    REQUIRE(pkb.getModifiesVariableFromProc("printResult").empty());
    REQUIRE(pkb.getModifiesVariableFromProc("main") ==
            std::unordered_set<std::string>{"normSq", "count", "a", "cenX", "cenY", "flag", "x"});

    REQUIRE(pkb.getAllStmtModifiesVarRelationPair(ASSIGN) ==
            IntStringPairResultSet{std::make_pair(1, "count"), std::make_pair(2, "cenX"),    std::make_pair(3, "cenY"),
                                   std::make_pair(5, "count"), std::make_pair(8, "a"),       std::make_pair(9, "count"),
                                   std::make_pair(13, "cenX"), std::make_pair(14, "count"),  std::make_pair(16, "flag"),
                                   std::make_pair(17, "cenX"), std::make_pair(18, "cenY"),   std::make_pair(19, "normSq"),
                                   std::make_pair(26, "x"),    std::make_pair(28, "normSq"), std::make_pair(29, "a"),
                                   std::make_pair(31, "b"),    std::make_pair(33, "cenX"),   std::make_pair(34, "c"),
                                   std::make_pair(35, "d"),    std::make_pair(36, "e"),      std::make_pair(37, "cenY")});
    REQUIRE(pkb.getAllStmtModifiesVarRelationPair(CALL) ==
            IntStringPairResultSet{std::make_pair(21, "x"), std::make_pair(25, "x")});
    REQUIRE(pkb.getAllStmtModifiesVarRelationPair(PRINT).empty());

    REQUIRE(pkb.getAllProcModifiesVarRelationPair() ==
            StringPairResultSet{std::make_pair("main", "count"), std::make_pair("main", "cenX"), std::make_pair("main", "cenY"),
                                std::make_pair("main", "a"), std::make_pair("main", "flag"), std::make_pair("main", "normSq"),
                                std::make_pair("main", "cenX"), std::make_pair("main", "x"), std::make_pair("first", "x"),
                                std::make_pair("second", "x"), std::make_pair("affect", "cenY"), std::make_pair("affect", "a"),
                                std::make_pair("affect", "normSq"), std::make_pair("affect", "b"),
                                std::make_pair("affect", "cenX"), std::make_pair("affect", "c"), std::make_pair("affect", "d"),
                                std::make_pair("affect", "e")});
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Uses Storage") {
    REQUIRE(pkb.isUses(22, "flag") == true);
    REQUIRE(pkb.isUses(10, "cenX") == true);
    REQUIRE(pkb.isUses(4, "cenX") == true);
    REQUIRE(pkb.isUses(11, "cenX") == false);
    REQUIRE(pkb.isUses(100, "cenY") == false);
    REQUIRE(pkb.isUses(4, "flag") == false);

    REQUIRE(pkb.isUses("main", "cenX") == true);
    REQUIRE(pkb.isUses("main", "a") == true);
    REQUIRE(pkb.isUses("printResults", "flag") == true);
    REQUIRE(pkb.isUses("main", "flag") == false);
    REQUIRE(pkb.isUses("printResults", "cenX") == false);
    REQUIRE(pkb.isUses("randomProc", "cenY") == false);

    REQUIRE(pkb.getUsesVariables(19) == std::unordered_set<std::string>{"cenX", "cenY"});
    REQUIRE(pkb.getUsesVariables(4) == std::unordered_set<std::string>{"count", "a", "cenX", "cenY", "x", "y"});
    REQUIRE(pkb.getUsesVariables(16).empty());
    REQUIRE(pkb.getUsesVariables(100).empty());

    REQUIRE(pkb.getUsesVariables(WHILE_STMT) ==
            std::unordered_set<std::string>{"count", "a", "cenX", "cenY", "x", "y", "c", "d", "b", "d", "e", "f", "normSq"});
    REQUIRE(pkb.getUsesVariables(IF_STMT) == std::unordered_set<std::string>{"count", "a", "cenX", "cenY"});
    REQUIRE(pkb.getUsesVariables(READ).empty());
    REQUIRE(pkb.getUsesVariables(100).empty());

    REQUIRE(pkb.getAllUsesStatement(STMT) == std::unordered_set<int>{4,  5,  6,  7,  9,  10, 13, 14, 15, 17, 18, 19, 20, 21, 22,
                                                                     23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37});
    REQUIRE(pkb.getAllUsesStatement(IF_STMT) == std::unordered_set<int>{6, 15});
    REQUIRE(pkb.getAllUsesStatement(READ).empty());

    REQUIRE(pkb.getUsesStatement(STMT, "count") == std::unordered_set<int>{4, 5, 6, 7, 14, 15, 17, 18, 21, 23, 24});
    REQUIRE(pkb.getUsesStatement(ASSIGN, "a") == std::unordered_set<int>{9, 28});
    REQUIRE(pkb.getUsesStatement(READ, "count").empty());
    REQUIRE(pkb.getUsesStatement(ASSIGN, "random").empty());

    REQUIRE(pkb.getUsesProc("a") == std::unordered_set<std::string>{"main", "affect"});
    REQUIRE(pkb.getUsesProc("unknown").empty());

    REQUIRE(pkb.getUsesProc() == std::unordered_set<std::string>{"main", "printResults", "first", "second", "affect"});

    REQUIRE(pkb.getUsesVariableFromProc("printResults") == std::unordered_set<std::string>{"count", "flag"});
    REQUIRE(pkb.getUsesVariableFromProc("main") ==
            std::unordered_set<std::string>{"count", "a", "cenX", "cenY", "x", "y", "normSq", "c", "d"});
    REQUIRE(pkb.getUsesVariableFromProc("unknown").empty());

    REQUIRE(pkb.getAllStmtUsesVarRelationPair(ASSIGN) ==
            IntStringPairResultSet{
                std::make_pair(5, "count"), std::make_pair(9, "a"),      std::make_pair(13, "cenX"), std::make_pair(14, "count"),
                std::make_pair(17, "cenX"), std::make_pair(17, "count"), std::make_pair(18, "cenY"), std::make_pair(18, "count"),
                std::make_pair(19, "cenX"), std::make_pair(19, "cenY"),  std::make_pair(26, "c"),    std::make_pair(26, "d"),
                std::make_pair(28, "a"),    std::make_pair(29, "b"),     std::make_pair(31, "c"),    std::make_pair(33, "normSq"),
                std::make_pair(34, "d"),    std::make_pair(35, "e"),     std::make_pair(36, "f"),    std::make_pair(37, "cenX")});
    REQUIRE(pkb.getAllStmtUsesVarRelationPair(CALL) == IntStringPairResultSet{std::make_pair(21, "count"),
                                                                              std::make_pair(21, "c"), std::make_pair(21, "d"),
                                                                              std::make_pair(25, "c"), std::make_pair(25, "d")});
    REQUIRE(pkb.getAllStmtUsesVarRelationPair(READ).empty());

    REQUIRE(pkb.getAllProcUsesVarRelationPair() == StringPairResultSet{std::make_pair("main", "count"),
                                                                       std::make_pair("main", "a"),
                                                                       std::make_pair("main", "cenX"),
                                                                       std::make_pair("main", "cenY"),
                                                                       std::make_pair("main", "c"),
                                                                       std::make_pair("main", "d"),
                                                                       std::make_pair("main", "x"),
                                                                       std::make_pair("main", "y"),
                                                                       std::make_pair("main", "normSq"),
                                                                       std::make_pair("printResults", "count"),
                                                                       std::make_pair("printResults", "flag"),
                                                                       std::make_pair("first", "count"),
                                                                       std::make_pair("first", "c"),
                                                                       std::make_pair("first", "d"),
                                                                       std::make_pair("second", "c"),
                                                                       std::make_pair("second", "d"),
                                                                       std::make_pair("affect", "a"),
                                                                       std::make_pair("affect", "b"),
                                                                       std::make_pair("affect", "c"),
                                                                       std::make_pair("affect", "d"),
                                                                       std::make_pair("affect", "e"),
                                                                       std::make_pair("affect", "f"),
                                                                       std::make_pair("affect", "cenX"),
                                                                       std::make_pair("affect", "normSq")});
}

TEST_CASE_METHOD(TestPKB, "PKB: Test General Storage") {
    REQUIRE(pkb.getAllVar() ==
            std::unordered_set<std::string>{"count", "a", "cenX", "cenY", "x", "y", "normSq", "flag", "b", "c", "d", "e", "f"});
    REQUIRE(pkb.getAllProc() == std::unordered_set<std::string>{"main", "printResults", "first", "second", "affect"});

    REQUIRE(pkb.getAllStatements() == std::unordered_set<int>{1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                                                              14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                                                              27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37});
    REQUIRE(pkb.getAllStatements(IF_STMT) == std::unordered_set<int>{6, 15});
    REQUIRE(pkb.getAllStatements(ASSIGN) ==
            std::unordered_set<int>{1, 2, 3, 5, 8, 9, 13, 14, 16, 17, 18, 19, 26, 28, 29, 31, 33, 34, 35, 36, 37});

    REQUIRE(pkb.getAllVarsInStatement(std::unordered_set<int>{19}) == std::unordered_set<std::string>{"cenX", "cenY", "normSq"});
    REQUIRE(pkb.getAllVarsInStatement(std::unordered_set<int>{1, 2, 3, 16}) ==
            std::unordered_set<std::string>{"count", "cenX", "cenY", "flag"});
    REQUIRE(pkb.getAllVarsInStatement(std::unordered_set<int>{100}).empty());
    REQUIRE(pkb.getStatmentNumberFromVars(std::unordered_set<std::string>{"cenX"}) ==
            std::unordered_set<int>{2, 6, 10, 11, 13, 17, 19, 33, 37});
    REQUIRE(pkb.getStatmentNumberFromVars(std::unordered_set<std::string>{"x", "a"}) ==
            std::unordered_set<int>{4, 8, 9, 26, 29, 28, 27, 30, 32});
    REQUIRE(pkb.getStatmentNumberFromVars(std::unordered_set<std::string>{"unknown"}).empty());

    REQUIRE(pkb.getAllConstants() == std::unordered_set<int>{0, 1});

    REQUIRE(pkb.isValidStatementNum(5, ASSIGN));
    REQUIRE(pkb.isValidStatementNum(21, CALL));
    REQUIRE(pkb.isValidStatementNum(22, PRINT));
    REQUIRE(pkb.isValidStatementNum(5, STMT));
    REQUIRE_FALSE(pkb.isValidStatementNum(10, IF_STMT));
    REQUIRE_FALSE(pkb.isValidStatementNum(24, PRINT));
    REQUIRE_FALSE(pkb.isValidStatementNum(0, ASSIGN));
    REQUIRE_FALSE(pkb.isValidStatementNum(100, READ));
    REQUIRE_FALSE(pkb.isValidStatementNum(100, STMT));
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Pattern Storage") {
    REQUIRE(pkb.getStatementsWithPattern("cenX", "cenX + 1", true, true) == std::unordered_set<int>{13});
    REQUIRE(pkb.getStatementsWithPattern("cenX", "cenX + 1", false, false) == std::unordered_set<int>{13});
    REQUIRE(pkb.getStatementsWithPattern("cenX * cenX", false, false) == std::unordered_set<int>{19});
    REQUIRE(pkb.getStatementsWithPattern("cenX * cenX", true, true).empty());
    REQUIRE(pkb.getStatementsWithPattern("cenX * cenX + 1", false, false).empty());
    REQUIRE(pkb.getStatementsWithPattern("cenY * cenY", false, false) == std::unordered_set<int>{19});
    REQUIRE(pkb.getStatementsWithPattern("cenY * cenY", true, false).empty());
    REQUIRE(pkb.getStatementsWithPattern("cenY * cenY", false, false) == std::unordered_set<int>{19});
    REQUIRE(pkb.getStatementsWithPattern("CENY * CENY", false, false).empty());
    CHECK_THROWS(pkb.getStatementsWithPattern("cenY ? cenY", false, false));
    CHECK_THROWS(pkb.getStatementsWithPattern("a & * ( !", false, false));

    REQUIRE(pkb.getConditionStatementsWithAnyVar(WHILE_STMT) == std::unordered_set<int>{4, 10, 24, 27, 30, 32});
    REQUIRE(pkb.getConditionStatementsWithAnyVar(IF_STMT) == std::unordered_set<int>{6, 15});
    REQUIRE(pkb.getConditionStatementsWithAnyVar(ASSIGN).empty());

    REQUIRE(pkb.getVarMatchConditionStatements("count", IF_STMT) == std::unordered_set<int>{15});
    REQUIRE(pkb.getVarMatchConditionStatements("count", WHILE_STMT) == std::unordered_set<int>{24});
    REQUIRE(pkb.getVarMatchConditionStatements("unknown", WHILE_STMT).empty());
    REQUIRE(pkb.getVarMatchConditionStatements("unknown", ASSIGN).empty());

    REQUIRE(pkb.getVarMatchConditionStatementPairs(IF_STMT) ==
            IntStringPairResultSet{std::make_pair(6, "cenX"), std::make_pair(6, "cenY"), std::make_pair(15, "count")});
    REQUIRE(pkb.getVarMatchConditionStatementPairs(WHILE_STMT) ==
            IntStringPairResultSet{std::make_pair(4, "x"), std::make_pair(4, "y"), std::make_pair(10, "cenY"),
                                   std::make_pair(10, "cenX"), std::make_pair(24, "count"), std::make_pair(27, "a"),
                                   std::make_pair(27, "c"), std::make_pair(30, "a"), std::make_pair(30, "c"),
                                   std::make_pair(32, "a"), std::make_pair(32, "c")});
    REQUIRE(pkb.getVarMatchConditionStatementPairs(ASSIGN).empty());
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Next Storage") {
    // Test Next relation for Next(stmt#, _)
    REQUIRE(pkb.hasNext(7));
    REQUIRE(pkb.hasNext(22));
    REQUIRE(pkb.hasNext(10));
    REQUIRE_FALSE(pkb.hasNext(21));
    REQUIRE_FALSE(pkb.hasNext(23));
    REQUIRE_FALSE(pkb.hasNext(0));
    REQUIRE_FALSE(pkb.hasNext(10000));

    // Test Next relation for Next(syn, _)
    REQUIRE(pkb.hasNext(ASSIGN));
    REQUIRE(pkb.hasNext(WHILE_STMT));
    REQUIRE(pkb.hasNext(STMT));
    REQUIRE(pkb.hasNext(PRINT));
    REQUIRE(pkb.hasNext(IF_STMT));

    // Test Next relation for Next(_, stmt#)
    REQUIRE(pkb.hasPrevious(15));
    REQUIRE(pkb.hasPrevious(20));
    REQUIRE(pkb.hasPrevious(23));
    REQUIRE_FALSE(pkb.hasPrevious(22));
    REQUIRE_FALSE(pkb.hasPrevious(1));
    REQUIRE_FALSE(pkb.hasPrevious(0));
    REQUIRE_FALSE(pkb.hasPrevious(10000));

    // Test Next relation for Next(_, syn)
    REQUIRE(pkb.hasPrevious(ASSIGN));
    REQUIRE(pkb.hasPrevious(WHILE_STMT));
    REQUIRE(pkb.hasPrevious(STMT));
    REQUIRE(pkb.hasPrevious(PRINT));
    REQUIRE(pkb.hasPrevious(IF_STMT));

    // Test Next relation for Next(stmt#1, stmt#2)
    REQUIRE(pkb.isNext(1, 2));
    REQUIRE(pkb.isNext(22, 23));
    REQUIRE(pkb.isNext(4, 15));
    REQUIRE(pkb.isNext(13, 10));
    REQUIRE(pkb.isNext(7, 4));
    REQUIRE_FALSE(pkb.isNext(1, 22));
    REQUIRE_FALSE(pkb.isNext(4, 16));
    REQUIRE_FALSE(pkb.isNext(13, 14));
    REQUIRE_FALSE(pkb.isNext(14, 15));
    REQUIRE_FALSE(pkb.isNext(0, 1000));

    // Test Next* relation for Next*(stmt#1, stmt#2)
    REQUIRE(pkb.isNext(1, 18, true));
    REQUIRE(pkb.isNext(22, 23, true));
    REQUIRE(pkb.isNext(12, 5, true));
    REQUIRE(pkb.isNext(13, 10, true));
    REQUIRE(pkb.isNext(7, 4, true));
    REQUIRE(pkb.isNext(6, 4, true));
    REQUIRE_FALSE(pkb.isNext(1, 22, true));
    REQUIRE_FALSE(pkb.isNext(16, 17, true));
    REQUIRE_FALSE(pkb.isNext(6, 3, true));
    REQUIRE_FALSE(pkb.isNext(22, 21, true));
    REQUIRE_FALSE(pkb.isNext(0, 1000, true));

    // Test Next relation for syn such that Next(stmt#, syn)
    REQUIRE(pkb.getNextStatements(6) == std::unordered_set<int>{7, 8});
    REQUIRE(pkb.getNextStatements(6, PRINT) == std::unordered_set<int>{7});
    REQUIRE(pkb.getNextStatements(15) == std::unordered_set<int>{16, 17});
    REQUIRE(pkb.getNextStatements(15, STMT) == std::unordered_set<int>{16, 17});
    REQUIRE(pkb.getNextStatements(21).empty());
    REQUIRE(pkb.getNextStatements(23).empty());
    REQUIRE(pkb.getNextStatements(0).empty());
    REQUIRE(pkb.getNextStatements(1000).empty());

    // Test Next relation for syn such that Next*(stmt#, syn)
    REQUIRE(pkb.getNextStatementsT(1) ==
            std::unordered_set<int>{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21});
    REQUIRE(pkb.getNextStatementsT(1, PRINT) == std::unordered_set<int>{7, 20});
    REQUIRE(pkb.getNextStatementsT(16) == std::unordered_set<int>{19, 20, 21});
    REQUIRE(pkb.getNextStatementsT(16, STMT) == std::unordered_set<int>{19, 20, 21});
    REQUIRE(pkb.getNextStatementsT(21).empty());
    REQUIRE(pkb.getNextStatementsT(23).empty());
    REQUIRE(pkb.getNextStatementsT(0).empty());
    REQUIRE(pkb.getNextStatementsT(1000).empty());

    // Test Next relation for syn such that Next(syn, stmt#)
    REQUIRE(pkb.getPreviousStatements(10) == std::unordered_set<int>{9, 13});
    REQUIRE(pkb.getPreviousStatements(10, ASSIGN) == std::unordered_set<int>{9, 13});
    REQUIRE(pkb.getPreviousStatements(19) == std::unordered_set<int>{16, 18});
    REQUIRE(pkb.getPreviousStatements(19, STMT) == std::unordered_set<int>{16, 18});
    REQUIRE(pkb.getPreviousStatements(1).empty());
    REQUIRE(pkb.getPreviousStatements(22).empty());
    REQUIRE(pkb.getPreviousStatements(0).empty());
    REQUIRE(pkb.getPreviousStatements(1000).empty());

    // Test Next relation for syn such that Next*(syn, stmt#)
    REQUIRE(pkb.getPreviousStatementsT(20) ==
            std::unordered_set<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    REQUIRE(pkb.getPreviousStatementsT(20, READ) == std::unordered_set<int>{11, 12});
    REQUIRE(pkb.getPreviousStatementsT(13) == std::unordered_set<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
    REQUIRE(pkb.getPreviousStatementsT(18, PRINT) == std::unordered_set<int>{7});
    REQUIRE(pkb.getPreviousStatementsT(1).empty());
    REQUIRE(pkb.getPreviousStatementsT(22).empty());
    REQUIRE(pkb.getPreviousStatementsT(0).empty());
    REQUIRE(pkb.getPreviousStatementsT(1000).empty());

    // Test Next relation for syn such that Next(_, syn) or Next*(_, syn)
    REQUIRE(pkb.getNextStatements(ASSIGN) ==
            std::unordered_set<int>{2, 3, 5, 8, 9, 13, 14, 16, 17, 18, 19, 28, 29, 31, 33, 34, 35, 36, 37});
    REQUIRE(pkb.getNextStatements(IF_STMT) == std::unordered_set<int>{6, 15});
    REQUIRE(pkb.getNextStatements(READ) == std::unordered_set<int>{11, 12});
    REQUIRE(pkb.getNextStatements(PRINT) == std::unordered_set<int>{7, 20, 23});

    // Test Next relation for syn such that Next(syn, _) or Next*(syn, _)
    REQUIRE(pkb.getPreviousStatements(ASSIGN) ==
            std::unordered_set<int>{1, 2, 3, 5, 8, 9, 13, 14, 16, 17, 18, 19, 28, 29, 31, 33, 34, 35, 36});
    REQUIRE(pkb.getPreviousStatements(IF_STMT) == std::unordered_set<int>{6, 15});
    REQUIRE(pkb.getPreviousStatements(READ) == std::unordered_set<int>{11, 12});
    REQUIRE(pkb.getPreviousStatements(PRINT) == std::unordered_set<int>{7, 20, 22});

    // Test Next relation for Next(syn1, syn2)
    REQUIRE(pkb.getNextRelationPairings(ASSIGN, ASSIGN) ==
            IntPairResultSet{std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(8, 9), std::make_pair(17, 18),
                             std::make_pair(16, 19), std::make_pair(18, 19), std::make_pair(28, 29), std::make_pair(33, 34),
                             std::make_pair(34, 35), std::make_pair(35, 36)});
    REQUIRE(pkb.getNextRelationPairings(ASSIGN, WHILE_STMT) ==
            IntPairResultSet{std::make_pair(13, 10), std::make_pair(14, 4), std::make_pair(9, 10), std::make_pair(3, 4),
                             std::make_pair(36, 32), std::make_pair(29, 30), std::make_pair(31, 32)});
    REQUIRE(pkb.getNextRelationPairings(PRINT, PRINT) == IntPairResultSet{std::make_pair(22, 23)});

    // Test Next relation for Next*(syn1, syn2)
    REQUIRE(pkb.getNextRelationPairingsT(ASSIGN, ASSIGN) ==
            IntPairResultSet{std::make_pair(1, 2),   std::make_pair(1, 3),   std::make_pair(1, 5),   std::make_pair(1, 8),
                             std::make_pair(1, 9),   std::make_pair(1, 13),  std::make_pair(1, 14),  std::make_pair(1, 16),
                             std::make_pair(1, 17),  std::make_pair(1, 18),  std::make_pair(1, 19),  std::make_pair(2, 3),
                             std::make_pair(2, 5),   std::make_pair(2, 8),   std::make_pair(2, 9),   std::make_pair(2, 13),
                             std::make_pair(2, 14),  std::make_pair(2, 16),  std::make_pair(2, 17),  std::make_pair(2, 18),
                             std::make_pair(2, 19),  std::make_pair(3, 5),   std::make_pair(3, 8),   std::make_pair(3, 9),
                             std::make_pair(3, 13),  std::make_pair(3, 14),  std::make_pair(3, 16),  std::make_pair(3, 17),
                             std::make_pair(3, 18),  std::make_pair(3, 19),  std::make_pair(5, 5),   std::make_pair(5, 8),
                             std::make_pair(5, 9),   std::make_pair(5, 13),  std::make_pair(5, 14),  std::make_pair(5, 16),
                             std::make_pair(5, 17),  std::make_pair(5, 18),  std::make_pair(5, 19),  std::make_pair(8, 5),
                             std::make_pair(8, 8),   std::make_pair(8, 9),   std::make_pair(8, 13),  std::make_pair(8, 14),
                             std::make_pair(8, 16),  std::make_pair(8, 17),  std::make_pair(8, 18),  std::make_pair(8, 19),
                             std::make_pair(9, 5),   std::make_pair(9, 8),   std::make_pair(9, 9),   std::make_pair(9, 13),
                             std::make_pair(9, 14),  std::make_pair(9, 16),  std::make_pair(9, 17),  std::make_pair(9, 18),
                             std::make_pair(9, 19),  std::make_pair(13, 5),  std::make_pair(13, 8),  std::make_pair(13, 9),
                             std::make_pair(13, 13), std::make_pair(13, 14), std::make_pair(13, 16), std::make_pair(13, 17),
                             std::make_pair(13, 18), std::make_pair(13, 19), std::make_pair(14, 5),  std::make_pair(14, 8),
                             std::make_pair(14, 9),  std::make_pair(14, 13), std::make_pair(14, 14), std::make_pair(14, 16),
                             std::make_pair(14, 17), std::make_pair(14, 18), std::make_pair(14, 19), std::make_pair(16, 19),
                             std::make_pair(17, 18), std::make_pair(17, 19), std::make_pair(18, 19), std::make_pair(28, 28),
                             std::make_pair(28, 29), std::make_pair(28, 31), std::make_pair(28, 33), std::make_pair(28, 34),
                             std::make_pair(28, 35), std::make_pair(28, 36), std::make_pair(28, 37), std::make_pair(29, 28),
                             std::make_pair(29, 29), std::make_pair(29, 31), std::make_pair(29, 33), std::make_pair(29, 34),
                             std::make_pair(29, 35), std::make_pair(29, 36), std::make_pair(29, 37), std::make_pair(31, 28),
                             std::make_pair(31, 29), std::make_pair(31, 31), std::make_pair(31, 33), std::make_pair(31, 34),
                             std::make_pair(31, 35), std::make_pair(31, 36), std::make_pair(31, 37), std::make_pair(33, 28),
                             std::make_pair(33, 29), std::make_pair(33, 31), std::make_pair(33, 33), std::make_pair(33, 34),
                             std::make_pair(33, 35), std::make_pair(33, 36), std::make_pair(33, 37), std::make_pair(34, 28),
                             std::make_pair(34, 29), std::make_pair(34, 31), std::make_pair(34, 33), std::make_pair(34, 34),
                             std::make_pair(34, 35), std::make_pair(34, 36), std::make_pair(34, 37), std::make_pair(35, 28),
                             std::make_pair(35, 29), std::make_pair(35, 31), std::make_pair(35, 33), std::make_pair(35, 34),
                             std::make_pair(35, 35), std::make_pair(35, 36), std::make_pair(35, 37), std::make_pair(36, 28),
                             std::make_pair(36, 29), std::make_pair(36, 31), std::make_pair(36, 33), std::make_pair(36, 34),
                             std::make_pair(36, 35), std::make_pair(36, 36), std::make_pair(36, 37)});
    REQUIRE(pkb.getNextRelationPairingsT(ASSIGN, WHILE_STMT) ==
            IntPairResultSet{std::make_pair(1, 4),   std::make_pair(1, 10),  std::make_pair(2, 4),   std::make_pair(2, 10),
                             std::make_pair(3, 4),   std::make_pair(3, 10),  std::make_pair(5, 4),   std::make_pair(5, 10),
                             std::make_pair(8, 4),   std::make_pair(8, 10),  std::make_pair(9, 4),   std::make_pair(9, 10),
                             std::make_pair(13, 4),  std::make_pair(13, 10), std::make_pair(14, 4),  std::make_pair(14, 10),
                             std::make_pair(28, 27), std::make_pair(28, 30), std::make_pair(28, 32), std::make_pair(29, 27),
                             std::make_pair(29, 30), std::make_pair(29, 32), std::make_pair(31, 27), std::make_pair(31, 30),
                             std::make_pair(31, 32), std::make_pair(33, 27), std::make_pair(33, 30), std::make_pair(33, 32),
                             std::make_pair(34, 27), std::make_pair(34, 30), std::make_pair(34, 32), std::make_pair(35, 27),
                             std::make_pair(35, 30), std::make_pair(35, 32), std::make_pair(36, 27), std::make_pair(36, 30),
                             std::make_pair(36, 32)});
    REQUIRE(pkb.getNextRelationPairingsT(READ, READ) ==
            IntPairResultSet{std::make_pair(11, 12), std::make_pair(11, 11), std::make_pair(12, 12), std::make_pair(12, 11)});

    CHECK_NOTHROW(pkb.clearCache());
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Call Storage") {
    REQUIRE(pkb.getProceduresCalledBy("main") == std::unordered_set<std::string>{"first"});
    REQUIRE(pkb.getProceduresCalledBy("main", true) == std::unordered_set<std::string>{"first", "second"});
    REQUIRE(pkb.getProceduresCalledBy("second", true).empty());
    REQUIRE(pkb.getProceduresCalledBy("unknown", true).empty());
    REQUIRE(pkb.getProceduresCalledBy("unknown").empty());

    REQUIRE(pkb.getProceduresThatCalls("second") == std::unordered_set<std::string>{"first"});
    REQUIRE(pkb.getProceduresThatCalls("second", true) == std::unordered_set<std::string>{"first", "main"});
    REQUIRE(pkb.getProceduresThatCalls("main", true).empty());
    REQUIRE(pkb.getProceduresThatCalls("unknown", true).empty());
    REQUIRE(pkb.getProceduresThatCalls("unknown").empty());

    REQUIRE(pkb.getCallsRelationPairings() ==
            StringPairResultSet{std::make_pair("main", "first"), std::make_pair("first", "second")});
    REQUIRE(pkb.getCallsRelationPairings(true) == StringPairResultSet{std::make_pair("main", "first"),
                                                                      std::make_pair("main", "second"),
                                                                      std::make_pair("first", "second")});

    REQUIRE(pkb.isCalls("main", "first"));
    REQUIRE(pkb.isCalls("first", "second"));
    REQUIRE_FALSE(pkb.isCalls("main", "second"));
    REQUIRE_FALSE(pkb.isCalls("second", "first"));
    REQUIRE_FALSE(pkb.isCalls("unknown", "second"));

    REQUIRE(pkb.isCalls("main", "first", true));
    REQUIRE(pkb.isCalls("main", "second", true));
    REQUIRE(pkb.isCalls("first", "second", true));
    REQUIRE_FALSE(pkb.isCalls("first", "main", true));
    REQUIRE_FALSE(pkb.isCalls("second", "first", true));
    REQUIRE_FALSE(pkb.isCalls("unknown", "second", true));

    REQUIRE(pkb.procedureIsCaller("main"));
    REQUIRE(pkb.procedureIsCaller("first"));
    REQUIRE_FALSE(pkb.procedureIsCaller("second"));
    REQUIRE_FALSE(pkb.procedureIsCaller("unknown"));

    REQUIRE(pkb.procedureIsCallee("second"));
    REQUIRE(pkb.procedureIsCallee("first"));
    REQUIRE_FALSE(pkb.procedureIsCallee("main"));
    REQUIRE_FALSE(pkb.procedureIsCallee("unknown"));

    REQUIRE(pkb.containsCallRelation());

    REQUIRE(pkb.getStatementsThatCalls("second") == std::unordered_set<int>{25});
    REQUIRE(pkb.getStatementsThatCalls("first") == std::unordered_set<int>{21});
    REQUIRE(pkb.getStatementsThatCalls("main").empty());
    REQUIRE(pkb.getStatementsThatCalls("unknown").empty());

    REQUIRE(pkb.getAllCallStmtWithProcPair() ==
            IntStringPairResultSet{std::make_pair(21, "first"), std::make_pair(25, "second")});
}

TEST_CASE_METHOD(TestPKB, "PKB: Test Affect Storage") {
    // Affects(stmt#, stmt#)
    REQUIRE(pkb.isAffects(1, 5));
    REQUIRE(pkb.isAffects(5, 5));
    REQUIRE(pkb.isAffects(14, 5));
    REQUIRE(pkb.isAffects(36, 35));

    REQUIRE_FALSE(pkb.isAffects(5, 14));
    REQUIRE_FALSE(pkb.isAffects(13, 13));
    REQUIRE_FALSE(pkb.isAffects(1, 37));
    REQUIRE_FALSE(pkb.isAffects(1, 15));
    REQUIRE_FALSE(pkb.isAffects(0, 100));

    // Affects*(stmt#, stmt#)
    REQUIRE(pkb.isAffects(5, 19, true));
    REQUIRE(pkb.isAffects(8, 5, true));
    REQUIRE(pkb.isAffects(36, 37, true));
    REQUIRE_FALSE(pkb.isAffects(1, 13, true));
    REQUIRE_FALSE(pkb.isAffects(4, 15, true));
    REQUIRE_FALSE(pkb.isAffects(-1, 100, true));

    // Affects(syn, stmt#)
    REQUIRE(pkb.getStatementsThatAffects(14) == std::unordered_set<int>{9});
    REQUIRE(pkb.getStatementsThatAffects(19) == std::unordered_set<int>{2, 3, 17, 18, 13});
    REQUIRE(pkb.getStatementsThatAffects(17) == std::unordered_set<int>{2, 1, 5, 14, 13});
    REQUIRE(pkb.getStatementsThatAffects(37) == std::unordered_set<int>{33});

    // Affects*(syn, stmt#)
    REQUIRE(pkb.getStatementsThatAffectsT(19) == std::unordered_set<int>{1, 2, 3, 5, 8, 9, 13, 14, 17, 18});
    REQUIRE(pkb.getStatementsThatAffectsT(37) == std::unordered_set<int>{28, 29, 31, 33, 34, 35, 36});
    REQUIRE(pkb.getStatementsThatAffectsT(14) == std::unordered_set<int>{8, 9});
    REQUIRE(pkb.getStatementsThatAffectsT(13).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(15).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(-1).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(100).empty());

    // Affects(stmt#, syn)
    REQUIRE(pkb.getStatementsAffectedBy(1) == std::unordered_set<int>{5, 17, 18});
    REQUIRE(pkb.getStatementsAffectedBy(5) == std::unordered_set<int>{17, 18, 5});
    REQUIRE(pkb.getStatementsAffectedBy(14) == std::unordered_set<int>{17, 18, 5});
    REQUIRE(pkb.getStatementsAffectedBy(17) == std::unordered_set<int>{19});
    REQUIRE(pkb.getStatementsAffectedBy(36) == std::unordered_set<int>{35});

    // Affects*(stmt#, syn)
    REQUIRE(pkb.getStatementsAffectedByT(8) == std::unordered_set<int>{9, 14, 5, 17, 18, 19});
    REQUIRE(pkb.getStatementsAffectedByT(36) == std::unordered_set<int>{35, 34, 33, 31, 29, 28, 37});
    REQUIRE(pkb.getStatementsAffectedByT(1) == std::unordered_set<int>{5, 17, 18, 19});
    REQUIRE(pkb.getStatementsAffectedByT(13) == std::unordered_set<int>{17, 19});
    REQUIRE(pkb.getStatementsAffectedByT(16).empty());
    REQUIRE(pkb.getStatementsAffectedByT(11).empty());
    REQUIRE(pkb.getStatementsAffectedByT(-1).empty());
    REQUIRE(pkb.getStatementsAffectedByT(100).empty());

    // Affects(_, _)
    REQUIRE(pkb.containsAffectsRelation());

    // Affects(syn, _)
    REQUIRE(pkb.getStatementsThatAffects() ==
            std::unordered_set<int>{1, 2, 3, 8, 9, 13, 14, 5, 17, 18, 28, 29, 31, 33, 34, 35, 36});

    // Affects(_, syn)
    REQUIRE(pkb.getStatementsAffectedBy() == std::unordered_set<int>{5, 9, 14, 17, 18, 19, 28, 29, 31, 33, 34, 35, 37});

    // Affects(syn, syn)
    REQUIRE(pkb.getAffectsRelationPairings() ==
            IntPairResultSet{std::make_pair(1, 5),   std::make_pair(1, 17),  std::make_pair(1, 18),  std::make_pair(2, 17),
                             std::make_pair(2, 19),  std::make_pair(3, 18),  std::make_pair(3, 19),  std::make_pair(5, 5),
                             std::make_pair(5, 17),  std::make_pair(5, 18),  std::make_pair(8, 9),   std::make_pair(9, 14),
                             std::make_pair(14, 5),  std::make_pair(14, 17), std::make_pair(14, 18), std::make_pair(13, 17),
                             std::make_pair(13, 19), std::make_pair(17, 19), std::make_pair(18, 19), std::make_pair(28, 33),
                             std::make_pair(29, 28), std::make_pair(31, 29), std::make_pair(33, 37), std::make_pair(34, 31),
                             std::make_pair(35, 34), std::make_pair(36, 35)});

    // Affects*(syn, syn)
    REQUIRE(pkb.getAffectsRelationPairingsT() ==
            IntPairResultSet{std::make_pair(1, 5),   std::make_pair(1, 17),  std::make_pair(1, 18),  std::make_pair(1, 19),
                             std::make_pair(2, 17),  std::make_pair(2, 19),  std::make_pair(3, 18),  std::make_pair(3, 19),
                             std::make_pair(5, 5),   std::make_pair(5, 17),  std::make_pair(5, 18),  std::make_pair(5, 19),
                             std::make_pair(8, 9),   std::make_pair(8, 14),  std::make_pair(8, 5),   std::make_pair(8, 17),
                             std::make_pair(8, 18),  std::make_pair(8, 19),  std::make_pair(9, 14),  std::make_pair(9, 5),
                             std::make_pair(9, 17),  std::make_pair(9, 18),  std::make_pair(9, 19),  std::make_pair(13, 17),
                             std::make_pair(13, 19), std::make_pair(14, 5),  std::make_pair(14, 17), std::make_pair(14, 18),
                             std::make_pair(14, 19), std::make_pair(17, 19), std::make_pair(18, 19), std::make_pair(28, 33),
                             std::make_pair(28, 37), std::make_pair(29, 28), std::make_pair(29, 33), std::make_pair(29, 37),
                             std::make_pair(31, 29), std::make_pair(31, 28), std::make_pair(31, 33), std::make_pair(31, 37),
                             std::make_pair(33, 37), std::make_pair(34, 31), std::make_pair(34, 29), std::make_pair(34, 28),
                             std::make_pair(34, 33), std::make_pair(34, 37), std::make_pair(35, 34), std::make_pair(35, 31),
                             std::make_pair(35, 29), std::make_pair(35, 28), std::make_pair(35, 33), std::make_pair(35, 37),
                             std::make_pair(36, 35), std::make_pair(36, 34), std::make_pair(36, 31), std::make_pair(36, 29),
                             std::make_pair(36, 28), std::make_pair(36, 33), std::make_pair(36, 37)});

    CHECK_NOTHROW(pkb.clearCache());
}

TEST_CASE_METHOD(TestPKB, "PKB: Clear") {
    CHECK_NOTHROW(pkb.clearCache());
    CHECK_NOTHROW(pkb.clear());
}
