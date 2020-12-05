#include "QueryParser.h"
#include "QueryParserHelper.h"
#include "catch.hpp"

#include <unordered_set>
#include <string>

// ************************ Validate Pattern ********************
// --------------- wildcard, expression with 2 wildcard --------
TEST_CASE("pattern a(_, _\"x + 1\"_): validatePattern Test") {
//    QueryParser queryParser;
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_x + 1_";
    input2.entityType = "expression";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "a";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------------- variable, expression with 1 wildcard --------
TEST_CASE("pattern a2(v, \"x + 1\"_): validatePattern Test") {
//    QueryParser queryParser;
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"a2", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "x + 1_";
    input2.entityType = "expression";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "a2";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------------- variable, expression --------
TEST_CASE("pattern a2(v, \"x + 1\"): validatePattern Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"a2", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "x + 1";
    input2.entityType = "expression";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "a2";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------------- wildcard, wildcard ------------
TEST_CASE("pattern c3(_, _): validatePattern Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"c3", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "c3";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------------- validate pattern for if/while --------
TEST_CASE("pattern ifs(\"z\", _,_): validatePattern -VALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_,_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "ifPattern";
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern ifs(v, _,_): validatePattern -VALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "_,_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "ifPattern";
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern ifs(\'count\", _,_): validatePattern -VALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "count";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_,_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "ifPattern";
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// for pattern while
TEST_CASE("pattern w(\"z\", _): validatePattern -VALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "whilePattern";
    patternClause.clauseVariable = "w";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern w2(v, _): validatePattern -VALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w2", "while"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "whilePattern";
    patternClause.clauseVariable = "w2";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern w42(\'count\", _): validatePattern -VALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"w42", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "count";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "whilePattern";
    patternClause.clauseVariable = "w42";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == true);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


// --------------- print var(identifier, expression with 2 wildcard) -----INVALID---
TEST_CASE("pattern p(\"z\", _\"x + 1\"_): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "print"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_x + 1_";
    input2.entityType = "expression";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "p";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern ifs(z, _\"x + 1\"_): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"z", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "while";

    QueryParser::InputType input2;
    input2.var = "_x + 1_";
    input2.entityType = "expression";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern whi(z, whyy): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"whi", "while"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"whyy", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "whyy";
    input2.entityType = "variable";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "whi";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern assign(z, \")_\"): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"assign", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = ")_";
    input2.entityType = "identifier";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "assign";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern ifss(z, _): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifss", "if"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "identifier";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifss";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("pattern w(z, __): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "__";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "w";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ************************ Process Pattern ********************
// --------------- wildcard, wildcard --------
TEST_CASE("pattern assign(_, _): processPattern Test") {
    std::vector<std::string> input;
    input.emplace_back("assign(_, _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"assign", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "assignPattern";
    patternClause.clauseVariable = "assign";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    REQUIRE(result.clauseType == patternClause.clauseType);
    REQUIRE(result.clauseVariable == patternClause.clauseVariable);
    REQUIRE(result.firstVar.var == patternClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == patternClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------------- identifier, expression with 1 wildcard --------
TEST_CASE("pattern a(\"z\", _\"x + 1\"): processPattern Test") {
    std::vector<std::string> input;
    input.emplace_back("a(\"z\", _\"x+i\")");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_x+i";
    input2.entityType = "expression";

    QueryParser::Clause patternClause;
    patternClause.clauseType = "assignPattern";
    patternClause.clauseVariable = "a";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    REQUIRE(result.clauseType == patternClause.clauseType);
    REQUIRE(result.clauseVariable == patternClause.clauseVariable);
    REQUIRE(result.firstVar.var == patternClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == patternClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- identifier, expression with 1 wildcard ---- with spaces/tabs --------
TEST_CASE("pattern a(\"     z\", _\"x + y*7-    1\"): processPattern Test") {
    SECTION("muliple tabs and spaces with front wildcard") {
        std::vector<std::string> input;
        input.emplace_back("a(\"       z\", _\"      x   + y* 7-    1  \")");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "z";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "_x + y* 7- 1";
        input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("muliple tabs and spaces between front wildcard") {
        std::vector<std::string> input;
        input.emplace_back("a(\"       z\", _        \"      count   + y* 7-    1  \")");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "z";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "_count + y* 7- 1";
        input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("muliple tabs and spaces with back wildcard") {
        std::vector<std::string> input;
        input.emplace_back("a(\"       z \", \"  3*    x   + y* 7-    1  \"_)");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "z";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "3* x + y* 7- 1_";
        input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("muliple tabs and spaces between back wildcard") {
        std::vector<std::string> input;
        input.emplace_back("a(\"       z\", \"      count   + y* 7-    1    \"   _)");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "z";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "count + y* 7- 1_";
        input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("muliple tabs and spaces in front and back wildcard") {
        std::vector<std::string> input;
        input.emplace_back("a(\"       z\", _\"      count   + y* 7-    1    \"   _)");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "z";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "_count + y* 7- 1_";
        input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }
}

// --------------------- Tests for if and while ----------------------
TEST_CASE("Tests for pattern if : processPattern Test") {
    SECTION("pattern i42(_, _,_)") {
        std::vector<std::string> input;
        input.emplace_back("i42(_, _,_)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"i42", "if"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "_";
        input1.entityType = "wildcard";

        QueryParser::InputType input2;
        input2.var = "_,_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "ifPattern";
        patternClause.clauseVariable = "i42";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("pattern if(v, _,_)") {
        std::vector<std::string> input;
        input.emplace_back("if(v, _,_)");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"v", "variable"});
        inputMap.insert({"if", "if"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "v";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "_,_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "ifPattern";
        patternClause.clauseVariable = "if";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("pattern i64ce(\"count\", _,_)") {
        std::vector<std::string> input;
        input.emplace_back("i64ce(\"count\", _,_)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"i64ce", "if"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "count";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "_,_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "ifPattern";
        patternClause.clauseVariable = "i64ce";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("pattern i64ce(\"count\", _ ,    _)") {
        std::vector<std::string> input;
        input.emplace_back("i64ce(\"count\", _ ,   _)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"i64ce", "if"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "count";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "_,_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "ifPattern";
        patternClause.clauseVariable = "i64ce";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }
}

TEST_CASE("Tests for pattern while : processPattern Test") {
    SECTION("pattern w5s32f(_, _)") {
        std::vector<std::string> input;
        input.emplace_back("w5s32f  ( _    , _)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w5s32f", "while"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "_";
        input1.entityType = "wildcard";

        QueryParser::InputType input2;
        input2.var = "_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "whilePattern";
        patternClause.clauseVariable = "w5s32f";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("pattern w1(v, _)") {
        std::vector<std::string> input;
        input.emplace_back("w1(v, _)");
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"v", "variable"});
        inputMap.insert({"w1", "while"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "v";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "whilePattern";
        patternClause.clauseVariable = "w1";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("pattern while(\"count\", _)") {
        std::vector<std::string> input;
        input.emplace_back("while(\"count\", _)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"v", "variable"});
        inputMap.insert({"while", "while"});
        QueryParser::setDeclarationsMap(inputMap);

        std::vector<QueryParser::Clause> output = QueryParser::processPattern(input);
        QueryParser::Clause result = output.back();

        QueryParser::InputType input1;
        input1.var = "count";
        input1.entityType = "identifier";

        QueryParser::InputType input2;
        input2.var = "_";
        input2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "whilePattern";
        patternClause.clauseVariable = "while";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        REQUIRE(result.clauseType == patternClause.clauseType);
        REQUIRE(result.clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }
}

// --------- unknown, expression with 1 wildcard -----INVALID-------
TEST_CASE("pattern a(z\", _\"x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a(z\", _\"x+i\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- unknown, expression with 1 wildcard -----INVALID-------
TEST_CASE("pattern a(\"v\", x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a(\"v\", x+i\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- unknown unknown, expression with 1 wildcard -----INVALID-------
TEST_CASE("pattern a\"y, (_, _\"x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a\"y, (_, _\"x + 1\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- undeclared (unknown, expression with 1 wildcard) -----INVALID-------
TEST_CASE("pattern  (_, _\"x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("    e(_, _\"x + 1\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- invalid (unknown, expression with 1 wildcard) -----INVALID-------
TEST_CASE("pattern ? (_, _\"x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back(" (_, _\"x + 1\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- not assign stmt (unknown, expression with 1 wildcard) -----INVALID-------
TEST_CASE("pattern p (_, _\"x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("p (_, _\"x + 1\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "print"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ---------  (wildcard, expression with missing left " ) -----INVALID-------
TEST_CASE("pattern a (_, _x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a (_, _x + 1\"_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ---------  (wildcard, expression with missing right " ) -----INVALID-------
TEST_CASE("pattern a (_, _\"x + 1_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a (_, _\"x + 1_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ---------  (wildcard, expression with invalid right " ) -----INVALID-------
TEST_CASE("pattern a (_, _\"x + 1\"): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a (_, _\"x + \"1    _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ---------  (wildcard, expression with missing " ) -----INVALID-------
TEST_CASE("pattern a (_, _x + 1_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a (_, _x + 1    _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ---------  (wildcard, expression with left " at wrong position) -----INVALID-------
TEST_CASE("pattern a (_, __\"x + 1\"_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a (_, __\"x + 1 \"   _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ---------  (wildcard, expression with left " at wrong position) -----INVALID-------
TEST_CASE("pattern a (_, _\"x + 1\"__): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("a (_, _\"x + 1  \"_  _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}






// --------- if (wildcard, wildcard with wrong format at wrong position) -----INVALID-------
TEST_CASE("pattern ifs (_, _): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern ifs (_, _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- if (wildcard, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern ifs (_, v): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern ifs (_, v)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- if (wildcard, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern ifs (ifs, _,_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern ifs (ifs, _,_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- undeclared if (wildcard, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern ifs (v, _,_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern ifs (v, _,_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- if (invalid identifier, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern ifs (\"count, _,_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern ifs (\"count, _,_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- if (identifier, invalid 2nd input, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern ifs (\"count\", v,_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern ifs (\"count\", v,_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}




// --------- undeclared while (var, wildcard  at wrong position) -----INVALID-------
TEST_CASE("pattern while (v, _): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern while (v, _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- while (var, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern while (v, _,_): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern while (v, _,_)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"while", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- while (var, wildcard with wrong input type at wrong position) -----INVALID-------
TEST_CASE("pattern w (\"count, _): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern while (\"count, _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"w", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- while (1 input only) -----INVALID-------
TEST_CASE("pattern w (\"count\"): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern while (\"count\")");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"w", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


// --------- while (invalid 1st input) -----INVALID-------
TEST_CASE("pattern w (v, _): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern while (v, _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "assign"});
    inputMap.insert({"w", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// --------- while (invalid 1st input) -----INVALID-------
TEST_CASE("pattern w (5, _): processPattern -INVALID- Test") {
    std::vector<std::string> input;
    input.emplace_back("pattern while (5, _)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "assign"});
    inputMap.insert({"w", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}




// -----BOOLEAN------- INVALID validatePattern Semantic checks -----WHILE-------
TEST_CASE("BOOLEAN  pattern w(z, __): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "__";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "w";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN  pattern w(w, _): 1st invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "w";
    input1.entityType = "while";

    QueryParser::InputType input2;
    input2.var = "__";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "w";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}


TEST_CASE("BOOLEAN  pattern wildcard(z, _): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "wildcard";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN  pattern blahh(z, _): validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "z";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "blahh";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}




// -----BOOLEAN------- INVALID validatePattern Semantic checks -----ASSIGN-------
TEST_CASE("BOOLEAN  pattern a(\"x\", z): 2nd invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "x";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "z";
    input2.entityType = "variable";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "a";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN  pattern a(a, _): 1st invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "a";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}
//
TEST_CASE("BOOLEAN  pattern a(_, _, _): 2nd invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_, _";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "a";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}


// -----BOOLEAN------- INVALID validatePattern Semantic checks -----IFS-------
TEST_CASE("BOOLEAN  pattern ifs(\"x\", z): 2nd invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "x";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "z";
    input2.entityType = "variable";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}


TEST_CASE("BOOLEAN  pattern ifs(\"x\", _): 2nd invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "x";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN  pattern ifs(w, _, _): 1st invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "w";
    input1.entityType = "while";

    QueryParser::InputType input2;
    input2.var = "_, _";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN  pattern ifs(v, _, _): undeclared 1st invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "_, _";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN  pattern ifs(\"deq%8\", _, _): 2nd invalid input - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "deq%8";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_, _";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}


TEST_CASE("BOOLEAN  pattern ifs(\"count\", _, _): undeclared main var - validatePattern -INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    QueryParser::InputType input1;
    input1.var = "count";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_, _";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    bool result = QueryParser::validatePattern(patternClause);
    REQUIRE(result == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

// -----BOOLEAN------- INVALID processPattern SYNTACTIC checks -----IFS-------
TEST_CASE("BOOLEAN  pattern ifs(deq%8, _, _): 2nd invalid input - processPattern - SYNTACTIC INVALID- Test") {
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"z", "variable"});
    inputMap.insert({"ifs", "if"});
    inputMap.insert({"wildcard", "call"});
    QueryParser::setDeclarationsMap(inputMap);
    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    std::vector<std::string> input;
    input.emplace_back("ifs(deq%8, _, _)");

    QueryParser::InputType input1;
    input1.var = "deq%8";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_, _";
    input2.entityType = "wildcard";

    QueryParser::Clause patternClause;
    patternClause.clauseVariable = "ifs";
    patternClause.firstVar = input1;
    patternClause.secondVar = input2;

    std::vector<QueryParser::Clause> patternVec;
    patternVec.push_back(patternClause);

    REQUIRE_THROWS(QueryParser::processPattern(input));
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

