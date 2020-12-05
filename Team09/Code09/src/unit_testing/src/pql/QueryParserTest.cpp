#include "QueryParser.h"
#include "QueryParserHelper.h"
#include "catch.hpp"

#include <unordered_set>
#include <string>


// ******************* parse *****************************
TEST_CASE("Parse query: parse Test") {          // main test
    SECTION("valid query with only declaration and select") {
        std::string query = "while w; Select w";
        std::list<std::string> result = QueryParser::parse(query);

        std::list<std::string> expected;
        REQUIRE(result.empty());
        REQUIRE(QueryParser::getDeclarationsMap().size() == 1);
        REQUIRE(QueryParser::getDeclarationsMap().at("w") == "while");
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("invalid query with only declaration and select") {
        std::string query = "while s; Select w";
        std::list<std::string> result = QueryParser::parse(query);

        std::list<std::string> expected;
        REQUIRE(result.empty());
        REQUIRE(QueryParser::getDeclarationsMap().size() == 1);
        REQUIRE(QueryParser::getDeclarationsMap().at("s") == "while");
        REQUIRE(result == expected);
        REQUIRE(QueryParser::getMainQuery() == "w");
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

}

// ******************* validateQuery *****************************
TEST_CASE("Validate the query and process them: validateQuery Test") {
    SECTION( "valid query" ) {
        std::vector<std::string> input;
        input.emplace_back("while w");
        input.emplace_back("Select w such that Parent(w, 7)");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == true);
        REQUIRE(QueryParser::getDeclarationsMap().size() == 1);
        REQUIRE(QueryParser::getDeclarationsMap().at("w") == "while");
        REQUIRE(QueryParser::getMainQuery() == "Select w such that Parent(w, 7)");
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with stmtLst" ) {
        std::vector<std::string> input;
        input.emplace_back("stmtLst stmt");
        input.emplace_back("Select stmt");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == true);
        REQUIRE(QueryParser::getDeclarationsMap().size() == 1);
        REQUIRE(QueryParser::getDeclarationsMap().at("stmt") == "stmtLst");
        REQUIRE(QueryParser::getMainQuery() == "Select stmt");
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with keywords declared" ) {
        std::vector<std::string> input;
        input.emplace_back("while Select");
        input.emplace_back("Select Select such that Parent(Select, 7)");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == true);
        REQUIRE(QueryParser::getDeclarationsMap().size() == 1);
        REQUIRE(QueryParser::getDeclarationsMap().at("Select") == "while");
        REQUIRE(QueryParser::getMainQuery() == "Select Select such that Parent(Select, 7)");
    }

    SECTION( "valid query with keywords and multiple variables declared" ) {
        std::vector<std::string> input;
        input.emplace_back("while Select, w31,   a, d");
        input.emplace_back("procedure       p, q, l,    m");
        input.emplace_back("Select l such   that Parent(a, 7)");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == true);
        REQUIRE(QueryParser::getDeclarationsMap().size() == 8);
        REQUIRE(QueryParser::getDeclarationsMap().at("Select") == "while");
        REQUIRE(QueryParser::getDeclarationsMap().at("m") == "procedure");
//        REQUIRE(queryParser.getMainQuery() == "Select l such   that Parent(a, 7)");
    }

    SECTION( "invalid query with unknown type for declared variable" ) {
        std::vector<std::string> input;
        input.emplace_back("blah Select");
        input.emplace_back("Select Select such that Parent(Select, 7)");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == false);
    }

    SECTION( "invalid query with variable in invalid format declared" ) {
        std::vector<std::string> input;
        input.emplace_back("assign 42rrw");
        input.emplace_back("Select 42rrw such that Parent(42rrw, 7)");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == false);
    }

    SECTION( "invalid query without 'Select' as first word " ) {
        std::vector<std::string> input;
        input.emplace_back("assign rrw");
        input.emplace_back("blah Select rrw such that Parent(rrw, 7)");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == false);
    }

    SECTION( "Missing query with only declaration" ) {
        std::vector<std::string> input;
        input.emplace_back("assign rrw");

        bool result = QueryParser::validateQuery(input);
        REQUIRE(result == false);
    }
}


// ******************* VALID ProcessQuery with at most 1 such that, 1 pattern *****************************
TEST_CASE("Process VALID queries with at most 1 such that, 1 pattern: processQuery Test") {

    SECTION("valid query with such that clause only") {
        QueryParser::setMainQuery("Select w such that Parent(w, 7)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;
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
        std::vector<QueryParser::Clause> patternVec;

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "w";
        selectInput1.entityType = "while";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with such that clause only") {
        QueryParser::setMainQuery("Select c such that Uses(6, \"b\")");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"c", "constant"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;
        QueryParser::InputType input1;
        input1.var = "6";
        input1.entityType = "stmtNum";

        QueryParser::InputType input2;
        input2.var = "b";
        input2.entityType = "identifier";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseType = "suchThat";
        suchThatClause.clauseVariable = "Uses";
        suchThatClause.firstVar = input1;
        suchThatClause.secondVar = input2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);
        std::vector<QueryParser::Clause> patternVec;

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "c";
        selectInput1.entityType = "constant";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThatClause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with pattern clause only") {
        QueryParser::setMainQuery("Select a pattern a(_, _\"x + 1\" _)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        QueryParser::InputType input1;
        input1.var = "_";
        input1.entityType = "wildcard";

        QueryParser::InputType input2;
        input2.var = "_x + 1_";
        input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = input1;
        patternClause.secondVar = input2;

        std::vector<QueryParser::Clause> suchThatVec;
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with with clause only") {
        QueryParser::setMainQuery("Select s1 with s1.stmt# = n");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"s1", "stmt"});
        inputMap.insert({"n", "prog_line"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;
        QueryParser::InputType input1;
        input1.var = "s1.stmt#";
        input1.entityType = "stmt";

        QueryParser::InputType input2;
        input2.var = "n";
        input2.entityType = "prog_line";

        QueryParser::Clause withClause;
        withClause.clauseType = "with";
        withClause.clauseVariable = "equal";
        withClause.firstVar = input1;
        withClause.secondVar = input2;

        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(withClause);
        std::vector<QueryParser::Clause> patternVec;

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "s1";
        selectInput1.entityType = "stmt";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.second.second = withVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseType == withClause.clauseType);
        REQUIRE(result.second.second.second.front().clauseVariable == withClause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == withClause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == withClause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == withClause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == withClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with such that (follows*) and pattern clause") {
        QueryParser::setMainQuery("Select a  such    that Follows*   (  a, 7)     pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThatInput1;
        suchThatInput1.var = "a";
        suchThatInput1.entityType = "assign";

        QueryParser::InputType suchThatInput2;
        suchThatInput2.var = "7";
        suchThatInput2.entityType = "stmtNum";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseType = "suchThat";
        suchThatClause.clauseVariable = "Follows*";
        suchThatClause.firstVar = suchThatInput1;
        suchThatClause.secondVar = suchThatInput2;

        // for pattern
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "_";
        PatternInput1.entityType = "wildcard";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "x + 1_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThatClause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with such that(parent*) and pattern clause") {
        QueryParser::setMainQuery("Select a    pattern a( _, \"x + 1\"_ )   such    that Parent*   (  ifs, 7)   ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"ifs", "if"});
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // For suchThat
        QueryParser::InputType suchThatInput1;
        suchThatInput1.var = "ifs";
        suchThatInput1.entityType = "if";

        QueryParser::InputType suchThatInput2;
        suchThatInput2.var = "7";
        suchThatInput2.entityType = "stmtNum";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseType = "suchThat";
        suchThatClause.clauseVariable = "Parent*";
        suchThatClause.firstVar = suchThatInput1;
        suchThatClause.secondVar = suchThatInput2;

        // For pattern
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "_";
        PatternInput1.entityType = "wildcard";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "x + 1_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThatClause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with such that(uses) and pattern clause") {
        QueryParser::setMainQuery("Select a  such   that Uses(2, w) pattern a ( _, \"r + 1\"_ )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"2", "stmtNum"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "variable"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // For suchThat
        QueryParser::InputType suchThatInput1;
        suchThatInput1.var = "2";
        suchThatInput1.entityType = "stmtNum";

        QueryParser::InputType suchThatInput2;
        suchThatInput2.var = "w";
        suchThatInput2.entityType = "variable";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseType = "suchThat";
        suchThatClause.clauseVariable = "Uses";
        suchThatClause.firstVar = suchThatInput1;
        suchThatClause.secondVar = suchThatInput2;

        // For pattern
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "_";
        PatternInput1.entityType = "wildcard";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "r + 1_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThatClause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with such that(modify) and pattern clause") {
        QueryParser::setMainQuery(
                "Select ifs      pattern a   (  _    , \" 6 + r + 1\"_  )     such    that Modifies(6, w) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"ifs", "if"});
        inputMap.insert({"6", "stmtNum"});
        inputMap.insert({"w", "variable"});
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // For suchThat
        QueryParser::InputType suchThatInput1;
        suchThatInput1.var = "6";
        suchThatInput1.entityType = "stmtNum";

        QueryParser::InputType suchThatInput2;
        suchThatInput2.var = "w";
        suchThatInput2.entityType = "variable";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseType = "suchThat";
        suchThatClause.clauseVariable = "Modifies";
        suchThatClause.firstVar = suchThatInput1;
        suchThatClause.secondVar = suchThatInput2;

        // For pattern
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "_";
        PatternInput1.entityType = "wildcard";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "6 + r + 1_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "ifs";
        selectInput1.entityType = "if";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThatClause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with such that(modify) and with clause") {
        QueryParser::setMainQuery(
                "Select a     with a.stmt#  =  5   such    that Modifies(6, w) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "variable"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // For suchThat
        QueryParser::InputType suchThatInput1;
        suchThatInput1.var = "6";
        suchThatInput1.entityType = "stmtNum";

        QueryParser::InputType suchThatInput2;
        suchThatInput2.var = "w";
        suchThatInput2.entityType = "variable";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseType= "suchThat";
        suchThatClause.clauseVariable = "Modifies";
        suchThatClause.firstVar = suchThatInput1;
        suchThatClause.secondVar = suchThatInput2;

        // For with
        QueryParser::InputType withInput1;
        withInput1.var = "a.stmt#";
        withInput1.entityType = "assign";

        QueryParser::InputType withInput2;
        withInput2.var = "5";
        withInput2.entityType = "num";

        QueryParser::Clause withClause;
        withClause.clauseType= "with";
        withClause.clauseVariable = "equal";
        withClause.firstVar = withInput1;
        withClause.secondVar = withInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);
        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(withClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.second = withVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == withClause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == withClause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == withClause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == withClause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == withClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with pattern and with clause") {
        QueryParser::setMainQuery(
                "Select a     with a.stmt#  =  5   pattern a(v, _  ) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"v", "variable"});
        QueryParser::setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();

        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // For pattern
        QueryParser::InputType patternInput1;
        patternInput1.var = "v";
        patternInput1.entityType = "variable";

        QueryParser::InputType patternInput2;
        patternInput2.var = "_";
        patternInput2.entityType = "wildcard";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = patternInput1;
        patternClause.secondVar = patternInput2;

        // For with
        QueryParser::InputType withInput1;
        withInput1.var = "a.stmt#";
        withInput1.entityType = "assign";

        QueryParser::InputType withInput2;
        withInput2.var = "5";
        withInput2.entityType = "num";

        QueryParser::Clause withClause;
        withClause.clauseType= "assignPattern";
        withClause.clauseVariable = "equal";
        withClause.firstVar = withInput1;
        withClause.secondVar = withInput2;

        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(withClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.second.first = patternVec;
        expected.second.second.second = withVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == withClause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == withClause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == withClause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == withClause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == withClause.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }
}



// ******************* VALID ProcessQuery with multiple such that,pattern *****************************
TEST_CASE("Process VALID queries with multiple such that, pattern: processQuery Test") {

    SECTION( "valid query with such that and such that clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a such     that Follows*   (  a, 7) such that Modifies(w, v) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // 2nd such that
        QueryParser::InputType suchThat2Input1;
        suchThat2Input1.var = "w";
        suchThat2Input1.entityType = "while";

        QueryParser::InputType suchThat2Input2;
        suchThat2Input2.var = "v";
        suchThat2Input2.entityType = "variable";

        QueryParser::Clause suchThat2Clause;
        suchThat2Clause.clauseVariable = "Modifies";
        suchThat2Clause.firstVar = suchThat2Input1;
        suchThat2Clause.secondVar = suchThat2Input2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        suchThatVec.push_back(suchThat2Clause);
        std::vector<QueryParser::Clause> patternVec;
//        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(1).clauseVariable == suchThat2Clause.clauseVariable);
        REQUIRE(result.second.first.at(1).firstVar.var == suchThat2Clause.firstVar.var);
        REQUIRE(result.second.first.at(1).secondVar.var == suchThat2Clause.secondVar.var);
        REQUIRE(result.second.first.at(1).firstVar.entityType == suchThat2Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(1).secondVar.entityType == suchThat2Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with pattern clause pattern clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a pattern a(_, _\"x + 1\"_) pattern a42(ewr, \"x + 1\")");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a42", "assign"});
        inputMap.insert({"ewr", "variable"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        QueryParser::InputType pattern1Input1;
        pattern1Input1.var = "_";
        pattern1Input1.entityType = "wildcard";

        QueryParser::InputType pattern1Input2;
        pattern1Input2.var = "_x + 1_";
        pattern1Input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseVariable = "a";
        patternClause.firstVar = pattern1Input1;
        patternClause.secondVar = pattern1Input2;

        QueryParser::InputType pattern2Input1;
        pattern2Input1.var = "ewr";
        pattern2Input1.entityType = "variable";

        QueryParser::InputType pattern2Input2;
        pattern2Input2.var = "x + 1";
        pattern2Input2.entityType = "expression";

        QueryParser::Clause patternClause2;
        patternClause2.clauseVariable = "a42";
        patternClause2.firstVar = pattern2Input1;
        patternClause2.secondVar = pattern2Input2;

        std::vector<QueryParser::Clause> suchThatVec;
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        patternVec.push_back(patternClause2);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.at(1).clauseVariable == patternClause2.clauseVariable);
        REQUIRE(result.second.second.first.at(1).firstVar.var == patternClause2.firstVar.var);
        REQUIRE(result.second.second.first.at(1).secondVar.var == patternClause2.secondVar.var);
        REQUIRE(result.second.second.first.at(1).firstVar.entityType == patternClause2.firstVar.entityType);
        REQUIRE(result.second.second.first.at(1).secondVar.entityType == patternClause2.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with pattern clause 'and' pattern clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a pattern a(_, _\"x + 1    \"_     ) and a42(ewr, \"x  + 1 \"   )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a42", "assign"});
        inputMap.insert({"ewr", "variable"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        QueryParser::InputType pattern1Input1;
        pattern1Input1.var = "_";
        pattern1Input1.entityType = "wildcard";

        QueryParser::InputType pattern1Input2;
        pattern1Input2.var = "_x + 1_";
        pattern1Input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseVariable = "a";
        patternClause.firstVar = pattern1Input1;
        patternClause.secondVar = pattern1Input2;

        QueryParser::InputType pattern2Input1;
        pattern2Input1.var = "ewr";
        pattern2Input1.entityType = "variable";

        QueryParser::InputType pattern2Input2;
        pattern2Input2.var = "x + 1";
        pattern2Input2.entityType = "expression";

        QueryParser::Clause patternClause2;
        patternClause2.clauseVariable = "a42";
        patternClause2.firstVar = pattern2Input1;
        patternClause2.secondVar = pattern2Input2;

        std::vector<QueryParser::Clause> suchThatVec;
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        patternVec.push_back(patternClause2);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.at(1).clauseVariable == patternClause2.clauseVariable);
        REQUIRE(result.second.second.first.at(1).firstVar.var == patternClause2.firstVar.var);
        REQUIRE(result.second.second.first.at(1).secondVar.var == patternClause2.secondVar.var);
        REQUIRE(result.second.second.first.at(1).firstVar.entityType == patternClause2.firstVar.entityType);
        REQUIRE(result.second.second.first.at(1).secondVar.entityType == patternClause2.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with 2 such that, 1 pattern clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a such    that Follows*   (  a, 7  ) pattern a   (v, \"  i+1 *  3   \"_   )        such that Modifies(   w, v)       ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // 2nd such that
        QueryParser::InputType suchThat2Input1;
        suchThat2Input1.var = "w";
        suchThat2Input1.entityType = "while";

        QueryParser::InputType suchThat2Input2;
        suchThat2Input2.var = "v";
        suchThat2Input2.entityType = "variable";

        QueryParser::Clause suchThat2Clause;
        suchThat2Clause.clauseVariable = "Modifies";
        suchThat2Clause.firstVar = suchThat2Input1;
        suchThat2Clause.secondVar = suchThat2Input2;

        // pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        suchThatVec.push_back(suchThat2Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(1).clauseVariable == suchThat2Clause.clauseVariable);
        REQUIRE(result.second.first.at(1).firstVar.var == suchThat2Clause.firstVar.var);
        REQUIRE(result.second.first.at(1).secondVar.var == suchThat2Clause.secondVar.var);
        REQUIRE(result.second.first.at(1).firstVar.entityType == suchThat2Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(1).secondVar.entityType == suchThat2Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with 3 such that, 2 pattern clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a such    that Follows*   (  a, 7)  such that Modifies(w, v)   pattern a(v, \"i+1 *  3   \"_)   such    that Next*   (  a2, 27) pattern a2(v, \"a +7  \"_) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a2", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // 2nd such that
        QueryParser::InputType suchThat2Input1;
        suchThat2Input1.var = "w";
        suchThat2Input1.entityType = "while";

        QueryParser::InputType suchThat2Input2;
        suchThat2Input2.var = "v";
        suchThat2Input2.entityType = "variable";

        QueryParser::Clause suchThat2Clause;
        suchThat2Clause.clauseVariable = "Modifies";
        suchThat2Clause.firstVar = suchThat2Input1;
        suchThat2Clause.secondVar = suchThat2Input2;

        // 3rd such that
        QueryParser::InputType suchThat3Input1;
        suchThat3Input1.var = "a2";
        suchThat3Input1.entityType = "assign";

        QueryParser::InputType suchThat3Input2;
        suchThat3Input2.var = "27";
        suchThat3Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat3Clause;
        suchThat3Clause.clauseVariable = "Next*";
        suchThat3Clause.firstVar = suchThat3Input1;
        suchThat3Clause.secondVar = suchThat3Input2;

        // 1st pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        // 2nd pattern clauses
        QueryParser::InputType Pattern2Input1;
        Pattern2Input1.var = "v";
        Pattern2Input1.entityType = "variable";

        QueryParser::InputType Pattern2Input2;
        Pattern2Input2.var = "a +7_";
        Pattern2Input2.entityType = "expression";

        QueryParser::Clause pattern2Clause;
        pattern2Clause.clauseVariable = "a2";
        pattern2Clause.firstVar = Pattern2Input1;
        pattern2Clause.secondVar = Pattern2Input2;


        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        suchThatVec.push_back(suchThat2Clause);
        suchThatVec.push_back(suchThat3Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        patternVec.push_back(pattern2Clause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(1).clauseVariable == suchThat2Clause.clauseVariable);
        REQUIRE(result.second.first.at(1).firstVar.var == suchThat2Clause.firstVar.var);
        REQUIRE(result.second.first.at(1).secondVar.var == suchThat2Clause.secondVar.var);
        REQUIRE(result.second.first.at(1).firstVar.entityType == suchThat2Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(1).secondVar.entityType == suchThat2Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(2).clauseVariable == suchThat3Clause.clauseVariable);
        REQUIRE(result.second.first.at(2).firstVar.var == suchThat3Clause.firstVar.var);
        REQUIRE(result.second.first.at(2).secondVar.var == suchThat3Clause.secondVar.var);
        REQUIRE(result.second.first.at(2).firstVar.entityType == suchThat3Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(2).secondVar.entityType == suchThat3Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.at(1).clauseVariable == pattern2Clause.clauseVariable);
        REQUIRE(result.second.second.first.at(1).firstVar.var == pattern2Clause.firstVar.var);
        REQUIRE(result.second.second.first.at(1).secondVar.var == pattern2Clause.secondVar.var);
        REQUIRE(result.second.second.first.at(1).firstVar.entityType == pattern2Clause.firstVar.entityType);
        REQUIRE(result.second.second.first.at(1).secondVar.entityType == pattern2Clause.secondVar.entityType);

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

}

// ******************* VALID ProcessQuery with multiple such that, with, pattern *****************************
TEST_CASE("Process VALID queries with multiple such that, with, pattern: processQuery Test") {

    SECTION("valid query with 'with', 'with' clause") {
        QueryParser queryParser;
        queryParser.setMainQuery("Select <a, p.procName, n, a.stmt#> with a.stmt# = n with p.procName = v.varName ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p", "procedure"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // 2nd with
        QueryParser::InputType with2Input1;
        with2Input1.var = "p.procName";
        with2Input1.entityType = "procedure";

        QueryParser::InputType with2Input2;
        with2Input2.var = "v.varName";
        with2Input2.entityType = "variable";

        QueryParser::Clause with2Clause;
        with2Clause.clauseType = "with";
        with2Clause.clauseVariable = "equal";
        with2Clause.firstVar = with2Input1;
        with2Clause.secondVar = with2Input2;

        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        withVec.push_back(with2Clause);
        std::vector<QueryParser::Clause> patternVec;

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        QueryParser::InputType selectInput2;
        selectInput2.var = "p.procName";
        selectInput2.entityType = "procedure";
        selectVec.emplace_back(selectInput2);

        QueryParser::InputType selectInput3;
        selectInput3.var = "n";
        selectInput3.entityType = "prog_line";
        selectVec.emplace_back(selectInput3);

        QueryParser::InputType selectInput4;
        selectInput4.var = "a.stmt#";
        selectInput4.entityType = "assign";
        selectVec.emplace_back(selectInput4);

        expected.first = selectVec;
        expected.second.second.second = withVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.first.at(1).var == expected.first.at(1).var);
        REQUIRE(result.first.at(1).entityType == expected.first.at(1).entityType);
        REQUIRE(result.first.at(2).var == expected.first.at(2).var);
        REQUIRE(result.first.at(2).entityType == expected.first.at(2).entityType);
        REQUIRE(result.first.at(3).var == expected.first.at(3).var);
        REQUIRE(result.first.at(3).entityType == expected.first.at(3).entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.second.second.at(1).clauseVariable == with2Clause.clauseVariable);
        REQUIRE(result.second.second.second.at(1).firstVar.var == with2Clause.firstVar.var);
        REQUIRE(result.second.second.second.at(1).secondVar.var == with2Clause.secondVar.var);
        REQUIRE(result.second.second.second.at(1).firstVar.entityType == with2Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.at(1).secondVar.entityType == with2Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with 'with', 'such that' clause") {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a with a.stmt# = n     such that Modifies(w, v)  ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // For suchThat
        QueryParser::InputType suchThatInput1;
        suchThatInput1.var = "w";
        suchThatInput1.entityType = "while";

        QueryParser::InputType suchThatInput2;
        suchThatInput2.var = "v";
        suchThatInput2.entityType = "variable";

        QueryParser::Clause suchThatClause;
        suchThatClause.clauseVariable = "Modifies";
        suchThatClause.firstVar = suchThatInput1;
        suchThatClause.secondVar = suchThatInput2;

        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThatClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.second.second = withVec;
        expected.second.first = suchThatVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.first.front().clauseVariable == suchThatClause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThatClause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThatClause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThatClause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThatClause.secondVar.entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with 'with', 'pattern' clause") {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a  pattern a(v, \" i+  1 *  3 \" _   )   with a.stmt# = n    ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // 1st pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+ 1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.second.second = withVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION("valid query with 'with', 'pattern', 'suchThat' clause") {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a  pattern a(v, \" i+  1 *  3 \" _   )   with  a.stmt#      =    n   such     that Follows*   (  a, 7)  ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseType = "suchThat";
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // 1st pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+ 1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        with1Clause.clauseType = "pattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.second = withVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }
}


// ******************* VALID ProcessQuery with multiple such that, with, pattern with "AND" *****************************

TEST_CASE("Process VALID queries with multiple such that, with, and, pattern: processQuery Test") {

    SECTION( "valid query with 2 'such that', 2 'and' clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a  such     that Next*   (  a, 7) and Modifies(w, v) and Affects(a    , a2      )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a2", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseType = "suchThat";
        suchThat1Clause.clauseVariable = "Next*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // 2nd such that
        QueryParser::InputType suchThat2Input1;
        suchThat2Input1.var = "w";
        suchThat2Input1.entityType = "while";

        QueryParser::InputType suchThat2Input2;
        suchThat2Input2.var = "v";
        suchThat2Input2.entityType = "variable";

        QueryParser::Clause suchThat2Clause;
        suchThat2Clause.clauseType = "suchThat";
        suchThat2Clause.clauseVariable = "Modifies";
        suchThat2Clause.firstVar = suchThat2Input1;
        suchThat2Clause.secondVar = suchThat2Input2;

        // 3rd such that
        QueryParser::InputType suchThat3Input1;
        suchThat3Input1.var = "a";
        suchThat3Input1.entityType = "assign";

        QueryParser::InputType suchThat3Input2;
        suchThat3Input2.var = "a2";
        suchThat3Input2.entityType = "assign";

        QueryParser::Clause suchThat3Clause;
        suchThat3Clause.clauseType = "suchThat";
        suchThat3Clause.clauseVariable = "Affects";
        suchThat3Clause.firstVar = suchThat3Input1;
        suchThat3Clause.secondVar = suchThat3Input2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        suchThatVec.push_back(suchThat2Clause);
        suchThatVec.push_back(suchThat3Clause);
        std::vector<QueryParser::Clause> patternVec;

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThat1Clause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(1).clauseType == suchThat2Clause.clauseType);
        REQUIRE(result.second.first.at(1).clauseVariable == suchThat2Clause.clauseVariable);
        REQUIRE(result.second.first.at(1).firstVar.var == suchThat2Clause.firstVar.var);
        REQUIRE(result.second.first.at(1).secondVar.var == suchThat2Clause.secondVar.var);
        REQUIRE(result.second.first.at(1).firstVar.entityType == suchThat2Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(1).secondVar.entityType == suchThat2Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(2).clauseType == suchThat3Clause.clauseType);
        REQUIRE(result.second.first.at(2).clauseVariable == suchThat3Clause.clauseVariable);
        REQUIRE(result.second.first.at(2).firstVar.var == suchThat3Clause.firstVar.var);
        REQUIRE(result.second.first.at(2).secondVar.var == suchThat3Clause.secondVar.var);
        REQUIRE(result.second.first.at(2).firstVar.entityType == suchThat3Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(2).secondVar.entityType == suchThat3Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with 3 'pattern', 2 'and' clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select BOOLEAN pattern a(_, _       \"count * 3 + 1\"_ ) and as3     (ewr, \"x + 1\" )     and ifs   (ewr   , _, _  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"as3", "assign"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"ewr", "variable"});
        inputMap.insert({"ifs", "if"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        QueryParser::InputType pattern1Input1;
        pattern1Input1.var = "_";
        pattern1Input1.entityType = "wildcard";

        QueryParser::InputType pattern1Input2;
        pattern1Input2.var = "_count * 3 + 1_";
        pattern1Input2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = pattern1Input1;
        patternClause.secondVar = pattern1Input2;

        QueryParser::InputType pattern2Input1;
        pattern2Input1.var = "ewr";
        pattern2Input1.entityType = "variable";

        QueryParser::InputType pattern2Input2;
        pattern2Input2.var = "x + 1";
        pattern2Input2.entityType = "expression";

        QueryParser::Clause patternClause2;
        patternClause2.clauseType = "assignPattern";
        patternClause2.clauseVariable = "as3";
        patternClause2.firstVar = pattern2Input1;
        patternClause2.secondVar = pattern2Input2;

        // 3rd
        QueryParser::InputType pattern3Input1;
        pattern3Input1.var = "ewr";
        pattern3Input1.entityType = "variable";

        QueryParser::InputType pattern3Input2;
        pattern3Input2.var = "_,_";
        pattern3Input2.entityType = "wildcard";

        QueryParser::Clause patternClause3;
        patternClause3.clauseType = "ifPattern";
        patternClause3.clauseVariable = "ifs";
        patternClause3.firstVar = pattern3Input1;
        patternClause3.secondVar = pattern3Input2;

        std::vector<QueryParser::Clause> suchThatVec;
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        patternVec.push_back(patternClause2);
        patternVec.push_back(patternClause3);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "BOOLEAN";
        selectInput1.entityType = "boolean";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;

        REQUIRE(QueryParser::getIsBoolean() == true);
        REQUIRE(QueryParser::getErrorMsg() == "");
        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.at(1).clauseType == patternClause2.clauseType);
        REQUIRE(result.second.second.first.at(1).clauseVariable == patternClause2.clauseVariable);
        REQUIRE(result.second.second.first.at(1).firstVar.var == patternClause2.firstVar.var);
        REQUIRE(result.second.second.first.at(1).secondVar.var == patternClause2.secondVar.var);
        REQUIRE(result.second.second.first.at(1).firstVar.entityType == patternClause2.firstVar.entityType);
        REQUIRE(result.second.second.first.at(1).secondVar.entityType == patternClause2.secondVar.entityType);
        REQUIRE(result.second.second.first.at(2).clauseType == patternClause3.clauseType);
        REQUIRE(result.second.second.first.at(2).clauseVariable == patternClause3.clauseVariable);
        REQUIRE(result.second.second.first.at(2).firstVar.var == patternClause3.firstVar.var);
        REQUIRE(result.second.second.first.at(2).secondVar.var == patternClause3.secondVar.var);
        REQUIRE(result.second.second.first.at(2).firstVar.entityType == patternClause3.firstVar.entityType);
        REQUIRE(result.second.second.first.at(2).secondVar.entityType == patternClause3.secondVar.entityType);
        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION("valid query with 2 'with', 1 'and' clause") {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a with a.stmt# = n and p.procName = v.varName ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p", "procedure"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // 2nd with
        QueryParser::InputType with2Input1;
        with2Input1.var = "p.procName";
        with2Input1.entityType = "procedure";

        QueryParser::InputType with2Input2;
        with2Input2.var = "v.varName";
        with2Input2.entityType = "variable";

        QueryParser::Clause with2Clause;
        with2Clause.clauseType = "with";
        with2Clause.clauseVariable = "equal";
        with2Clause.firstVar = with2Input1;
        with2Clause.secondVar = with2Input2;

        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        withVec.push_back(with2Clause);
        std::vector<QueryParser::Clause> patternVec;

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.second.second = withVec;
        expected.second.second.first = patternVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseType == with1Clause.clauseType);
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.second.second.at(1).clauseType == with2Clause.clauseType);
        REQUIRE(result.second.second.second.at(1).clauseVariable == with2Clause.clauseVariable);
        REQUIRE(result.second.second.second.at(1).firstVar.var == with2Clause.firstVar.var);
        REQUIRE(result.second.second.second.at(1).secondVar.var == with2Clause.secondVar.var);
        REQUIRE(result.second.second.second.at(1).firstVar.entityType == with2Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.at(1).secondVar.entityType == with2Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

}

// ******************* VALID ProcessQuery with mixed such that, with, pattern with "AND" *****************************


TEST_CASE("Process VALID queries with mixed such that, with, and, pattern: processQuery Test") {

    SECTION( "valid query with 2 such that connected with 'and', 1 pattern, 1 with clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a such    that Follows*   (  a, 7  )        and Modifies(   w, v)   with c.procName = r.varName    pattern a   (v, \"  i+1 *  3   \"_   ) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"r", "read"});
        inputMap.insert({"c", "call"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // 2nd such that
        QueryParser::InputType suchThat2Input1;
        suchThat2Input1.var = "w";
        suchThat2Input1.entityType = "while";

        QueryParser::InputType suchThat2Input2;
        suchThat2Input2.var = "v";
        suchThat2Input2.entityType = "variable";

        QueryParser::Clause suchThat2Clause;
        suchThat2Clause.clauseVariable = "Modifies";
        suchThat2Clause.firstVar = suchThat2Input1;
        suchThat2Clause.secondVar = suchThat2Input2;

        // with
        QueryParser::InputType with2Input1;
        with2Input1.var = "c.procName";
        with2Input1.entityType = "call";

        QueryParser::InputType with2Input2;
        with2Input2.var = "r.varName";
        with2Input2.entityType = "read";

        QueryParser::Clause with2Clause;
        with2Clause.clauseType = "with";
        with2Clause.clauseVariable = "equal";
        with2Clause.firstVar = with2Input1;
        with2Clause.secondVar = with2Input2;

        // pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        suchThatVec.push_back(suchThat2Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with2Clause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;
        expected.second.second.second = withVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(1).clauseVariable == suchThat2Clause.clauseVariable);
        REQUIRE(result.second.first.at(1).firstVar.var == suchThat2Clause.firstVar.var);
        REQUIRE(result.second.first.at(1).secondVar.var == suchThat2Clause.secondVar.var);
        REQUIRE(result.second.first.at(1).firstVar.entityType == suchThat2Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(1).secondVar.entityType == suchThat2Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.second.front().clauseType == with2Clause.clauseType);
        REQUIRE(result.second.second.second.front().clauseVariable == with2Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with2Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with2Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with2Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with2Clause.secondVar.entityType);

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with 1 such that, 1 with, 2 pattern connected by 'and' clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a such    that Follows*   (  a, 7) with c.procName = r.varName  pattern a(v, \"i+1 *  3   \"_)   and a2(v, \"a +7  \"_) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a2", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"r", "read"});
        inputMap.insert({"c", "call"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseType = "suchThat";
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // with
        QueryParser::InputType with2Input1;
        with2Input1.var = "c.procName";
        with2Input1.entityType = "call";

        QueryParser::InputType with2Input2;
        with2Input2.var = "r.varName";
        with2Input2.entityType = "read";

        QueryParser::Clause with2Clause;
        with2Clause.clauseType = "with";
        with2Clause.clauseVariable = "equal";
        with2Clause.firstVar = with2Input1;
        with2Clause.secondVar = with2Input2;

        // 1st pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        // 2nd pattern clauses
        QueryParser::InputType Pattern2Input1;
        Pattern2Input1.var = "v";
        Pattern2Input1.entityType = "variable";

        QueryParser::InputType Pattern2Input2;
        Pattern2Input2.var = "a +7_";
        Pattern2Input2.entityType = "expression";

        QueryParser::Clause pattern2Clause;
        pattern2Clause.clauseType = "assignPattern";
        pattern2Clause.clauseVariable = "a2";
        pattern2Clause.firstVar = Pattern2Input1;
        pattern2Clause.secondVar = Pattern2Input2;


        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        patternVec.push_back(pattern2Clause);
        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with2Clause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;
        expected.second.second.second = withVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThat1Clause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.at(1).clauseType == pattern2Clause.clauseType);
        REQUIRE(result.second.second.first.at(1).clauseVariable == pattern2Clause.clauseVariable);
        REQUIRE(result.second.second.first.at(1).firstVar.var == pattern2Clause.firstVar.var);
        REQUIRE(result.second.second.first.at(1).secondVar.var == pattern2Clause.secondVar.var);
        REQUIRE(result.second.second.first.at(1).firstVar.entityType == pattern2Clause.firstVar.entityType);
        REQUIRE(result.second.second.first.at(1).secondVar.entityType == pattern2Clause.secondVar.entityType);
        REQUIRE(result.second.second.second.front().clauseType == with2Clause.clauseType);
        REQUIRE(result.second.second.second.front().clauseVariable == with2Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with2Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with2Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with2Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with2Clause.secondVar.entityType);

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with 1 such that, 1 pattern, 2 with connected by 'and' clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select <a> such    that Follows*   (  a, 7) with a.stmt# = n  and c.procName = r.varName  pattern a(v, \"i+1 * 3 \"_  )  ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a2", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"r", "read"});
        inputMap.insert({"c", "call"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseType = "suchThat";
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // 2nd with
        QueryParser::InputType with2Input1;
        with2Input1.var = "c.procName";
        with2Input1.entityType = "call";

        QueryParser::InputType with2Input2;
        with2Input2.var = "r.varName";
        with2Input2.entityType = "read";

        QueryParser::Clause with2Clause;
        with2Clause.clauseType = "with";
        with2Clause.clauseVariable = "equal";
        with2Clause.firstVar = with2Input1;
        with2Clause.secondVar = with2Input2;

        // 1st pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;


        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        withVec.push_back(with2Clause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;
        expected.second.second.second = withVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThat1Clause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.second.second.at(1).clauseVariable == with2Clause.clauseVariable);
        REQUIRE(result.second.second.second.at(1).firstVar.var == with2Clause.firstVar.var);
        REQUIRE(result.second.second.second.at(1).secondVar.var == with2Clause.secondVar.var);
        REQUIRE(result.second.second.second.at(1).firstVar.entityType == with2Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.at(1).secondVar.entityType == with2Clause.secondVar.entityType);

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

    SECTION( "valid query with 2 such that, 2 pattern, 2 with connected by 'and' clause" ) {
        QueryParser queryParser;
        queryParser.setMainQuery("Select a such    that Follows*   (  a, 7)       and Modifies(   w, v) with a.stmt# = n  and c.procName = r.varName  pattern a(v, \"i+1 * 3 \"_  ) and a2(v, \"a +7  \"_) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"a2", "assign"});
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"r", "read"});
        inputMap.insert({"c", "call"});
        inputMap.insert({"n", "prog_line"});
        queryParser.setDeclarationsMap(inputMap);
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> result = QueryParser::processQuery();
        std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> expected;

        // for suchThat
        QueryParser::InputType suchThat1Input1;
        suchThat1Input1.var = "a";
        suchThat1Input1.entityType = "assign";

        QueryParser::InputType suchThat1Input2;
        suchThat1Input2.var = "7";
        suchThat1Input2.entityType = "stmtNum";

        QueryParser::Clause suchThat1Clause;
        suchThat1Clause.clauseType = "suchThat";
        suchThat1Clause.clauseVariable = "Follows*";
        suchThat1Clause.firstVar = suchThat1Input1;
        suchThat1Clause.secondVar = suchThat1Input2;

        // 2nd such that
        QueryParser::InputType suchThat2Input1;
        suchThat2Input1.var = "w";
        suchThat2Input1.entityType = "while";

        QueryParser::InputType suchThat2Input2;
        suchThat2Input2.var = "v";
        suchThat2Input2.entityType = "variable";

        QueryParser::Clause suchThat2Clause;
        suchThat1Clause.clauseType = "suchThat";
        suchThat2Clause.clauseVariable = "Modifies";
        suchThat2Clause.firstVar = suchThat2Input1;
        suchThat2Clause.secondVar = suchThat2Input2;

        // for 1st with
        QueryParser::InputType with1Input1;
        with1Input1.var = "a.stmt#";
        with1Input1.entityType = "assign";

        QueryParser::InputType with1Input2;
        with1Input2.var = "n";
        with1Input2.entityType = "prog_line";

        QueryParser::Clause with1Clause;
        with1Clause.clauseType = "with";
        with1Clause.clauseVariable = "equal";
        with1Clause.firstVar = with1Input1;
        with1Clause.secondVar = with1Input2;

        // 2nd with
        QueryParser::InputType with2Input1;
        with2Input1.var = "c.procName";
        with2Input1.entityType = "call";

        QueryParser::InputType with2Input2;
        with2Input2.var = "r.varName";
        with2Input2.entityType = "read";

        QueryParser::Clause with2Clause;
        with2Clause.clauseType = "with";
        with2Clause.clauseVariable = "equal";
        with2Clause.firstVar = with2Input1;
        with2Clause.secondVar = with2Input2;

        // 1st pattern clauses
        QueryParser::InputType PatternInput1;
        PatternInput1.var = "v";
        PatternInput1.entityType = "variable";

        QueryParser::InputType PatternInput2;
        PatternInput2.var = "i+1 * 3_";
        PatternInput2.entityType = "expression";

        QueryParser::Clause patternClause;
        patternClause.clauseType = "assignPattern";
        patternClause.clauseVariable = "a";
        patternClause.firstVar = PatternInput1;
        patternClause.secondVar = PatternInput2;

        // 2nd pattern clauses
        QueryParser::InputType Pattern2Input1;
        Pattern2Input1.var = "v";
        Pattern2Input1.entityType = "variable";

        QueryParser::InputType Pattern2Input2;
        Pattern2Input2.var = "a +7_";
        Pattern2Input2.entityType = "expression";

        QueryParser::Clause pattern2Clause;
        pattern2Clause.clauseType = "assignPattern";
        pattern2Clause.clauseVariable = "a2";
        pattern2Clause.firstVar = Pattern2Input1;
        pattern2Clause.secondVar = Pattern2Input2;


        std::vector<QueryParser::Clause> suchThatVec;
        suchThatVec.push_back(suchThat1Clause);
        suchThatVec.push_back(suchThat2Clause);
        std::vector<QueryParser::Clause> patternVec;
        patternVec.push_back(patternClause);
        patternVec.push_back(pattern2Clause);
        std::vector<QueryParser::Clause> withVec;
        withVec.push_back(with1Clause);
        withVec.push_back(with2Clause);

        std::vector<QueryParser::InputType> selectVec;
        QueryParser::InputType selectInput1;
        selectInput1.var = "a";
        selectInput1.entityType = "assign";
        selectVec.emplace_back(selectInput1);

        expected.first = selectVec;
        expected.second.first = suchThatVec;
        expected.second.second.first = patternVec;
        expected.second.second.second = withVec;

        REQUIRE(result.first.size() == expected.first.size());
        REQUIRE(result.first.front().var == expected.first.front().var);
        REQUIRE(result.first.front().entityType == expected.first.front().entityType);
        REQUIRE(result.second.first.size() == expected.second.first.size());
        REQUIRE(result.second.first.front().clauseType == suchThat1Clause.clauseType);
        REQUIRE(result.second.first.front().clauseVariable == suchThat1Clause.clauseVariable);
        REQUIRE(result.second.first.front().firstVar.var == suchThat1Clause.firstVar.var);
        REQUIRE(result.second.first.front().secondVar.var == suchThat1Clause.secondVar.var);
        REQUIRE(result.second.first.front().firstVar.entityType == suchThat1Clause.firstVar.entityType);
        REQUIRE(result.second.first.front().secondVar.entityType == suchThat1Clause.secondVar.entityType);
        REQUIRE(result.second.first.at(1).clauseVariable == suchThat2Clause.clauseVariable);
        REQUIRE(result.second.first.at(1).firstVar.var == suchThat2Clause.firstVar.var);
        REQUIRE(result.second.first.at(1).secondVar.var == suchThat2Clause.secondVar.var);
        REQUIRE(result.second.first.at(1).firstVar.entityType == suchThat2Clause.firstVar.entityType);
        REQUIRE(result.second.first.at(1).secondVar.entityType == suchThat2Clause.secondVar.entityType);
        REQUIRE(result.second.second.first.size() == expected.second.second.first.size());
        REQUIRE(result.second.second.first.front().clauseType == patternClause.clauseType);
        REQUIRE(result.second.second.first.front().clauseVariable == patternClause.clauseVariable);
        REQUIRE(result.second.second.first.front().firstVar.var == patternClause.firstVar.var);
        REQUIRE(result.second.second.first.front().secondVar.var == patternClause.secondVar.var);
        REQUIRE(result.second.second.first.front().firstVar.entityType == patternClause.firstVar.entityType);
        REQUIRE(result.second.second.first.front().secondVar.entityType == patternClause.secondVar.entityType);
        REQUIRE(result.second.second.first.at(1).clauseType == pattern2Clause.clauseType);
        REQUIRE(result.second.second.first.at(1).clauseVariable == pattern2Clause.clauseVariable);
        REQUIRE(result.second.second.first.at(1).firstVar.var == pattern2Clause.firstVar.var);
        REQUIRE(result.second.second.first.at(1).secondVar.var == pattern2Clause.secondVar.var);
        REQUIRE(result.second.second.first.at(1).firstVar.entityType == pattern2Clause.firstVar.entityType);
        REQUIRE(result.second.second.first.at(1).secondVar.entityType == pattern2Clause.secondVar.entityType);
        REQUIRE(result.second.second.second.size() == expected.second.second.second.size());
        REQUIRE(result.second.second.second.front().clauseVariable == with1Clause.clauseVariable);
        REQUIRE(result.second.second.second.front().firstVar.var == with1Clause.firstVar.var);
        REQUIRE(result.second.second.second.front().secondVar.var == with1Clause.secondVar.var);
        REQUIRE(result.second.second.second.front().firstVar.entityType == with1Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.front().secondVar.entityType == with1Clause.secondVar.entityType);
        REQUIRE(result.second.second.second.at(1).clauseVariable == with2Clause.clauseVariable);
        REQUIRE(result.second.second.second.at(1).firstVar.var == with2Clause.firstVar.var);
        REQUIRE(result.second.second.second.at(1).secondVar.var == with2Clause.secondVar.var);
        REQUIRE(result.second.second.second.at(1).firstVar.entityType == with2Clause.firstVar.entityType);
        REQUIRE(result.second.second.second.at(1).secondVar.entityType == with2Clause.secondVar.entityType);

        std::map<std::string, std::string> cleanMap;
        queryParser.setDeclarationsMap(cleanMap);
    }

}


// ******************* INVALID ProcessQuery *****************************
TEST_CASE("Process invalid queries") {

    SECTION( "invalid query with incomplete clause" ) {
        QueryParser::setMainQuery("Select a   such       pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid query with missing desired var ") {
        QueryParser::setMainQuery("Select such   that      pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid query with missing clause ") {
        QueryParser::setMainQuery("Select a  such    that Follows*   (  a, 7)  such that    (  w, 7)   pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid query in wrong format") {
        QueryParser::setMainQuery(" a Select a  such    that Follows*   (  a, 7)     pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid such that, valid pattern clauses ") {
        QueryParser::setMainQuery("Select a  such    that Follows*   (  \"blah\", 7)     pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid pattern, valid such that clauses ") {
        QueryParser::setMainQuery("Select a  such    that Follows*   (  3, 7)     pattern a(  _, x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid 'pattern' keyword  ") {
        QueryParser::setMainQuery("Select a  such    that Follows*   (  5, 7)      patrn a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid such that clauses ") {
        QueryParser::setMainQuery("Select a  such    that Modifies(_, \"count\")");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid parent clauses ") {
        QueryParser::setMainQuery("Select a  such    that Parent(4, \"count\")");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION( "invalid parent clauses with same inputs") {
        QueryParser::setMainQuery("Select a  such    that Parent(4, 4)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

}

// ******************* INVALID ProcessQuery with 'and' operator *****************************
TEST_CASE("Process invalid queries with 'and' operator") {

    SECTION("invalid query with suchThat, pattern connected by 'and' ") {
        QueryParser::setMainQuery("Select a  such    that Parent(2, 4) and a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("invalid query with suchThat, with connected by 'and' ") {
        QueryParser::setMainQuery("Select a   with p.procName = v.varName and Parent(1, 4)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p", "assign"});
        inputMap.insert({"v", "variable"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("invalid query with pattern, with connected by 'and' ") {
        QueryParser::setMainQuery("Select a  pattern a(  _, \"x + 1\"_  ) and p.procName = v.varName");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p", "assign"});
        inputMap.insert({"v", "variable"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("invalid 'and' format query") {
        QueryParser::setMainQuery("Select a  and a(  _, \"x + 1\"_  ) such    that Parent(3, 4) ");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("invalid query with extra 'and' ") {
        QueryParser::setMainQuery("Select a  such    that Parent(3, 4) and pattern a(  _, \"x + 1\"_  )");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

    SECTION("invalid query with invalid reference ") {
        QueryParser::setMainQuery("Select <a.varName> such    that Parent(3, 4)");

        std::map<std::string, std::string> inputMap;
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        REQUIRE_THROWS(QueryParser::processQuery());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
    }

}


