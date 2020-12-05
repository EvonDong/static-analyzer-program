#include "QueryParser.h"
#include "QueryParserHelper.h"
#include "catch.hpp"

#include <unordered_set>
#include <string>

// Validate such that
TEST_CASE("while w; Select w such that Parent(w, 7) -VALID- validateSuchThat Test") {
        QueryParser::InputType input1;
        input1.var = "w";
        input1.entityType = "while";

        QueryParser::InputType input2;
        input2.var = "7";
        input2.entityType = "stmtNum";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseVariable = "Parent";
        suchThatClause.firstVar = input1;
        suchThatClause.secondVar = input2;

        bool result = QueryParser::validateSuchThat(suchThatClause);

        REQUIRE(result == true);
}

TEST_CASE("assign a; Select a such that Follows*(3, a) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Follows*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("assign a; call c; Select a such that Next*(2, c) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "2";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "c";
    input2.entityType = "call";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("assign a; read r; Select a such that Next*(r, 5) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "r";
    input1.entityType = "read";

    QueryParser::InputType input2;
    input2.var = "5";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("assign a; print p; Select a such that Next(_, p) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "print";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("assign a; print p; Select a such that Next*(a, _) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("assign a; procedure p, q; Select a such that Calls(p, q) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "procedure";

    QueryParser::InputType input2;
    input2.var = "q";
    input2.entityType = "procedure";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("assign a; procedure p; Select a such that Calls*(p, \"procA\") -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "procedure";

    QueryParser::InputType input2;
    input2.var = "procA";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("assign a; procedure p; Select a such that Calls(\"procA\", _) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "procA";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("assign a; procedure p; Select a such that Calls*(_, \"procA\") -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "procA";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("assign a; procedure p; Select a such that Calls(_, _) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("assign a; procedure p; Select a such that Calls*(\"procA\", \"procB\") -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "procA";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "procB";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);

    REQUIRE(result == true);
}

TEST_CASE("read r; Select r such that Modifies(r, _) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "r";
    input1.entityType = "read";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("read r; Select pn such that Modifies(r, _) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "r";
    input1.entityType = "read";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("call c; Select c such that Uses(_, _) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("print p; Select r such that Uses(p, _) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "print";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}


TEST_CASE("variable p; Select p such that Modifies(1, \"count\") -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "1";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "count";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("variable p; Select p such that Uses(1, \"count\") -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "1";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "count";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}


TEST_CASE("variable p; Select p such that Affects(1, 3) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "1";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("assign a2, a; Select p such that Affects*(a2, a) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "a2";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("prog_line p; assign a2, a; Select p such that Affects*(a2, p) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "a2";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "prog_line";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

TEST_CASE("stmt s; assign a2, a; Select p such that Affects(s, p) -VALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "s";
    input1.entityType = "stmt";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "prog_line";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == true);
}

// ---------------------- INVALID SUCHTHAT ---------------------------------------
TEST_CASE("call c; Select c such that Modifies(_, v) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "v";
    input2.entityType = "variable";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("call c; Select c such that Modifies(_, _) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "_";
    input1.entityType = "wildcard";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}



TEST_CASE("call c; Select c such that Follows(9, 1) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "9";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "1";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Follows";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("print pn; Select p such that Parent(6, 1) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "6";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "1";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Parent";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("assign a; Select a such that Follows(a, a) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Follows";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("assign a; Select a such that Follows(3, 3) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Follows";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("assign a; Select a such that Parent*(3, 3) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Parent*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("read r; Select a such that Uses(r, 3) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "r";
    input1.entityType = "read";

    QueryParser::InputType input2;
    input2.var = "3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}

TEST_CASE("print p; Select a such that Modifies(p, 3) -INVALID- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "print";

    QueryParser::InputType input2;
    input2.var = "3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}


TEST_CASE("assign a; Select a such that Next*(-3, 3) -INVALID negative num- validateSuchThat Test") {
    QueryParser::InputType input1;
    input1.var = "-3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
}


TEST_CASE("assign a; Select a such that Next*(a, -3) -INVALID 2nd negative input- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "while";

    QueryParser::InputType input2;
    input2.var = "-3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("assign a; Select a such that Next*(v, 4) -INVALID 1st Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "4";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; Select p such that Next*(9, p) -INVALID 2nd Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "9";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "procedure";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; Select p such that Calls*(9, p) -INVALID 1st Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "9";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "procedure";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; Select p such that Calls*(p, v) -INVALID 2nd Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "procedure";

    QueryParser::InputType input2;
    input2.var = "v";
    input2.entityType = "variable";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; Select p such that Calls*(p, const) -undeclared variable- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"const", "constant"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "procedure";

    QueryParser::InputType input2;
    input2.var = "const";
    input2.entityType = "constant";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; Select p such that Calls*(\"425\", p) -INVALID 1st Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "\"425\"";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "procedure";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Calls*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; assign a; Select p such that Affects*(\"name\", a) -INVALID 1st Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "name";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; assign a; Select p such that Affects*(a, p) -INVALID 1st Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "procedure";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("procedure p; assign a; Select p such that Affects*(r, a) -INVALID 1st Type- validateSuchThat Test") {

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"a", "assign"});
    inputMap.insert({"r", "read"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "r";
    input1.entityType = "read";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    bool result = QueryParser::validateSuchThat(suchThatClause);
    REQUIRE(result == false);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}



// ******************************    Process such that    ****************
TEST_CASE("while w; Select w such that Parent(w, 7): -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Parent(w, 7)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "w";
    input1.entityType = "while";

    QueryParser::InputType input2;
    input2.var = "7";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Parent";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Follows*(3, a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Follows*(3, a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Follows*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Modifies(\"modifyCount\", \"count\") -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Modifies(\"modifyCount\", \"count\")");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "modifyCount";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "count";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Modifies";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Uses(            \"modifyCount   \"     ,    _) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Uses(            \"modifyCount   \"     ,    _     )");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "modifyCount";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "_";
    input2.entityType = "wildcard";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Uses(   \"modifyCount   \"     , \"count   \") -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Uses(   \"modifyCount   \"     ,   \"count   \"    )");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "modifyCount";
    input1.entityType = "identifier";

    QueryParser::InputType input2;
    input2.var = "count";
    input2.entityType = "identifier";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Affects(   3     , 5) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects(   3     , 5)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "5";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Affects(   6     , 5) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects(   6     , 5)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "6";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "5";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Affects*(   a     , 5) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects*(   a     , 5)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "5";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Affects*(   3     , a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects*(   3     , a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "3";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; Select a such that Affects*(   a     , a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects*(   a     , a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; prog_line p; Select a such that Affects*(   8     , p) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects*(   8     , p)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "8";
    input1.entityType = "stmtNum";

    QueryParser::InputType input2;
    input2.var = "p";
    input2.entityType = "prog_line";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; prog_line p; Select a such that Affects(   p     ,    a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("Affects(   p     ,    a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "prog_line";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "Affects";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ------------------ VALID processSuchThat tests ----------- EXTENSION ---------------------
TEST_CASE("assign a; prog_line p; Select a such that NextBip(   p     ,    a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("NextBip(   p     ,    a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "prog_line";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "NextBip";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; print p; Select a such that NextBip*(   p     ,    a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("NextBip*(   p     ,    a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "print"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "print";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "NextBip*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; prog_line p; Select a such that AffectsBip(   p     ,    a) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("AffectsBip(   p     ,    a)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "prog_line";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "AffectsBip";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; stmt s; Select a such that AffectsBip*(   p     ,    s) -VALID- processSuchThat Test") {
    std::vector<std::string> input;
    input.emplace_back("AffectsBip*(   p     ,    s)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    inputMap.insert({"s", "stmt"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::processSuchThat(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p";
    input1.entityType = "prog_line";

    QueryParser::InputType input2;
    input2.var = "s";
    input2.entityType = "stmt";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseType = "suchThat";
    suchThatClause.clauseVariable = "AffectsBip*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    std::vector<QueryParser::Clause> suchThatVec;
    suchThatVec.push_back(suchThatClause);

    REQUIRE(result.clauseType == suchThatClause.clauseType);
    REQUIRE(result.clauseVariable == suchThatClause.clauseVariable);
    REQUIRE(result.firstVar.var == suchThatClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == suchThatClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == suchThatClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == suchThatClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}



// ------------------ INVALID processSuchThat tests --------------------------------
TEST_CASE("assign a; Select a such that Folocw((4, 7) -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Folocw(4, 7)");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; Select a such that Uses( -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Uses(");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; Select a such that Uses(,) -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Uses(,)");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; Select a such that Uses(,\" -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Uses(,\"");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; Select a such that Uses(_,_) -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Uses(_, _)");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; Select a such that Follows*(3, \"blah\") -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Follows*(3, \"blah\")");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; Select a such that Parent(3, \"blah\") -INVALID- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Parent(3, \"blah\")");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; variable v; Select a such that Nxt*(3, 4) -Wrong Next relation - processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Nxt*(3, 4)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; Select a such that Next*(v, 4) -INVALID 1st input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Next*(v, 4)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; Select a such that Next(2, v) -INVALID 2nd input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Next*(2, v)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; Select a such that Next(a, ?) -INVALID 2nd input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Next*(a, ?)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; Select a such that Next*(-4, 5) -INVALID 1st input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Next*(-4, 5)");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("variable v; Select a such that Next*(a, 5) -INVALID UNDECLARED 1st input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Next*(a, 5)");

    REQUIRE_THROWS(queryParser.processSuchThat(input));
}

TEST_CASE("assign a; variable v; procedure p; Select a such that Calls*(2, p) -INVALID 1st input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Calls*(2, p)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; procedure p; Select a such that Calls*(p, 9) -INVALID 2nd input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Calls*(p, 9)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; procedure p; Select a such that Calls*(p, v) -INVALID 2nd input type- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Calls*(p, v)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("assign a; variable v; procedure p; Select a such that Calls*(p, \"3142count\") -INVALID 2nd input identifier- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Calls*(p, \"3142count\")");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("assign a; variable v; procedure p; Select a such that Calls*(p, \"count) -INVALID 2nd input identifier- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Calls*(p, \"count)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; procedure p; Select a such that Calls*(\"count\', \"count\") -INVALID 1st input identifier- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Calls*(\"count\', \"count\")");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; procedure p; Select a such that Affects*(\"count\', \"count\") -INVALID 1st input identifier- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Affects*(\"count\', \"count\")");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; procedure p; Select a such that Affects*(v, a) -INVALID 1st input variable- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Affects*(v, a)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; calls c; procedure p; Select a such that Affects*(a, c) -INVALID 2nd input call- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Affects*(a, c)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "calls"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; call c; procedure p; Select a such that Affects*(a, -3) -INVALID 2nd input neg- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Affects*(a, -3)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("assign a; variable v; call c; procedure p; Select a such that Affecs*(a, p) -INVALID 2nd input neg- processSuchThat Test") {
    QueryParser queryParser;
    std::vector<std::string> input;
    input.emplace_back("Affecs*(a, p)");

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "prog_line"});
    inputMap.insert({"c", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(queryParser.processSuchThat(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


 //------------------ SEMANTIC INVALID validateSuchThat tests --------------------------------
TEST_CASE("assign a; variable v; call c; procedure p; Select BOOLEAN such that Affects(   p     ,    -3) -INVALID 2nd input neg- validateSuchThat Test") {
    QueryParser::setIsBoolean(true);

    std::vector<std::string> input;
    input.emplace_back("Affects*(   a2     ,    -3)");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a2", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);


    QueryParser::InputType input1;
    input1.var = "a2";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "-3";
    input2.entityType = "stmtNum";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Affects*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    REQUIRE(QueryParser::validateSuchThat(suchThatClause) == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("assign a; variable v; call c; procedure p; Select BOOLEAN such that Uses(   v    ,  v) -INVALID 2nd input neg- validateSuchThat Test") {
    QueryParser::setIsBoolean(true);

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a2", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);


    QueryParser::InputType input1;
    input1.var = "v";
    input1.entityType = "variable";

    QueryParser::InputType input2;
    input2.var = "v";
    input2.entityType = "variable";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Uses";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    REQUIRE(QueryParser::validateSuchThat(suchThatClause) == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("assign a; variable v; call c; procedure p; Select BOOLEAN such that Next*(   a    ,  v) -INVALID 2nd input neg- validateSuchThat Test") {
    QueryParser::setIsBoolean(true);

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "v";
    input2.entityType = "variable";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Next*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    REQUIRE(QueryParser::validateSuchThat(suchThatClause) == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}


TEST_CASE("assign a; variable v; call c; procedure p; Select BOOLEAN such that Nxt*(   a    , a) -INVALID 2nd input neg- validateSuchThat Test") {
    QueryParser::setIsBoolean(true);

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"a", "assign"});
    inputMap.insert({"p", "prog_line"});
    inputMap.insert({"v", "variable"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::InputType input1;
    input1.var = "a";
    input1.entityType = "assign";

    QueryParser::InputType input2;
    input2.var = "a";
    input2.entityType = "assign";

    QueryParser::Clause suchThatClause;
    suchThatClause.clauseVariable = "Nxt*";
    suchThatClause.firstVar = input1;
    suchThatClause.secondVar = input2;

    REQUIRE(QueryParser::validateSuchThat(suchThatClause) == false);
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}
