#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Hasher.h"
#include "Optimizer.h"
#include "PKB.h"
#include "QueryParser.h"
#include "Table.h"
#include "constants.h"

// extern static PKB& pkb;

class QueryEvaluator {
  public:
    QueryEvaluator();

    static std::list<std::string> evaluateQuery(std::vector<QueryParser::InputType> selectedEntites,
                                                std::vector<QueryParser::Clause> suchThatClauses,
                                                std::vector<QueryParser::Clause> patternClauses,
                                                std::vector<QueryParser::Clause> withClauses);

    static bool isAbstractVariable(QueryParser::InputType var);
    static bool isTransitiveRelation(std::string relationType);
    static bool isRelationBetweenIdentifiers(QueryParser::Clause clause);

    static bool getBooleanFromClauseHelper(QueryParser::Clause clause);
    static bool getBooleanFromSuchThatClause(QueryParser::Clause suchThatClause);
    static bool getBooleanFromPatternClause(QueryParser::Clause suchThatClause);
    static bool getBooleanFromWithClause(QueryParser::Clause suchThatClause);
    static bool hasWildCard(std::string expresion, std::string position);

    static std::string removeWildCard(std::string expression, bool hasLeftWildCard, bool hasRightWildCard);
    static std::string getReturnTypeForEntity(std::string designEntity);
    static std::string getReturnTypeForSuchThatClause(QueryParser::Clause clause);
    static std::string stripDelimiter(std::string relationType);

    static std::list<std::string> convertStringSetToList(std::unordered_set<std::string> input);

    static std::unordered_set<std::string> convertIntSetToStrings(std::unordered_set<int> statementNumbers);

    static std::unordered_set<std::string> queryAllDesignEntHelper(QueryParser::InputType selectedEntity);

    static std::unordered_set<std::string> queryAllDesignEntHelper(std::vector<QueryParser::InputType> selectedEntity);

    static std::vector<std::string> getColsToMatch(QueryParser::Clause suchThatClause, QueryParser::Clause patternClause);

    static std::vector<QueryParser::InputType> getInputTypeNotInTable(std::vector<std::string> synonymsNotInTable,
                                                                      std::vector<QueryParser::InputType> selectedEntities);

    static std::vector<std::string> parseAttributeInput(std::string var);

    static std::list<std::string> returnEmptyListHelper(std::vector<QueryParser::InputType> selectedEntities);

    // ======================================== EVALUATE FUNCTIONS ========================================
    static std::unordered_set<std::string> evaluateSuchThatClause(QueryParser::InputType designEntity,
                                                                  QueryParser::Clause clause);
    static std::unordered_set<std::string> evaluatePatternClause(QueryParser::InputType designEntity, QueryParser::Clause clause);
    static std::unordered_set<std::string> evaluateSuchThatAndPatternClause(QueryParser::InputType designEntity,
                                                                            QueryParser::Clause suchThat,
                                                                            QueryParser::Clause pattern);

    // ======================================== TABLE FUNCTIONS ========================================
    static Table getTableFromClauseHelper(QueryParser::Clause suchThatClause);

    static Table getTableFromRelation(QueryParser::Clause suchThatClause);
    static Table getTableFromFollowsQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive);

    static Table getTableFromParentQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive);

    static Table getTableFromCallsQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive);

    static Table getTableFromNextQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive);

    static Table getTableFromAffectsQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive);

    static Table getTableFromUsesQuery(QueryParser::InputType v1, QueryParser::InputType v2);

    static Table getTableFromModifiesQuery(QueryParser::InputType v1, QueryParser::InputType v2);

    static Table getTableFromPattern(QueryParser::Clause patternClause);

    static Table getTableFromWithClause(QueryParser::Clause withClause);

    static Table getTableFromWithClauseHelper(QueryParser::InputType input, std::vector<std::string> synonymToAttr);

    static Table getTableFromSelectedEntities(std::vector<QueryParser::InputType> selectedEntities);

    static std::string debugClause(QueryParser::Clause clause);

    static std::unordered_map<std::string, std::vector<std::string>>
    getEntitiesInTable(std::vector<std::string> headers, std::vector<QueryParser::InputType> selectedEntities);

    static Table handleAttributesInSelectedEntities(Table table, std::vector<QueryParser::InputType> selectedEntities);
};
