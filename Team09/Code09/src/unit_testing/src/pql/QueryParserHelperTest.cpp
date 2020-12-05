#include "QueryParserHelper.h"
#include "catch.hpp"

#include <unordered_set>

TEST_CASE("Basic Query Check: basicCheck Test") {
    bool result1 = QueryParserHelper::basicCheck("while w; Select w such that Parent(w, 7)");
    bool result2 = QueryParserHelper::basicCheck("assign a1, a2; variable v; Select v pattern a1(v, _) such that Uses(a2, v)");
    bool result3 = QueryParserHelper::basicCheck("         assign a; Select a such that Parent*(4, a)");
    bool result4 = QueryParserHelper::basicCheck("stmt s;            Select s such that Modifies(s, \"i\")");
    bool result5 = QueryParserHelper::basicCheck("while w; select w such that Parent(w, 7)");
    bool result6 = QueryParserHelper::basicCheck("blah");
    bool result7 = QueryParserHelper::basicCheck("while w; Select <w> such that Parent(w, 7)");
    bool result8 = QueryParserHelper::basicCheck("while w; variable v; Select <w, v.varName>");
    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == true);
    REQUIRE(result4 == true);
    REQUIRE(result5 == false);
    REQUIRE(result6 == false);
    REQUIRE(result7 == true);
    REQUIRE(result8 == true);
}

TEST_CASE("Split query by semicolon: splitQuery Test") {

    SECTION( "valid query" ) {
        std::vector<std::string> actual = QueryParserHelper::splitQuery("while w; Select w such that Parent(w, 7)");

        std::vector<std::string> expected;
        expected.emplace_back("while w");
        expected.emplace_back("Select w such that Parent(w, 7)");

        REQUIRE(actual == expected);
    }
    SECTION( "valid query with multiple declarations" ) {
        std::vector<std::string> actual = QueryParserHelper::splitQuery("while w     ; variable v; Select w such that Modifies(w, v)");

        std::vector<std::string> expected;
        expected.emplace_back("while w");
        expected.emplace_back("variable v");
        expected.emplace_back("Select w such that Modifies(w, v)");

        REQUIRE(actual == expected);
    }

    SECTION( "valid query with extra tabs, spaces" ) {
        std::vector<std::string> actual = QueryParserHelper::splitQuery("        stmt s;             Select s such that Modifies(s, \"i\")      ");

        std::vector<std::string> expected;
        expected.emplace_back("stmt s");
        expected.emplace_back("Select s such that Modifies(s, \"i\")");

        REQUIRE(actual == expected);
    }

}