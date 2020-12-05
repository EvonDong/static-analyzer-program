#pragma once

#include "PatternStorage.h"
#include "catch.hpp"

TEST_CASE("Pattern Storage: getPatternMatchStatement Test with pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "count + 100");
    pStore->setStmtToPostfix(7, "cenX", "cenX + x");
    pStore->setStmtToPostfix(8, "cenY", "cenY + y");
    pStore->setStmtToPostfix(12, "cenX", "cenX / count");
    pStore->setStmtToPostfix(13, "cenY", "cenY / count");
    pStore->setStmtToPostfix(14, "normSq", "cenX * cenX + cenY * cenY");

    REQUIRE(pStore->getPatternMatchStatement("count + 100", true, true) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("count + 1", true, true) == std::unordered_set<int>{});

    REQUIRE(pStore->getPatternMatchStatement("cenY * cenY", true, true) == std::unordered_set<int>{});
    REQUIRE(pStore->getPatternMatchStatement("cenX + cenY * cenY", false, false) == std::unordered_set<int>{});
    REQUIRE(pStore->getPatternMatchStatement("cenX", false, false) == std::unordered_set<int>{7, 12, 14});
    REQUIRE(pStore->getPatternMatchStatement("0", false, false) == std::unordered_set<int>{});
    REQUIRE(pStore->getPatternMatchStatement("10", false, false) == std::unordered_set<int>{});
    REQUIRE(pStore->getPatternMatchStatement("count + 1", false, false) == std::unordered_set<int>{});

    pStore->clear();
}

TEST_CASE("Pattern Storage: getPatternAndVarMatchStatement Test with pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "count + 1");
    pStore->setStmtToPostfix(7, "cenX", "cenX + x");
    pStore->setStmtToPostfix(8, "cenY", "cenY + y");
    pStore->setStmtToPostfix(12, "cenX", "cenX / count");
    pStore->setStmtToPostfix(13, "cenY", "cenY / count");
    pStore->setStmtToPostfix(14, "normSq", "cenX * cenX + cenY * cenY");

    REQUIRE(pStore->getPatternAndVarMatchStatement("normSq", "cenX * cenX", false, false) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternAndVarMatchStatement("count", "", false, false) == std::unordered_set<int>{6});

    pStore->clear();
}

TEST_CASE("Pattern Storage: getPatternAndVarMatchStatement Test with empty pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "count + 1");
    pStore->setStmtToPostfix(7, "cenX", "cenX + x");
    pStore->setStmtToPostfix(8, "cenY", "cenY + y");
    pStore->setStmtToPostfix(12, "cenX", "cenX / count");
    pStore->setStmtToPostfix(13, "cenY", "cenY / count");
    pStore->setStmtToPostfix(14, "normSq", "cenX * cenX + cenY * cenY");

    REQUIRE(pStore->getPatternAndVarMatchStatement("normSq", "", false, false) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternAndVarMatchStatement("count", "", false, false) == std::unordered_set<int>{6});

    pStore->clear();
}

TEST_CASE("Pattern Storage: getPatternMatchStatement Test with empty pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "count + 1");
    pStore->setStmtToPostfix(7, "cenX", "cenX + x");
    pStore->setStmtToPostfix(8, "cenY", "cenY + y");
    pStore->setStmtToPostfix(12, "cenX", "cenX / count");
    pStore->setStmtToPostfix(13, "cenY", "cenY / count");
    pStore->setStmtToPostfix(14, "normSq", "cenX * cenX + cenY * cenY");

    REQUIRE(pStore->getPatternMatchStatement("", false, false) == std::unordered_set<int>{6, 7, 8, 12, 13, 14});
    REQUIRE(pStore->getPatternMatchStatement("   ", false, false) == std::unordered_set<int>{6, 7, 8, 12, 13, 14});
    REQUIRE(pStore->getPatternMatchStatement("", true, true) == std::unordered_set<int>{});
    REQUIRE(pStore->getPatternMatchStatement("   ", true, true) == std::unordered_set<int>{});

    pStore->clear();
}

TEST_CASE("Pattern Storage: getPatternAndVarMatchStatement Test with one var pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "count + 1");
    pStore->setStmtToPostfix(7, "cenX", "cenX + x");
    pStore->setStmtToPostfix(8, "cenY", "cenY + y");
    pStore->setStmtToPostfix(12, "cenX", "cenX / count");
    pStore->setStmtToPostfix(13, "cenY", "cenY / count");
    pStore->setStmtToPostfix(14, "normSq", "cenX * cenX + cenY * cenY");

    REQUIRE(pStore->getPatternAndVarMatchStatement("normSq", "cenX", true, false) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternAndVarMatchStatement("normSq", "cenX", true, true) == std::unordered_set<int>{});
    REQUIRE(pStore->getPatternAndVarMatchStatement("count", "count", true, false) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternAndVarMatchStatement("count", "count", true, true) == std::unordered_set<int>{});

    pStore->clear();
}

TEST_CASE("Pattern Storage: getPatternMatchStatement Test with one var pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "count + 1");
    pStore->setStmtToPostfix(7, "cenX", "cenX + x");
    pStore->setStmtToPostfix(8, "cenY", "cenY + y");
    pStore->setStmtToPostfix(12, "cenX", "cenX / count");
    pStore->setStmtToPostfix(13, "cenY", "cenY / count");
    pStore->setStmtToPostfix(14, "normSq", "cenX * cenX + cenY * cenY");

    REQUIRE(pStore->getPatternMatchStatement("count", false, false) == std::unordered_set<int>{6, 12, 13});
    REQUIRE(pStore->getPatternMatchStatement("count", true, false) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("1", false, true) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("count", true, true).empty());

    REQUIRE(pStore->getPatternMatchStatement("cenY", true, false) == std::unordered_set<int>{8, 13});
    REQUIRE(pStore->getPatternMatchStatement("cenY", true, true).empty());

    pStore->clear();
}

TEST_CASE("Pattern Storage: getPatternMatchStatement Test with multiple brackets pattern") {
    PatternStorage *pStore = PatternStorage::getInstance();
    pStore->setStmtToPostfix(6, "count", "(count) + (1)");
    pStore->setStmtToPostfix(7, "cenX", "(cenX + x)");
    pStore->setStmtToPostfix(8, "normSq", "cenX * ((cenX + cenY) * cenY)");
    pStore->setStmtToPostfix(14, "normSq", "((cenX * cenX) + cenY * ((cenY)))");

    REQUIRE(pStore->getPatternMatchStatement("count", false, false) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("count", false, false) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("(count + 1)", false, false) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("count + (1)", false, false) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("count + (1)", false, true) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("count + (1)", true, true) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("((((1))))", false, true) == std::unordered_set<int>{6});
    REQUIRE(pStore->getPatternMatchStatement("((((1))))", true, false).empty());
    REQUIRE(pStore->getPatternMatchStatement("((((1))))", true, true).empty());

    REQUIRE(pStore->getPatternMatchStatement("cenX", false, false) == std::unordered_set<int>{7, 8, 14});
    REQUIRE(pStore->getPatternMatchStatement("cenX * cenX", true, false) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternMatchStatement("cenY", false, true) == std::unordered_set<int>{8, 14});
    REQUIRE(pStore->getPatternMatchStatement("cenX + cenY", false, false) == std::unordered_set<int>{8});
    REQUIRE(pStore->getPatternMatchStatement("(((cenY)) * cenY)", false, false) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternMatchStatement("(cenX * cenX) + (cenY * cenY)", true, true) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternMatchStatement("cenX * cenX + cenY * cenY", true, true) == std::unordered_set<int>{14});
    REQUIRE(pStore->getPatternMatchStatement("cenX * (cenX + cenY) * cenY", true, true).empty());

    pStore->clear();
}

TEST_CASE("Pattern Storage: setStmtToPostfix invalid pattern validation throwing errors.") {
    PatternStorage *pStore = PatternStorage::getInstance();

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "count +"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "+ count"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "count ? b"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "a & b"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "(a + by"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "(by + "));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "by + )"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "a + by)"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "(a + by) + "));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "+ b c d"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "+"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "( +"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "+ )"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "(by + )"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "( + by)"));

    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "1A"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "count", "?1234"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "1A", "count"));
    CHECK_THROWS(pStore->setStmtToPostfix(6, "?1234", "count"));

    CHECK_NOTHROW(pStore->setStmtToPostfix(6, "count", "   "));
    CHECK_NOTHROW(pStore->setStmtToPostfix(6, "count", "  x "));
    CHECK_NOTHROW(pStore->setStmtToPostfix(6, "count", "  input "));
    CHECK_NOTHROW(pStore->setStmtToPostfix(6, "count", "	input		"));

    pStore->clear();
}

TEST_CASE("Pattern Storage: convertInfixToPostfix test.") {
    PatternStorage *pStore = PatternStorage::getInstance();

    REQUIRE(pStore->convertInfixToPostfix("AX + B * C") == "#AX# #B# #C# *+");
    REQUIRE(pStore->convertInfixToPostfix("AX +B*C ") == "#AX# #B# #C# *+");
    REQUIRE(pStore->convertInfixToPostfix("AX+B*C") == "#AX# #B# #C# *+");
    REQUIRE(pStore->convertInfixToPostfix("AX +B * C ") == "#AX# #B# #C# *+");

    REQUIRE(pStore->convertInfixToPostfix("AX + B % C * 3") == "#AX# #B# #C# %#3# *+");
    REQUIRE(pStore->convertInfixToPostfix("AX +B%C *3") == "#AX# #B# #C# %#3# *+");
    REQUIRE(pStore->convertInfixToPostfix("AX+B%C*3") == "#AX# #B# #C# %#3# *+");
    REQUIRE(pStore->convertInfixToPostfix("AX +B % C*3 ") == "#AX# #B# #C# %#3# *+");

    CHECK_THROWS(pStore->convertInfixToPostfix("0001"));
    CHECK_THROWS(pStore->convertInfixToPostfix("000010"));

    CHECK_THROWS(pStore->convertInfixToPostfix("1count"));
    CHECK_THROWS(pStore->convertInfixToPostfix("   1count   "));
    CHECK_THROWS(pStore->convertInfixToPostfix("1count   "));
    CHECK_THROWS(pStore->convertInfixToPostfix("   1count"));

    CHECK_THROWS(pStore->convertInfixToPostfix("1count +"));
    CHECK_THROWS(pStore->convertInfixToPostfix("+ 1count"));

    CHECK_THROWS(pStore->convertInfixToPostfix("1count+"));
    CHECK_THROWS(pStore->convertInfixToPostfix("+1count"));

    CHECK_THROWS(pStore->convertInfixToPostfix("count +"));
    CHECK_THROWS(pStore->convertInfixToPostfix("+ count"));

    CHECK_THROWS(pStore->convertInfixToPostfix("count ? b"));
    CHECK_THROWS(pStore->convertInfixToPostfix("a & b"));
    CHECK_THROWS(pStore->convertInfixToPostfix("& b"));
    CHECK_THROWS(pStore->convertInfixToPostfix("a ^ b"));

    CHECK_THROWS(pStore->convertInfixToPostfix("(a + by"));
    CHECK_THROWS(pStore->convertInfixToPostfix("(by + "));
    CHECK_THROWS(pStore->convertInfixToPostfix("by + )"));
    CHECK_THROWS(pStore->convertInfixToPostfix("a + by)"));

    CHECK_THROWS(pStore->convertInfixToPostfix("(a + by) + "));

    CHECK_THROWS(pStore->convertInfixToPostfix("+ b c d"));

    CHECK_THROWS(pStore->convertInfixToPostfix("+"));

    CHECK_THROWS(pStore->convertInfixToPostfix("( +"));
    CHECK_THROWS(pStore->convertInfixToPostfix("+ )"));

    CHECK_THROWS(pStore->convertInfixToPostfix("(by + )"));
    CHECK_THROWS(pStore->convertInfixToPostfix("( + by)"));

    CHECK_THROWS(pStore->convertInfixToPostfix("((((1)))"));

    CHECK_NOTHROW(pStore->convertInfixToPostfix("   "));
    CHECK_NOTHROW(pStore->convertInfixToPostfix("	input	"));
    CHECK_NOTHROW(pStore->convertInfixToPostfix(" input "));
    CHECK_NOTHROW(pStore->convertInfixToPostfix("		input "));

    pStore->clear();
}

TEST_CASE("Pattern Storage: getVarMatchConditionStatements Test") {
    PatternStorage *pStore = PatternStorage::getInstance();
    std::unordered_set<std::string> vars{"apple", "orange"};
    std::unordered_set<std::string> vars2{"pineapple", "banana"};
    pStore->setConditionPattern(2, vars, IF_STMT);
    pStore->setConditionPattern(8, vars2, IF_STMT);
    pStore->setConditionPattern(11, vars2, IF_STMT);

    std::unordered_set<std::string> vars3{"count", "x"};
    pStore->setConditionPattern(5, vars, WHILE_STMT);
    pStore->setConditionPattern(9, vars3, WHILE_STMT);
    pStore->setConditionPattern(10, vars2, WHILE_STMT);

    REQUIRE(pStore->getConditionStatementsWithAnyVar(IF_STMT) == std::unordered_set<int>{2, 8, 11});
    REQUIRE(pStore->getConditionStatementsWithAnyVar(WHILE_STMT) == std::unordered_set<int>{5, 9, 10});

    REQUIRE(pStore->getVarMatchConditionStatements("apple", IF_STMT) == std::unordered_set<int>{2});
    REQUIRE(pStore->getVarMatchConditionStatements("pineapple", IF_STMT) == std::unordered_set<int>{8, 11});
    REQUIRE(pStore->getVarMatchConditionStatements("x", IF_STMT) == std::unordered_set<int>{});
    REQUIRE(pStore->getVarMatchConditionStatements("count", IF_STMT) == std::unordered_set<int>{});

    REQUIRE(pStore->getVarMatchConditionStatements("count", WHILE_STMT) == std::unordered_set<int>{9});
    REQUIRE(pStore->getVarMatchConditionStatements("x", WHILE_STMT) == std::unordered_set<int>{9});
    REQUIRE(pStore->getVarMatchConditionStatements("cat", WHILE_STMT) == std::unordered_set<int>{});

    // std::list<std::pair<std::string, int>> ans;
    IntStringPairResultSet ans;
    ans.emplace(std::pair<int, std::string>{9, "count"});
    ans.emplace(std::pair<int, std::string>{9, "x"});

    ans.emplace(std::pair<int, std::string>{5, "apple"});
    ans.emplace(std::pair<int, std::string>{5, "orange"});

    ans.emplace(std::pair<int, std::string>{10, "pineapple"});
    ans.emplace(std::pair<int, std::string>{10, "banana"});
    IntStringPairResultSet result = pStore->getVarMatchConditionStatementPairs(WHILE_STMT);

    REQUIRE(result == ans);

    IntStringPairResultSet ans1;
    ans1.emplace(std::pair<int, std::string>{2, "apple"});
    ans1.emplace(std::pair<int, std::string>{2, "orange"});
    ans1.emplace(std::pair<int, std::string>{8, "pineapple"});
    ans1.emplace(std::pair<int, std::string>{8, "banana"});
    ans1.emplace(std::pair<int, std::string>{11, "pineapple"});
    ans1.emplace(std::pair<int, std::string>{11, "banana"});
    result = pStore->getVarMatchConditionStatementPairs(IF_STMT);
    REQUIRE(ans1 == result);

    pStore->clear();
}
