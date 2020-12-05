#include "SimplifiedAST.h"
#include "catch.hpp"

/*
 * Unit-testing for SimplifiedAST is mostly unecessary since it's a data structure that has
 * almost no behaviour and logic.
 * Testing should be done using Parser to ensure tree correctness after parsing.
 */
TEST_CASE("StatementWrapper: Test basic functionalities") {
    // Unit-testing not needed as class only has basic getters and setters
}

TEST_CASE("Procedure: Test basic functionalities") {
    // Unit-testing for getter-setters not included

    // Variables to be used
    std::vector<std::string> emptyTokenList;

    StatementWrapper stmt1{1, ASSIGN, emptyTokenList};
    StatementWrapper stmt2{2, ASSIGN, emptyTokenList};

    Procedure testProcedure{"testProcedure"};

    SECTION("Check numStatements is properly initialized to 0") {
        REQUIRE(testProcedure.getNumStatements() == 0);
    }

    SECTION("Check addStatements() properly adds a statement to statementList") {
        testProcedure.addStatement(stmt1);
        testProcedure.addStatement(stmt2);

        // Here, we don't use getNumberStatements() because we need to recursively count.
        REQUIRE(testProcedure.getStatementList().size() == 2);
    }
}

TEST_CASE("SimplifiedAST: Test basic functionalities") {
    // Unit-testing for getter-setters not included

    // Variables to be used
    std::vector<std::string> emptyTokenList;

    StatementWrapper procOneStmt1{1, ASSIGN, emptyTokenList};
    StatementWrapper procOnestmt2{2, ASSIGN, emptyTokenList};
    StatementWrapper procTwoStmt1{3, ASSIGN, emptyTokenList};
    StatementWrapper procTwoStmt2{4, ASSIGN, emptyTokenList};
    StatementWrapper procTwoStmt3{5, ASSIGN, emptyTokenList};

    Procedure firstProcedure{"firstProcedure"};
    Procedure secondProcedure{"secondProcedure"};

    firstProcedure.addStatement(procOneStmt1);
    firstProcedure.addStatement(procOnestmt2);
    secondProcedure.addStatement(procTwoStmt1);
    secondProcedure.addStatement(procTwoStmt2);
    secondProcedure.addStatement(procTwoStmt3);

    SimplifiedAST testAST;

    SECTION("Check addProcedure() properly adds a procedure to procedureList") {
        testAST.addProcedure(firstProcedure);
        testAST.addProcedure(secondProcedure);
        REQUIRE(testAST.getNumProcedures() == 2);
    }
}