#include "PKB.h"
#include "catch.hpp"
#include "QueryEvaluator.h"
#include "QueryParser.h"
#include "Table.h"
#include "TestUtility.h"

/*
*       procedure main {
*  1        count = 0;
*  2        cenX = 0;
*  3        cenY = 0;
*  4        while ((x != 0) && (y != 0)) {
*  5            count = count + 1;
*  6            if (cenX == cenY) then {
*  7                print count;
*               } else {
*  8                a = 1;
*  9                count = a;
* 10                while (cenX < cenY) {
* 11                    read cenX;
* 12                    read cenY;
* 13                    cenX = cenX + 1;
*                   }
* 14                count = count + 1;
*               }
*           }
* 15        if (count == 0) then {
* 16            flag = 1;
*           } else {
* 17            cenX = cenX / count;
* 18            cenY = cenY / count;
*           }
* 19        normSq = cenX * cenX + cenY * cenY;
* 20        print normSq;
*       }
* 
*       procedure printResults {
* 21        print flag;
* 22        print count;
*       }
*/

QueryEvaluator qe;
QueryParser::InputType se;
QueryParser::InputType st1;
QueryParser::InputType st2;
QueryParser::Clause suchThatClause;
QueryParser::Clause patternClause;
QueryParser::InputType pc1;
QueryParser::InputType pc2;
std::list<std::string> listRes;
std::unordered_set<std::string> ans;
std::unordered_set<std::string> setRes;

std::unordered_set<std::string> ALL_ASSIGNMENT_STMTS = {
    "1","2","3","5","8","9","13","14","16","17","18","19"
};
std::unordered_set<std::string> ALL_CONSTANTS = {"0", "1"};
std::unordered_set<std::string> ALL_STMTS = {
    "1","2","3","4","5","6","7","8","9",
    "10","11","12","13","14","15","16","17","18","19","20","21","22"
};

std::unordered_set<std::string> ALL_VARS = {"a", "count", "cenX", "cenY", "x", "y", "flag", "normSq"};
std::unordered_set<std::string> EMPTY_SET;

void setQueries(std::string selectedType, std::string selectedVar,
    std::string relationType, std::string v1Type, std::string v1, std::string v2Type, std::string v2,
    std::string patternSynonym, std::string p1Type, std::string p1, std::string p2Type, std::string p2
) {
    se.entityType = selectedType;
    se.var = selectedVar;

    suchThatClause.clauseVariable = relationType;
    suchThatClause.clauseType = "suchThat";
    st1.entityType = v1Type;
    st1.var = v1;
    st2.entityType = v2Type;
    st2.var = v2;
    suchThatClause.firstVar = st1;
    suchThatClause.secondVar = st2;

    patternClause.clauseVariable = patternSynonym;
    patternClause.clauseType = "assignPattern";
    pc1.entityType = p1Type;
    pc1.var = p1;
    pc2.entityType = p2Type;
    pc2.var = p2;
    patternClause.firstVar = pc1;
    patternClause.secondVar = pc2;
}

std::list<std::string> evaluateMultipleQueriesHelper(
    std::vector<std::vector<std::string>> selectedEntitiesStr,
    std::vector<std::vector<std::string>> suchThatClausesStr,
    std::vector<std::vector<std::string>> patternClausesStr,
    std::vector<std::vector<std::string>> withClausesStr
) {
    std::vector<QueryParser::Clause> withTypeClauses;
    std::vector<QueryParser::InputType> selectedEntites;
    std::vector<QueryParser::Clause> suchThatClauses;
    std::vector<QueryParser::Clause> patternClauses;

    for (std::vector<std::string> selectedEntityStr : selectedEntitiesStr) {
        QueryParser::InputType it;
        it.entityType = selectedEntityStr[0];
        it.var = selectedEntityStr[1];
        selectedEntites.push_back(it);
    }

    // {"Follows", STMT_NUM, "2", STMT_NUM, "3"}
    for (std::vector<std::string> suchThatStr : suchThatClausesStr) {
        QueryParser::Clause c;
        QueryParser::InputType lhs;
        QueryParser::InputType rhs;
        c.clauseType = "suchThat";
        c.clauseVariable = suchThatStr[0]; // e.g. Follows
        lhs.entityType = suchThatStr[1]; // e.g. stmtNum
        lhs.var = suchThatStr[2]; // e.g. "2"
        rhs.entityType = suchThatStr[3];
        rhs.var = suchThatStr[4];
        c.firstVar = lhs;
        c.secondVar = rhs;
        suchThatClauses.push_back(c);
    }

    // {"assignPattern", "a", VAR, "v", WILDCARD, _}
    for (std::vector<std::string> patternStr : patternClausesStr) {
        QueryParser::Clause c;
        QueryParser::InputType lhs;
        QueryParser::InputType rhs;
        c.clauseType = patternStr[0]; // e.g. assign/if/while pattern
        c.clauseVariable = patternStr[1]; // e.g. patternSynonm 
        lhs.entityType = patternStr[2]; 
        lhs.var = patternStr[3]; // e.g. "2"
        rhs.entityType = patternStr[4];
        rhs.var = patternStr[5];
        c.firstVar = lhs;
        c.secondVar = rhs;
        patternClauses.push_back(c);
    }

    // {"s1.stmt#", STMT, "n", "prog_line"}
    for (std::vector<std::string> withStr : withClausesStr) {
        QueryParser::Clause c;
        QueryParser::InputType lhs;
        QueryParser::InputType rhs;
        c.clauseType = "with";
        c.clauseVariable = "with";
        lhs.var = withStr[0]; 
        lhs.entityType = withStr[1];
        rhs.var = withStr[2];
        rhs.entityType = withStr[3];
        c.firstVar = lhs;
        c.secondVar = rhs;
        withTypeClauses.push_back(c);
    }

    return QueryEvaluator::evaluateQuery(
        selectedEntites,
        suchThatClauses,
        patternClauses,
        withTypeClauses
    );
}

TEST_CASE("QE/PKB API") {
    PKB& pkb = PKB::Instance();
    // Populate Follows/Follows* relationships
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
    CHECK_NOTHROW(pkb.addFollowsRelation(21, 22));

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
    CHECK_NOTHROW(pkb.addFollowsStarRelation(21, 22));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 3));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 4));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 15));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 19));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(1, 20));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 4));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 15));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 19));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(2, 20));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 15));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 19));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(3, 20));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(4, 19));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(4, 20));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(15, 20));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(8, 10));
    CHECK_NOTHROW(pkb.addFollowsStarRelation(8, 14));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(9, 14));

    CHECK_NOTHROW(pkb.addFollowsStarRelation(11, 13));

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

    // Populate Uses relationships
    CHECK_NOTHROW(pkb.addStmtUsesRelation(4, WHILE_STMT, std::unordered_set<std::string> {"x", "y", "cenX", "cenY", "count", "a"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(5, ASSIGN, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(6, IF_STMT, std::unordered_set<std::string> {"cenX", "cenY", "count", "a"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(7, PRINT, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(9, ASSIGN, std::unordered_set<std::string> {"a"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(10, WHILE_STMT, std::unordered_set<std::string> {"cenX", "cenY"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(13, ASSIGN, std::unordered_set<std::string> {"cenX"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(14, ASSIGN, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(15, IF_STMT, std::unordered_set<std::string> {"cenX", "cenY", "count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(6, IF_STMT, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(17, ASSIGN, std::unordered_set<std::string> {"cenX", "count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(18, ASSIGN, std::unordered_set<std::string> {"cenY", "count"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(19, ASSIGN, std::unordered_set<std::string> {"cenX", "cenY"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(20, PRINT, std::unordered_set<std::string> {"normSq"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(21, PRINT, std::unordered_set<std::string> {"flag"}));
    CHECK_NOTHROW(pkb.addStmtUsesRelation(22, PRINT, std::unordered_set<std::string> {"count"}));

    CHECK_NOTHROW(pkb.addProcUsesRelation("main", std::unordered_set<std::string> {"x", "y", "cenX", "cenY", "count", "a", "normSq"}));
    CHECK_NOTHROW(pkb.addProcUsesRelation("printResults", std::unordered_set<std::string> {"flag", "count"}));

    // Populate Modifies relationships
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(1, ASSIGN, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(2, ASSIGN, std::unordered_set<std::string> {"cenX"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(3, ASSIGN, std::unordered_set<std::string> {"cenY"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(4, WHILE_STMT, std::unordered_set<std::string> {"count", "a", "cenX", "cenY"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(5, ASSIGN, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(6, IF_STMT, std::unordered_set<std::string> {"count", "a", "cenX"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(8, ASSIGN, std::unordered_set<std::string> {"a"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(9, ASSIGN, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(10, WHILE_STMT, std::unordered_set<std::string> {"cenX", "cenY"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(11, READ, std::unordered_set<std::string> {"cenX"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(12, READ, std::unordered_set<std::string> {"cenY"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(13, ASSIGN, std::unordered_set<std::string> {"cenX"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(14, ASSIGN, std::unordered_set<std::string> {"count"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(15, IF_STMT, std::unordered_set<std::string> {"flag", "cenX", "cenY"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(16, ASSIGN, std::unordered_set<std::string> {"flag"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(17, ASSIGN, std::unordered_set<std::string> {"cenX"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(18, ASSIGN, std::unordered_set<std::string> {"cenY"}));
    CHECK_NOTHROW(pkb.addStmtModifiesRelation(19, ASSIGN, std::unordered_set<std::string> {"normSq"}));

    CHECK_NOTHROW(pkb.addProcModifiesRelation("main", std::unordered_set<std::string> {"normSq", "flag", "cenX", "cenY", "count", "a"}));

    // Populate General Storage with Statements
    CHECK_NOTHROW(pkb.storeStatement(1, ASSIGN, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {0}));
    CHECK_NOTHROW(pkb.storeStatement(2, ASSIGN, std::unordered_set<std::string> {"cenX"}, std::unordered_set<int> {0}));
    CHECK_NOTHROW(pkb.storeStatement(3, ASSIGN, std::unordered_set<std::string> {"cenY"}, std::unordered_set<int> {0}));
    CHECK_NOTHROW(pkb.storeStatement(4, WHILE_STMT, std::unordered_set<std::string> {"x", "y"}, std::unordered_set<int> {0}));
    CHECK_NOTHROW(pkb.storeStatement(5, ASSIGN, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {1}));
    CHECK_NOTHROW(pkb.storeStatement(6, IF_STMT, std::unordered_set<std::string> {"cenX", "cenY"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(7, PRINT, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(8, ASSIGN, std::unordered_set<std::string> {"a"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(9, ASSIGN, std::unordered_set<std::string> {"count", "a"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(10, WHILE_STMT, std::unordered_set<std::string> {"cenX", "cenY"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(11, READ, std::unordered_set<std::string> {"cenX"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(12, READ, std::unordered_set<std::string> {"cenY"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(13, ASSIGN, std::unordered_set<std::string> {"cenX"}, std::unordered_set<int> {1}));
    CHECK_NOTHROW(pkb.storeStatement(14, ASSIGN, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {1}));
    CHECK_NOTHROW(pkb.storeStatement(15, IF_STMT, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {0}));
    CHECK_NOTHROW(pkb.storeStatement(16, ASSIGN, std::unordered_set<std::string> {"flag"}, std::unordered_set<int> {1}));
    CHECK_NOTHROW(pkb.storeStatement(17, ASSIGN, std::unordered_set<std::string> {"cenX", "count"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(18, ASSIGN, std::unordered_set<std::string> {"cenY", "count"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(19, ASSIGN, std::unordered_set<std::string> {"normSq", "cenY", "cenX"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(20, PRINT, std::unordered_set<std::string> {"normSq"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(21, PRINT, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {}));
    CHECK_NOTHROW(pkb.storeStatement(22, PRINT, std::unordered_set<std::string> {"count"}, std::unordered_set<int> {}));

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

    CHECK_NOTHROW(pkb.addProcInfo("main", 1, 20));
    CHECK_NOTHROW(pkb.addProcInfo("printResults", 21, 22));


    std::string STMT_NUM = "stmtNum";
    std::string STMT = "stmt";
    std::string ASSIGN = "assign";
    std::string VAR = "variable";
    std::string PROC = "procedure";
    std::string IDENT = "identifier";
    std::string WILDCARD = "wildcard";
    std::string EXPRESSION = "expression";
    std::string CONSTANT = "constant";

    // ============================ NO OVERLAPS ============================
    // select a such that Follows(2, 3)
    setQueries(
        ASSIGN, "a",
        "Follows", STMT_NUM, "2", STMT_NUM, "3",
        "a", VAR, "v", WILDCARD, "_" // don't need this 
    );
    ans = ALL_ASSIGNMENT_STMTS;
    listRes = qe.evaluateQuery(
        {se}, 
        {suchThatClause},
        {}, {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true); 

    // select a such that Follows(_, 3)
    setQueries(
        ASSIGN, "a",
        "Follows", WILDCARD, "_", STMT_NUM, "3",
        "a", VAR, "v", WILDCARD, "_" // don't need this 
    );
    ans = ALL_ASSIGNMENT_STMTS;
        listRes = qe.evaluateQuery(
        {se}, 
        {suchThatClause},
        {}, {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);
    
    // select a such that Follows(3, _)
    setQueries(
        ASSIGN, "a",
        "Modifies", STMT_NUM, "3", WILDCARD, "_",
        "a", STMT_NUM, "3", WILDCARD, "_" // don't need this 
    );
    ans = ALL_ASSIGNMENT_STMTS;
    listRes = qe.evaluateQuery(
        {se}, 
        {suchThatClause},
        {}, {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true); 
    
    // select a such that Follows(_, _)
    setQueries(
        ASSIGN, "a",
        "Follows", WILDCARD, "_", WILDCARD, "_",
        "a", STMT_NUM, "3", WILDCARD, "_" // don't need this 
    );
    ans = ALL_ASSIGNMENT_STMTS;
    listRes = qe.evaluateQuery(
        {se}, 
        {suchThatClause},
        {}, {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true); 

    // Select a such that Follows(2,3) pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Follows", STMT_NUM, "2", STMT_NUM, "3",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = ALL_ASSIGNMENT_STMTS;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(ans, setRes) == true);

    // // Select a such that Follows(3,2) pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Follows", STMT_NUM, "3", STMT_NUM, "2",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = EMPTY_SET;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // // Select a1 such that uses(a1, v1) pattern a(v, _)
    setQueries(
        ASSIGN, "a1",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = {"5","9", "13","14","17", "18","19"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select a such that uses(a1, v1) pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = ALL_ASSIGNMENT_STMTS;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);


    // =================== TEST PATTERN ==================
    // Select a such that pattern a(v, _"cenY * cenY"_)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", EXPRESSION, "_    cenY      * cenY_"
    );
    ans = {"19"};
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select v such that pattern a(v, _"cenY * cenY"_)
    setQueries(
        VAR, "v",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", EXPRESSION, "_    cenY      * cenY_"
    );
    ans = {"normSq"};
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select a such that pattern a(v, _"cenX + cenY"_)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", EXPRESSION, "_cenX + cenY_"
    );
    ans = EMPTY_SET;
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select a such that pattern a(_, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = ALL_ASSIGNMENT_STMTS;
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select a such that pattern a("normSq", _"cenY*cenY")
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", IDENT, "normSq", EXPRESSION, "_cenY      * cenY_"
    );
    ans = {"19"};
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select c such that pattern a(_, _"cenY*cenY")
    setQueries(
        "constant", "c",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", WILDCARD, "_", EXPRESSION, "_cenY      * cenY_"
    );
    ans = ALL_CONSTANTS;
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);
    
    // Select a such that uses(a1, v1) pattern a(v, _)

    // ============================ TWO OVERLAPS ============================
    
    // Select a such that Uses(a, v) pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", VAR, "v",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = {"5", "13", "14", "17", "18"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select v such that Uses(a, v) pattern a(v, _)
    setQueries(
        VAR, "v",
        "Uses", ASSIGN, "a", VAR, "v",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = {"count", "cenX", "cenY"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select s such that Uses(a, v) pattern a(v, _)
    setQueries(
        STMT, "s",
        "Uses", ASSIGN, "a1", VAR, "v1",
        "a", VAR, "v", WILDCARD, "_"
    );
    ans = ALL_STMTS;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true); 
    
    
    // ============================ ONE OVERLAP (ASSIGN) ============================

    // select a such that Uses(a, v)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" // don't need this 
    );
    ans = {"5", "9", "13", "14", "17", "18", "19"};
    listRes = qe.evaluateQuery(
        {se}, 
        {suchThatClause},
        {}, {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such pattern a(count, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", VAR, "v", // don't need this
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = {"1", "5", "9", "14"};
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such pattern a(count, _)
    setQueries(
        VAR, "v",
        "Uses", ASSIGN, "a", VAR, "v", // don't need this
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = ALL_VARS;
    listRes = qe.evaluateQuery(
            {se}, 
            {},
            {patternClause},
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Uses(a, v) pattern a(count, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", VAR, "v",
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = {"5", "9", "14"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Uses(a, "doesNotExist") pattern a(count, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", IDENT, "doesNotExist",
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = EMPTY_SET;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Uses(a, "count") pattern a(count, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = {"5", "14"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Uses(a, "count") pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"5", "14", "17", "18"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Uses(a, "count") pattern a(v, _)
    setQueries(
        VAR, "v",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"count", "cenX", "cenY"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);


    // select a such that Uses(a, "count") pattern a("count", _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = {"5", "14"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Uses(a, "count") pattern a(_, _)
    setQueries(
        ASSIGN, "a",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", WILDCARD, "_", WILDCARD, "_" 
    );
    ans = {"5", "14", "17", "18"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Uses(a, "count") pattern a("count", _)
    setQueries(
        VAR, "v",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", IDENT, "count", WILDCARD, "_" 
    );
    ans = ALL_VARS;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Uses(a, "count") pattern a(_, _)
    setQueries(
        VAR, "v",
        "Uses", ASSIGN, "a", IDENT, "count",
        "a", WILDCARD, "_", WILDCARD, "_" 
    );
    ans = ALL_VARS;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);
    
    // ============================ ONE OVERLAP (PATTERN) ============================
    // select p such that Modifies(p, v) pattern a(v, _)
    setQueries(
        PROC, "p",
        "Modifies", PROC, "p", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"main"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select s such that Modifies(s, v) pattern a(v, _)
    setQueries(
        STMT, "s",
        "Modifies", STMT, "s", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"1", "2", "3", "4", "5", "6", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select s such that Modifies(p, v) pattern a(v, _)
    setQueries(
        STMT, "s",
        "Modifies", PROC, "p", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = ALL_STMTS;
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Modifies(s, v) pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Modifies", STMT, "s", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"1", "2", "3", "5", "8", "9", "13", "14", "16", "17", "18", "19"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Modifies(p, v) pattern a(v, _)
    setQueries(
        ASSIGN, "a",
        "Modifies", PROC, "p", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"1", "2", "3", "5", "8", "9", "13", "14", "16", "17", "18", "19"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Modifies(p, v) pattern a(v, _)
    setQueries(
        VAR, "v",
        "Modifies", PROC, "p", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"count", "cenX", "cenY", "a", "flag", "normSq"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Modifies(s, v) pattern a(v, _)
    setQueries(
        VAR, "v",
        "Modifies", STMT, "s", VAR, "v",
        "a", VAR, "v", WILDCARD, "_" 
    );
    ans = {"count", "cenX", "cenY", "a", "flag", "normSq"};
    listRes = qe.evaluateQuery(
            {se}, 
            {suchThatClause},
            {patternClause}, 
            {}
        );
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // ====================================================================================
    // 
    // 
    //                                     TEST WITH CLAUSES 
    // 
    // 
    // 
    // ====================================================================================

    QueryParser::Clause withClause;
    QueryParser::InputType v1;
    QueryParser::InputType v2;
    Table resTable;

    // with s1.stmt# = n
    v1.var = "s1.stmt#";
    v1.entityType = STMT;
    v2.var = "n";
    v2.entityType = "prog_line";
    withClause.firstVar = v1;
    withClause.secondVar = v2;

    resTable = qe.getTableFromWithClause(withClause);
    // resTable.print();
    
    // with s1.stmt# = 10
    v1.var = "s1.stmt#";
    v1.entityType = STMT;
    v2.var = "10";
    v2.entityType = "num";
    withClause.firstVar = v1;
    withClause.secondVar = v2;

    resTable = qe.getTableFromWithClause(withClause);
    // resTable.print();

    // with p.procName = v.varName 
    v1.var = "p.procName";
    v1.entityType = PROC;
    v2.var = "v.varName";
    v2.entityType = VAR;
    withClause.firstVar = v1;
    withClause.secondVar = v2;

    resTable = qe.getTableFromWithClause(withClause);
    // resTable.print();
    
    // with s.stmt# = c.value
    v1.var = "s.stmt#";
    v1.entityType = STMT;
    v2.var = "c.value";
    v2.entityType = CONSTANT;
    withClause.firstVar = v1;
    withClause.secondVar = v2;

    resTable = qe.getTableFromWithClause(withClause);
    // resTable.print();
    
    // ====================================================================================
    // 
    // 
    //                                     TEST MULTIPLE CLAUSES 
    // 
    // 
    // 
    // ====================================================================================
    
    // Test helper method works
    // select v such that Modifies(s, v) pattern a(v, _)
    listRes = evaluateMultipleQueriesHelper(
        {{VAR, "v"}},
        {{"Modifies", STMT, "s", VAR, "v"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {}
    );
    ans = {"count", "cenX", "cenY", "a", "flag", "normSq"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Modifies(s, v) Follows(3,2) pattern a(v, _)
    listRes = evaluateMultipleQueriesHelper(
        {{VAR, "v"}},
        {{"Modifies", STMT, "s", VAR, "v"}, {"Follows", STMT_NUM, "3", STMT_NUM, "2"},},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {}
    );
    ans = {};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select v such that Modifies(s, v) Follows(2,3) pattern a(v, _) with s.stmt# = c.value
    listRes = evaluateMultipleQueriesHelper(
        {{VAR, "v"}},
        {{"Modifies", STMT, "s", VAR, "v"}, {"Follows", STMT_NUM, "2", STMT_NUM, "3"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {{"s.stmt#", "stmt", "c.value", "constant"}}
    );
    ans = {"count"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select boolean such that Modifies(s, v) Follows(2,3) pattern a(v, _) with s.stmt# = c.value
    listRes = evaluateMultipleQueriesHelper(
        {{"boolean", "doesnotMatter"}},
        {{"Modifies", STMT, "s", VAR, "v"}, {"Follows", STMT_NUM, "2", STMT_NUM, "3"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {{"s.stmt#", "stmt", "c.value", "constant"}}
    );
    ans = {"TRUE"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Modifies(p, v) Follows(2,3) pattern a(v, _) with a.stmt# = 19
    listRes = evaluateMultipleQueriesHelper(
        {{ASSIGN, "a"}},
        {{"Modifies", PROC, "p", VAR, "v"}, {"Follows", STMT_NUM, "2", STMT_NUM, "3"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {{"a.stmt#", STMT, "19", "num"}}
    );
    ans = {"19"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Modifies(p, v) Follows(2,3) pattern a(v, _) with s.stmt# = 19
    listRes = evaluateMultipleQueriesHelper(
        {{ASSIGN, "a"}},
        {{"Modifies", PROC, "p", VAR, "v"}, {"Follows", STMT_NUM, "2", STMT_NUM, "3"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {{"s.stmt#", STMT, "19", "num"}}
    );
    ans = {"1", "2", "3", "5", "8", "9", "13", "14", "16", "17", "18", "19"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Modifies(p, v) Follows(2,3) pattern a(v, _) with s.stmt# = 19 with a.stmt# = 19
    listRes = evaluateMultipleQueriesHelper(
        {{ASSIGN, "a"}},
        {{"Modifies", PROC, "p", VAR, "v"}, {"Follows", STMT_NUM, "2", STMT_NUM, "3"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {{"s.stmt#", STMT, "19", "num"}, {"a.stmt#", STMT, "19", "num"}}
    );
    ans = {"19"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select a such that Modifies(p, v) Follows(2,3) pattern a(v, _) with s.stmt# = 19 with a.stmt# = 1999
    listRes = evaluateMultipleQueriesHelper(
        {{ASSIGN, "a"}},
        {{"Modifies", PROC, "p", VAR, "v"}, {"Follows", STMT_NUM, "2", STMT_NUM, "3"}},
        {{"assignPattern", "a", VAR, "v", WILDCARD, "_"}},
        {{"s.stmt#", STMT, "19", "num"}, {"a.stmt#", STMT, "1999", "num"}}
    );
    ans = {};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select <v, p>
    listRes = evaluateMultipleQueriesHelper(
        {{VAR, "v"}, {PROC, "p"}},
        {},
        {},
        {}
    );
    ans = {
        "a main", "a printResults",
        "count main", "count printResults",
        "cenX main", "cenX printResults",
        "cenY main", "cenY printResults",
        "x main", "x printResults",
        "y main", "y printResults",
        "flag main", "flag printResults",
        "normSq main", "normSq printResults",
    };
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // select <v, p, boolean>
    listRes = evaluateMultipleQueriesHelper(
        {{VAR, "v"}, {PROC, "p"}, {"boolean", "b"}},
        {},
        {},
        {}
    );
    ans = {
        "a main TRUE", "a printResults TRUE",
        "count main TRUE", "count printResults TRUE",
        "cenX main TRUE", "cenX printResults TRUE",
        "cenY main TRUE", "cenY printResults TRUE",
        "x main TRUE", "x printResults TRUE",
        "y main TRUE", "y printResults TRUE",
        "flag main TRUE", "flag printResults TRUE",
        "normSq main TRUE", "normSq printResults TRUE",
    };
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    std::unordered_set<std::string> ALL_ASSIGNMENT_STMTS = {
        "1","2","3","5","8","9","13","14","16","17","18","19"
    };
    // select <v, a> SUCH THAT Follow*(17, a)
    listRes = evaluateMultipleQueriesHelper(
        {{ASSIGN, "a"}, {VAR, "v"}},
        {{"Follows*", STMT_NUM, "15", ASSIGN, "a"}},
        {},
        {}
    );
    ans = {
        "19 a",
        "19 count",
        "19 cenX",
        "19 cenY",
        "19 x",
        "19 y",
        "19 flag",
        "19 normSq"
    };
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // ====================================================================================
    // 
    // 
    //                                     TEST ATTRIBUTES CLAUSES 
    // 
    // 
    // 
    // ====================================================================================
    
    // call procName, stmt#
    // read varName, stmt#
    // print varName, stmt#


    // TODO: Select pr.varName


    // Select pr.varName such that Uses(pr, v)
    listRes = evaluateMultipleQueriesHelper(
        {{PRINT, "pr.varName"}},
        {{"Uses", PRINT, "pr", VAR, "v"}},
        {},
        {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    ans = {"count", "flag", "normSq"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);
    

    // Select pr.varName such that Uses(pr, "count")
    listRes = evaluateMultipleQueriesHelper(
        {{PRINT, "pr.varName"}},
        {{"Uses", PRINT, "pr", IDENT, "count"}},
        {},
        {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    ans = {"count"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);

    // Select <pr.varName, v> such that Uses(pr, "count")
    listRes = evaluateMultipleQueriesHelper(
        {{PRINT, "pr.varName"}, {VAR, "v"}},
        {{"Uses", PRINT, "pr", IDENT, "count"}},
        {},
        {}
    );
    setRes = TestUtility::convertListToSet(listRes);
    ans = {"count a", "count count", "count cenX", "count cenY", "count x", "count y", "count flag", "count normSq"};
    setRes = TestUtility::convertListToSet(listRes);
    REQUIRE(TestUtility::checkSet(setRes, ans) == true);
    

    // read r;
    listRes = evaluateMultipleQueriesHelper(
        {{READ, "rd"}},
        {},
        {},
        {}
    );
    setRes = TestUtility::convertListToSet(listRes);

    listRes = evaluateMultipleQueriesHelper(
        {{CALL, "call"}},
        {},
        {},
        {}
    );
    setRes = TestUtility::convertListToSet(listRes);

    pkb.clear();
}

