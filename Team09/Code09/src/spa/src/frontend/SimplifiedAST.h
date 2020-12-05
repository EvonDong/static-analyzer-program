#pragma once

#include <string>
#include <vector>

#include "constants.h"

/*
 * StatementWrapper encapsulates a statement type data structure in our AST.
 * This class emulates the depth properties that a tree has.
 * In this AST, a Statement either contain a Statement only, or possibly with nested statement lists.
 * For example, the stmtLst of conditional statement of while <cond_expr> stmtLst
 * exists as the containerStatementList
 */
class StatementWrapper {
  private:
    /*
     * Statement encapsulates a statement.
     * It contains:
     * - statement number
     * - the tokens used in the statement
     * - statement type / design entity
     * It should only be created by StatementWrapper due to the design of the AST.
     * Refer to the documentation for more information on the AST design.
     */
    struct Statement {
        int stmtNum;
        std::vector<std::string> content;
        std::string type;
    };

    Statement statement;

    // FOR BLOCK STATEMENTS USE ONLY

    // a list of statements inside the container e.g. for WHILE
    std::vector<StatementWrapper> containerStatementList;

    // the block of if should contain an if and else list (check diagram)
    std::vector<StatementWrapper> ifStatementList;
    std::vector<StatementWrapper> elseStatementList;

  public:
    // wrapper should create a Statement obj and set into attribute
    // content should be duplicaetd, so not pass by reference
    StatementWrapper(int stmtNum, std::string type, std::vector<std::string> content);

    //==================================================================
    // APIs
    //==================================================================

    /*
     * Description: Returns the statement number of the current statement.
     */
    int getStatementNum();

    /*
     * Description: Returns a reference to the tokens used in the current statement.
     */
    std::vector<std::string> &getStatementTokens();

    /*
     * Description: Returns the type of the current statement.
     */
    std::string getStatementType();

    /*
     * Description: Returns the list of statements in the container of WHILE
     */
    std::vector<StatementWrapper> &getContainerStatementList();

    /*
     * Description: Returns the list of statements in the container of if
     */
    std::vector<StatementWrapper> &getIfStatementList();

    /*
     * Description: Returns the list of statements in the container of else
     */
    std::vector<StatementWrapper> &getElseStatementList();

    /*
     * Description: Set container statement list given a statement list.
     */
    void setContainerStatementList(std::vector<StatementWrapper> stmtList);

    /*
     * Description: Set if-block statement list given a statement list.
     */
    void setIfStatementList(std::vector<StatementWrapper> stmtList);

    /*
     * Description: Set else-block statement list given a statement list.
     */
    void setElseStatementList(std::vector<StatementWrapper> stmtList);
};

/*
 *  Procedure contains a list of statements that are from the same procedure.
 */
class Procedure {
  private:
    std::string name;
    std::vector<StatementWrapper> statementList;
    int numStatements;

  public:
    //==================================================================
    // APIs
    //==================================================================

    /*
     * Description: Constructor for creating name
     */
    Procedure(std::string name);

    /*
     * Description: Gets the name of the procedure.
     */
    std::string getName();

    /*
     * Description: Returns the list of statements that exists in the procedure.
     *
     * Note that the statementList only contains statements that are not in containers.
     * Statements inside containers are nested inside the statement in which they are nested in.
     * For more information, refer to documentation for the diagram.
     */
    std::vector<StatementWrapper> &getStatementList();

    /*
     * Description: Returns the number of statements inside the procedure.
     *
     * Includes any nested statements.
     * i.e. The total number of statements from the SIMPLE program
     */
    int getNumStatements();

    /*
     * Description: Sets the attribute <numStatements>.
     *
     * This is needed due to the recursive nature of AST which results in difficulty
     * in updating the total number. Parser can use this method to update at the end.
     */
    void setNumStatements(int num);

    /*
     * Description: Adds a statement into the statement list.
     *
     * Note that the statement to be added inside should be of type StatementWrapper
     */
    void addStatement(StatementWrapper &statement);
};

/*
 * SimplifiedAST contains a list of procedures.
 */
class SimplifiedAST {
  private:
    std::vector<Procedure> procedureList;

  public:
    //==================================================================
    // APIs
    //==================================================================

    /*
     * Description: Adds a Procedure into the AST.
     */
    void addProcedure(Procedure &procedure);

    /*
     * Description: Returns a reference to the list of procedures currently in the AST.
     */
    std::vector<Procedure> &getProcedureList();

    /*
     * Description: Returns the number of procedures currently in the AST.
     */
    int getNumProcedures();
};
