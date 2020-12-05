#include "catch.hpp"

// Front-end
#include "DesignExtractor.h"
#include "FrontEnd.h"
#include "Lexer.h"
#include "Parser.h"
#include "SimplifiedAST.h"

// PKB
#include "PKB.h"

PKB &pkb = PKB::Instance();

// ======== Helper functions for set/vector creations ============
// Description: Provide a string delimited by space.
// Example: generateTokenList("a b program dist") will return a vector {"a", "b", "program", "dist"}

std::vector<std::string> generateTokenList(std::string stmt) {
    std::stringstream ss(stmt);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> splitted(begin, end);
    return splitted;
}

std::unordered_set<std::string> generateVarSet(std::string stmt) {
    std::stringstream ss(stmt);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    return splitted;
}

std::unordered_set<int> generateStmtNumSet(std::string set) {
    std::stringstream ss(set);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    std::unordered_set<int> intSet;
    for (std::string s : splitted) {
        intSet.insert(std::stoi(s));
    }
    return intSet;
}

TEST_CASE("Program 01: Populate PKB") {
    std::string filename = "./SIMPLESourceFiles/Program01.txt";
    FrontEnd frontend;

    frontend.start(filename);
}

TEST_CASE("Program 01: Test Follows Storage") {

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
}

TEST_CASE("Program 01: Test Parent Storage") {
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
}

TEST_CASE("Program 01: Test Modifies Storage") {
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
}

TEST_CASE("Program 01: Test Uses Storage") {
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
}

TEST_CASE("Program 01: Test General Storage") {
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

TEST_CASE("Program 01: Test Pattern Storage") {
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

TEST_CASE("Program 01: Test Next Storage") {
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
}

TEST_CASE("Program 01: Test Call Storage") {
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
}

TEST_CASE("Program 01: Test Affect Storage") {
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
}

TEST_CASE("Program 01: PKB Teardown") {
    pkb.clear();
}

// ==============================================================================
TEST_CASE("Program 02: Populate PKB") {
    std::string filename = "./SIMPLESourceFiles/Program02.txt";
    FrontEnd frontend;

    frontend.start(filename);
}

TEST_CASE("Program 02: Check procedures are added") {
    // only need to run one time
    REQUIRE(pkb.getAllProc() == generateVarSet("raymarch spheresdf"));
}

TEST_CASE("Program 02: Test Follows relation") {

    SECTION("Check basic follows relations") {
        // Use case: Follows/Follows*(stmtNum, stmtNum)
        REQUIRE(pkb.isFollows(1, 2) == true);
        REQUIRE(pkb.isFollows(2, 3) == true);
        REQUIRE(pkb.isFollows(3, 4) == true);
        REQUIRE(pkb.isFollows(4, 13) == true);
        REQUIRE(pkb.isFollows(13, 14) == true);
        REQUIRE(pkb.isFollows(14, 17) == true);

        REQUIRE(pkb.isFollows(5, 6) == true);
        REQUIRE(pkb.isFollows(6, 7) == true);

        REQUIRE(pkb.isFollows(8, 9) == true);
        REQUIRE(pkb.isFollows(9, 12) == true);

        REQUIRE(pkb.isFollows(10, 11) == true);
    }

    SECTION("Check that follows is not followsStar") {
        REQUIRE(pkb.isFollows(1, 3) == false);
        REQUIRE(pkb.isFollows(1, 4) == false);
        REQUIRE(pkb.isFollows(2, 4) == false);
        REQUIRE(pkb.isFollows(1, 13) == false);
        REQUIRE(pkb.isFollows(1, 14) == false);
        REQUIRE(pkb.isFollows(1, 17) == false);
        REQUIRE(pkb.isFollows(2, 13) == false);
        REQUIRE(pkb.isFollows(2, 14) == false);
        REQUIRE(pkb.isFollows(2, 17) == false);
        REQUIRE(pkb.isFollows(3, 13) == false);
        REQUIRE(pkb.isFollows(3, 14) == false);
        REQUIRE(pkb.isFollows(3, 17) == false);
        REQUIRE(pkb.isFollows(4, 14) == false);
        REQUIRE(pkb.isFollows(4, 17) == false);

        REQUIRE(pkb.isFollows(5, 7) == false);
    }

    SECTION("Check that statements in different stmt list don't follow each other") {
        REQUIRE(pkb.isFollows(2, 5) == false);
        REQUIRE(pkb.isFollows(4, 6) == false);
        REQUIRE(pkb.isFollows(4, 8) == false);
        REQUIRE(pkb.isFollows(4, 11) == false);
        REQUIRE(pkb.isFollows(5, 11) == false);
        REQUIRE(pkb.isFollows(3, 12) == false);
        REQUIRE(pkb.isFollows(11, 12) == false);
        REQUIRE(pkb.isFollows(13, 15) == false);
        REQUIRE(pkb.isFollows(15, 16) == false);
        REQUIRE(pkb.isFollows(16, 17) == false);
    }
}

TEST_CASE("Program 02: Test FollowsStar relation") {

    SECTION("Check basic followsStar relations") {
        // normal follows are also follow*
        REQUIRE(pkb.isFollows(1, 2, true) == true);
        REQUIRE(pkb.isFollows(14, 17, true) == true);
        REQUIRE(pkb.isFollows(5, 6, true) == true);
        REQUIRE(pkb.isFollows(6, 7, true) == true);

        REQUIRE(pkb.isFollows(1, 3, true) == true);
        REQUIRE(pkb.isFollows(1, 4, true) == true);
        REQUIRE(pkb.isFollows(2, 4, true) == true);
        REQUIRE(pkb.isFollows(1, 13, true) == true);
        REQUIRE(pkb.isFollows(1, 14, true) == true);
        REQUIRE(pkb.isFollows(1, 17, true) == true);
        REQUIRE(pkb.isFollows(2, 13, true) == true);
        REQUIRE(pkb.isFollows(2, 14, true) == true);
        REQUIRE(pkb.isFollows(2, 17, true) == true);
        REQUIRE(pkb.isFollows(3, 13, true) == true);
        REQUIRE(pkb.isFollows(3, 14, true) == true);
        REQUIRE(pkb.isFollows(3, 17, true) == true);
        REQUIRE(pkb.isFollows(4, 13, true) == true);
        REQUIRE(pkb.isFollows(4, 14, true) == true);
        REQUIRE(pkb.isFollows(4, 17, true) == true);

        REQUIRE(pkb.isFollows(5, 7, true) == true);
    }

    SECTION("Check that statements in different stmt list don't followsStar each other") {
        REQUIRE(pkb.isFollows(2, 5, true) == false);
        REQUIRE(pkb.isFollows(4, 6, true) == false);
        REQUIRE(pkb.isFollows(4, 8, true) == false);
        REQUIRE(pkb.isFollows(4, 11, true) == false);
        REQUIRE(pkb.isFollows(5, 11, true) == false);
        REQUIRE(pkb.isFollows(3, 12, true) == false);
        REQUIRE(pkb.isFollows(11, 12, true) == false);
        REQUIRE(pkb.isFollows(13, 15, true) == false);
        REQUIRE(pkb.isFollows(15, 16, true) == false);
        REQUIRE(pkb.isFollows(16, 17, true) == false);
    }
}

TEST_CASE("Program 02: Parent relation") {

    SECTION("Check basic parent relations") {
        REQUIRE(pkb.isParent(4, 5) == true);
        REQUIRE(pkb.isParent(4, 6) == true);
        REQUIRE(pkb.isParent(4, 7) == true);

        REQUIRE(pkb.isParent(7, 8) == true);
        REQUIRE(pkb.isParent(7, 9) == true);
        REQUIRE(pkb.isParent(7, 12) == true);

        REQUIRE(pkb.isParent(14, 15) == true);
        REQUIRE(pkb.isParent(14, 16) == true);
    }

    SECTION("Check that parent is not parentStar") {
        REQUIRE(pkb.isParent(4, 8) == false);
        REQUIRE(pkb.isParent(4, 9) == false);
        REQUIRE(pkb.isParent(4, 10) == false);
        REQUIRE(pkb.isParent(4, 11) == false);
        REQUIRE(pkb.isParent(4, 12) == false);

        REQUIRE(pkb.isParent(7, 10) == false);
        REQUIRE(pkb.isParent(7, 11) == false);
    }

    SECTION("Check that statements under different parents don't have a wrong parent relation") {
        REQUIRE(pkb.isParent(4, 15) == false);
        REQUIRE(pkb.isParent(4, 16) == false);
        REQUIRE(pkb.isParent(9, 16) == false);
        REQUIRE(pkb.isParent(9, 8) == false);
    }

    SECTION("Check that statements without parent / are not parents should not have the relation") {
        REQUIRE(pkb.isParent(1, 5) == false);
        REQUIRE(pkb.isParent(2, 6) == false);
        REQUIRE(pkb.isParent(3, 7) == false);

        REQUIRE(pkb.isParent(5, 7) == false);
        REQUIRE(pkb.isParent(5, 8) == false);
        REQUIRE(pkb.isParent(8, 10) == false);

        REQUIRE(pkb.isParent(13, 14) == false);
        REQUIRE(pkb.isParent(13, 16) == false);
    }
}

TEST_CASE("Program 02: Test ParentStar relation") {

    SECTION("Check basic parentStar relations") {
        // normal parent are also parent*
        REQUIRE(pkb.isParent(4, 5, true) == true);
        REQUIRE(pkb.isParent(4, 6, true) == true);
        REQUIRE(pkb.isParent(4, 7, true) == true);
        REQUIRE(pkb.isParent(7, 8, true) == true);
        REQUIRE(pkb.isParent(7, 9, true) == true);
        REQUIRE(pkb.isParent(7, 12, true) == true);
        REQUIRE(pkb.isParent(14, 15, true) == true);
        REQUIRE(pkb.isParent(14, 16, true) == true);

        REQUIRE(pkb.isParent(4, 8, true) == true);
        REQUIRE(pkb.isParent(4, 9, true) == true);
        REQUIRE(pkb.isParent(4, 10, true) == true);
        REQUIRE(pkb.isParent(4, 11, true) == true);
        REQUIRE(pkb.isParent(4, 12, true) == true);

        REQUIRE(pkb.isParent(7, 10, true) == true);
        REQUIRE(pkb.isParent(7, 11, true) == true);
    }

    SECTION("Check that statements under different parents* don't have a wrong parent star relation") {
        REQUIRE(pkb.isParent(4, 15, true) == false);
        REQUIRE(pkb.isParent(4, 16, true) == false);
        REQUIRE(pkb.isParent(9, 16, true) == false);
        REQUIRE(pkb.isParent(9, 8, true) == false);
        REQUIRE(pkb.isParent(14, 10, true) == false);
    }

    SECTION("Check that statements without parent should not have the relation") {
        REQUIRE(pkb.isParent(1, 5, true) == false);
        REQUIRE(pkb.isParent(2, 6, true) == false);
        REQUIRE(pkb.isParent(3, 7, true) == false);

        REQUIRE(pkb.isParent(5, 7, true) == false);
        REQUIRE(pkb.isParent(5, 8, true) == false);
        REQUIRE(pkb.isParent(8, 10, true) == false);

        REQUIRE(pkb.isParent(13, 14, true) == false);
        REQUIRE(pkb.isParent(13, 16, true) == false);
    }
}

TEST_CASE("Program 02: Modifies relation") {

    SECTION("Check correct modifies relation for statements") {
        REQUIRE(pkb.getModifiesVariables(1) == generateVarSet("ro"));
        REQUIRE(pkb.getModifiesVariables(2) == generateVarSet("rd"));
        REQUIRE(pkb.getModifiesVariables(3) == generateVarSet("depth"));
        REQUIRE(pkb.getModifiesVariables(4) == generateVarSet("po a cba b"));
        REQUIRE(pkb.getModifiesVariables(5) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(6) == generateVarSet("po"));
        REQUIRE(pkb.getModifiesVariables(7) == generateVarSet("a cba b"));
        REQUIRE(pkb.getModifiesVariables(8) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(9) == generateVarSet("cba"));
        REQUIRE(pkb.getModifiesVariables(10) == generateVarSet("cba"));
        REQUIRE(pkb.getModifiesVariables(11) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(12) == generateVarSet("b"));
        REQUIRE(pkb.getModifiesVariables(13) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(14) == generateVarSet("done depth"));
        REQUIRE(pkb.getModifiesVariables(15) == generateVarSet("done"));
        REQUIRE(pkb.getModifiesVariables(16) == generateVarSet("depth"));
        REQUIRE(pkb.getModifiesVariables(17) == generateVarSet("count"));
        REQUIRE(pkb.getModifiesVariables(18) == generateVarSet("dist"));
        REQUIRE(pkb.getModifiesVariables(19) == generateVarSet(""));
    }

    SECTION("Check correct modifies relation for procedures") {
        REQUIRE(pkb.getModifiesVariableFromProc("raymarch") == generateVarSet("ro rd depth po a cba b done depth count"));
        REQUIRE(pkb.getModifiesVariableFromProc("spheresdf") == generateVarSet("dist"));
    }
}

TEST_CASE("Program 02: Uses relation") {

    SECTION("Check correct uses relation for statements") {
        REQUIRE(pkb.getUsesVariables(1) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(2) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(3) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(4) == generateVarSet("count steps depth ro rd a b c d e abc"));
        REQUIRE(pkb.getUsesVariables(5) == generateVarSet("depth"));
        REQUIRE(pkb.getUsesVariables(6) == generateVarSet("ro rd depth"));
        REQUIRE(pkb.getUsesVariables(7) == generateVarSet("a b c d e abc rd depth"));
        REQUIRE(pkb.getUsesVariables(8) == generateVarSet("depth c d e b"));
        REQUIRE(pkb.getUsesVariables(9) == generateVarSet("abc"));
        REQUIRE(pkb.getUsesVariables(10) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(11) == generateVarSet("abc"));
        REQUIRE(pkb.getUsesVariables(12) == generateVarSet("rd"));
        REQUIRE(pkb.getUsesVariables(13) == generateVarSet("depth"));
        REQUIRE(pkb.getUsesVariables(14) == generateVarSet("dist epsilon depth"));
        REQUIRE(pkb.getUsesVariables(15) == generateVarSet("depth"));
        REQUIRE(pkb.getUsesVariables(16) == generateVarSet("depth dist"));
        REQUIRE(pkb.getUsesVariables(17) == generateVarSet("count"));
        REQUIRE(pkb.getUsesVariables(18) == generateVarSet("x y z"));
        REQUIRE(pkb.getUsesVariables(19) == generateVarSet("z"));
    }

    SECTION("Check correct uses relation for procedures") {
        REQUIRE(pkb.getUsesVariableFromProc("raymarch") ==
                generateVarSet("count steps depth ro rd a b c d e abc abc rd dist epsilon count"));
        REQUIRE(pkb.getUsesVariableFromProc("spheresdf") == generateVarSet("x y z"));
    }
}

TEST_CASE("Program 02: Calls relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct calls relation for statements") {
        REQUIRE(pkb.getProceduresCalledBy("raymarch") == generateVarSet(""));
        REQUIRE(pkb.getProceduresCalledBy("spheresdf") == generateVarSet(""));
    }
}

TEST_CASE("Program 02: Affects relation") {

    SECTION("Check correct Affects(*) relation from CFG") {
        REQUIRE(pkb.isAffects(1, 6));
        REQUIRE(pkb.isAffects(2, 6));
        REQUIRE(pkb.isAffects(12, 8));
        REQUIRE_FALSE(pkb.isAffects(19, 21));
        REQUIRE_FALSE(pkb.isAffects(5, 23));
        REQUIRE_FALSE(pkb.isAffects(-1, 100));

        REQUIRE(pkb.isAffects(1, 6, true));
        REQUIRE(pkb.isAffects(2, 6, true));
        REQUIRE(pkb.isAffects(12, 8, true));
        REQUIRE(pkb.isAffects(2, 8, true));
        REQUIRE_FALSE(pkb.isAffects(19, 21, true));
        REQUIRE_FALSE(pkb.isAffects(5, 23, true));
        REQUIRE_FALSE(pkb.isAffects(-1, 100, true));

        REQUIRE(pkb.containsAffectsRelation());

        REQUIRE(pkb.getStatementsThatAffects() == std::unordered_set<int>{1, 2, 12});
        REQUIRE(pkb.getStatementsThatAffects(6) == std::unordered_set<int>{1, 2});
        REQUIRE(pkb.getStatementsThatAffects(8) == std::unordered_set<int>{12});
        REQUIRE(pkb.getStatementsThatAffects(26).empty());
        REQUIRE(pkb.getStatementsThatAffects(-1).empty());

        REQUIRE(pkb.getStatementsThatAffectsT(6) == std::unordered_set<int>{1, 2});
        REQUIRE(pkb.getStatementsThatAffectsT(8) == std::unordered_set<int>{2, 12});
        REQUIRE(pkb.getStatementsThatAffectsT(26).empty());
        REQUIRE(pkb.getStatementsThatAffectsT(-1).empty());

        REQUIRE(pkb.getStatementsAffectedBy() == std::unordered_set<int>{6, 8, 12});
        REQUIRE(pkb.getStatementsAffectedBy(1) == std::unordered_set<int>{6});
        REQUIRE(pkb.getStatementsAffectedBy(2) == std::unordered_set<int>{6, 12});
        REQUIRE(pkb.getStatementsAffectedBy(15).empty());
        REQUIRE(pkb.getStatementsAffectedBy(4).empty());
        REQUIRE(pkb.getStatementsAffectedBy(-1).empty());

        REQUIRE(pkb.getStatementsAffectedByT(1) == std::unordered_set<int>{6});
        REQUIRE(pkb.getStatementsAffectedByT(2) == std::unordered_set<int>{6, 8, 12});
        REQUIRE(pkb.getStatementsAffectedByT(15).empty());
        REQUIRE(pkb.getStatementsAffectedByT(4).empty());
        REQUIRE(pkb.getStatementsAffectedByT(-1).empty());

        REQUIRE(pkb.getAffectsRelationPairings() ==
                IntPairResultSet{std::make_pair(1, 6), std::make_pair(2, 6), std::make_pair(2, 12), std::make_pair(12, 8)});

        REQUIRE(pkb.getAffectsRelationPairingsT() == IntPairResultSet{std::make_pair(1, 6), std::make_pair(2, 6),
                                                                      std::make_pair(2, 12), std::make_pair(2, 8),
                                                                      std::make_pair(12, 8)});
    }
}

TEST_CASE("Program 02: Next relation") {

    SECTION("Check correct next relation from CFG") {
        REQUIRE(pkb.isNext(1, 2));
        REQUIRE(pkb.isNext(2, 3));
        REQUIRE(pkb.isNext(3, 4));
        REQUIRE(pkb.isNext(4, 5));
        REQUIRE(pkb.isNext(4, 13));
        REQUIRE(pkb.isNext(5, 6));
        REQUIRE(pkb.isNext(6, 7));
        REQUIRE(pkb.isNext(7, 4));
        REQUIRE(pkb.isNext(7, 8));
        REQUIRE(pkb.isNext(8, 9));
        REQUIRE(pkb.isNext(9, 10));
        REQUIRE(pkb.isNext(9, 12));
        REQUIRE(pkb.isNext(10, 11));
        REQUIRE(pkb.isNext(11, 9));
        REQUIRE(pkb.isNext(12, 7));
        REQUIRE(pkb.isNext(13, 14));
        REQUIRE(pkb.isNext(14, 15));
        REQUIRE(pkb.isNext(14, 16));
        REQUIRE(pkb.isNext(15, 17));
        REQUIRE(pkb.isNext(18, 19));

        REQUIRE_FALSE(pkb.isNext(1, 22));
        REQUIRE_FALSE(pkb.isNext(4, 16));
        REQUIRE_FALSE(pkb.isNext(13, 16));
        REQUIRE_FALSE(pkb.isNext(14, 18));
        REQUIRE_FALSE(pkb.isNext(0, 1000));
        REQUIRE_FALSE(pkb.isNext(17, 18));
    }
}

TEST_CASE("Program 02: Populate PKB with Statements - and their attributes") {

    SECTION("Check that PKB is populated with correct variables in a statement") {
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("1")) == generateVarSet("ro"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("2")) == generateVarSet("rd"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("3")) == generateVarSet("depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("4")) == generateVarSet("count steps"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("5")) == generateVarSet("depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("6")) == generateVarSet("po ro rd depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("7")) == generateVarSet("a b c d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("8")) == generateVarSet("a depth c d e b"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("9")) == generateVarSet(""));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("10")) == generateVarSet("cba"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("11")) == generateVarSet("abc"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("12")) == generateVarSet("b rd"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("13")) == generateVarSet("depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("14")) == generateVarSet("dist epsilon"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("15")) == generateVarSet("done depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("16")) == generateVarSet("depth dist"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("17")) == generateVarSet("count"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("18")) == generateVarSet("dist x y z"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("19")) == generateVarSet("z"));
    }
}

TEST_CASE("Program 02: Populate PKB with Assignment pattern") {
    SECTION("Check that assignment statements have the correct pattern stored") {
        REQUIRE(pkb.getStatementsWithPattern("ro", "13", true, true) == generateStmtNumSet("1"));
        REQUIRE(pkb.getStatementsWithPattern("rd", "19", true, true) == generateStmtNumSet("2"));
        REQUIRE(pkb.getStatementsWithPattern("po", "ro + rd * depth", true, true) == generateStmtNumSet("6"));
        REQUIRE(pkb.getStatementsWithPattern("a", "( depth + ( c - ( d * ( e ) ) ) + 2 - ( 3 ) + b   )", true, true) ==
                generateStmtNumSet("8"));
        REQUIRE(pkb.getStatementsWithPattern("b", "rd", true, true) == generateStmtNumSet("12"));
        REQUIRE(pkb.getStatementsWithPattern("done", "depth", true, true) == generateStmtNumSet("15"));
        REQUIRE(pkb.getStatementsWithPattern("depth", "depth + dist", true, true) == generateStmtNumSet("16"));
        REQUIRE(pkb.getStatementsWithPattern("count", "count + 1", true, true) == generateStmtNumSet("17"));
        REQUIRE(pkb.getStatementsWithPattern("dist", "x * x + y * y + z * z + 2 - 3", true, true) == generateStmtNumSet("18"));
    }

    SECTION("Check that conditional patterns have the correct pattern stored") {
        REQUIRE(pkb.getVarMatchConditionStatements("count", WHILE_STMT) == generateStmtNumSet("4"));
        REQUIRE(pkb.getVarMatchConditionStatements("steps", WHILE_STMT) == generateStmtNumSet("4"));
        REQUIRE(pkb.getVarMatchConditionStatements("a", WHILE_STMT) == generateStmtNumSet("7"));
        REQUIRE(pkb.getVarMatchConditionStatements("b", WHILE_STMT) == generateStmtNumSet("7"));
        REQUIRE(pkb.getVarMatchConditionStatements("c", WHILE_STMT) == generateStmtNumSet("7"));
        REQUIRE(pkb.getVarMatchConditionStatements("d", WHILE_STMT) == generateStmtNumSet("7"));

        REQUIRE(pkb.getVarMatchConditionStatements("dist", IF_STMT) == generateStmtNumSet("14"));
        REQUIRE(pkb.getVarMatchConditionStatements("epsilon", IF_STMT) == generateStmtNumSet("14"));
    }
}

TEST_CASE("Program 02: PKB Teardown") {
    pkb.clear();
}

// ==============================================================================
// test_case("program 03: populate pkb") {
//	std::string filename = "./simplesourcefiles/program1.txt";
//	frontend frontend;
//
//	frontend.start(filename);
//}
//
// test_case("program 03: check procedures are added") {
//	// only need to run one time
//	require(pkb.getallproc() == generatevarset("main raymarch spheresdf"));
//}
//
// test_case("program 03: pkb teardown") {
//	pkb.clear();
//}

// ==============================================================================
TEST_CASE("Program 04: Populate PKB") {
    std::string filename = "./SIMPLESourceFiles/Program04.txt";
    FrontEnd frontend;

    frontend.start(filename);
}

TEST_CASE("Program 04: Check procedures are added") {

    REQUIRE(pkb.getAllProc() == generateVarSet("procedure test2"));
}

TEST_CASE("Program 04: Test Follows relation") {

    SECTION("Check basic follows relations") {
        // Use case: Follows/Follows*(stmtNum, stmtNum)
        REQUIRE(pkb.isFollows(1, 24) == true);
        REQUIRE(pkb.isFollows(24, 25) == true);
        REQUIRE(pkb.isFollows(25, 26) == true);
        REQUIRE(pkb.isFollows(28, 29) == true);
        REQUIRE(pkb.isFollows(30, 31) == true);

        REQUIRE(pkb.isFollows(18, 19) == true);

        REQUIRE(pkb.isFollows(20, 21) == true);
        REQUIRE(pkb.isFollows(22, 23) == true);

        REQUIRE(pkb.isFollows(32, 33) == true);

        REQUIRE(pkb.isFollows(35, 36) == true);
        REQUIRE(pkb.isFollows(36, 37) == true);
        REQUIRE(pkb.isFollows(37, 38) == true);
    }

    SECTION("Check that follows is not followsStar") {
        REQUIRE(pkb.isFollows(1, 25) == false);
        REQUIRE(pkb.isFollows(1, 26) == false);
        REQUIRE(pkb.isFollows(24, 26) == false);
        REQUIRE(pkb.isFollows(24, 30) == false);
        REQUIRE(pkb.isFollows(26, 31) == false);

        REQUIRE(pkb.isFollows(20, 22) == false);
        REQUIRE(pkb.isFollows(20, 23) == false);
        REQUIRE(pkb.isFollows(21, 23) == false);

        REQUIRE(pkb.isFollows(35, 37) == false);
        REQUIRE(pkb.isFollows(35, 38) == false);
    }

    SECTION("Check that statements in different stmt list don't follow each other") {
        REQUIRE(pkb.isFollows(1, 2) == false);
        REQUIRE(pkb.isFollows(1, 3) == false);
        REQUIRE(pkb.isFollows(1, 4) == false);
        REQUIRE(pkb.isFollows(4, 5) == false);
        REQUIRE(pkb.isFollows(4, 6) == false);
        REQUIRE(pkb.isFollows(5, 6) == false);
        REQUIRE(pkb.isFollows(6, 7) == false);
        REQUIRE(pkb.isFollows(7, 8) == false);
        REQUIRE(pkb.isFollows(8, 9) == false);

        REQUIRE(pkb.isFollows(5, 10) == false);
        REQUIRE(pkb.isFollows(10, 11) == false);
        REQUIRE(pkb.isFollows(11, 12) == false);
        REQUIRE(pkb.isFollows(12, 13) == false);
        REQUIRE(pkb.isFollows(13, 14) == false);
        REQUIRE(pkb.isFollows(14, 15) == false);
        REQUIRE(pkb.isFollows(15, 16) == false);
        REQUIRE(pkb.isFollows(16, 17) == false);

        REQUIRE(pkb.isFollows(6, 18) == false);
        REQUIRE(pkb.isFollows(12, 19) == false);

        REQUIRE(pkb.isFollows(6, 20) == false);

        REQUIRE(pkb.isFollows(20, 24) == false);
        REQUIRE(pkb.isFollows(12, 24) == false);
        REQUIRE(pkb.isFollows(31, 32) == false);
        REQUIRE(pkb.isFollows(33, 34) == false);

        REQUIRE(pkb.isFollows(30, 35) == false);
        REQUIRE(pkb.isFollows(1, 34) == false);
    }
}

TEST_CASE("Program 04: Test FollowsStar relation") {

    SECTION("Check basic followsStar relations") {
        // normal follows are also follow*
        REQUIRE(pkb.isFollows(1, 24, true) == true);
        REQUIRE(pkb.isFollows(24, 25, true) == true);
        REQUIRE(pkb.isFollows(25, 26, true) == true);
        REQUIRE(pkb.isFollows(28, 29, true) == true);
        REQUIRE(pkb.isFollows(30, 31, true) == true);
        REQUIRE(pkb.isFollows(18, 19, true) == true);
        REQUIRE(pkb.isFollows(20, 21, true) == true);
        REQUIRE(pkb.isFollows(22, 23, true) == true);
        REQUIRE(pkb.isFollows(32, 33, true) == true);

        REQUIRE(pkb.isFollows(1, 25, true) == true);
        REQUIRE(pkb.isFollows(1, 26, true) == true);
        REQUIRE(pkb.isFollows(1, 27, true) == true);
        REQUIRE(pkb.isFollows(1, 28, true) == true);
        REQUIRE(pkb.isFollows(1, 29, true) == true);
        REQUIRE(pkb.isFollows(1, 30, true) == true);
        REQUIRE(pkb.isFollows(1, 31, true) == true);

        REQUIRE(pkb.isFollows(24, 26, true) == true);
        REQUIRE(pkb.isFollows(24, 27, true) == true);
        REQUIRE(pkb.isFollows(24, 28, true) == true);
        REQUIRE(pkb.isFollows(24, 29, true) == true);
        REQUIRE(pkb.isFollows(24, 30, true) == true);
        REQUIRE(pkb.isFollows(24, 31, true) == true);
        REQUIRE(pkb.isFollows(26, 28, true) == true);
        REQUIRE(pkb.isFollows(26, 29, true) == true);
        REQUIRE(pkb.isFollows(26, 30, true) == true);
        REQUIRE(pkb.isFollows(26, 31, true) == true);
        REQUIRE(pkb.isFollows(28, 30, true) == true);
        REQUIRE(pkb.isFollows(28, 31, true) == true);

        REQUIRE(pkb.isFollows(20, 22, true) == true);
        REQUIRE(pkb.isFollows(20, 23, true) == true);
        REQUIRE(pkb.isFollows(21, 23, true) == true);

        REQUIRE(pkb.isFollows(35, 36, true) == true);
        REQUIRE(pkb.isFollows(35, 37, true) == true);
        REQUIRE(pkb.isFollows(35, 38, true) == true);
        REQUIRE(pkb.isFollows(36, 38, true) == true);
    }

    SECTION("Check that statements in different stmt list don't followsStar each other") {
        REQUIRE(pkb.isFollows(1, 3, true) == false);
        REQUIRE(pkb.isFollows(1, 4, true) == false);
        REQUIRE(pkb.isFollows(4, 6, true) == false);

        REQUIRE(pkb.isFollows(5, 10, true) == false);

        REQUIRE(pkb.isFollows(6, 18, true) == false);
        REQUIRE(pkb.isFollows(12, 19, true) == false);

        REQUIRE(pkb.isFollows(6, 20, true) == false);

        REQUIRE(pkb.isFollows(20, 24, true) == false);
        REQUIRE(pkb.isFollows(12, 24, true) == false);

        REQUIRE(pkb.isFollows(30, 35, true) == false);
        REQUIRE(pkb.isFollows(1, 34, true) == false);
    }
}

TEST_CASE("Program 04: Parent relation") {

    SECTION("Check basic parent relations") {
        REQUIRE(pkb.isParent(1, 2) == true);
        REQUIRE(pkb.isParent(2, 3) == true);

        REQUIRE(pkb.isParent(1, 4) == true);
        REQUIRE(pkb.isParent(4, 5) == true);
        REQUIRE(pkb.isParent(5, 6) == true);
        REQUIRE(pkb.isParent(6, 7) == true);
        REQUIRE(pkb.isParent(7, 8) == true);
        REQUIRE(pkb.isParent(8, 9) == true);

        REQUIRE(pkb.isParent(6, 10) == true);
        REQUIRE(pkb.isParent(10, 11) == true);
        REQUIRE(pkb.isParent(11, 12) == true);
        REQUIRE(pkb.isParent(12, 13) == true);
        REQUIRE(pkb.isParent(13, 14) == true);
        REQUIRE(pkb.isParent(14, 15) == true);
        REQUIRE(pkb.isParent(14, 16) == true);
        REQUIRE(pkb.isParent(16, 17) == true);
        REQUIRE(pkb.isParent(10, 18) == true);
        REQUIRE(pkb.isParent(10, 19) == true);

        REQUIRE(pkb.isParent(5, 20) == true);
        REQUIRE(pkb.isParent(5, 21) == true);
        REQUIRE(pkb.isParent(5, 22) == true);
        REQUIRE(pkb.isParent(5, 23) == true);

        REQUIRE(pkb.isParent(31, 32) == true);
        REQUIRE(pkb.isParent(31, 33) == true);
        REQUIRE(pkb.isParent(31, 34) == true);
    }

    SECTION("Check that parent is not parentStar") {
        REQUIRE(pkb.isParent(4, 8) == false);
        REQUIRE(pkb.isParent(4, 9) == false);
        REQUIRE(pkb.isParent(4, 10) == false);
        REQUIRE(pkb.isParent(4, 11) == false);
        REQUIRE(pkb.isParent(4, 12) == false);

        REQUIRE(pkb.isParent(7, 10) == false);
        REQUIRE(pkb.isParent(7, 11) == false);
    }

    SECTION("Check that statements under different parents don't have a wrong parent relation") {
        REQUIRE(pkb.isParent(4, 15) == false);
        REQUIRE(pkb.isParent(4, 16) == false);
        REQUIRE(pkb.isParent(9, 16) == false);
        REQUIRE(pkb.isParent(9, 8) == false);
    }

    SECTION("Check that statements without parent / are not parents should not have the relation") {
        REQUIRE(pkb.isParent(1, 24) == false);
        REQUIRE(pkb.isParent(20, 21) == false);
        REQUIRE(pkb.isParent(24, 25) == false);
        REQUIRE(pkb.isParent(26, 28) == false);

        REQUIRE(pkb.isParent(32, 33) == false);
        REQUIRE(pkb.isParent(33, 34) == false);

        REQUIRE(pkb.isParent(34, 35) == false);
        REQUIRE(pkb.isParent(35, 36) == false);
    }
}

TEST_CASE("Program 04: Test ParentStar relation") {

    SECTION("Check basic parentStar relations") {
        // normal parent are also parent*
        REQUIRE(pkb.isParent(1, 2, true) == true);
        REQUIRE(pkb.isParent(2, 3, true) == true);
        REQUIRE(pkb.isParent(1, 4, true) == true);
        REQUIRE(pkb.isParent(4, 5, true) == true);
        REQUIRE(pkb.isParent(8, 9, true) == true);
        REQUIRE(pkb.isParent(6, 10, true) == true);
        REQUIRE(pkb.isParent(10, 11, true) == true);
        REQUIRE(pkb.isParent(10, 18, true) == true);
        REQUIRE(pkb.isParent(10, 19, true) == true);
        REQUIRE(pkb.isParent(5, 20, true) == true);
        REQUIRE(pkb.isParent(5, 23, true) == true);
        REQUIRE(pkb.isParent(31, 34, true) == true);

        REQUIRE(pkb.isParent(1, 3, true) == true);
        REQUIRE(pkb.isParent(1, 4, true) == true);
        REQUIRE(pkb.isParent(1, 5, true) == true);
        REQUIRE(pkb.isParent(1, 6, true) == true);
        REQUIRE(pkb.isParent(1, 7, true) == true);
        REQUIRE(pkb.isParent(1, 8, true) == true);
        REQUIRE(pkb.isParent(1, 9, true) == true);
        REQUIRE(pkb.isParent(1, 17, true) == true);

        REQUIRE(pkb.isParent(4, 6, true) == true);
        REQUIRE(pkb.isParent(4, 7, true) == true);
        REQUIRE(pkb.isParent(4, 8, true) == true);
        REQUIRE(pkb.isParent(4, 9, true) == true);
        REQUIRE(pkb.isParent(4, 17, true) == true);

        REQUIRE(pkb.isParent(5, 7, true) == true);
        REQUIRE(pkb.isParent(5, 8, true) == true);
        REQUIRE(pkb.isParent(5, 9, true) == true);
        REQUIRE(pkb.isParent(5, 17, true) == true);

        REQUIRE(pkb.isParent(6, 11, true) == true);
        REQUIRE(pkb.isParent(6, 12, true) == true);
        REQUIRE(pkb.isParent(6, 13, true) == true);
        REQUIRE(pkb.isParent(6, 14, true) == true);
        REQUIRE(pkb.isParent(6, 15, true) == true);
        REQUIRE(pkb.isParent(6, 16, true) == true);
        REQUIRE(pkb.isParent(6, 17, true) == true);

        REQUIRE(pkb.isParent(10, 15, true) == true);
        REQUIRE(pkb.isParent(10, 17, true) == true);
        REQUIRE(pkb.isParent(11, 15, true) == true);
        REQUIRE(pkb.isParent(11, 17, true) == true);
        REQUIRE(pkb.isParent(12, 15, true) == true);
        REQUIRE(pkb.isParent(12, 17, true) == true);
        REQUIRE(pkb.isParent(13, 17, true) == true);
        REQUIRE(pkb.isParent(14, 17, true) == true);
    }

    SECTION("Check that statements under different parents* don't have a wrong parent star relation") {
        REQUIRE(pkb.isParent(1, 32, true) == false);
        REQUIRE(pkb.isParent(1, 33, true) == false);
        REQUIRE(pkb.isParent(10, 32, true) == false);
        REQUIRE(pkb.isParent(13, 20, true) == false);
        REQUIRE(pkb.isParent(13, 34, true) == false);
    }

    SECTION("Check that statements without parent should not have the relation") {
        REQUIRE(pkb.isParent(1, 24, true) == false);
        REQUIRE(pkb.isParent(20, 21, true) == false);
        REQUIRE(pkb.isParent(24, 25, true) == false);
        REQUIRE(pkb.isParent(26, 28, true) == false);

        REQUIRE(pkb.isParent(32, 33, true) == false);
        REQUIRE(pkb.isParent(33, 34, true) == false);

        REQUIRE(pkb.isParent(35, 37, true) == false);
        REQUIRE(pkb.isParent(35, 38, true) == false);
    }
}

TEST_CASE("Program 04: Modifies relation") {

    SECTION("Check correct modifies relation for statements") {
        REQUIRE(pkb.getModifiesVariables(1) == generateVarSet("a i p a d j m"));
        REQUIRE(pkb.getModifiesVariables(2) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(3) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(4) == generateVarSet("a i p a d j m"));
        REQUIRE(pkb.getModifiesVariables(5) == generateVarSet("a i p a d j m"));
        REQUIRE(pkb.getModifiesVariables(6) == generateVarSet("i p"));
        REQUIRE(pkb.getModifiesVariables(7) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(8) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(9) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(10) == generateVarSet("i p"));
        REQUIRE(pkb.getModifiesVariables(11) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(12) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(13) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(14) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(15) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(16) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(17) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(18) == generateVarSet("p"));
        REQUIRE(pkb.getModifiesVariables(19) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(20) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(21) == generateVarSet("d"));
        REQUIRE(pkb.getModifiesVariables(22) == generateVarSet("j"));
        REQUIRE(pkb.getModifiesVariables(23) == generateVarSet("m"));
        REQUIRE(pkb.getModifiesVariables(24) == generateVarSet("f"));
        REQUIRE(pkb.getModifiesVariables(25) == generateVarSet("read"));
        REQUIRE(pkb.getModifiesVariables(26) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(27) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(28) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(29) == generateVarSet("j"));
        REQUIRE(pkb.getModifiesVariables(30) == generateVarSet("procedure"));
        REQUIRE(pkb.getModifiesVariables(31) == generateVarSet("then print"));
        REQUIRE(pkb.getModifiesVariables(32) == generateVarSet("then"));
        REQUIRE(pkb.getModifiesVariables(33) == generateVarSet("print"));
        REQUIRE(pkb.getModifiesVariables(34) == generateVarSet(""));

        REQUIRE(pkb.getModifiesVariables(35) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(36) == generateVarSet("k"));
        REQUIRE(pkb.getModifiesVariables(37) == generateVarSet("check"));
        REQUIRE(pkb.getModifiesVariables(38) == generateVarSet("a i p a d j m f read a j procedure then print"));
    }

    SECTION("Check correct modifies relation for procedures") {
        REQUIRE(pkb.getModifiesVariableFromProc("procedure") == generateVarSet("a i p a d j m f read a j procedure then print"));
        REQUIRE(pkb.getModifiesVariableFromProc("test2") ==
                generateVarSet("a i p a d j m f read a j procedure then print a k check"));
    }
}

TEST_CASE("Program 04: Uses relation") {

    SECTION("Check correct uses relation for statements") {
        REQUIRE(
            pkb.getUsesVariables(1) ==
            generateVarSet("a b c b d a c a f g g a b k c g d f c a b a b i j q b c d e f g b g k f k e h e j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(2) == generateVarSet("a"));
        REQUIRE(pkb.getUsesVariables(3) == generateVarSet(""));
        REQUIRE(
            pkb.getUsesVariables(4) ==
            generateVarSet("a b c b d a c a f g g a b k c g d f c a b a b i j q b c d e f g b g k f k e h e j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(5) ==
                generateVarSet("b c b d a c a f g g a b k c g d f c a b a b i j q b c d e f g b g k f k e h e j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(6) == generateVarSet("b d a c a f g g a b k c g d f c a b a b i j q"));
        REQUIRE(pkb.getUsesVariables(7) == generateVarSet("d a c a"));
        REQUIRE(pkb.getUsesVariables(8) == generateVarSet("c a"));
        REQUIRE(pkb.getUsesVariables(9) == generateVarSet("a"));
        REQUIRE(pkb.getUsesVariables(10) == generateVarSet("f g g a b k c g d f c a b a b i j q"));
        REQUIRE(pkb.getUsesVariables(11) == generateVarSet("g a b k c g d f c a b a b i j"));
        REQUIRE(pkb.getUsesVariables(12) == generateVarSet("c g d f c a b a b i j"));
        REQUIRE(pkb.getUsesVariables(13) == generateVarSet("b a b i j"));
        REQUIRE(pkb.getUsesVariables(14) == generateVarSet("i j"));
        REQUIRE(pkb.getUsesVariables(15) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(16) == generateVarSet("j"));
        REQUIRE(pkb.getUsesVariables(17) == generateVarSet("j"));
        REQUIRE(pkb.getUsesVariables(18) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(19) == generateVarSet("q"));
        REQUIRE(pkb.getUsesVariables(20) == generateVarSet("b c d e f g"));
        REQUIRE(pkb.getUsesVariables(21) == generateVarSet("b g k f"));
        REQUIRE(pkb.getUsesVariables(22) == generateVarSet("k e h e"));
        REQUIRE(pkb.getUsesVariables(23) == generateVarSet("j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(24) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(25) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(26) == generateVarSet("l"));
        REQUIRE(pkb.getUsesVariables(27) == generateVarSet("h k d"));
        REQUIRE(pkb.getUsesVariables(28) == generateVarSet("x"));
        REQUIRE(pkb.getUsesVariables(29) == generateVarSet("while"));
        REQUIRE(pkb.getUsesVariables(30) == generateVarSet("if"));
        REQUIRE(pkb.getUsesVariables(31) == generateVarSet("procedure statement read print call while if k h j"));
        REQUIRE(pkb.getUsesVariables(32) == generateVarSet("read"));
        REQUIRE(pkb.getUsesVariables(33) == generateVarSet("call k h j"));
        REQUIRE(pkb.getUsesVariables(34) == generateVarSet("print"));

        REQUIRE(pkb.getUsesVariables(35) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(36) == generateVarSet("h"));
        REQUIRE(pkb.getUsesVariables(37) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(38) ==
                generateVarSet("a b c d e f g h i j k l m q x while procedure statement read print call if"));
    }

    SECTION("Check correct uses relation for procedures") {
        REQUIRE(pkb.getUsesVariableFromProc("procedure") ==
                generateVarSet("a b c d e f g h i j k l m q x while procedure statement read print call if"));
        REQUIRE(pkb.getUsesVariableFromProc("test2") ==
                generateVarSet("a b c d e f g h i j k l m q x while procedure statement read print call if"));
    }
}

TEST_CASE("Program 04: Calls relation") {
    SECTION("Check correct calls relation for statements") {
        REQUIRE(pkb.getProceduresCalledBy("procedure") == generateVarSet(""));
        REQUIRE(pkb.getProceduresCalledBy("test2") == generateVarSet("procedure"));
    }
}

TEST_CASE("Program 04: Next relation") {

    SECTION("Check correct next relation from CFG") {
        REQUIRE(pkb.isNext(1, 2));
        REQUIRE(pkb.isNext(1, 4));
        REQUIRE(pkb.isNext(2, 3));
        REQUIRE(pkb.isNext(2, 24));
        REQUIRE(pkb.isNext(3, 2));
        REQUIRE(pkb.isNext(4, 5));
        REQUIRE(pkb.isNext(4, 24));
        REQUIRE(pkb.isNext(5, 6));
        REQUIRE(pkb.isNext(5, 20));
        REQUIRE(pkb.isNext(6, 7));
        REQUIRE(pkb.isNext(6, 10));
        REQUIRE(pkb.isNext(7, 4));
        REQUIRE(pkb.isNext(7, 8));
        REQUIRE(pkb.isNext(8, 9));
        REQUIRE(pkb.isNext(8, 7));
        REQUIRE(pkb.isNext(9, 8));
        REQUIRE(pkb.isNext(10, 11));
        REQUIRE(pkb.isNext(10, 18));
        REQUIRE(pkb.isNext(11, 4));
        REQUIRE(pkb.isNext(11, 12));
        REQUIRE(pkb.isNext(12, 13));
        REQUIRE(pkb.isNext(12, 11));
        REQUIRE(pkb.isNext(13, 14));
        REQUIRE(pkb.isNext(14, 15));
        REQUIRE(pkb.isNext(14, 16));
        REQUIRE(pkb.isNext(15, 13));
        REQUIRE(pkb.isNext(18, 19));
        REQUIRE(pkb.isNext(30, 31));

        REQUIRE_FALSE(pkb.isNext(1, 22));
        REQUIRE_FALSE(pkb.isNext(4, 16));
        REQUIRE_FALSE(pkb.isNext(13, 16));
        REQUIRE_FALSE(pkb.isNext(14, 18));
        REQUIRE_FALSE(pkb.isNext(0, 1000));
        REQUIRE_FALSE(pkb.isNext(17, 18));
        REQUIRE_FALSE(pkb.isNext(33, 34));
    }
}

TEST_CASE("Program 04: Populate PKB with Statements - and their attributes") {

    SECTION("Check that PKB is populated with correct variables in a statement") {
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("1")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("2")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("3")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("4")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("5")) == generateVarSet("b c"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("6")) == generateVarSet("b c"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("7")) == generateVarSet("a c d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("8")) == generateVarSet("c"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("9")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("10")) == generateVarSet("f g"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("11")) == generateVarSet("g a b k"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("12")) == generateVarSet("c g d f c a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("13")) == generateVarSet("a b"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("14")) == generateVarSet("i j"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("15")) == generateVarSet("i"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("16")) == generateVarSet(""));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("17")) == generateVarSet("j"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("18")) == generateVarSet("p"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("19")) == generateVarSet("q"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("20")) == generateVarSet("a b c d e f g"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("21")) == generateVarSet("d b g k f"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("22")) == generateVarSet("j k e h"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("23")) == generateVarSet("m j h i j k f a k m d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("24")) == generateVarSet("f"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("25")) == generateVarSet("read"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("26")) == generateVarSet("l"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("27")) == generateVarSet("a h k d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("28")) == generateVarSet("x"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("29")) == generateVarSet("j while"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("30")) == generateVarSet("procedure if"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("31")) ==
                generateVarSet("if procedure statement read print call while if k"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("32")) == generateVarSet("then read"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("33")) == generateVarSet("print call k h j"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("34")) == generateVarSet("print"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("35")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("36")) == generateVarSet("k h"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("37")) == generateVarSet("check"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("38")) == generateVarSet(""));
    }
}

TEST_CASE("Program 04: Populate PKB with Assignment pattern") {

    SECTION("Check that assignment statements have the correct pattern stored") {
        REQUIRE(pkb.getStatementsWithPattern("a", "b + c - d * e / f % g", true, true) == generateStmtNumSet("20"));
        REQUIRE(pkb.getStatementsWithPattern("d", "b * ( ( g * k ) / f )", true, true) == generateStmtNumSet("21"));
        REQUIRE(pkb.getStatementsWithPattern("j", "( ( k * e ) + h ) / e", true, true) == generateStmtNumSet("22"));
        REQUIRE(pkb.getStatementsWithPattern("m", "j + ( h - ( i * ( j - k ) / ( f % a ) - k ) + m ) * d", true, true) ==
                generateStmtNumSet("23"));
        REQUIRE(pkb.getStatementsWithPattern("a", "h * k + d * ( k )", true, true) == generateStmtNumSet("27"));
        REQUIRE(pkb.getStatementsWithPattern("procedure", "341242 * if + 8248", true, true) == generateStmtNumSet("30"));
        REQUIRE(pkb.getStatementsWithPattern("then", "read", true, true) == generateStmtNumSet("32"));
        REQUIRE(pkb.getStatementsWithPattern("print", "call * k + h - h % j", true, true) == generateStmtNumSet("33"));
    }

    SECTION("Check that conditional patterns have the correct pattern stored") {
        REQUIRE(pkb.getVarMatchConditionStatements("a", WHILE_STMT) == generateStmtNumSet("2 4 7 11 12 13"));
        REQUIRE(pkb.getVarMatchConditionStatements("b", WHILE_STMT) == generateStmtNumSet("11 13"));
        REQUIRE(pkb.getVarMatchConditionStatements("c", WHILE_STMT) == generateStmtNumSet("7 8 12"));
        REQUIRE(pkb.getVarMatchConditionStatements("d", WHILE_STMT) == generateStmtNumSet("7 12"));
        REQUIRE(pkb.getVarMatchConditionStatements("f", WHILE_STMT) == generateStmtNumSet("12"));
        REQUIRE(pkb.getVarMatchConditionStatements("g", WHILE_STMT) == generateStmtNumSet("11 12"));
        REQUIRE(pkb.getVarMatchConditionStatements("k", WHILE_STMT) == generateStmtNumSet("11"));

        REQUIRE(pkb.getVarMatchConditionStatements("a", IF_STMT) == generateStmtNumSet("1"));
        REQUIRE(pkb.getVarMatchConditionStatements("b", IF_STMT) == generateStmtNumSet("5 6"));
        REQUIRE(pkb.getVarMatchConditionStatements("c", IF_STMT) == generateStmtNumSet("5 6"));
        REQUIRE(pkb.getVarMatchConditionStatements("f", IF_STMT) == generateStmtNumSet("10"));
        REQUIRE(pkb.getVarMatchConditionStatements("g", IF_STMT) == generateStmtNumSet("10"));
        REQUIRE(pkb.getVarMatchConditionStatements("i", IF_STMT) == generateStmtNumSet("14"));
        REQUIRE(pkb.getVarMatchConditionStatements("j", IF_STMT) == generateStmtNumSet("14"));
        REQUIRE(pkb.getVarMatchConditionStatements("procedure", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("statement", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("read", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("print", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("call", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("while", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("if", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("k", IF_STMT) == generateStmtNumSet("31"));
    }
}

TEST_CASE("Program 04: Test Affect Storage") {
    // Affects(stmt#, stmt#)
    REQUIRE(pkb.isAffects(20, 23));
    REQUIRE(pkb.isAffects(21, 23));
    REQUIRE(pkb.isAffects(22, 23));
    REQUIRE(pkb.isAffects(23, 23));
    REQUIRE(pkb.isAffects(29, 33));
    REQUIRE(pkb.isAffects(21, 20));
    REQUIRE(pkb.isAffects(21, 27));

    REQUIRE_FALSE(pkb.isAffects(22, 22));
    REQUIRE_FALSE(pkb.isAffects(27, 29));

    // Affects*(stmt#, stmt#)
    REQUIRE_FALSE(pkb.isAffects(20, 33, true));
    REQUIRE_FALSE(pkb.isAffects(21, 33, true));
    REQUIRE_FALSE(pkb.isAffects(-1, 100, true));

    // Affects(syn, stmt#)
    REQUIRE(pkb.getStatementsThatAffects(23) == std::unordered_set<int>{20, 21, 22, 23});
    REQUIRE(pkb.getStatementsThatAffects(33) == std::unordered_set<int>{29});
    REQUIRE(pkb.getStatementsThatAffects(29).empty());
    REQUIRE(pkb.getStatementsThatAffects(-1).empty());
    REQUIRE(pkb.getStatementsThatAffects(100).empty());

    // Affects*(syn, stmt#)
    REQUIRE(pkb.getStatementsThatAffectsT(23) == std::unordered_set<int>{20, 21, 22, 23});
    REQUIRE(pkb.getStatementsThatAffectsT(29).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(-1).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(100).empty());

    // Affects(stmt#, syn)
    REQUIRE(pkb.getStatementsAffectedBy(20) == std::unordered_set<int>{23});
    REQUIRE(pkb.getStatementsAffectedBy(21) == std::unordered_set<int>{23, 20, 27});
    REQUIRE(pkb.getStatementsAffectedBy(22) == std::unordered_set<int>{23});

    // Affects*(stmt#, syn)
    REQUIRE(pkb.getStatementsAffectedByT(20) == std::unordered_set<int>{23});
    REQUIRE(pkb.getStatementsAffectedByT(21) == std::unordered_set<int>{23, 20, 27});
    REQUIRE(pkb.getStatementsAffectedByT(33).empty());
    REQUIRE(pkb.getStatementsAffectedByT(-1).empty());
    REQUIRE(pkb.getStatementsAffectedByT(100).empty());

    // Affects(_, _)
    REQUIRE(pkb.containsAffectsRelation());

    // Affects(syn, _)
    REQUIRE(pkb.getStatementsThatAffects() == std::unordered_set<int>{20, 21, 22, 23, 29});

    // Affects(_, syn)
    REQUIRE(pkb.getStatementsAffectedBy() == std::unordered_set<int>{23, 33, 20, 27});

    // Affects(syn, syn)
    REQUIRE(pkb.getAffectsRelationPairings() ==
            IntPairResultSet{std::make_pair(23, 23), std::make_pair(20, 23), std::make_pair(21, 23), std::make_pair(22, 23),
                             std::make_pair(29, 33), std::make_pair(21, 20), std::make_pair(21, 27)});

    // Affects*(syn, syn)
    REQUIRE(pkb.getAffectsRelationPairingsT() ==
            IntPairResultSet{std::make_pair(23, 23), std::make_pair(20, 23), std::make_pair(21, 23), std::make_pair(22, 23),
                             std::make_pair(29, 33), std::make_pair(21, 20), std::make_pair(21, 27)});
}

TEST_CASE("Program 04: PKB Teardown") {
    pkb.clear();
}

TEST_CASE("Program 05: Populate PKB") {
    std::string filename = "./SIMPLESourceFiles/Program05.txt";
    FrontEnd frontend;

    CHECK_NOTHROW(frontend.start(filename));
}

TEST_CASE("Program 05: Test Follows Storage") {
    // Test Follows(stmt#, stmt#)
    REQUIRE(pkb.isFollows(11, 16) == true);
    REQUIRE(pkb.isFollows(2, 31) == false);
    REQUIRE(pkb.isFollows(1000, 2) == false);
    REQUIRE(pkb.isFollows(0, 1) == false);

    // Test Follows*(stmt#, stmt#)
    REQUIRE(pkb.isFollows(2, 31, true) == true);
    REQUIRE(pkb.isFollows(1, 33, true) == false);
    REQUIRE(pkb.isFollows(1000, 2, true) == false);
    REQUIRE(pkb.isFollows(0, 1, true) == false);

    // Test Follows(syn, stmt#)
    REQUIRE(pkb.isFollows(PRINT, 19) == true);
    REQUIRE(pkb.isFollows(IF_STMT, 12) == false);
    REQUIRE(pkb.isFollows(ASSIGN, 33) == false);

    // Test Follows*(syn, stmt#)
    REQUIRE(pkb.isFollows(WHILE_STMT, 22, true) == true);
    REQUIRE(pkb.isFollows(IF_STMT, 33, true) == false);
    REQUIRE(pkb.isFollows(ASSIGN, 100, true) == false);

    // Test Follows(stmt#, syn)
    REQUIRE(pkb.isFollows(20, ASSIGN) == true);
    REQUIRE(pkb.isFollows(31, READ) == false);
    REQUIRE(pkb.isFollows(100, ASSIGN) == false);

    // Test Follows*(stmt#, syn)
    REQUIRE(pkb.isFollows(1, READ, true) == true);
    REQUIRE(pkb.isFollows(11, IF_STMT, true) == false);
    REQUIRE(pkb.isFollows(100, READ, true) == false);

    // Test Follows(syn, syn)
    REQUIRE(pkb.isFollows(WHILE_STMT, ASSIGN) == true);
    REQUIRE(pkb.isFollows(WHILE_STMT, READ) == false);

    // Test Follows*(syn, syn)
    REQUIRE(pkb.isFollows(WHILE_STMT, WHILE_STMT, true) == true);
    REQUIRE(pkb.isFollows(IF_STMT, IF_STMT, true) == false);

    // Test Select syn such that Follows(stmt#, syn)
    REQUIRE(pkb.getFollowingStatements(1) == std::unordered_set<int>{2});
    REQUIRE(pkb.getFollowingStatements(1, ASSIGN).empty());
    REQUIRE(pkb.getFollowingStatements(100).empty());

    // Test Select syn such that Follows*(stmt#, syn)
    REQUIRE(pkb.getFollowingStatementsT(1) == std::unordered_set<int>{2, 11, 16, 17, 18, 19, 20, 21, 22, 31});
    REQUIRE(pkb.getFollowingStatementsT(1, WHILE_STMT) == std::unordered_set<int>{2, 22});
    REQUIRE(pkb.getFollowingStatementsT(19, READ).empty());
    REQUIRE(pkb.getFollowingStatementsT(100).empty());
    REQUIRE(pkb.getFollowingStatementsT(100, READ).empty());

    // Test Select syn2 such that Follows(syn1, syn2)
    REQUIRE(pkb.getFollowingStatements(WHILE_STMT) == std::unordered_set<int>{11, 31});
    REQUIRE(pkb.getFollowingStatements(WHILE_STMT, IF_STMT) == std::unordered_set<int>{11});
    REQUIRE(pkb.getFollowingStatements(WHILE_STMT, READ).empty());

    // Test Select syn2 such that Follows*(syn1, syn2)
    REQUIRE(pkb.getFollowingStatementsT(IF_STMT) == std::unordered_set<int>{9, 10, 16, 17, 18, 19, 20, 21, 22, 31});
    REQUIRE(pkb.getFollowingStatementsT(IF_STMT, ASSIGN) == std::unordered_set<int>{9, 10, 20, 21, 31});
    REQUIRE(pkb.getFollowingStatementsT(IF_STMT, IF_STMT).empty());

    // Test Select syn such that Follows(syn, stmt#)
    REQUIRE(pkb.getLeadingStatements(11) == std::unordered_set<int>{2});
    REQUIRE(pkb.getLeadingStatements(11, WHILE_STMT) == std::unordered_set<int>{2});
    REQUIRE(pkb.getLeadingStatements(11, READ).empty());
    REQUIRE(pkb.getLeadingStatements(1).empty());
    REQUIRE(pkb.getLeadingStatements(100).empty());

    // Test Select syn such that Follows(syn, stmt#)
    REQUIRE(pkb.getLeadingStatementsT(11) == std::unordered_set<int>{1, 2});
    REQUIRE(pkb.getLeadingStatementsT(11, ASSIGN) == std::unordered_set<int>{1});
    REQUIRE(pkb.getLeadingStatementsT(15, READ).empty());
    REQUIRE(pkb.getLeadingStatementsT(1).empty());
    REQUIRE(pkb.getLeadingStatementsT(100).empty());

    // Test Select syn1 such that Follows(syn1, syn2)
    REQUIRE(pkb.getLeadingStatements(PRINT) == std::unordered_set<int>{11, 12, 14, 17, 32});
    REQUIRE(pkb.getLeadingStatements(PRINT, IF_STMT) == std::unordered_set<int>{11});
    REQUIRE(pkb.getLeadingStatements(PRINT, WHILE_STMT).empty());

    // Test Select syn1 such that Follows(syn1, syn2)
    REQUIRE(pkb.getLeadingStatementsT(PRINT) == std::unordered_set<int>{1, 2, 11, 12, 14, 16, 17, 32});
    REQUIRE(pkb.getLeadingStatementsT(PRINT, ASSIGN) == std::unordered_set<int>{1, 12, 14});
    REQUIRE(pkb.getLeadingStatementsT(IF_STMT, IF_STMT).empty());
}

TEST_CASE("Program 05: Test Parent Storage") {
    // Test Parent(stmt#, stmt#)
    REQUIRE(pkb.isParent(2, 4) == true);
    REQUIRE(pkb.isParent(2, 8) == false);
    REQUIRE(pkb.isParent(5, 4) == false);
    REQUIRE(pkb.isParent(100, 4) == false);

    // Test Parent*(stmt#, stmt#)
    REQUIRE(pkb.isParent(22, 30, true) == true);
    REQUIRE(pkb.isParent(2, 13, true) == false);
    REQUIRE(pkb.isParent(11, 4, true) == false);
    REQUIRE(pkb.isParent(100, 4, true) == false);

    // Test Parent(stmt#, syn)
    REQUIRE(pkb.isParent(2, IF_STMT) == true);
    REQUIRE(pkb.isParent(24, READ) == false);
    REQUIRE(pkb.isParent(100, ASSIGN) == false);
    REQUIRE(pkb.isParent(1, READ) == false);

    // Test Parent*(stmt#, syn)
    REQUIRE(pkb.isParent(22, ASSIGN, true) == true);
    REQUIRE(pkb.isParent(2, WHILE_STMT, true) == false);
    REQUIRE(pkb.isParent(100, ASSIGN, true) == false);
    REQUIRE(pkb.isParent(2, READ, true) == false);

    // Test Parent(syn, stmt#)
    REQUIRE(pkb.isParent(WHILE_STMT, 9) == true);
    REQUIRE(pkb.isParent(ASSIGN, 4) == false);
    REQUIRE(pkb.isParent(IF_STMT, 100) == false);

    // Test Parent*(syn, stmt#)
    REQUIRE(pkb.isParent(WHILE_STMT, 6, true) == true);
    REQUIRE(pkb.isParent(WHILE_STMT, 22, true) == false);
    REQUIRE(pkb.isParent(ASSIGN, 4, true) == false);
    REQUIRE(pkb.isParent(IF_STMT, 100, true) == false);

    // Test Parent(syn1, syn2)
    REQUIRE(pkb.isParent(WHILE_STMT, PRINT) == true);
    REQUIRE(pkb.isParent(IF_STMT, READ) == false);
    REQUIRE(pkb.isParent(ASSIGN, ASSIGN) == false);
    REQUIRE(pkb.isParent(READ, IF_STMT) == false);

    // Test Parent*(syn1, syn2)
    REQUIRE(pkb.isParent(WHILE_STMT, ASSIGN, true) == true);
    REQUIRE(pkb.isParent(IF_STMT, WHILE_STMT, true) == false);
    REQUIRE(pkb.isParent(READ, IF_STMT, true) == false);

    // Test Select syn such that Parent(syn, stmt#)
    REQUIRE(pkb.getParentStatements(28) == std::unordered_set<int>{27});
    REQUIRE(pkb.getParentStatements(28, WHILE_STMT) == std::unordered_set<int>{27});
    REQUIRE(pkb.getParentStatements(28, IF_STMT).empty());
    REQUIRE(pkb.getParentStatements(1).empty());
    REQUIRE(pkb.getParentStatements(100).empty());

    // Test Select syn such that Parent*(syn, stmt#)
    REQUIRE(pkb.getParentStatementsT(8) == std::unordered_set<int>{2, 4});
    REQUIRE(pkb.getParentStatementsT(13, IF_STMT) == std::unordered_set<int>{11});
    REQUIRE(pkb.getParentStatementsT(13, ASSIGN).empty());
    REQUIRE(pkb.getParentStatementsT(1).empty());
    REQUIRE(pkb.getParentStatementsT(100).empty());

    // Test Select syn1 such that Parent(syn1, syn2)
    REQUIRE(pkb.getParentStatements(PRINT) == std::unordered_set<int>{2, 11});
    REQUIRE(pkb.getParentStatements(PRINT, ASSIGN).empty());

    // Test Select syn1 such that Parent*(syn1, syn2)
    REQUIRE(pkb.getParentStatementsT(ASSIGN) == std::unordered_set<int>{2, 4, 11, 22, 23, 24, 25, 26, 27, 37});
    REQUIRE(pkb.getParentStatementsT(ASSIGN, IF_STMT) == std::unordered_set<int>{4, 11, 37});
    REQUIRE(pkb.getParentStatementsT(READ, WHILE_STMT).empty());
    REQUIRE(pkb.getParentStatementsT(READ, ASSIGN).empty());

    // Test Select syn such that Parent(stmt#, syn)
    REQUIRE(pkb.getChildrenStatements(2) == std::unordered_set<int>{3, 4, 9, 10});
    REQUIRE(pkb.getChildrenStatements(2, IF_STMT) == std::unordered_set<int>{4});
    REQUIRE(pkb.getChildrenStatements(2, READ).empty());
    REQUIRE(pkb.getChildrenStatements(1).empty());
    REQUIRE(pkb.getChildrenStatements(100).empty());

    // Test Select syn such that Parent*(stmt#, syn)
    REQUIRE(pkb.getChildrenStatementsT(22) == std::unordered_set<int>{23, 24, 25, 26, 27, 28, 29, 30});
    REQUIRE(pkb.getChildrenStatementsT(22, ASSIGN) == std::unordered_set<int>{28, 29, 30});
    REQUIRE(pkb.getChildrenStatementsT(22, PRINT).empty());
    REQUIRE(pkb.getChildrenStatementsT(1).empty());
    REQUIRE(pkb.getChildrenStatementsT(100).empty());

    // Test Select syn2 such that Parent(syn1, syn2)
    REQUIRE(pkb.getChildrenStatements(IF_STMT) == std::unordered_set<int>{5, 6, 7, 8, 12, 13, 14, 15, 38, 39});
    REQUIRE(pkb.getChildrenStatements(IF_STMT, PRINT) == std::unordered_set<int>{13, 15});
    REQUIRE(pkb.getChildrenStatements(READ, WHILE_STMT).empty());

    // Test Select syn2 such that Parent*(syn1, syn2)
    REQUIRE(pkb.getChildrenStatementsT(WHILE_STMT) ==
            std::unordered_set<int>{3, 4, 5, 6, 7, 8, 9, 10, 23, 24, 25, 26, 27, 28, 29, 30, 35});
    REQUIRE(pkb.getChildrenStatementsT(WHILE_STMT, ASSIGN) == std::unordered_set<int>{5, 6, 7, 8, 9, 10, 28, 29, 30});
    REQUIRE(pkb.getChildrenStatementsT(WHILE_STMT, READ).empty());
    REQUIRE(pkb.getChildrenStatementsT(READ, WHILE_STMT).empty());
}

TEST_CASE("Program 05: Test Modifies Storage") {
    // Test Modifies(stmt#, var)
    REQUIRE(pkb.isModifies(2, "print") == true);
    REQUIRE(pkb.isModifies(23, "then") == true);
    REQUIRE(pkb.isModifies(11, "if") == false);
    REQUIRE(pkb.isModifies(100, "cenY") == false);

    // Test Modifies(proc, var)
    REQUIRE(pkb.isModifies("main", "if") == true);
    REQUIRE(pkb.isModifies("main", "read") == true);
    REQUIRE(pkb.isModifies("main", "steps") == false);
    REQUIRE(pkb.isModifies("randomProc", "cenY") == false);

    // Test Select v such that Modifies(stmt#, v)
    REQUIRE(pkb.getModifiesVariables(22) == std::unordered_set<std::string>{"if", "then"});
    REQUIRE(pkb.getModifiesVariables(35) == std::unordered_set<std::string>{"ca11", "re4d"});
    REQUIRE(pkb.getModifiesVariables(16).empty());
    REQUIRE(pkb.getModifiesVariables(100).empty());

    // Test Select v such that Modifies(syn, v)
    REQUIRE(pkb.getModifiesVariables(WHILE_STMT) ==
            std::unordered_set<std::string>{"if", "then", "print", "main", "read", "true", "while", "ca11", "re4d"});
    REQUIRE(pkb.getModifiesVariables(PRINT).empty());
    REQUIRE(pkb.getModifiesVariables(100).empty());

    // Test Select syn such that Modifies(syn, _)
    REQUIRE(pkb.getAllModifiesStatement(STMT) == std::unordered_set<int>{1,  2,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                                                         14, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27,
                                                                         28, 29, 30, 31, 32, 34, 35, 37, 38, 39, 40});
    REQUIRE(pkb.getAllModifiesStatement(PRINT).empty());

    // Test Select syn such that Modifies(syn, v)
    REQUIRE(pkb.getModifiesStatement(STMT, "main") == std::unordered_set<int>{1, 2, 4, 6});
    REQUIRE(pkb.getModifiesStatement(IF_STMT, "main") == std::unordered_set<int>{4});
    REQUIRE(pkb.getModifiesStatement(PRINT, "print").empty());
    REQUIRE(pkb.getModifiesStatement(ASSIGN, "unknown").empty());

    // Test Select proc such that Modifies(proc, v)
    REQUIRE(pkb.getModifiesProc("steps") == std::unordered_set<std::string>{"raymarch"});
    REQUIRE(pkb.getModifiesProc("unknown").empty());

    // Test Select proc such that Modifies(proc, _)
    REQUIRE(pkb.getModifiesProc() == std::unordered_set<std::string>{"main", "raymarch", "call", "print", "re4d"});

    // Test Select v such that Modifies(proc, v)
    REQUIRE(pkb.getModifiesVariableFromProc("raymarch") == std::unordered_set<std::string>{"steps"});
    REQUIRE(pkb.getModifiesVariableFromProc("main") == std::unordered_set<std::string>{"main", "print", "read", "true", "while",
                                                                                       "else", "if", "then", "procedure",
                                                                                       "ifthenelse"});
    REQUIRE(pkb.getModifiesVariableFromProc("call") == std::unordered_set<std::string>{"ca11", "re4d"});
    REQUIRE(pkb.getModifiesVariableFromProc("unknown").empty());
}

TEST_CASE("Program 05: Test Uses Storage") {
    // Test Uses(stmt#, v)
    REQUIRE(pkb.isUses(2, "read") == true);
    REQUIRE(pkb.isUses(11, "else") == true);
    REQUIRE(pkb.isUses(100, "cenY") == false);
    REQUIRE(pkb.isUses(-1, "flag") == false);

    // Test Uses(proc, v)
    REQUIRE(pkb.isUses("main", "else") == true);
    REQUIRE(pkb.isUses("raymarch", "depth") == true);
    REQUIRE(pkb.isUses("main", "depth") == false);
    REQUIRE(pkb.isUses("randomProc", "cenY") == false);

    // Test Select v such that Uses(stmt#, v)
    REQUIRE(pkb.getUsesVariables(22) == std::unordered_set<std::string>{"while", "if", "then", "else", "a", "b", "c", "d", "e"});
    REQUIRE(pkb.getUsesVariables(35) == std::unordered_set<std::string>{"print", "call", "ca11"});
    REQUIRE(pkb.getUsesVariables(19).empty());
    REQUIRE(pkb.getUsesVariables(100).empty());

    // Test Select v such that Uses(syn, v)
    REQUIRE(pkb.getUsesVariables(WHILE_STMT) == std::unordered_set<std::string>{"then", "else", "main", "x", "y", "print", "else",
                                                                                "read", "true", "while", "if", "a", "b", "c", "d",
                                                                                "e", "ca11", "call"});
    REQUIRE(pkb.getUsesVariables(READ).empty());
    REQUIRE(pkb.getUsesVariables(100).empty());

    // Test Select syn such that Uses(syn, _)
    REQUIRE(pkb.getAllUsesStatement(STMT) == std::unordered_set<int>{2,  3,  4,  6,  7,  8,  10, 11, 13, 15, 16, 18, 20, 21, 22,
                                                                     23, 24, 25, 26, 27, 28, 29, 30, 31, 33, 34, 35, 36, 37, 38});
    REQUIRE(pkb.getAllUsesStatement(READ).empty());

    // Test Select syn such that Uses(syn, v)
    REQUIRE(pkb.getUsesStatement(STMT, "else") == std::unordered_set<int>{2, 4, 6, 11, 13, 22, 23, 24, 25, 26, 27, 29, 30});
    REQUIRE(pkb.getUsesStatement(IF_STMT, "else") == std::unordered_set<int>{4, 11});
    REQUIRE(pkb.getUsesStatement(PRINT, "steps").empty());
    REQUIRE(pkb.getUsesStatement(ASSIGN, "unknown").empty());

    // Test Select proc such that Uses(proc, v)
    REQUIRE(pkb.getUsesProc("else") == std::unordered_set<std::string>{"main"});
    REQUIRE(pkb.getUsesProc("unknown").empty());

    // Test Select proc such that Uses(proc, _)
    REQUIRE(pkb.getUsesProc() == std::unordered_set<std::string>{"main", "raymarch", "call", "print"});

    // Test Select v such that Uses(proc, v)
    REQUIRE(pkb.getUsesVariableFromProc("raymarch") == std::unordered_set<std::string>{"depth"});
    REQUIRE(pkb.getUsesVariableFromProc("main") == std::unordered_set<std::string>{"main", "print", "else", "read", "true", "x",
                                                                                   "y", "ifthenelse", "while", "if", "then",
                                                                                   "else", "a", "b", "c", "d", "e"});
    REQUIRE(pkb.getUsesVariableFromProc("call") == std::unordered_set<std::string>{"call", "print", "ca11"});
    REQUIRE(pkb.getUsesVariableFromProc("unknown").empty());
}

TEST_CASE("Program 05: Test General Storage") {
    // Test Select v
    REQUIRE(pkb.getAllVar() == std::unordered_set<std::string>{"x",     "y",          "main",      "print", "if",   "then",
                                                               "else",  "ifthenelse", "procedure", "while", "read", "true",
                                                               "steps", "depth",      "a",         "b",     "c",    "d",
                                                               "e",     "call",       "ca11",      "re4d"});

    // Test Select proc
    REQUIRE(pkb.getAllProc() == std::unordered_set<std::string>{"main", "raymarch", "call", "print", "re4d"});

    // Test Select syn
    REQUIRE(pkb.getAllStatements(IF_STMT) == std::unordered_set<int>{4, 11, 37});

    // Test get variables from statements
    REQUIRE(pkb.getAllVarsInStatement(std::unordered_set<int>{22}) ==
            std::unordered_set<std::string>{"while", "if", "then", "else"});
    REQUIRE(pkb.getAllVarsInStatement(std::unordered_set<int>{32, 33}) == std::unordered_set<std::string>{"steps", "depth"});
    REQUIRE(pkb.getAllVarsInStatement(std::unordered_set<int>{1000}).empty());

    // Test get statements from variables
    REQUIRE(pkb.getStatmentNumberFromVars(std::unordered_set<std::string>{"if", "then"}) ==
            std::unordered_set<int>{22, 28, 29, 30});
    REQUIRE(pkb.getStatmentNumberFromVars(std::unordered_set<std::string>{"unknown"}).empty());
    REQUIRE(pkb.getAllConstants() == std::unordered_set<int>{0, 1, 2, 10, 17, 100});
}

TEST_CASE("Program 05: Test Pattern Storage") {
    REQUIRE(pkb.getStatementsWithPattern("procedure", "main + 2", true, true) == std::unordered_set<int>{31});
    REQUIRE(pkb.getStatementsWithPattern("(main) + (2)", false, false) == std::unordered_set<int>{31});
    REQUIRE(pkb.getStatementsWithPattern("cenX * cenX", true, true).empty());

    REQUIRE(pkb.getConditionStatementsWithAnyVar(IF_STMT) == std::unordered_set<int>{4, 11, 37});
    REQUIRE(pkb.getConditionStatementsWithAnyVar(ASSIGN).empty());
    REQUIRE(pkb.getVarMatchConditionStatements("call", WHILE_STMT) == std::unordered_set<int>{34});
    REQUIRE(pkb.getVarMatchConditionStatements("e", WHILE_STMT) == std::unordered_set<int>{27});
    REQUIRE(pkb.getVarMatchConditionStatements("unknown", WHILE_STMT).empty());
    REQUIRE(pkb.getVarMatchConditionStatementPairs(IF_STMT) ==
            IntStringPairResultSet{std::make_pair(4, "main"), std::make_pair(4, "x"), std::make_pair(37, "print"),
                                   std::make_pair(37, "call"), std::make_pair(37, "ca11")});
    REQUIRE(pkb.getVarMatchConditionStatementPairs(WHILE_STMT) ==
            IntStringPairResultSet{std::make_pair(2, "main"), std::make_pair(2, "x"), std::make_pair(2, "x"),
                                   std::make_pair(2, "y"), std::make_pair(22, "if"), std::make_pair(22, "while"),
                                   std::make_pair(22, "then"), std::make_pair(22, "else"), std::make_pair(23, "a"),
                                   std::make_pair(24, "b"), std::make_pair(25, "c"), std::make_pair(26, "d"),
                                   std::make_pair(27, "e"), std::make_pair(34, "call")});
    REQUIRE(pkb.getVarMatchConditionStatementPairs(ASSIGN).empty());
}

TEST_CASE("Program 05: Test Next Storage") {
    // Test Next relation for Next(stmt#, _)
    REQUIRE(pkb.hasNext(22));
    REQUIRE(pkb.hasNext(32));
    REQUIRE(pkb.hasNext(30));
    REQUIRE_FALSE(pkb.hasNext(31));
    REQUIRE_FALSE(pkb.hasNext(33));
    REQUIRE_FALSE(pkb.hasNext(40));
    REQUIRE_FALSE(pkb.hasNext(39));
    REQUIRE_FALSE(pkb.hasNext(0));
    REQUIRE_FALSE(pkb.hasNext(10000));

    // Test Next relation for Next(syn, _)
    REQUIRE(pkb.hasNext(ASSIGN));
    REQUIRE(pkb.hasNext(WHILE_STMT));
    REQUIRE(pkb.hasNext(STMT));
    REQUIRE(pkb.hasNext(PRINT));
    REQUIRE(pkb.hasNext(IF_STMT));
    REQUIRE(pkb.hasNext(CALL));

    // Test Next relation for Next(_, stmt#)
    REQUIRE(pkb.hasPrevious(24));
    REQUIRE(pkb.hasPrevious(34));
    REQUIRE(pkb.hasPrevious(11));
    REQUIRE_FALSE(pkb.hasPrevious(32));
    REQUIRE_FALSE(pkb.hasPrevious(40));
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
    REQUIRE(pkb.isNext(32, 33));
    REQUIRE(pkb.isNext(22, 23));
    REQUIRE(pkb.isNext(30, 27));
    REQUIRE(pkb.isNext(27, 26));
    REQUIRE_FALSE(pkb.isNext(1, 22));
    REQUIRE_FALSE(pkb.isNext(6, 7));
    REQUIRE_FALSE(pkb.isNext(5, 7));
    REQUIRE_FALSE(pkb.isNext(33, 34));
    REQUIRE_FALSE(pkb.isNext(39, 40));
    REQUIRE_FALSE(pkb.isNext(0, 1000));

    // Test Next* relation for Next*(stmt#1, stmt#2)
    REQUIRE(pkb.isNext(1, 31, true));
    REQUIRE(pkb.isNext(22, 23, true));
    REQUIRE(pkb.isNext(35, 34, true));
    REQUIRE(pkb.isNext(5, 7, true));
    REQUIRE(pkb.isNext(5, 4, true));
    REQUIRE(pkb.isNext(30, 22, true));
    REQUIRE_FALSE(pkb.isNext(12, 15, true));
    REQUIRE_FALSE(pkb.isNext(16, 2, true));
    REQUIRE_FALSE(pkb.isNext(30, 18, true));
    REQUIRE_FALSE(pkb.isNext(33, 34, true));
    REQUIRE_FALSE(pkb.isNext(0, 1000, true));

    // Test Next relation for syn such that Next(stmt#, syn)
    REQUIRE(pkb.getNextStatements(37) == std::unordered_set<int>{38, 39});
    REQUIRE(pkb.getNextStatements(37, CALL) == std::unordered_set<int>{39});
    REQUIRE(pkb.getNextStatements(23) == std::unordered_set<int>{24, 22});
    REQUIRE(pkb.getNextStatements(23, WHILE_STMT) == std::unordered_set<int>{24, 22});
    REQUIRE(pkb.getNextStatements(40).empty());
    REQUIRE(pkb.getNextStatements(31).empty());
    REQUIRE(pkb.getNextStatements(0).empty());
    REQUIRE(pkb.getNextStatements(1000).empty());

    // Test Next relation for syn such that Next*(stmt#, syn)
    REQUIRE(pkb.getNextStatementsT(1) == std::unordered_set<int>{2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                                                                 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31});
    REQUIRE(pkb.getNextStatementsT(1, PRINT) == std::unordered_set<int>{3, 13, 15, 16, 18});
    REQUIRE(pkb.getNextStatementsT(12) ==
            std::unordered_set<int>{13, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31});
    REQUIRE(pkb.getNextStatementsT(10) == std::unordered_set<int>{2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                                                                  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31});
    REQUIRE(pkb.getNextStatementsT(31).empty());
    REQUIRE(pkb.getNextStatementsT(38).empty());
    REQUIRE(pkb.getNextStatementsT(0).empty());
    REQUIRE(pkb.getNextStatementsT(1000).empty());

    // Test Next relation for syn such that Next(syn, stmt#)
    REQUIRE(pkb.getPreviousStatements(27) == std::unordered_set<int>{26, 30});
    REQUIRE(pkb.getPreviousStatements(10, ASSIGN) == std::unordered_set<int>{9});
    REQUIRE(pkb.getPreviousStatements(34) == std::unordered_set<int>{35});
    REQUIRE(pkb.getPreviousStatements(34, CALL) == std::unordered_set<int>{35});
    REQUIRE(pkb.getPreviousStatements(1).empty());
    REQUIRE(pkb.getPreviousStatements(36).empty());
    REQUIRE(pkb.getPreviousStatements(0).empty());
    REQUIRE(pkb.getPreviousStatements(1000).empty());

    // Test Next relation for syn such that Next*(syn, stmt#)
    REQUIRE(pkb.getPreviousStatementsT(31) == std::unordered_set<int>{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                                                      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                                                      21, 22, 23, 24, 25, 26, 27, 28, 29, 30});
    REQUIRE(pkb.getPreviousStatementsT(31, READ) == std::unordered_set<int>{17, 19});
    REQUIRE(pkb.getPreviousStatementsT(3) == std::unordered_set<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    REQUIRE(pkb.getPreviousStatementsT(15) == std::unordered_set<int>{14, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1});
    REQUIRE(pkb.getPreviousStatementsT(15, READ).empty());
    REQUIRE(pkb.getPreviousStatementsT(1).empty());
    REQUIRE(pkb.getPreviousStatementsT(32).empty());
    REQUIRE(pkb.getPreviousStatementsT(0).empty());
    REQUIRE(pkb.getPreviousStatementsT(1000).empty());

    // Test Next relation for syn such that Next(_, syn) or Next*(_, syn)
    REQUIRE(pkb.getNextStatements(ASSIGN) == std::unordered_set<int>{5, 6, 7, 8, 9, 10, 12, 14, 20, 21, 28, 29, 30, 31, 38});
    REQUIRE(pkb.getNextStatements(IF_STMT) == std::unordered_set<int>{4, 11, 37});
    REQUIRE(pkb.getNextStatements(READ) == std::unordered_set<int>{17, 19});
    REQUIRE(pkb.getNextStatements(PRINT) == std::unordered_set<int>{3, 13, 15, 16, 18, 33});

    // Test Next relation for syn such that Next(syn, _) or Next*(syn, _)
    REQUIRE(pkb.getPreviousStatements(ASSIGN) == std::unordered_set<int>{1, 5, 6, 7, 8, 9, 10, 12, 14, 20, 21, 28, 29, 30});
    REQUIRE(pkb.getPreviousStatements(IF_STMT) == std::unordered_set<int>{4, 11, 37});
    REQUIRE(pkb.getPreviousStatements(READ) == std::unordered_set<int>{17, 19, 32});
    REQUIRE(pkb.getPreviousStatements(PRINT) == std::unordered_set<int>{3, 13, 15, 16, 18, 36});

    // Test Next relation for Next(syn1, syn2)
    REQUIRE(pkb.getNextRelationPairings(ASSIGN, ASSIGN) ==
            IntPairResultSet{std::make_pair(5, 6), std::make_pair(7, 8), std::make_pair(6, 9), std::make_pair(8, 9),
                             std::make_pair(9, 10), std::make_pair(20, 21), std::make_pair(28, 29), std::make_pair(29, 30)});
    REQUIRE(pkb.getNextRelationPairings(ASSIGN, WHILE_STMT) ==
            IntPairResultSet{std::make_pair(30, 27), std::make_pair(10, 2), std::make_pair(1, 2), std::make_pair(21, 22)});

    // Test Next relation for Next*(syn1, syn2)
    REQUIRE(pkb.getNextRelationPairingsT(ASSIGN, ASSIGN) ==
            IntPairResultSet{std::make_pair(1, 5),   std::make_pair(1, 6),   std::make_pair(1, 7),   std::make_pair(1, 8),
                             std::make_pair(1, 9),   std::make_pair(1, 10),  std::make_pair(1, 12),  std::make_pair(1, 14),
                             std::make_pair(1, 20),  std::make_pair(1, 21),  std::make_pair(1, 28),  std::make_pair(1, 29),
                             std::make_pair(1, 30),  std::make_pair(1, 31),  std::make_pair(5, 5),   std::make_pair(5, 6),
                             std::make_pair(5, 7),   std::make_pair(5, 8),   std::make_pair(5, 9),   std::make_pair(5, 10),
                             std::make_pair(5, 12),  std::make_pair(5, 14),  std::make_pair(5, 20),  std::make_pair(5, 21),
                             std::make_pair(5, 28),  std::make_pair(5, 29),  std::make_pair(5, 30),  std::make_pair(5, 31),
                             std::make_pair(6, 5),   std::make_pair(6, 6),   std::make_pair(6, 7),   std::make_pair(6, 8),
                             std::make_pair(6, 9),   std::make_pair(6, 10),  std::make_pair(6, 12),  std::make_pair(6, 14),
                             std::make_pair(6, 20),  std::make_pair(6, 21),  std::make_pair(6, 28),  std::make_pair(6, 29),
                             std::make_pair(6, 30),  std::make_pair(6, 31),  std::make_pair(7, 5),   std::make_pair(7, 6),
                             std::make_pair(7, 7),   std::make_pair(7, 8),   std::make_pair(7, 9),   std::make_pair(7, 10),
                             std::make_pair(7, 12),  std::make_pair(7, 14),  std::make_pair(7, 20),  std::make_pair(7, 21),
                             std::make_pair(7, 28),  std::make_pair(7, 29),  std::make_pair(7, 30),  std::make_pair(7, 31),
                             std::make_pair(8, 5),   std::make_pair(8, 6),   std::make_pair(8, 7),   std::make_pair(6, 8),
                             std::make_pair(8, 8),   std::make_pair(8, 9),   std::make_pair(8, 10),  std::make_pair(8, 12),
                             std::make_pair(8, 14),  std::make_pair(8, 20),  std::make_pair(8, 21),  std::make_pair(8, 28),
                             std::make_pair(8, 29),  std::make_pair(8, 30),  std::make_pair(8, 31),  std::make_pair(9, 5),
                             std::make_pair(9, 6),   std::make_pair(9, 7),   std::make_pair(9, 8),   std::make_pair(9, 9),
                             std::make_pair(9, 10),  std::make_pair(9, 12),  std::make_pair(9, 14),  std::make_pair(9, 20),
                             std::make_pair(9, 21),  std::make_pair(9, 28),  std::make_pair(9, 29),  std::make_pair(9, 30),
                             std::make_pair(9, 31),  std::make_pair(10, 5),  std::make_pair(10, 6),  std::make_pair(10, 7),
                             std::make_pair(10, 8),  std::make_pair(10, 9),  std::make_pair(10, 10), std::make_pair(10, 12),
                             std::make_pair(10, 14), std::make_pair(10, 20), std::make_pair(10, 21), std::make_pair(10, 28),
                             std::make_pair(10, 29), std::make_pair(10, 30), std::make_pair(10, 31), std::make_pair(12, 20),
                             std::make_pair(12, 21), std::make_pair(12, 28), std::make_pair(12, 29), std::make_pair(12, 30),
                             std::make_pair(12, 31), std::make_pair(14, 20), std::make_pair(14, 21), std::make_pair(14, 28),
                             std::make_pair(14, 29), std::make_pair(14, 30), std::make_pair(14, 31), std::make_pair(20, 21),
                             std::make_pair(20, 28), std::make_pair(20, 29), std::make_pair(20, 30), std::make_pair(20, 31),
                             std::make_pair(21, 28), std::make_pair(21, 29), std::make_pair(21, 30), std::make_pair(21, 31),
                             std::make_pair(28, 28), std::make_pair(28, 29), std::make_pair(28, 30), std::make_pair(28, 31),
                             std::make_pair(29, 28), std::make_pair(29, 29), std::make_pair(29, 30), std::make_pair(29, 31),
                             std::make_pair(30, 28), std::make_pair(30, 29), std::make_pair(30, 30), std::make_pair(30, 31)});
    // REQUIRE(pkb.getNextRelationPairingsT(ASSIGN, WHILE_STMT) ==
    //        IntPairResultSet{std::make_pair(1, 4), std::make_pair(1, 10), std::make_pair(2, 4), std::make_pair(2, 10),
    //                         std::make_pair(3, 4), std::make_pair(3, 10), std::make_pair(5, 4), std::make_pair(5, 10),
    //                         std::make_pair(8, 4), std::make_pair(8, 10), std::make_pair(9, 4), std::make_pair(9, 10),
    //                         std::make_pair(13, 4), std::make_pair(13, 10), std::make_pair(14, 4),
    //                         std::make_pair(14, 10)});
}

TEST_CASE("Program 05: Test Call Storage") {
    REQUIRE(pkb.getProceduresCalledBy("call") == std::unordered_set<std::string>{"print"});
    REQUIRE(pkb.getProceduresCalledBy("call", true) == std::unordered_set<std::string>{"print", "re4d"});
    REQUIRE(pkb.getProceduresCalledBy("main", true).empty());
    REQUIRE(pkb.getProceduresCalledBy("unknown", true).empty());
    REQUIRE(pkb.getProceduresCalledBy("unknown").empty());

    REQUIRE(pkb.getProceduresThatCalls("re4d") == std::unordered_set<std::string>{"print"});
    REQUIRE(pkb.getProceduresThatCalls("re4d", true) == std::unordered_set<std::string>{"call", "print"});
    REQUIRE(pkb.getProceduresThatCalls("main", true).empty());
    REQUIRE(pkb.getProceduresThatCalls("unknown", true).empty());
    REQUIRE(pkb.getProceduresThatCalls("unknown").empty());

    REQUIRE(pkb.getCallsRelationPairings() ==
            StringPairResultSet{std::make_pair("call", "print"), std::make_pair("print", "re4d")});
    REQUIRE(pkb.getCallsRelationPairings(true) == StringPairResultSet{std::make_pair("call", "print"),
                                                                      std::make_pair("call", "re4d"),
                                                                      std::make_pair("print", "re4d")});

    REQUIRE(pkb.isCalls("call", "print"));
    REQUIRE(pkb.isCalls("print", "re4d"));
    REQUIRE_FALSE(pkb.isCalls("main", "call"));
    REQUIRE_FALSE(pkb.isCalls("print", "main"));
    REQUIRE_FALSE(pkb.isCalls("unknown", "second"));

    REQUIRE(pkb.isCalls("call", "print", true));
    REQUIRE(pkb.isCalls("call", "re4d", true));
    REQUIRE(pkb.isCalls("print", "re4d", true));
    REQUIRE_FALSE(pkb.isCalls("main", "re4d", true));
    REQUIRE_FALSE(pkb.isCalls("print", "call", true));
    REQUIRE_FALSE(pkb.isCalls("unknown", "second", true));

    REQUIRE(pkb.procedureIsCaller("call"));
    REQUIRE(pkb.procedureIsCaller("print"));
    REQUIRE_FALSE(pkb.procedureIsCaller("re4d"));
    REQUIRE_FALSE(pkb.procedureIsCaller("main"));
    REQUIRE_FALSE(pkb.procedureIsCaller("unknown"));

    REQUIRE(pkb.procedureIsCallee("print"));
    REQUIRE(pkb.procedureIsCallee("re4d"));
    REQUIRE_FALSE(pkb.procedureIsCallee("main"));
    REQUIRE_FALSE(pkb.procedureIsCallee("call"));
    REQUIRE_FALSE(pkb.procedureIsCallee("unknown"));

    REQUIRE(pkb.containsCallRelation());
}

TEST_CASE("Program 05: Test Affect Storage") {
    // Affects(stmt#, stmt#)
    REQUIRE(pkb.isAffects(5, 6));
    REQUIRE(pkb.isAffects(6, 31));
    REQUIRE(pkb.isAffects(7, 8));
    REQUIRE(pkb.isAffects(9, 10));
    REQUIRE(pkb.isAffects(20, 21));
    REQUIRE(pkb.isAffects(29, 28));
    REQUIRE(pkb.isAffects(1, 31));
    REQUIRE(pkb.isAffects(6, 7));
    REQUIRE(pkb.isAffects(1, 7));
    REQUIRE(pkb.isAffects(12, 29));
    REQUIRE(pkb.isAffects(12, 30));

    REQUIRE_FALSE(pkb.isAffects(22, 22));
    REQUIRE_FALSE(pkb.isAffects(14, 15));
    REQUIRE_FALSE(pkb.isAffects(-1, 100));

    // Affects*(stmt#, stmt#)
    REQUIRE(pkb.isAffects(5, 31, true));
    REQUIRE_FALSE(pkb.isAffects(21, 31, true));
    REQUIRE_FALSE(pkb.isAffects(-1, 100, true));

    // Affects(syn, stmt#)
    REQUIRE(pkb.getStatementsThatAffects(31) == std::unordered_set<int>{6, 1});
    REQUIRE(pkb.getStatementsThatAffects(21) == std::unordered_set<int>{20});
    REQUIRE(pkb.getStatementsThatAffects(18).empty());
    REQUIRE(pkb.getStatementsThatAffects(-1).empty());
    REQUIRE(pkb.getStatementsThatAffects(100).empty());

    // Affects*(syn, stmt#)
    REQUIRE(pkb.getStatementsThatAffectsT(31) == std::unordered_set<int>{5, 6, 1});
    REQUIRE(pkb.getStatementsThatAffectsT(21) == std::unordered_set<int>{20});
    REQUIRE(pkb.getStatementsThatAffectsT(18).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(-1).empty());
    REQUIRE(pkb.getStatementsThatAffectsT(100).empty());

    // Affects(stmt#, syn)
    REQUIRE(pkb.getStatementsAffectedBy(20) == std::unordered_set<int>{21});
    REQUIRE(pkb.getStatementsAffectedBy(29) == std::unordered_set<int>{28});
    REQUIRE(pkb.getStatementsAffectedBy(1) == std::unordered_set<int>{31, 7});

    // Affects*(stmt#, syn)
    REQUIRE(pkb.getStatementsAffectedByT(20) == std::unordered_set<int>{21});
    REQUIRE(pkb.getStatementsAffectedByT(5) == std::unordered_set<int>{6, 31, 7, 8});
    REQUIRE(pkb.getStatementsAffectedByT(18).empty());
    REQUIRE(pkb.getStatementsAffectedByT(-1).empty());
    REQUIRE(pkb.getStatementsAffectedByT(100).empty());

    // Affects(_, _)
    REQUIRE(pkb.containsAffectsRelation());

    // Affects(syn, _)
    REQUIRE(pkb.getStatementsThatAffects() == std::unordered_set<int>{1, 5, 6, 7, 9, 20, 29, 12});

    // Affects(_, syn)
    REQUIRE(pkb.getStatementsAffectedBy() == std::unordered_set<int>{6, 31, 8, 10, 21, 28, 31, 7, 30, 29});

    // Affects(syn, syn)
    REQUIRE(pkb.getAffectsRelationPairings() ==
            IntPairResultSet{std::make_pair(5, 6), std::make_pair(6, 31), std::make_pair(7, 8), std::make_pair(9, 10),
                             std::make_pair(20, 21), std::make_pair(29, 28), std::make_pair(1, 31), std::make_pair(6, 7),
                             std::make_pair(1, 7), std::make_pair(12, 29), std::make_pair(12, 30)});

    // Affects*(syn, syn)
    REQUIRE(pkb.getAffectsRelationPairingsT() ==
            IntPairResultSet{std::make_pair(5, 6), std::make_pair(6, 31), std::make_pair(5, 31), std::make_pair(6, 7),
                             std::make_pair(5, 7), std::make_pair(5, 8), std::make_pair(7, 8), std::make_pair(6, 8),
                             std::make_pair(1, 7), std::make_pair(1, 8), std::make_pair(1, 31), std::make_pair(12, 29),
                             std::make_pair(29, 28), std::make_pair(12, 28), std::make_pair(12, 30), std::make_pair(9, 10),
                             std::make_pair(20, 21)});
}

TEST_CASE("Program 05: Clear") {
    pkb.clear();
}
