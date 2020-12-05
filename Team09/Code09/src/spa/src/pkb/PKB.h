#pragma once

#include "AffectStorage.h"
#include "CallStorage.h"
#include "FollowStorage.h"
#include "GeneralStorage.h"
#include "ModifyStorage.h"
#include "NextStorage.h"
#include "ParentStorage.h"
#include "PatternStorage.h"
#include "StatementStruct.h"
#include "UseStorage.h"
#include "constants.h"

#include <algorithm>
#include <queue>

class PKB {
  private:
    PKB() {
    }
    PKB(const PKB &) = delete;
    PKB(PKB &&) = delete;
    PKB &operator=(const PKB &) = delete;
    PKB &operator=(PKB &&) = delete;

    static FollowStorage *fStore;
    static UseStorage *uStore;
    static ModifyStorage *mStore;
    static ParentStorage *pStore;
    static PatternStorage *ptStore;
    static GeneralStorage *gStore;
    static NextStorage *nStore;
    static CallStorage *cStore;
    static AffectStorage *aStore;

    //==================================================================
    // Internal helper function(s)
    //==================================================================

    void filterSetByEntType(std::unordered_set<int> &originalSet, std::unordered_set<int> &filteredSetDest, std::string filter);
    bool isContainerStmt(std::string);

  public:
    static PKB &Instance() {
        static PKB pkb_instance;
        return pkb_instance;
    }

    //==================================================================
    // APIs for Parser
    //==================================================================

    /*
     * Description: Stores a Follows(leadingStmt, followingStmt) relation
     * into the PKB storage.
     */
    void addFollowsRelation(int leadingStmt, int followingStmt);

    /*
     * Description: Stores a Follows*(leadingStmt, followingStmt) relation
     * into the PKB storage.
     */
    void addFollowsStarRelation(int leadingStmt, int followingStmt);

    /*
     * Description: Stores a Parent(parentStmt, childStmt) relation
     * into the PKB storage.
     */
    void addParentRelation(int parentStmt, int childStmt);

    // ADDED PARAM DESIGN ENT
    /*
     * Description: Stores a Parent*(parentStmt, childStmt) relation
     * into the PKB storage.
     */
    void addParentStarRelation(int parentStmt, int childStmt);

    /*
     * Description: Stores a single or multiple Uses(stmt, var)
     * relation(s), with its corresponding design entity,
     * depending on number of variables used by stmt,
     * into the PKB storage.
     */
    void addStmtUsesRelation(int stmt, std::string designEnt, std::unordered_set<std::string> vars);

    /*
     * Description: Stores a single or multiple Uses(proc, var)
     * relation(s), depending on number of variables used by proc,
     * into the PKB storage.
     */
    void addProcUsesRelation(std::string proc, std::unordered_set<std::string> vars);

    /*
     * Description: Stores a single or multiple Modifies(stmt, var)
     * relation(s), with its corresponding design entity,
     * depending on number of variables modified by stmt,
     * into the PKB storage.
     */
    void addStmtModifiesRelation(int stmt, std::string entType, std::unordered_set<std::string> vars);

    /*
     * Description: Stores a single or multiple Modifies(proc, var)
     * relation(s), depending on number of variables modified by proc,
     * into the PKB storage.
     */
    void addProcModifiesRelation(std::string proc, std::unordered_set<std::string> vars);

    /*
     * Description: Stores a stmt information into the PKB storage.
     */
    void storeStatement(int stmtLine, std::string entType, std::unordered_set<std::string> setOfVars,
                        std::unordered_set<int> setOfConstants);

    /*
     * Description: Stores the pattern with its left hand side variable for
     * an assignment statement.
     */
    void storeAssignPattern(int stmt, std::string lhs, std::string pattern);

    /*
     * Description: Stores a patterns for conditional statements in the PKB Storage.
     */
    void addConditionPattern(int stmtNum, std::unordered_set<std::string> varsUsed, std::string designEnt);

    /*
     * Description: Stores the adjacency list into the PKB storage.
     */
    void addNextAdjacencyList(std::unordered_map<int, std::set<int>> adjList);

    /*
     * Description: Stores general information about a procedure in the PKB
     * storage.
     */
    void addProcInfo(std::string procname, int startLine, int endLine);

    /*
     * Description: Stores a Calls relation between two procedures in the PKB Storage.
     */
    void addCallsRelation(std::string caller, std::string callee, int stmtNum);

    /*
     * Description: Indicates to the PKB that all Calls relation has been populated from the FrontEnd, and that the PKB
     * can start populating the Calls* information as well as Uses and Modifies information related to calls statements,
     * using PKB's internal functions.
     */
    void startPopulateCallsStarInfo();

    //==================================================================
    // APIs for PQL
    //==================================================================

    /*
     * Description: Validates a Follows/Follows* relation between 2 statements.
     *
     * isRecursive is an optional flag. If set to True, this method will check
     * for the Follows* relation, else the flag is False by default and will
     * only check for a Parent relation.
     *
     * If isRecursive is False/not set:
     * Returns True if Follows(s1, s2) is true, else returns False.
     *
     * If isRecursive is True:
     * Returns True if Follows*(s1, s2) is true, else returns False.
     */
    bool isFollows(int s1, int s2, bool isRecursive = false);

    /*
     * Description: Validates if there is a statement y of the specified
     * entType, such that Follows(y, s1) or Follows*(y, s1) is true. For
     * example, isFollows(w, s1) will check if s1 follows a "while" statement
     * directly.
     *
     * isRecursive is an optional flag. If set to True, this method will check
     * for the Follows* relation, else the flag is False by default and will
     * only check for a Parent relation.
     *
     * If isRecursive is not set/set to False:
     * Returns True if Follows(entType, s1) is true, else returns False.
     *
     * If isRecursive is set to True:
     * Returns True if Follows*(entType, s1) is true, else returns False.
     */
    bool isFollows(std::string entType, int s1, bool isRecursive = false);

    /*
     * Description: Validates if there is a statement y of the specified
     * entType, such that Follows(s1, y) or Follows*(s1, y) is true. For
     * example, isFollows(s1, a) will there is an assignment statement that
     * follows s1 directly.
     *
     * isRecursive is an optional flag. If set to True, this method will check
     * for the Follows* relation, else the flag is False by default and will
     * only check for a Parent relation.
     *
     * If isRecursive is not set/set to False:
     * Returns True if Follows(s1, entType) is true, else returns False.
     *
     * If isRecursive is set to True:
     * Returns True if Follows*(s1, entType) is true, else returns False.
     */
    bool isFollows(int s1, std::string entType, bool isRecursive = false);

    /*
     * Description: Validates if there exists a statement x of the specified
     * entType1, and a statement y of the specified entType2, such that
     * Follows(x, y) or Follows*(x, y) is true. For example, isFollows(ifs, a)
     * will there is an assignment statement that follows an "if" statement
     * directly.
     *
     * isRecursive is an optional flag. If set to True, this method will check
     * for the Follows* relation, else the flag is False by default and will
     * only check for a Parent relation.
     *
     * If isRecursive is False/not set:
     * Returns True if there exists 2 statements such that
     * Follows(entType1, entType2) is true, else returns False.
     *
     * If isRecursive is True:
     * Returns True if there exists 2 statements such that
     * Follows*(entType1, entType2) is true, else returns False.
     */
    bool isFollows(std::string entType1, std::string entType2, bool isRecursive = false);

    /*
     * Description: Gets all statements s such that Follows(s1, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "assign" then this method returns
     * all assignment statements a, such that Follows(s1, a) is true.
     *
     * Returns a set of statement(s) that directly follows statement s1, else it
     * returns an empty set.
     */
    std::unordered_set<int> getFollowingStatements(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows*(s1, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "assign" then this method returns
     * all assignment statements a, such that Follows*(s1, a) is true.
     *
     * Returns a set of statement(s) that directly and indirectly follows (by
     * transitive relation) statement s1, else it returns an empty set.
     */
    std::unordered_set<int> getFollowingStatementsT(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows(entType, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "w" and resultFilter is "assign", then
     * this method returns all assignment statements a, such that Follows*(w, a)
     * is true.
     *
     * Returns a set of statement(s) that directly follows other statements with
     * statement type matching entType, else it returns an empty set.
     */
    std::unordered_set<int> getFollowingStatements(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows*(entType, s) is
     * true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "while" and resultFilter is "assign"
     * then this method returns all assignment statements a, such that
     * Follows*(w, a) is true.
     *
     * Returns a set of statement(s) that directly and indirectly follows other
     * statements (by the transitive relation) with statement type matching
     * entType, else it returns an empty set.
     */
    std::unordered_set<int> getFollowingStatementsT(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows(s, s1) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "assign", then this method
     * returns all assignment statements a, such that Follows(a, s1) is true.
     *
     * Returns a set of statement(s) that s1 follows directly, else it returns
     * an empty set.
     */
    std::unordered_set<int> getLeadingStatements(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows*(s, s1) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "assign", then this method
     * returns all assignment statements a, such that Follows*(a, s1) is true.
     *
     * Returns a set of statement(s) that s1 follows directly or indirectly by
     * the transitive relation, else it returns an empty set.
     */
    std::unordered_set<int> getLeadingStatementsT(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows(entType, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "while" and resultFilter is "assign",
     * then this method returns all assignment statements a such that Follows(w,
     * a) is true.
     *
     * Returns a set of statement(s) that is directly followed by statements
     * matching the specified entType, else returns an empty set.
     */
    std::unordered_set<int> getLeadingStatements(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Follows*(entType, s) is
     * true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "while" and resultFilter is "assign",
     * then this method returns all assignment statements a such that
     * Follows*(w, a) is true.
     *
     * Returns a set of statement(s) that is directly or indirectly (by
     * transitive relation) followed by statements matching the specified
     * entType, else returns an empty set.
     */
    std::unordered_set<int> getLeadingStatementsT(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all pairs of Follows(entType1, entType2) relation in the program.
     *
     * Returns a set of statements pairs that matches entType1 and entType2 respectively and is a valid Follows
     * relation in the SIMPLE program.
     */
    IntPairResultSet getAllFollowsRelationPairings(std::string entType1, std::string entType2);

    /*
     * Description: Gets all pairs of Follows*(entType1, entType2) relation in the program.
     *
     * Returns a set of statements pairs that matches entType1 and entType2 respectively and is a valid Follows*
     * relation in the SIMPLE program.
     */
    IntPairResultSet getAllFollowsRelationPairingsT(std::string entType1, std::string entType2);

    /*
     * Description: Validates a Parent/Parent* relation between 2 statements.
     * isRecursive is an optional flag, when set to True, will check for the
     * Parent* relation, else the flag is False by default and will only check
     * for the Parent relation.
     *
     * If isRecursive is not set/set to False:
     * Returns True if Parent(s1, s2) is true, else returns False.
     *
     * If isRecursive is set to True:
     * Returns True if Parent*(s1, s2) is true, else returns False.
     */
    bool isParent(int s1, int s2, bool isRecursive = false);

    /*
     * Description: Validates if there is a statement y of the specified
     * entType, such that Parent(s1, y) or Parent*(s1, y) is true. For example,
     * isParent(s1, a) will check if s1 is a parent of an assignment statement.
     *
     * isRecursive is an optional flag, when set to True, will check for the
     * Parent* relation, else the flag is False by default and will only check
     * for the Parent relation.
     *
     * If isRecursive is not set/set to False:
     * Returns True if Parent(s1, entType) is true, else returns False.
     *
     * If isRecursive is set to True:
     * Returns True if Parent*(s1, entType) is true, else returns False.
     */
    bool isParent(int s1, std::string entType, bool isRecursive = false);

    /*
     * Description: Validates if there is a statement y of the specified
     * entType, such that Parent(y, s1) or Parent*(y, s1) is true. For example,
     * isParent(w, s1) will check if there exist a 'while' statement
     * that is the parent of s1. Ie. s1 is nested in a 'while' container.
     *
     * isRecursive is an optional flag, when set to True, will check for the
     * Parent* relation, else the flag is False by default and will only check
     * for the Parent relation.
     *
     * If isRecursive is not set/set to False:
     * Returns True if Parent(y, s1) is true, else returns False.
     *
     * If isRecursive is set to True:
     * Returns True if Parent*(y, s1) is true, else returns False.
     */
    bool isParent(std::string entType, int s1, bool isRecursive = false);

    /*
     * Description: Validates if there is a statement x of the specified
     * entType1, and statement y of the specified entType2, such that Parent(x,
     * y) or Parent*(x, y) is true. For example, isParent(w, a) will check if
     * there exists a while statement that is the parent of an assignment
     * statement.
     *
     * isRecursive is an optional flag, when set to True, will check for the
     * Parent* relation, else the flag is False by default and will only check
     * for the Parent relation.
     *
     * If isRecursive is not set/set to False:
     * Returns True if Parent(entType1, entType2) is true, else returns False.
     *
     * If isRecursive is set to True:
     * Returns True if Parent*(entType1, entType2) is true, else returns False.
     */
    bool isParent(std::string entType1, std::string entType2, bool isRecursive = false);

    /*
     * Description: Gets all statements s such that Parent(s, s1) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "while", then this method returns
     * all while statements w such that Parent(w, s1) is true.
     *
     * Returns a set containing a statement that is a direct parent of statement
     * s1, otherwise this method returns an empty set if none is found.
     */
    std::unordered_set<int> getParentStatements(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Parent*(s, s1) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "while", then this method returns all
     * while statements w such that Parent*(w, s1) is true.
     *
     * Returns a set of statement(s) that is a direct or
     * indirect parent (by the transitive realtion) of statement s1, otherwise
     * this method returns an empty set if none is found.
     */
    std::unordered_set<int> getParentStatementsT(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Parent(s, entType) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "assign" and resultFilter is "while",
     * then this method returns all "while" statements w such that Parent(w, a)
     * is true, i.e. all "while" statements that contains an assignment
     * statement.
     *
     * Returns a set of statement(s) that is a direct parent of a statement that
     * matches the specifed entType type, otherwise it returns an empty set if
     * none is found.
     */
    std::unordered_set<int> getParentStatements(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Parent*(s, entType) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "assign" and resultFilter is "while",
     * then this method returns all "while" statements w such that Parent*(w, a)
     * is true.
     *
     * Returns a set of statement(s) that is a direct or indirect (by transitive
     * relation) parent of a statement that matches the specifed entType type,
     * otherwise it returns an empty set if none is found.
     */
    std::unordered_set<int> getParentStatementsT(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Parent(s1, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "assign", then this method
     * returns all assignment statements a, such that Parent(s1, a) is true.
     *
     * Returns a set of statement(s) that is directly nested under s1, otherwise
     * it returns an empty set if none is found.
     */
    std::unordered_set<int> getChildrenStatements(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Parent*(s1, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if resultFilter is "assign", then this method
     * returns all assignment statements a, such that Parent*(s1, a) is true.
     *
     * Returns a set of statement(s) that is directly or indirectly (by
     * transitive relation) nested under s1, otherwise it returns an empty set
     * if none is found.
     */
    std::unordered_set<int> getChildrenStatementsT(int s1, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s such that Parent(entType, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "if_stmt" and resultFilter is
     * "assign", then this method returns all assignment statements a, such that
     * Parent(ifs, a) is true.
     *
     * Returns a set of statement(s) that is directly nested under statements
     * that matches the specified entType type, otherwise it returns an empty
     * set if none is found.
     */
    std::unordered_set<int> getChildrenStatements(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all statements s where Parent*(entType, s) is true.
     *
     * resultFilter is an optional param to filter the result to a specified
     * statement type. E.g. if entType is "if_stmt" and resultFilter is
     * "assign", then this method returns all assignment statements a, such that
     * Parent*(ifs, a) is true.
     *
     * Returns a set of statement(s) that is directly or indirectly (by
     * transitive relation) nested under statements that matches the specified
     * entType, otherwise it returns an empty set if none is found.
     */
    std::unordered_set<int> getChildrenStatementsT(std::string entType, std::string resultFilter = STMT);

    /*
     * Description: Gets all pairs of Parent(entType1, entType2) relation in the program.
     *
     * Returns a set of statements pairs that matches entType1 and entType2 respectively and is a valid Parent
     * relation in the SIMPLE program.
     */
    IntPairResultSet getAllParentRelationPairings(std::string entType1, std::string entType2);

    /*
     * Description: Gets all pairs of Parent*(entType1, entType2) relation in the program.
     *
     * Returns a set of statements pairs that matches entType1 and entType2 respectively and is a valid Parent*
     * relation in the SIMPLE program.
     */
    IntPairResultSet getAllParentRelationPairingsT(std::string entType1, std::string entType2);

    /*
     * Description: Validates if statement s1 modifies variable v.
     *
     * Returns True if Modifies(s1, v) is true, and False if otherwise.
     */
    bool isModifies(int s1, std::string v);

    /*
     * Description: Validates if procedure proc modifies variable v.
     *
     * Returns True if Modifies(proc, v) is true, and False if otherwise.
     */
    bool isModifies(std::string proc, std::string v);

    /*
     * Description: Gets all variables v such that Modifies(s1, v) is true.
     *
     * Returns a set of variable(s) that statement s1 modifies, else returns
     * an empty set if none is found.
     */
    std::unordered_set<std::string> getModifiesVariables(int s1);

    /*
     * Description: Gets all variables modified in statements of entType.
     * For example if entType is "assign" then this methods gets all variables
     * modified in all assignment statements.
     *
     * Returns an unordered set of variables where Modifies(s, v) is true.
     */
    std::unordered_set<std::string> getModifiesVariables(std::string entType);

    /*
     * Description: Gets all statements of a specific entType that modifies.
     *
     * Returns an unordered set of statements where Modifies(s, _) is true.
     */
    std::unordered_set<int> getAllModifiesStatement(std::string entType);

    /*
     * Description: Gets all statements of a specific entType that modifies
     * variable v.
     *
     * Returns an unordered set of statements where Modifies(s, v) is true.
     */
    std::unordered_set<int> getModifiesStatement(std::string entType, std::string v);

    /*
     * Description: Gets all procedures that modifies variable v.
     *
     * Returns an unordered set of procedures where Modifies(s, v) is true.
     */
    std::unordered_set<std::string> getModifiesProc(std::string v);

    /*
     * Description: Gets all procedures that modifies a variable.
     *
     * Returns an unordered set of procedures where Modifies(p, _) is true.
     */
    std::unordered_set<std::string> getModifiesProc();

    /*
     * Description: Gets all variables modified in the specified procedure.
     *
     * Returns an unordered set of procedures where Modifies(proc, v) is true.
     */
    std::unordered_set<std::string> getModifiesVariableFromProc(std::string proc);

    /*
     * Description: Gets all pairs of Modifies(proc, var) relation in the program.
     *
     * Returns a set containing pairs of a procedure and a variable that is modified in the procedure.
     */
    StringPairResultSet getAllProcModifiesVarRelationPair();

    /*
     * Description: Gets all pairs of Modifies(stmt, var) relation in the program.
     *
     * Returns a set containing pairs of a statement line (that matches stmtEnt) and a variable that is modified in the
     * statement line.
     */
    IntStringPairResultSet getAllStmtModifiesVarRelationPair(std::string stmtEnt);

    /*
     * Description: Validates if statement s1 uses variable v.
     *
     * Returns True if Uses(s1, v) is true, and False if otherwise.
     */
    bool isUses(int s1, std::string v);

    /*
     * Description: Validates if procedure p uses variable v.
     *
     * Returns True if Uses(p, v) is true, and False if otherwise.
     */
    bool isUses(std::string proc, std::string v);

    /*
     * Description: Gets all variables v such that Uses(s1, v) is true.
     *
     * Returns a set of variable(s) that statement s1 uses, else returns
     * an empty set if none is found.
     */
    std::unordered_set<std::string> getUsesVariables(int s1);

    /*
     * Description: Gets all variables used in statements of entType.
     * For example if entType is "assign" then this methods gets all variables
     * used in all assignment statements.
     *
     * Returns an unordered set of variables where Uses(s, v) is true.
     */
    std::unordered_set<std::string> getUsesVariables(std::string entType);

    /*
     * Description: Gets all statements of a specific entType that uses.
     *
     * Returns an unordered set of statements where Uses(s, _) is true.
     */
    std::unordered_set<int> getAllUsesStatement(std::string entType);

    /*
     * Description: Gets all statements of a specific entType that uses variable
     * v.
     *
     * Returns an unordered set of statements where Uses(s, v) is true.
     */
    std::unordered_set<int> getUsesStatement(std::string entType, std::string v);

    /*
     * Description: Gets all procedures that uses variable v.
     *
     * Returns an unordered set of procedures where Uses(p, v) is true.
     */
    std::unordered_set<std::string> getUsesProc(std::string v);

    /*
     * Description: Gets all procedures that uses a variable.
     *
     * Returns an unordered set of procedures where Uses(p, _) is true.
     */
    std::unordered_set<std::string> getUsesProc();

    /*
     * Description: Gets all variables used in the specified procedure.
     *
     * Returns an unordered set of procedures where Uses(proc, v) is true.
     */
    std::unordered_set<std::string> getUsesVariableFromProc(std::string proc);

    /*
     * Description: Gets all pairs of Uses(proc, var) relation in the program.
     *
     * Returns a set containing pairs of a procedure and a variable that is used in the procedure.
     */
    StringPairResultSet getAllProcUsesVarRelationPair();

    /*
     * Description: Gets all pairs of Uses(stmt, var) relation in the program.
     *
     * Returns a set containing pairs of a statement line (that matches stmtEnt) and a variable that is used in the
     * statement line.
     */
    IntStringPairResultSet getAllStmtUsesVarRelationPair(std::string stmtEnt);

    /*
     * Description: Gets all variables in the program.
     *
     * Returns an unordered set of variables.
     */
    std::unordered_set<std::string> getAllVar();

    /*
     * Description: Gets all procedures in the program.
     *
     * Returns an unordered set of procedures.
     */
    std::unordered_set<std::string> getAllProc();

    /*
     * Description: Gets all statements in the program.
     * entType is an optional param to specify the type of statements to
     * retrieve.
     *
     * Returns an unordered set of statements.
     */
    std::unordered_set<int> getAllStatements(std::string entType = STMT);

    /*
     * Description: Gets all the (used and/or modified) variables in a
     * statement.
     *
     * Returns an unordered set of variable names.
     */
    std::unordered_set<std::string> getAllVarsInStatement(std::unordered_set<int> stmtSet);

    /*
     * Description: Gets all statements that contains (used and/or modifed)
     * at least one of the variables in the set "vars".
     *
     * Returns an unordered set of statements.
     */
    std::unordered_set<int> getStatmentNumberFromVars(std::unordered_set<std::string> vars);

    /*
     * Description: Gets all the constants that appears in the source code.
     *
     * Returns an unordered set of constants.
     */
    std::unordered_set<int> getAllConstants();

    /*
     * Description: Validate if there exists a statement with stmtNum and of statement type matching DESIGN_ENT.
     *
     * Returns True if there exsits such a statement, and False if otherwise.
     */
    bool isValidStatementNum(int stmtNum, std::string designEnt);

    /*
     * Description: Gets all assignment statements a such that pattern a("_",
     * rhsPattern) is true.
     *
     * strictStart and strictEnd are bool flags indicating if there is no
     * wildcard "_" on either of the pattern's LHS and RHS respectively. For
     * example, getStatementsWithPattern("x + y", false, true) corresponds to
     * the query - pattern a(_, _"x + y").
     *
     * Returns an unordered set of assignment statements that assigns variable v
     * with the pattern.
     */
    std::unordered_set<int> getStatementsWithPattern(std::string rhsPattern, bool strictStart, bool strictEnd);

    /*
     * Description: Gets all assignment statements a where pattern a(lhsVar,
     * rhsPattern) is true.
     *
     * strictStart and strictEnd are bool flags indicating of there is no
     * wildcard "_" on either of the pattern's LHS or RHS respectively. For
     * example, getStatementsWithPattern("count", "x + y", true, false)
     * corresponds to the query - pattern a("count", "x + y"_).
     *
     * Returns an unordered set of assignment statements that assigns variable v
     * with the pattern.
     */
    std::unordered_set<int> getStatementsWithPattern(std::string lhsVar, std::string rhsPattern, bool strictStart,
                                                     bool strictEnd);

    /*
     * Description: Retrieves all conditional statements of statement type
     * matching designEnt, and uses var in its condition. For example,
     * getVarMatchConditionStatements("count", "while") retrieves all while
     * statements w, where pattern w("count", _) is True.
     *
     * Returns an unordered set of (conditional) statements that uses variable
     * var in its condition.
     */
    std::unordered_set<int> getVarMatchConditionStatements(std::string var, std::string designEnt);

    /*
     * Description: Retrieves all conditional statements of statement type
     * matching designEnt, as long as it uses a variable in its condition. For
     * example, getConditionStatementsWithAnyVar("if") retrieves all if
     * statements ifs, where pattern ifs("count", _, _) is True.
     *
     * Returns an unordered set of (conditional) statements that uses a variable
     * in its condition.
     */
    std::unordered_set<int> getConditionStatementsWithAnyVar(std::string designEnt);

    /*
     * Description: Retrieves all pattern pairings between a variable and the conditional statement where the variable
     * appears in its condition.
     *
     * Returns an unordered set of pairs, with each pair containing an statement number and a variable name.
     */
    IntStringPairResultSet getVarMatchConditionStatementPairs(std::string designEnt);

    /*
     * Description: Checks if there is another statement that executes
     * immediately after s. For example hasNext(1) checks for Next(1, _).
     *
     * Returns True if Next(s, _) is True, otherwise returns False.
     */
    bool hasNext(int s);

    /*
     * Description: Checks if for any statement of type matching designEnt,
     * there exists another statement that executes immediately after it. For
     * example, hasNext("assign") checks for Next(a, _).
     *
     * Returns True if Next(designEnt, _) is True, otherwise returns False.
     */
    bool hasNext(std::string designEnt);

    /*
     * Description: Checks if s is executed immediately after another statement.
     * For example hasPrevious(10) checks for Next(_, 10).
     *
     * Returns True if Next(_, s) is True, otherwise returns False.
     */
    bool hasPrevious(int s);

    /*
     * Description: Checks if for any statement of type matching designEnt,
     * there exists another statement that executes immediatetly before it. For
     * example, hasPrevious("assign) checks for Next(_, a).
     *
     * Returns True if Next(_, designEnt) is True, otherwise returns False.
     */
    bool hasPrevious(std::string designEnt);

    /*
     * Description: Checks for a Next/Next* relation between 2 statements s1 and
     * s2. isRecursive is an optional boolean flag, defaulted to false,
     * indicating if the search should be recursive or not i.e. Next or Next*.
     *
     * Returns True is Next(s1, s2) or Next*(s1, s2) is True, if isRecursive is
     * set to True in the latter case. Otherwise, this method returns False
     */
    bool isNext(int s1, int s2, bool isRecursive = false);

    /*
     * Description: Retrieves all statements that are executed immediately after
     * s. For example getNextStatements(10) returns all statements s' where
     * Next(10, s') is True.
     *
     * resultFilter is an optional parameter which indicates if the final result
     * should be filtered to statements of the type indicated in resultFilter.
     * If nothing is set, all valid statements will be returned.
     *
     * Returns a set of statement lines s', where Next(s, s') is True.
     */
    std::unordered_set<int> getNextStatements(int s, std::string resultFilter = STMT);

    /*
     * Description: Retrieves all statements that are executed after
     * s. For example getNextStatementsT(10) returns all statements s' where
     * Next*(10, s') is True.
     *
     * resultFilter is an optional parameter which indicates if the final result
     * should be filtered to statements of the type indicated in resultFilter.
     * If nothing is set, all valid statements will be returned.
     *
     * Returns a set of statement lines s', where Next*(s, s') is True.
     */
    std::unordered_set<int> getNextStatementsT(int s, std::string resultFilter = STMT);

    /*
     * Description: Retrieves all statements that are executed immediately
     * before s. For example getPreviousStatements(10) returns all statements s'
     * where Next(s', 10) is True.
     *
     * resultFilter is an optional parameter which indicates if the final result
     * should be filtered to statements of the type indicated in resultFilter.
     * If nothing is set, all valid statements will be returned.
     *
     * Returns a set of statement lines s', where Next(s', s) is True.
     */
    std::unordered_set<int> getPreviousStatements(int s, std::string resultFilter = STMT);

    /*
     * Description: Retrieves all statements that are executed before s.
     * For example getPreviousStatementsT(10) returns all statements s' where
     * Next*(s', 10) is True.
     *
     * resultFilter is an optional parameter which indicates if the final result
     * should be filtered to statements of the type indicated in resultFilter.
     * If nothing is set, all valid statements will be returned.
     *
     * Returns a set of statement lines s', where Next*(s', s) is True.
     */
    std::unordered_set<int> getPreviousStatementsT(int s, std::string resultFilter = STMT);

    /*
     * Description: Retrieves all statements of type matching designEnt, that
     * are executed immediately after some other statement. For example
     * getNextStatements("assign") retrieves all assignment statements where
     * Next(_, "assign") is True.
     *
     * Returns a set of statement lines where Next(_, designEnt) is True.
     */
    std::unordered_set<int> getNextStatements(std::string designEnt);

    /*
     * Description: Retrieves all statements of type matching designEnt, that
     * are executed immediately before some other statement. For example
     * getPreviousStatements("assign") retrieves all assignment statements where
     * Next("assign", _) is True.
     *
     * Returns a set of statement lines where Next(designEnt, _) is True.
     */
    std::unordered_set<int> getPreviousStatements(std::string designEnt);

    /*
     * Description: Retrieves all Next relationship pairings that contains
     * statements of type matching designEnt1 and designEnt2. For example
     * getNextRelationPairings("assign', "while') returns all possible pairings
     * of statement lines where Next(a, w) is True.
     *
     * Returns a set of pairs of statement lines that fufills the Next
     * relationship.
     */
    IntPairResultSet getNextRelationPairings(std::string designEnt1, std::string designEnt2);

    /*
     * Description: Retrieves all Next* relationship pairings that contains
     * statements of type matching designEnt1 and designEnt2. For example
     * getNextRelationPairingsT("assign', "while') returns all possible pairings
     * of statement lines where Next*(a, w) is True.
     *
     * Returns a set of pairs of statement lines that fufills the Next*
     * relationship.
     */
    IntPairResultSet getNextRelationPairingsT(std::string designEnt1, std::string designEnt2);

    /*
     * Description: Retrieves a set of procedure names called within proc. For
     * example getProceduresCalledBy("main") returns all procedures that "main"
     * calls (if any).
     * isRecursive is optional bool param defaulted to False,
     * indicating if the check should be transitive or not i.e. Calls/Calls*.
     *
     * proc can be an empty string, indicating a wildcard param instead of a fixed procedure,
     * therefore returning all procedures that gets called in the SIMPLE program. I.e. Calls(_, p).
     *
     * Returns a set of names of procedures p, such that Calls("proc", p) or
     * Calls*("proc", p) is True, if isRecursive is set to True in the latter
     * case. If none is found, this method returns an empty set.
     */
    std::unordered_set<std::string> getProceduresCalledBy(std::string proc, bool isRecursive = false);

    /*
     * Description: Retrieves a set of procedure names that calls proc. For
     * example getProceduresThatCalls("first") returns all procedures that calls
     * "first" (if any).
     * isRecursive is optional bool param defaulted to False,
     * indicating if the check should be transitive or not i.e. Calls/Calls*.
     *
     * proc can be an empty string, indicating a wildcard param instead of a fixed procedure,
     * therefore returning all procedures contains a call stmt in the SIMPLE program. I.e. Calls(p, _).
     *
     * Returns a set of names of procedures p, such that Calls(p, "proc") or
     * Calls*(p, "proc") is True, if isRecursive is set to True in the latter
     * case. If none is found, this method returns an empty set.
     */
    std::unordered_set<std::string> getProceduresThatCalls(std::string proc, bool isRecursive = false);

    /*
     * Description: Retrieves all call relationship pairings in the program.
     * isRecursive is optional bool param defaulted to False,
     * indicating if the check should be transitive or not i.e. Calls/Calls*.
     *
     * Returns a set containing pairs of procedure names. Each pair of procedure
     * name, p and q, corresponds to a valid Calls(p, q) relation or
     * Calls*(p, q) relation if isRecursive is set to True.
     */
    StringPairResultSet getCallsRelationPairings(bool isRecursive = false);

    /*
     * Description: Validates a Calls/Calls* relation between 2 procedures. For
     * example, isCalls("main, "first") validates for a Calls("main", "first")
     * relation.
     * isRecursive is optional bool param defaulted to False,
     * indicating if the check should be transitive or not i.e. Calls/Calls*.
     *
     * Returns True if there exists a valid Calls/Calls* relation between the
     * two procedure, otherwise this method returns False.
     */
    bool isCalls(std::string caller, std::string callee, bool isRecursive = false);

    /*
     * Description: Validates if proc contains a calls statement.
     *
     * Returns True if Calls(proc, _) is True, otherwise returns False.
     */
    bool procedureIsCaller(std::string proc);

    /*
     * Description: Validates if proc is called by another procedure.
     *
     * Returns True if Calls(_, proc) is True, otherwise returns False.
     */
    bool procedureIsCallee(std::string proc);

    /*
     * Description: Validates if there exists a call relation in the program.
     *
     * Returns True if Calls(_, _) is True, otherwise returns False.
     */
    bool containsCallRelation();

    /*
     * Description: Retrieves the procedure name called at the specified stmtLine.
     *
     * Returns the procedure name called at stmtLine. If stmtLine is not a call stmt or is an invalid stmtLine, this
     * method returns an empty string.
     */
    std::string getProcedureCalledAtStmt(int stmtLine);

    /*
     * Description: Retrieves all call statements that calls proc. I.e. c.procname = proc;
     *
     */
    std::unordered_set<int> getStatementsThatCalls(std::string proc);

    /*
     * Description: Retrieves all call statements and the corresponding procedure that it calls..
     *
     * Returns a set of pairs. Each pair contains a call statement and a procedure that the calls statement calls.
     */
    IntStringPairResultSet getAllCallStmtWithProcPair();

    /*
     * Description: Validates if there exists an Affects relation in the program.
     *
     * Returns True if Affects(_, _) is True and False if otherwise.
     */
    bool containsAffectsRelation();

    /*
     * Description: Validates if there exists an Affects relation between s1 and s2.
     *
     * isRecursive is optional bool param defaulted to False,
     * indicating if the check should be transitive or not i.e. Affects/Affects*.
     *
     * Returns True if there exists an Affects/Affects* relation between the 2 statements,
     * otherwise this method returns False.
     */
    bool isAffects(int s1, int s2, bool isRecursive = false);

    /*
     * Description: Retrieves all statements s where Affects(s, _) is True.
     *
     * Returns a set of statement numbers of statements that Affects other statements.
     */
    std::unordered_set<int> getStatementsThatAffects();

    /*
     * Description: Retrieves all assignment statements a where Affects(a, s1) is True.
     *
     * Returns a set of statement numbers of assignment statements that Affects s1.
     * If none is found or s1 is not an assignment statement, this method returns an empty set.
     */
    std::unordered_set<int> getStatementsThatAffects(int s1);

    /*
     * Description: Retrieves all assignment statements a where Affects*(a, s1) is True.
     *
     * Returns a set of statement numbers of assignment statements that Affects* s1.
     * If none is found or s1 is not an assignment statement, this method returns an empty set.
     */
    std::unordered_set<int> getStatementsThatAffectsT(int s1);

    /*
     * Description: Retrieves all statements s where Affects(_, s) is True.
     *
     * Returns a set of statement numbers of assignment statements that are Affects by if any, otherwise this method
     * returns an empty set.
     */
    std::unordered_set<int> getStatementsAffectedBy();

    /*
     * Description: Retrieves all assignment statements a where Affects(s1, a) is True.
     *
     * Returns a set of statement numbers of assignment statements that s1 Affects if any otherwise this method returns
     * an empty set. If s1 is an invalid statement number or not an assignment statement, then this method returns an
     * empty set.
     */
    std::unordered_set<int> getStatementsAffectedBy(int s1);

    /*
     * Description: Retrieves all assignment statements a where Affects*(s1, a) is True.
     *
     * Returns a set of statement numbers of assignment statements that s1 Affects* if any otherwise this method returns
     * an empty set. If s1 is an invalid statement number or not an assignment statement, then this method returns an
     * empty set.
     */
    std::unordered_set<int> getStatementsAffectedByT(int s1);

    /*
     * Description: Retrieves all pairs of assignments statements a1 and a2 where Affects(a1, a2) is True.
     *
     * Returns a set of pairs of statement numbers that are in a valid Affects relation in the program.
     * If none is found, this method returns an empty set.
     */
    IntPairResultSet getAffectsRelationPairings();

    /*
     * Description: Retrieves all pairs of assignments statements a1 and a2 where Affects*(a1, a2) is True.
     *
     * Returns a set of pairs of statement numbers that are in a valid Affects* relation in the program.
     * If none is found, this method returns an empty set.
     */
    IntPairResultSet getAffectsRelationPairingsT();

    /*
     * Description: This method clears the cache in both NextStorage and AffectStorage that is used to store any
     * intermmediate results. This method should be called after every query completion.
     *
     * This method does not return anything.
     */
    void clearCache();

    /*
     * Description: Clears all the storage information.
     *
     * Returns an unordered set of assignment statements that assigns variable v
     * with the pattern.
     */
    void clear();
};
