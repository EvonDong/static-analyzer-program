#pragma once

#include <cctype>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CallGraph.h"
#include "PKB.h"
#include "SimplifiedAST.h"

class DesignExtractor {
  private:
    SimplifiedAST ast;
    PKB &pkb;
    CallGraph callGraph;

    void populatePKBWithProcedureInfo(Procedure &procedure);
    void processFollowsRelations(std::vector<StatementWrapper> &stmts);
    void processFollowsStarRelations(std::vector<StatementWrapper> &stmts, std::vector<int> prevStmtList);
    void processParentRelations(std::vector<StatementWrapper> &stmts, int parent);
    void processParentStarRelations(std::vector<StatementWrapper> &stmts, std::vector<int> parentList);
    void processUsesRelations(Procedure &procedure);
    void processModifiesRelations(Procedure &procedure);
    void processCallsRelations(std::vector<StatementWrapper> &stmts, std::string procedureName);
    void populatePKBWithStatements(std::vector<StatementWrapper> &stmts);
    void populatePKBWithAssignmentAndConditionalPattern(std::vector<StatementWrapper> &stmts);

    /*
     * Description: Helper for populatePKBWithProcedureInfo
     *
     */
    int getEndLineOfProcedure(std::vector<StatementWrapper> &stmts);

    // ===== Helper functions for UsesRelations =====
    /*
     * Description: Returns the set of variables that fulfills the "Uses" relation in the statement list.
     *
     * Example: Can be called on the statement list of a procedure to obtain the full set of vars
     * that fulfills the "Uses" relation in the procedure.
     *
     */
    std::unordered_set<std::string> getUsesVarSet(std::vector<StatementWrapper> &stmts);

    /*
     * Description: Returns the set of variables that fulfills the "Uses" relation of a single statement.
     */
    std::unordered_set<std::string> extractUsesVarFromStmt(StatementWrapper &stmt);

    // ===== Helper functions for ModifiesRelations =====
    /*
     * Description: Returns the set of variables that fulfills the "Modifies" relation in the statement list.
     *
     * Example: Can be called on the statement list of a procedure to obtain the full set of vars
     * that fulfills the "Modifies" relation in the procedure.
     *
     */
    std::unordered_set<std::string> getModifiesVarSet(std::vector<StatementWrapper> &stmts);

    /*
     * Description: Returns the set of variables that fulfills the "Modifies" relation of a single statement.
     */
    std::unordered_set<std::string> extractModifiesVarFromStmt(StatementWrapper &stmt);

    // ===== Helper functions for populatePKBWithStatements =====
    /*
     * Description: Returns a set of variables used in an assignemt statement.
     */
    std::unordered_set<std::string> getAssignVar(std::vector<std::string> tokenList);

    /*
     * Description: Returns a set of variables used in a print/read/call statement.
     */
    std::unordered_set<std::string> getPrintReadCallVar(std::vector<std::string> tokenList);

    /*
     * Description: Returns a set of variables used in the conditional of an if/while statement.
     */
    std::unordered_set<std::string> getIfWhileVar(std::vector<std::string> tokenList);

    /*
     * Description: Returns a set of constants used in a statement.
     */
    std::unordered_set<int> getConstantList(std::vector<std::string> tokenList);

    // ===== Helper functions for populatePKBWithAssignmentPattern =====
    /*
     * Description: Returns the variables on the LHS of an assignment statement
     */
    std::string getAssignLHS(std::vector<std::string> tokenList);

    /*
     * Description: Returns the variables on the RHS of an assignment statement
     */
    std::string getAssignRHS(std::vector<std::string> tokenList);

    void CFGHelper(std::vector<StatementWrapper> &stmts, int prev, int containerOutNext,
                   std::unordered_map<int, std::set<int>> &adjList);
    void bipsCFGHelper(std::vector<StatementWrapper> &stmts, int prev, int containerOutNext, int procedureReturn,
                       std::unordered_map<int, std::set<int>> &adjList, SimplifiedAST &ast);

  public:
    DesignExtractor(SimplifiedAST ast, CallGraph &callGraph);

    /*
     * Description: Main method to start extracting design entities from the AST into the PKB.
     */
    void extract();

    std::unordered_map<int, std::set<int>> createCFG(SimplifiedAST ast);
    std::unordered_map<int, std::set<int>> createBipsCFG(SimplifiedAST &ast);
};
