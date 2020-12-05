#include <iterator>
#include <sstream>

#include "Optimizer.h"
#include "catch.hpp"

typedef std::unordered_map<std::string, std::vector<QueryParser::Clause>> optimizedResults;

/*
 * Description: Creates a QueryParser::Clause object quickly using the fields described.
 */
QueryParser::Clause createClause(std::string clauseType, std::string clauseVariable, std::string var1, std::string entityType1,
                                 std::string var2, std::string entityType2) {
    QueryParser::InputType input1 = {var1, entityType1};
    QueryParser::InputType input2 = {var2, entityType2};

    return {clauseType, clauseVariable, input1, input2};
}

/*
 * Description: Creates a set of size_t (used by clause index) using a string argument delimited by spaces
 * Example input: "1 2 3 4 5" returns a set {1, 2, 3, 4, 5}
 */
std::unordered_set<size_t> generateClauseIndexSetSet(std::string set) {
    std::stringstream ss(set);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    std::unordered_set<size_t> intSet;
    for (std::string s : splitted) {
        intSet.insert((size_t)std::stoi(s));
    }
    return intSet;
}

/*
 * Description: Creates a set of std::string (used by synonyms) using a string argument delimited by spaces
 * Example input: "s1 s2 s3 v1 v2" returns a set {"s1", "s2", "s3", "v1", "v2}
 */
std::unordered_set<std::string> generateSynSet(std::string stmt) {
    std::stringstream ss(stmt);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    return splitted;
}

/*
 * Description: Checker for DFS.
 * Checks if a Group (result) exists inside a vector of Group (expected)
 * If fails, prints out the failing Group of clauses.
 */
bool DFSChecker(std::vector<Optimizer::Group> &expected, Optimizer::Group &result) {
    for (Optimizer::Group check : expected) {
        if (check.clauses == result.clauses && check.synUsed == result.synUsed) {
            return true;
        }
    }
    std::cout << "\nCouldnt find the group for: {";

    for (std::string s : result.synUsed) {
        std::cout << s << ",";
    }

    std::cout << "}";
    return false;
}

/*
 * Description: Checker for sortClauses.
 * Checks if the order of the clauses (given) matches the order of the clauses (expected)
 * If fails, prints out the order of the given and expected clauses.
 */
bool clauseOrderChecker(std::vector<QueryParser::Clause> &expected, std::vector<QueryParser::Clause> &given) {
    for (auto i = 0; i < expected.size(); i++) {
        if (expected[i] == given[i]) {
        } else {
            std::cout << "\n Expected: ";
            for (QueryParser::Clause &ec : expected) {
                std::cout << ec.clauseVariable << "(" << ec.firstVar.var << "," << ec.secondVar.var << ") ";
            }
            std::cout << "\n Given: ";
            for (QueryParser::Clause &c : given) {
                std::cout << c.clauseVariable << "(" << c.firstVar.var << "," << c.secondVar.var << ") ";
            }
            return false;
        }
    }
    return true;
}

/*
 * Each TEST_CASE tests a certain query
 * Things to test: (In order of usage in optimization)
 *   1. AdjList is initialized properly
 *   2. DFS separates into correct grouping
 *   3. sortClauses returns the intended optimized order
 *
 * Finally, test if getOptimizedQuery returns the correct boolean : nonBooleanClauses
 */
TEST_CASE("Query 01") {
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("a1 a2 a3 s1 s2 s3 v1 v2 v3");
    std::vector<Optimizer::Group> groupedBySynonyms;
    /*
    assign a1, a2, a3; stmt s1, s2, s3; variable v1, v2, v3;
    Select <s1, s2, v2> such that Uses (s3, v1) and Modifies (s3, “x”) and Follows (s1, s2)
    and Parent (s3, s1) and Uses (s2, v1)
    such that Uses (5, “y”) and Follows (3, 4) pattern a1 (v2, _”x+y”_)
    such that Affects (a1, a2) with a2.stmt# = 20
    such that Modifies (a3, v3) pattern a3 (“z”, _)

    booleanClauses:
    0: Uses (5, “y”)
    1: Follows (3, 4)

    nonBooleanClauses:
    0: Uses (s3, v1)
    1: Modifies (s3, “x”)
    2: Follows (s1, s2)
    3: Parent (s3, s1)
    4: Uses (s2, v1)
    5: pattern a1 (v2, _”x+y”_)
    6: Affects (a1, a2)
    7: a2.stmt# = 20
    8: Modifies (a3, v3)
    9: pattern a3 (“z”, _)

    a1: 5, 6
    a2: 6, 7
    a3: 8, 9
    s1: 2, 3
    s2: 2, 4
    s3: 0, 1, 3
    v1: 0, 4
    v2: 5
    v3: 8

    Group 1: s1, s3, v1, s2
    Group 2: a1, a2, v2
    Group 3: a3, v3

    Group 1 clauses: 0, 1, 2, 3, 4
    Group 2 clauses: 5, 6, 7
    Group 3 clauses: 8, 9

    Intended order:
        bool0 -> bool1 ->           bool
        1 -> 3 -> 2 -> 0 / 4 ->     group1
        7 -> 6 -> 5                 group2
        9 -> 8                      group3
    */

    // === Initializing Clauses into nonBooleanClauses and booleanClauses vectors ===
    QueryParser::Clause clause0 = createClause("suchThat", "Uses", "s3", "stmt", "v1", "variable");
    QueryParser::Clause clause1 = createClause("suchThat", "Modifies", "s3", "stmt", "x", "identifier");
    QueryParser::Clause clause2 = createClause("suchThat", "Follows", "s1", "stmt", "s2", "stmt");
    QueryParser::Clause clause3 = createClause("suchThat", "Parent", "s3", "stmt", "s1", "stmt");
    QueryParser::Clause clause4 = createClause("suchThat", "Uses", "s2", "stmt", "v1", "variable");
    QueryParser::Clause clause5 = createClause("assignPattern", "a1", "v2", "variable", " _\"x+y\"_", "expression");
    QueryParser::Clause clause6 = createClause("suchThat", "Affects", "a1", "assign", "a2", "assign");
    QueryParser::Clause clause7 = createClause("with", "equal", "a2.stmt#", "assign", "20", "num");
    QueryParser::Clause clause8 = createClause("suchThat", "Modifies", "a3", "assign", "v3", "variable");
    QueryParser::Clause clause9 = createClause("assignPattern", "a3", "z", "identifier", "_", "wildcard");

    nonBooleanClauses.push_back(clause0);
    nonBooleanClauses.push_back(clause1);
    nonBooleanClauses.push_back(clause2);
    nonBooleanClauses.push_back(clause3);
    nonBooleanClauses.push_back(clause4);
    nonBooleanClauses.push_back(clause5);
    nonBooleanClauses.push_back(clause6);
    nonBooleanClauses.push_back(clause7);
    nonBooleanClauses.push_back(clause8);
    nonBooleanClauses.push_back(clause9);

    QueryParser::Clause boolClause0 = createClause("suchThat", "Uses", "5", "stmtNum", "y", "identifier");
    QueryParser::Clause boolClause1 = createClause("suchThat", "Follows", "3", "stmtNum", "4", "stmtNum");
    booleanClauses.push_back(boolClause0);
    booleanClauses.push_back(boolClause1);
    // ==============================================================================
    // === Test AdjList is initialized properly =====================================
    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    REQUIRE(adjList.find("a1")->second == generateClauseIndexSetSet("5 6"));
    REQUIRE(adjList.find("a2")->second == generateClauseIndexSetSet("6 7"));
    REQUIRE(adjList.find("a3")->second == generateClauseIndexSetSet("8 9"));
    REQUIRE(adjList.find("s1")->second == generateClauseIndexSetSet("2 3"));
    REQUIRE(adjList.find("s2")->second == generateClauseIndexSetSet("2 4"));
    REQUIRE(adjList.find("s3")->second == generateClauseIndexSetSet("0 1 3"));
    REQUIRE(adjList.find("v1")->second == generateClauseIndexSetSet("0 4"));
    REQUIRE(adjList.find("v2")->second == generateClauseIndexSetSet("5"));
    REQUIRE(adjList.find("v3")->second == generateClauseIndexSetSet("8"));
    // ==============================================================================
    // === Test DFS separates into correct grouping =================================

    // Initializing expected groups after DFS ===
    std::unordered_set<size_t> group1Cl{0, 1, 2, 3, 4};
    std::unordered_set<std::string> synUsed1{"s1", "s3", "v1", "s2"};
    Optimizer::Group group1 = {synUsed1, group1Cl};

    std::unordered_set<size_t> group2Cl{5, 6, 7};
    std::unordered_set<std::string> synUsed2{"a1", "a2", "v2"};
    Optimizer::Group group2 = {synUsed2, group2Cl};

    std::unordered_set<size_t> group3Cl{8, 9};
    std::unordered_set<std::string> synUsed3{"a3", "v3"};
    Optimizer::Group group3 = {synUsed3, group3Cl};

    std::vector<Optimizer::Group> expectedGroupsAfterDFS{group1, group2, group3};
    // ==========================================

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    for (Optimizer::Group g : groupedBySynonyms) {
        REQUIRE(DFSChecker(expectedGroupsAfterDFS, g));
    }
    // ==============================================================================
    //// === Test sortClauses returns the intended optimized order ====================

    //// Intended order :
    ////    bool0->bool1 ->             bool
    ////    1 -> 3 -> 2 -> 0 / 4 ->     group1
    ////    7 -> 6 -> 5                 group2
    ////    9 -> 8                      group3
    // std::vector<QueryParser::Clause> intendedOrderNonBoolean = {clause1, clause3, clause2, clause0, clause4,
    //                                                            clause7, clause6, clause5, clause9, clause8};

    // std::vector<QueryParser::Clause> intendedOrder = {boolClause0, boolClause1, clause1, clause3, clause2, clause0,
    //                                                  clause4,     clause7,     clause6, clause5, clause9, clause8};
    //// ===============================================

    // std::vector<QueryParser::Clause> newNonBooleanClauses =
    //    Optimizer::sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    // REQUIRE(newNonBooleanClauses.size() == intendedOrderNonBoolean.size());
    // REQUIRE(clauseOrderChecker(intendedOrderNonBoolean, newNonBooleanClauses));
    //// ==============================================================================
    //// === Test getOptimizedQuery returns the correct boolean : nonBooleanClauses ===

    //// Initializing the map
}

TEST_CASE("Query 02") {
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("p q a v w");
    std::vector<Optimizer::Group> groupedBySynonyms;

    /*
    procedure p, q; assign a; variable v; while w;
    Select <p, v> such that Uses (p, v) and Calls(p, q) and Uses(q, v)
    and Parent(w, a) pattern a(v, _)


    booleanClauses:

    nonBooleanClauses:
    0: Uses(p, v)
    1: Calls(p, q)
    2: Uses(q, v)
    3: Parent(w, a)
    4: pattern a(v, _)

    p: 0, 1
    q: 1, 2
    a: 3, 4
    v: 0, 2, 4
    w: 3

    Group 1: p, q, a, v, w

    Group 1 clauses: 0, 1, 2, 3, 4

    Intended order:
        3 -> 4 -> 0/2 -> 1 -> 0/2
    */

    // === Initializing Clauses into nonBooleanClauses and booleanClauses vectors ===
    QueryParser::Clause clause0 = createClause("suchThat", "Uses", "p", "procedure", "v", "variable");
    QueryParser::Clause clause1 = createClause("suchThat", "Calls", "p", "procedure", "q", "procedure");
    QueryParser::Clause clause2 = createClause("suchThat", "Uses", "q", "procedure", "v", "variable");
    QueryParser::Clause clause3 = createClause("suchThat", "Parent", "w", "while", "a", "assign");
    QueryParser::Clause clause4 = createClause("assignPattern", "a", "v", "variable", "_", "wildcard");

    nonBooleanClauses.push_back(clause0);
    nonBooleanClauses.push_back(clause1);
    nonBooleanClauses.push_back(clause2);
    nonBooleanClauses.push_back(clause3);
    nonBooleanClauses.push_back(clause4);

    // === Test AdjList is initialized properly =====================================
    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    REQUIRE(adjList.find("p")->second == generateClauseIndexSetSet("0 1"));
    REQUIRE(adjList.find("q")->second == generateClauseIndexSetSet("1 2"));
    REQUIRE(adjList.find("a")->second == generateClauseIndexSetSet("3 4"));
    REQUIRE(adjList.find("v")->second == generateClauseIndexSetSet("0 2 4"));
    REQUIRE(adjList.find("w")->second == generateClauseIndexSetSet("3"));
    // ==============================================================================
    // === Test DFS separates into correct grouping =================================

    // Initializing expected groups after DFS ===
    std::unordered_set<size_t> group1Cl{0, 1, 2, 3, 4};
    std::unordered_set<std::string> synUsed1{"p", "q", "a", "v", "w"};
    Optimizer::Group group1 = {synUsed1, group1Cl};

    std::vector<Optimizer::Group> expectedGroupsAfterDFS{group1};
    // ==========================================

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    for (Optimizer::Group g : groupedBySynonyms) {
        REQUIRE(DFSChecker(expectedGroupsAfterDFS, g));
    }
    //// ==============================================================================
    //// === Test sortClauses returns the intended optimized order ====================

    //// Intended order :
    ////    3 -> 4 -> 0 / 2 -> 1 -> 0 / 2
    // std::vector<QueryParser::Clause> intendedOrderNonBoolean = {clause3, clause4, clause0, clause1, clause2};
    //// ===============================================

    // std::vector<QueryParser::Clause> newNonBooleanClauses =
    //    Optimizer::sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    // REQUIRE(newNonBooleanClauses.size() == intendedOrderNonBoolean.size());
    // REQUIRE(clauseOrderChecker(intendedOrderNonBoolean, newNonBooleanClauses));
}

TEST_CASE("Query 03") {
    /*
    stmt s1, s2;
    Select s1 such that Next(s2, _)
    */
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("s1");
    std::vector<Optimizer::Group> groupedBySynonyms;

    QueryParser::Clause clause0 = createClause("suchThat", "Next", "s2", "stmt", "_", "wildcard");
    nonBooleanClauses.push_back(clause0);

    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    /*std::vector<QueryParser::Clause> newNonBooleanClauses =
        Optimizer::sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    REQUIRE(newNonBooleanClauses.size() == 0);
    REQUIRE(booleanClauses.size() == 1);*/
}

TEST_CASE("Query 04") {
    /*
    Select <p, ifs> such that Next*(1, ifs) with "procedure" = p.procName
    */
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("p ifs");
    std::vector<Optimizer::Group> groupedBySynonyms;

    QueryParser::Clause clause0 = createClause("suchThat", "Next*", "1", "literal", "ifs", "if");
    QueryParser::Clause clause1 = createClause("with", "equal", "\"procedure\"", "literal", "p.procName", "procedure");
    QueryParser::Clause clause1alt = createClause("with", "equal", "p.procName", "procedure", "\"procedure\"", "literal");
    nonBooleanClauses.push_back(clause0);
    nonBooleanClauses.push_back(clause1);

    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    /*std::vector<QueryParser::Clause> newNonBooleanClauses =
        Optimizer::sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    REQUIRE(newNonBooleanClauses.size() == 2);
    REQUIRE(booleanClauses.size() == 0);*/
}

TEST_CASE("Query 05") {
    /*
    stmt s, s1, s2, s3; assign a, a1, a2, a3;
    Select s such that
    Affects*(s, a) and Affects*(a, s1) and Affects*(s1, a1) and
    Affects*(a1, s2) and Affects*(s2, a2) and Affects*(a2, s3) and Affects*(s3, a3)
    */

    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("s s1 s2 s3 a a1 a2 a3");
    std::vector<Optimizer::Group> groupedBySynonyms;

    QueryParser::Clause clause0 = createClause("suchThat", "Affects*", "s", "stmt", "a", "assign");
    QueryParser::Clause clause1 = createClause("suchThat", "Affects*", "a", "assign", "s1", "stmt");
    QueryParser::Clause clause2 = createClause("suchThat", "Affects*", "s1", "stmt", "a1", "assign");
    QueryParser::Clause clause3 = createClause("suchThat", "Affects*", "a1", "assign", "s2", "stmt");
    QueryParser::Clause clause4 = createClause("suchThat", "Affects*", "s2", "stmt", "a2", "assign");
    QueryParser::Clause clause5 = createClause("suchThat", "Affects*", "a2", "assign", "s3", "stmt");
    QueryParser::Clause clause6 = createClause("suchThat", "Affects*", "s3", "stmt", "a3", "assign");

    nonBooleanClauses.push_back(clause0);
    nonBooleanClauses.push_back(clause1);
    nonBooleanClauses.push_back(clause2);
    nonBooleanClauses.push_back(clause3);
    nonBooleanClauses.push_back(clause4);
    nonBooleanClauses.push_back(clause5);
    nonBooleanClauses.push_back(clause6);

    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    /*std::vector<QueryParser::Clause> newNonBooleanClauses =
        Optimizer::sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    REQUIRE(newNonBooleanClauses.size() == 7);
    REQUIRE(booleanClauses.size() == 0);

    std::vector<QueryParser::Clause> intendedOrderNonBoolean = { clause0, clause1, clause2, clause3, clause4, clause5, clause6 };
    REQUIRE(clauseOrderChecker(intendedOrderNonBoolean, newNonBooleanClauses));*/
}

TEST_CASE("Query 06") {
    /*
    stmt s; assign a, a1; if ifs;
    Select s such that Uses(s, "b") and Affects(s,a) and Parent(ifs, a1) and Affects*(s,a1)
    with a1.stmt# = 31
    */
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("s a a1 ifs");
    std::vector<Optimizer::Group> groupedBySynonyms;

    QueryParser::Clause clause0 = createClause("suchThat", "Uses", "s", "stmt", "\"a\"", "identifier");
    QueryParser::Clause clause1 = createClause("suchThat", "Affects", "s", "stmt", "a", "assign");
    QueryParser::Clause clause2 = createClause("suchThat", "Parent", "ifs", "if", "a1", "assign");
    QueryParser::Clause clause3 = createClause("suchThat", "Affects*", "s", "stmt", "a1", "assign");
    QueryParser::Clause clause4 = createClause("with", "equal", "a1.stmt#", "assign", "31", "num");

    nonBooleanClauses.push_back(clause0);
    nonBooleanClauses.push_back(clause1);
    nonBooleanClauses.push_back(clause2);
    nonBooleanClauses.push_back(clause3);
    nonBooleanClauses.push_back(clause4);

    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);

    // std::vector<QueryParser::Clause> newNonBooleanClauses =
    //     Optimizer::sortClauses(groupedBySynonyms, usedSynonyms, booleanClauses, nonBooleanClauses);

    // REQUIRE(newNonBooleanClauses.size() == 5);
    // REQUIRE(booleanClauses.size() == 0);

    // std::vector<QueryParser::Clause> intendedOrderNonBoolean = { clause0, clause1, clause2, clause3, clause4};
    //// REQUIRE(clauseOrderChecker(intendedOrderNonBoolean, newNonBooleanClauses));
}

TEST_CASE("Query 07") {
    /*
    stmt s, s1, s2, s3; assign a, a1, a2, a3; while w;
    Select BOOLEAN such that Follows(a1, 44) pattern a1("a", "b") with a1.stmt# = 33
    */
    std::vector<QueryParser::Clause> booleanClauses;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::unordered_map<std::string, std::unordered_set<size_t>> adjList;
    std::unordered_set<std::string> usedSynonyms = generateSynSet("BOOLEAN");
    std::vector<Optimizer::Group> groupedBySynonyms;

    QueryParser::Clause clause0 = createClause("suchThat", "Follows", "a1", "assign", "44", "num");
    QueryParser::Clause clause1 = createClause("assignPattern", "a1", "\"a\"", "identifier", "\"b\"", "identifier");
    QueryParser::Clause clause2 = createClause("with", "equal", "a1.stmt#", "assign", "33", "num");

    nonBooleanClauses.push_back(clause0);
    nonBooleanClauses.push_back(clause1);
    nonBooleanClauses.push_back(clause2);

    Optimizer::initializeAdjList(adjList, nonBooleanClauses);

    Optimizer::DFS(adjList, groupedBySynonyms, nonBooleanClauses);
}