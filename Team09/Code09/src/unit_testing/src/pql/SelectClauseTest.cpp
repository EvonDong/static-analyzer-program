#include "QueryParser.h"
#include "QueryParserHelper.h"
#include "catch.hpp"

#include <unordered_set>
#include <string>


// ******************* processDesiredVar VALID *****************************
TEST_CASE("Valid processDesiredVar Test") {

    SECTION( "valid mainQuery with single variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      w such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("w");

        std::vector<QueryParser::InputType> result = QueryParser::processDesiredVar(selectVarVec);

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "w";
        input1.entityType = "while";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with single variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      w.stmt# such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("w.stmt#");

        std::vector<QueryParser::InputType> result = QueryParser::processDesiredVar(selectVarVec);

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "w.stmt#";
        input1.entityType = "while";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with single variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"c", "call"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      c.procName");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("c.procName");

        std::vector<QueryParser::InputType> result = QueryParser::processDesiredVar(selectVarVec);

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "c.procName";
        input1.entityType = "call";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with boolean variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      BOOLEAN such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("BOOLEAN");

        std::vector<QueryParser::InputType> result = QueryParser::processDesiredVar(selectVarVec);

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "BOOLEAN";
        input1.entityType = "boolean";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == true);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with tuple variables" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p3", "print"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <v.varName, a> such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("v.varName");
        selectVarVec.emplace_back("a");

        std::vector<QueryParser::InputType> result = QueryParser::processDesiredVar(selectVarVec);

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "a";
        input2.entityType = "assign";

        expectedVec.emplace_back(input1);
        expectedVec.emplace_back(input2);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(result.at(1).var == expectedVec.at(1).var);
        REQUIRE(result.at(1).entityType == expectedVec.at(1).entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with single tuple variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p3", "print"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <v.varName> such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("v.varName");

        std::vector<QueryParser::InputType> result = QueryParser::processDesiredVar(selectVarVec);

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }
}

// ******************* processDesiredVar INVALID *****************************
TEST_CASE("Invalid processDesiredVar Test") {

    SECTION( "invalid mainQuery with invalid variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      w?? such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("w??");

        REQUIRE_THROWS(QueryParser::processDesiredVar(selectVarVec));
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with undeclared variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      a such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("a");

        REQUIRE_THROWS(QueryParser::processDesiredVar(selectVarVec));
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with undeclared variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      a.procName such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("a.procName");

        REQUIRE_THROWS(QueryParser::processDesiredVar(selectVarVec));
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with undeclared variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      a.blah such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("a.blah");

        REQUIRE_THROWS(QueryParser::processDesiredVar(selectVarVec));
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with double boolean variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <BOOLEAN, BOOLEAN> such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;
        selectVarVec.emplace_back("BOOLEAN");
        selectVarVec.emplace_back("BOOLEAN");

        REQUIRE_THROWS(QueryParser::processDesiredVar(selectVarVec));
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with not-single boolean variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select   such that Parent(w, 7)");

        std::vector<std::string> selectVarVec;

        REQUIRE_THROWS(QueryParser::processDesiredVar(selectVarVec));
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

}





// ******************* GetDesiredVar VALID *****************************
TEST_CASE("Remove select v and get desired variable: VALID getDesiredVar Test") {

    SECTION( "valid mainQuery with single variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      w such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "w";
        input1.entityType = "while";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with single variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      w.stmt# such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "w.stmt#";
        input1.entityType = "while";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with single variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"c", "call"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      c.procName");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "c.procName";
        input1.entityType = "call";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with boolean variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      BOOLEAN such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "BOOLEAN";
        input1.entityType = "boolean";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == true);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with boolean variable declared as call" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"BOOLEAN", "call"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      BOOLEAN such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "BOOLEAN";
        input1.entityType = "call";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with only boolean variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      BOOLEAN");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "BOOLEAN";
        input1.entityType = "boolean";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == true);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with declared boolean with other tuple" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"BOOLEAN", "print"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select     <v.varName, BOOLEAN , a>");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "BOOLEAN";
        input2.entityType = "print";

        QueryParser::InputType input3;
        input3.var = "a";
        input3.entityType = "assign";

        expectedVec.emplace_back(input1);
        expectedVec.emplace_back(input2);
        expectedVec.emplace_back(input3);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with tuple variables" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p3", "print"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <v.varName, a> such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "a";
        input2.entityType = "assign";

        expectedVec.emplace_back(input1);
        expectedVec.emplace_back(input2);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(result.at(1).var == expectedVec.at(1).var);
        REQUIRE(result.at(1).entityType == expectedVec.at(1).entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with only tuple variables" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p3", "print"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <    v.varName, a>");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "a";
        input2.entityType = "assign";

        expectedVec.emplace_back(input1);
        expectedVec.emplace_back(input2);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(result.at(1).var == expectedVec.at(1).var);
        REQUIRE(result.at(1).entityType == expectedVec.at(1).entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "valid mainQuery with single tuple variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p3", "print"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <v.varName> such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        expectedVec.emplace_back(input1);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }


    SECTION( "valid mainQuery with tuple variables" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        inputMap.insert({"v", "variable"});
        inputMap.insert({"a", "assign"});
        inputMap.insert({"p3", "print"});
        inputMap.insert({"c", "constant"});
        inputMap.insert({"p", "procedure"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <    v.varName   , a , p3.stmt# , p3.varName,    c.value,  p.procName, w , w , p > such that Parent(w, 7)");

        std::vector<QueryParser::InputType> result = QueryParser::getDesiredVar();

        std::vector<QueryParser::InputType> expectedVec;
        QueryParser::InputType input1;
        input1.var = "v.varName";
        input1.entityType = "variable";

        QueryParser::InputType input2;
        input2.var = "a";
        input2.entityType = "assign";

        QueryParser::InputType input3;
        input3.var = "p3.stmt#";
        input3.entityType = "print";

        QueryParser::InputType input4;
        input4.var = "p3.varName";
        input4.entityType = "print";

        QueryParser::InputType input5;
        input5.var = "c.value";
        input5.entityType = "constant";

        QueryParser::InputType input6;
        input6.var = "p.procName";
        input6.entityType = "procedure";

        QueryParser::InputType input7;
        input7.var = "w";
        input7.entityType = "while";

        QueryParser::InputType input8;
        input8.var = "w";
        input8.entityType = "while";

        QueryParser::InputType input9;
        input9.var = "p";
        input9.entityType = "procedure";

        expectedVec.emplace_back(input1);
        expectedVec.emplace_back(input2);
        expectedVec.emplace_back(input3);
        expectedVec.emplace_back(input4);
        expectedVec.emplace_back(input5);
        expectedVec.emplace_back(input6);
        expectedVec.emplace_back(input7);
        expectedVec.emplace_back(input8);
        expectedVec.emplace_back(input9);

        REQUIRE(result.size() == expectedVec.size());
        REQUIRE(result.front().var == expectedVec.front().var);
        REQUIRE(result.front().entityType == expectedVec.front().entityType);
        REQUIRE(result.at(1).var == expectedVec.at(1).var);
        REQUIRE(result.at(1).entityType == expectedVec.at(1).entityType);
        REQUIRE(result.at(2).var == expectedVec.at(2).var);
        REQUIRE(result.at(2).entityType == expectedVec.at(2).entityType);
        REQUIRE(result.at(3).var == expectedVec.at(3).var);
        REQUIRE(result.at(3).entityType == expectedVec.at(3).entityType);
        REQUIRE(result.at(4).var == expectedVec.at(4).var);
        REQUIRE(result.at(4).entityType == expectedVec.at(4).entityType);
        REQUIRE(result.at(5).var == expectedVec.at(5).var);
        REQUIRE(result.at(5).entityType == expectedVec.at(5).entityType);
        REQUIRE(result.at(6).var == expectedVec.at(6).var);
        REQUIRE(result.at(6).entityType == expectedVec.at(6).entityType);
        REQUIRE(result.at(7).var == expectedVec.at(7).var);
        REQUIRE(result.at(7).entityType == expectedVec.at(7).entityType);
        REQUIRE(result.at(8).var == expectedVec.at(8).var);
        REQUIRE(result.at(8).entityType == expectedVec.at(8).entityType);
        REQUIRE(QueryParser::getIsBoolean() == false);
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }
}



// ******************* GetDesiredVar INVALID *****************************
TEST_CASE("Invalid getDesiredVar Test") {

    SECTION( "invalid mainQuery with invalid variable" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      w?? such that Parent(w, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with extra invalid" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select      a 31 such that Parent(w, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with only 'select' " ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with missing left tuple sign" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  w, w> such that Parent(w, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with missing right tuple sign" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select  <w, w such that Parent(w, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with no select var" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select   such that Parent(w, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with no select var" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select w  suent(w, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with input as pattern" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select w  (pattern, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with invalid words behind select var" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select w  42 with)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with random words after tuple" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select <w, w>  42 with)");
//        QueryParser::getDesiredVar();
        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with random pattern input after tuple" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select <     w , w>  (pattern, 7)");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

    SECTION( "invalid mainQuery with random pattern input after tuple" ) {
        std::map<std::string, std::string> inputMap;
        inputMap.insert({"w", "while"});
        QueryParser::setDeclarationsMap(inputMap);
        QueryParser::setMainQuery("Select ?? with w.stmt# = 10");

        REQUIRE_THROWS(QueryParser::getDesiredVar());
        std::map<std::string, std::string> cleanMap;
        QueryParser::setDeclarationsMap(cleanMap);
        QueryParser::setIsBoolean(false);
        QueryParser::setErrorMsg("");
    }

}