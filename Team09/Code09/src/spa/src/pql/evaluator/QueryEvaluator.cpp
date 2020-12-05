#include "QueryEvaluator.h"

std::string TRANSITIVE_DELIMITER = "*";

const std::string FOLLOWS = "Follows";
const std::string PARENT = "Parent";
const std::string USES = "Uses";
const std::string MODIFIES = "Modifies";
const std::string CALLS = "Calls";
const std::string NEXT = "Next";
const std::string AFFECTS = "Affects";
const std::string WITH = "equal";

const std::string PROGLINE = "prog_line";
const std::string NUMBER = "num";
const std::string NAME = "name";
const std::string VALUE = "value";

const std::unordered_set<std::string> ABSTRACT_REFERENCE_TYPES = {ASSIGN, CALL,     CONSTANT,   IF_STMT,  PRINT, PROCEDURE,
                                                                  STMT,   VARIABLE, WHILE_STMT, WILDCARD, READ,  PROGLINE};

const std::unordered_set<std::string> NAMED_REFERENCE_TYPES = {EXPRESSION, IDENTIFIER, STMT_NUM};

const std::unordered_set<std::string> DESIGN_ENT_WITH_TWO_ATTRIBUTES = {CALL, READ, PRINT};

const std::unordered_map<std::string, std::string> DESIGN_ENTITY_TO_RETURN_TYPE({{STMT, STMT_NUM},
                                                                                 {READ, STMT_NUM},
                                                                                 {PRINT, STMT_NUM},
                                                                                 {CALL, STMT_NUM},
                                                                                 {WHILE_STMT, STMT_NUM},
                                                                                 {IF_STMT, STMT_NUM},
                                                                                 {ASSIGN, STMT_NUM},
                                                                                 {PROGLINE, STMT_NUM},
                                                                                 {VARIABLE, NAME},
                                                                                 {PROCEDURE, NAME},
                                                                                 {CONSTANT, VALUE}});

QueryEvaluator::QueryEvaluator() {
}

// ======================================== PUBLIC API FOR PARSER ========================================
std::list<std::string> QueryEvaluator::evaluateQuery(std::vector<QueryParser::InputType> selectedEntities,
                                                     std::vector<QueryParser::Clause> suchThatClauses,
                                                     std::vector<QueryParser::Clause> patternClauses,
                                                     std::vector<QueryParser::Clause> withClauses) {
    std::list<std::string> res;
    std::vector<QueryParser::Clause> nonBooleanClauses;
    std::vector<QueryParser::Clause> booleanClauses;

    bool shouldOptimize = true;
    if (shouldOptimize) {
        std::unordered_map<std::string, std::vector<QueryParser::Clause>> optimizedMap =
            Optimizer::getOptimizedQuerySequence(selectedEntities, suchThatClauses, patternClauses, withClauses);
        booleanClauses = optimizedMap.find("booleanClauses")->second;
        nonBooleanClauses = optimizedMap.find("nonBooleanClauses")->second;
    } else {
        for (QueryParser::Clause clause : suchThatClauses) {
            nonBooleanClauses.push_back(clause);
        }

        for (QueryParser::Clause clause : patternClauses) {
            nonBooleanClauses.push_back(clause);
        }

        for (QueryParser::Clause clause : withClauses) {
            nonBooleanClauses.push_back(clause);
        }
    }

    // check boolean vector, early termination
    for (QueryParser::Clause eachClause : booleanClauses) {
        if (!getBooleanFromClauseHelper(eachClause)) {
            return returnEmptyListHelper(selectedEntities);
        }
    }

    if (nonBooleanClauses.size() == 0)
        return convertStringSetToList(queryAllDesignEntHelper(selectedEntities));

    Table intermediateTable;
    for (int i = 0; i < nonBooleanClauses.size(); i++) {
        QueryParser::Clause eachClause = nonBooleanClauses[i];
        if (i == 0) {
            intermediateTable = getTableFromClauseHelper(eachClause);
        } else {
            Table clauseTable = getTableFromClauseHelper(eachClause);
            intermediateTable = intermediateTable.getJoin(clauseTable);
            if (intermediateTable.getCardinality() == 0)
                return returnEmptyListHelper(selectedEntities);
        }
    }

    if (intermediateTable.getCardinality() == 0)
        return returnEmptyListHelper(selectedEntities);

    intermediateTable = handleAttributesInSelectedEntities(intermediateTable, selectedEntities);
    if (selectedEntities.size() == 1) {
        QueryParser::InputType selectedEntity = selectedEntities[0];
        if (selectedEntity.entityType == "boolean") {
            res.push_back("TRUE");
            return res;
        } else {
            std::string selectedEntitySynonym = selectedEntity.var;
            if (intermediateTable.getIndexFromHeader(selectedEntitySynonym) != -1) {
                return convertStringSetToList(intermediateTable.getValuesFromColumn(selectedEntitySynonym));
            } else {
                return convertStringSetToList(queryAllDesignEntHelper(selectedEntity));
            }
        }
    } else {
        std::unordered_map<std::string, std::vector<std::string>> inOutMaps =
            getEntitiesInTable(intermediateTable.getHeaders(), selectedEntities);
        std::vector<std::string> synonymsInTable = inOutMaps.find("synonymsInTable")->second;
        std::vector<std::string> synonymsNotInTable = inOutMaps.find("synonymsNotInTable")->second;
        std::vector<std::string> synonymsToQuery;

        for (QueryParser::InputType entity : selectedEntities) {
            synonymsToQuery.push_back(entity.var);
        }

        if (synonymsNotInTable.size() == 0 && synonymsInTable.size() > 0) { // all synonyms in table
            return convertStringSetToList(intermediateTable.getValuesFromColumns(synonymsToQuery));
        } else if (synonymsInTable.size() > 0 && synonymsNotInTable.size() > 0) {
            std::vector<QueryParser::InputType> entitiesNotInTable = getInputTypeNotInTable(synonymsNotInTable, selectedEntities);
            Table projectedTable = intermediateTable.getProjection(synonymsInTable);
            Table selectedEntTable = getTableFromSelectedEntities(entitiesNotInTable);
            projectedTable = projectedTable.getJoin(selectedEntTable);
            return convertStringSetToList(projectedTable.getValuesFromColumns(synonymsToQuery));
        } else if (synonymsInTable.size() == 0 && synonymsNotInTable.size() > 0) {
            return convertStringSetToList(queryAllDesignEntHelper(selectedEntities));
        } else {
            res.push_back("Invalid tuples");
            return res;
        }
    }

    return res;
}

std::unordered_map<std::string, std::vector<std::string>>
QueryEvaluator::getEntitiesInTable(std::vector<std::string> headers, std::vector<QueryParser::InputType> selectedEntities) {
    std::unordered_set<std::string> selectedEntitySynonyms;
    std::unordered_set<std::string> uniqueHeaders;
    std::unordered_map<std::string, std::vector<std::string>> returnMap;
    std::vector<std::string> synonymsInTable;
    std::vector<std::string> synonymsNotInTable;

    for (QueryParser::InputType selectedEntity : selectedEntities) {
        selectedEntitySynonyms.insert(selectedEntity.var);
    }

    for (std::string header : headers) {
        uniqueHeaders.insert(header);
    }

    for (const std::string &selectedEntitySynonym : selectedEntitySynonyms) {
        if (uniqueHeaders.find(selectedEntitySynonym) == uniqueHeaders.end()) {
            synonymsNotInTable.push_back(selectedEntitySynonym);
        } else {
            synonymsInTable.push_back(selectedEntitySynonym);
        }
    }

    returnMap.insert({"synonymsInTable", synonymsInTable});
    returnMap.insert({"synonymsNotInTable", synonymsNotInTable});
    return returnMap;
}

std::unordered_set<std::string> QueryEvaluator::evaluateSuchThatAndPatternClause(QueryParser::InputType selectedEntity,
                                                                                 QueryParser::Clause suchThat,
                                                                                 QueryParser::Clause pattern) {
    std::unordered_set<std::string> res;
    Table suchThatTable = getTableFromRelation(suchThat);
    Table patternTable = getTableFromPattern(pattern);
    std::vector<std::string> colsToMatch = getColsToMatch(suchThat, pattern);
    std::string selectedEntitySynonym = selectedEntity.var;

    if (colsToMatch.size() != 0) {
        Table joinTable = suchThatTable.getJoin(patternTable);
        if (joinTable.getIndexFromHeader(selectedEntitySynonym) != -1) {
            return joinTable.getValuesFromColumn(selectedEntitySynonym);
        } else {
            if (joinTable.getCardinality() > 0) {
                return queryAllDesignEntHelper(selectedEntity);
            }
        }
    } else {
        std::string queryReturnType = getReturnTypeForSuchThatClause(suchThat);
        bool isRelationTrue = (queryReturnType == "boolean") && getBooleanFromSuchThatClause(suchThat);
        if ((suchThatTable.getCardinality() > 0 || isRelationTrue) && patternTable.getCardinality() > 0) {
            if (suchThatTable.getIndexFromHeader(selectedEntitySynonym) != -1) {
                return suchThatTable.getValuesFromColumn(selectedEntitySynonym);
            } else if (patternTable.getIndexFromHeader(selectedEntitySynonym) != -1) {
                return patternTable.getValuesFromColumn(selectedEntitySynonym);
            } else {
                return queryAllDesignEntHelper(selectedEntity);
            }
        }
    }
    return res;
}

std::unordered_set<std::string> QueryEvaluator::evaluatePatternClause(QueryParser::InputType selectedEntity,
                                                                      QueryParser::Clause patternClause) {
    Table resultTable = getTableFromPattern(patternClause);
    std::string selectedEntitySynonym = selectedEntity.var;
    if (resultTable.getIndexFromHeader(selectedEntitySynonym) != -1) {
        return resultTable.getValuesFromColumn(selectedEntitySynonym);
    } else {
        if (resultTable.getCardinality() > 0) {
            return queryAllDesignEntHelper(selectedEntity);
        }
    }
    return std::unordered_set<std::string>();
}

Table QueryEvaluator::getTableFromPattern(QueryParser::Clause patternClause) {
    PKB &pkb = PKB::Instance();
    Table resultTable;
    std::unordered_set<int> stmtNums;
    QueryParser::InputType lhs = patternClause.firstVar;
    QueryParser::InputType rhs = patternClause.secondVar;
    std::string patternSynonym = patternClause.clauseVariable;
    std::string clauseType = patternClause.clauseType;
    bool isAssignClause = clauseType == "assignPattern";
    std::string patternDesignEnt = clauseType == "ifPattern" ? IF_STMT : WHILE_STMT;

    bool hasLeftWildCard;
    bool hasRightWildCard;
    std::string rawPattern;

    if (rhs.entityType == EXPRESSION) {
        hasLeftWildCard = hasWildCard(rhs.var, "left");
        hasRightWildCard = hasWildCard(rhs.var, "right");
        rawPattern = removeWildCard(rhs.var, hasLeftWildCard, hasRightWildCard);
    }

    if (lhs.entityType == VARIABLE) { // e.g. pattern a (v, "asds") or pattern a(v, _)
        resultTable = Table({patternSynonym, lhs.var});

        if (isAssignClause) {
            stmtNums = rhs.entityType == EXPRESSION
                           ? pkb.getStatementsWithPattern(rawPattern, !hasLeftWildCard, !hasRightWildCard)
                           : pkb.getStatementsWithPattern("", false, false);

            if (stmtNums.size() == 0)
                return resultTable;

            for (const int &stmtNum : stmtNums) {
                // there can only be one assign variable on the lhs
                std::unordered_set<std::string> names = pkb.getModifiesVariables(stmtNum);
                if (names.size() == 0)
                    continue;
                auto varName = names.begin();
                std::vector<std::string> row = {std::to_string(stmtNum), *varName};
                resultTable.insertRow(row);
            }
        } else {
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> pairs;
            pairs = pkb.getVarMatchConditionStatementPairs(patternDesignEnt); // e.g. pattern ifs (v, _)
            for (std::pair<int, std::string> pair : pairs) {
                resultTable.insertRow({std::to_string(pair.first), pair.second});
            }
        }
        return resultTable;
    } else { // e.g. pattern a (_, _) or pattern a ("asd", ...)
        resultTable = Table({patternSynonym});
        if (isAssignClause) {
            if (lhs.entityType == WILDCARD && rhs.entityType == WILDCARD) { // e.g. pattern a(_, _);
                stmtNums = pkb.getStatementsWithPattern("", false, false);
            } else if (lhs.entityType == WILDCARD && rhs.entityType == EXPRESSION) { // e.g. pattern a(_, _"asd");
                stmtNums = pkb.getStatementsWithPattern(rawPattern, !hasLeftWildCard, !hasRightWildCard);
            } else if (lhs.entityType == IDENTIFIER && rhs.entityType == WILDCARD) { // e.g. pattern a("testVar", _);
                stmtNums = pkb.getStatementsWithPattern(lhs.var, "", false, false);
            } else if (lhs.entityType == IDENTIFIER && rhs.entityType == EXPRESSION) { // e.g. pattern a("testVar", "asd");
                stmtNums = pkb.getStatementsWithPattern(lhs.var, rawPattern, !hasLeftWildCard, !hasRightWildCard);
            }
        } else { // while/if
            if (lhs.entityType == WILDCARD && rhs.entityType == WILDCARD) {
                stmtNums = pkb.getConditionStatementsWithAnyVar(patternDesignEnt);
            } else {
                stmtNums = (lhs.entityType == WILDCARD) ? pkb.getVarMatchConditionStatements("", patternDesignEnt)
                                                        : pkb.getVarMatchConditionStatements(lhs.var, patternDesignEnt);
            }
        }
        resultTable.insertSingleCol(convertIntSetToStrings(stmtNums));
        return resultTable;
    }
}

/**
 * Evalues a single SUCH THAT Clause
 * Based on the return type of the selectedEntity, the return results would either be name or stmtNums
 * For names: {"name1", "name2"}
 * For stmtNums: {"5", "3"}
 */
std::unordered_set<std::string> QueryEvaluator::evaluateSuchThatClause(QueryParser::InputType selectedEntity,
                                                                       QueryParser::Clause suchThatClause) {
    PKB &pkb = PKB::Instance();
    std::string selectedEntitySynonym = selectedEntity.var;
    std::unordered_set<std::string> results;
    std::string queryReturnType = getReturnTypeForSuchThatClause(suchThatClause);

    Table resultTable;

    if (queryReturnType == "boolean") {
        bool isRelationTrue = getBooleanFromSuchThatClause(suchThatClause);
        if (!isRelationTrue)
            return results; // empty set
        return queryAllDesignEntHelper(selectedEntity);
    } else {
        resultTable = getTableFromRelation(suchThatClause);
        if (resultTable.getIndexFromHeader(selectedEntitySynonym) != -1) {
            return resultTable.getValuesFromColumn(selectedEntitySynonym);
        } else {
            if (resultTable.getCardinality() > 0) {
                return queryAllDesignEntHelper(selectedEntity);
            }
        }
    }
    return results;
}

Table QueryEvaluator::getTableFromRelation(QueryParser::Clause suchThatClause) {
    std::string relationType = stripDelimiter(suchThatClause.clauseVariable);
    bool isTransitive = isTransitiveRelation(suchThatClause.clauseVariable);
    QueryParser::InputType v1 = suchThatClause.firstVar;
    QueryParser::InputType v2 = suchThatClause.secondVar;
    Table res;

    if (v1.entityType == PROGLINE)
        v1.entityType = STMT;
    if (v2.entityType == PROGLINE)
        v2.entityType = STMT;

    if (relationType == FOLLOWS) {
        res = getTableFromFollowsQuery(v1, v2, isTransitive);
    } else if (relationType == PARENT) {
        res = getTableFromParentQuery(v1, v2, isTransitive);
    } else if (relationType == USES) {
        res = getTableFromUsesQuery(v1, v2);
    } else if (relationType == MODIFIES) {
        res = getTableFromModifiesQuery(v1, v2);
    } else if (relationType == CALLS) {
        res = getTableFromCallsQuery(v1, v2, isTransitive);
    } else if (relationType == AFFECTS) {
        res = getTableFromAffectsQuery(v1, v2, isTransitive);
    } else if (relationType == NEXT) {
        res = getTableFromNextQuery(v1, v2, isTransitive);
    }
    return res;
}

Table QueryEvaluator::getTableFromFollowsQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive) {
    PKB &pkb = PKB::Instance();
    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<int> res;

    if (isV1Abstract && isV2Abstract) { // e.g. follows(w, a);
        if (v1.var == v2.var && v1.entityType != WILDCARD)
            return resultTable;
        if (v1.entityType == WILDCARD) { // e.g. follows(_, a)
            resultTable = Table({v2.var});
            res =
                isTransitive ? pkb.getFollowingStatementsT(STMT, v2.entityType) : pkb.getFollowingStatements(STMT, v2.entityType);
            resultTable.insertSingleCol(convertIntSetToStrings(res));
            return resultTable;
        } else if (v2.entityType == WILDCARD) { // e.g. follows(a, _)
            resultTable = Table({v1.var});
            res = isTransitive ? pkb.getLeadingStatementsT(STMT, v1.entityType) : pkb.getLeadingStatements(STMT, v1.entityType);
            resultTable.insertSingleCol(convertIntSetToStrings(res));
            return resultTable;
        } else {
            resultTable = Table({v1.var, v2.var});
            std::unordered_set<std::pair<int, int>, intPairHash, intPairComparator> stmtPairs =
                isTransitive ? pkb.getAllFollowsRelationPairingsT(v1.entityType, v2.entityType)
                             : pkb.getAllFollowsRelationPairings(v1.entityType, v2.entityType);

            for (std::pair<int, int> stmtPair : stmtPairs) {
                resultTable.insertRow({std::to_string(stmtPair.first), std::to_string(stmtPair.second)});
            }
            return resultTable;
        }
    } else if (!isV1Abstract && isV2Abstract) { // e.g. follows(4, w)
        if (v1.entityType != STMT_NUM)
            return resultTable; // defensive check
        resultTable = Table({v2.var});
        if (v2.entityType == WILDCARD) {
            res = isTransitive ? pkb.getFollowingStatementsT(stoi(v1.var)) : pkb.getFollowingStatements(stoi(v1.var));
        } else {
            res = isTransitive ? pkb.getFollowingStatementsT(stoi(v1.var), v2.entityType)
                               : pkb.getFollowingStatements(stoi(v1.var), v2.entityType);
        }
    } else if (isV1Abstract && !isV2Abstract) { // e.g. follows(w, 4) where v1 = w and v2 = 4
        if (v2.entityType != STMT_NUM)
            return resultTable; // defensive check
        resultTable = Table({v1.var});

        if (v1.entityType == WILDCARD) {
            res = isTransitive ? pkb.getLeadingStatementsT(stoi(v2.var)) : pkb.getLeadingStatements(stoi(v2.var));
        } else {
            res = isTransitive ? pkb.getLeadingStatementsT(stoi(v2.var), v1.entityType)
                               : pkb.getLeadingStatements(stoi(v2.var), v1.entityType);
        }
    }

    resultTable.insertSingleCol(convertIntSetToStrings(res));
    return resultTable;
}

std::list<std::string> QueryEvaluator::returnEmptyListHelper(std::vector<QueryParser::InputType> selectedEntities) {
    if (selectedEntities.size() == 1 && selectedEntities[0].entityType == "boolean") {
        return convertStringSetToList({"FALSE"});
    } else {
        return {};
    }
}

Table QueryEvaluator::getTableFromParentQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive) {
    PKB &pkb = PKB::Instance();
    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<int> res;

    if (isV1Abstract && isV2Abstract) { // e.g. parent(w, a);
        if (v1.var == v2.var && v1.entityType != WILDCARD)
            return resultTable;
        if (v1.entityType == WILDCARD) { // e.g. parent(_, a)
            resultTable = Table({v2.var});
            res = isTransitive ? pkb.getChildrenStatementsT(STMT, v2.entityType) : pkb.getChildrenStatements(STMT, v2.entityType);
            resultTable.insertSingleCol(convertIntSetToStrings(res));
            return resultTable;
        } else if (v2.entityType == WILDCARD) { // e.g. parent(a, _)
            resultTable = Table({v1.var});
            res = isTransitive ? pkb.getParentStatementsT(STMT, v1.entityType) : pkb.getParentStatements(STMT, v1.entityType);
            resultTable.insertSingleCol(convertIntSetToStrings(res));
            return resultTable;
        } else {
            resultTable = Table({v1.var, v2.var});
            std::unordered_set<std::pair<int, int>, intPairHash, intPairComparator> stmtPairs =
                isTransitive ? pkb.getAllParentRelationPairingsT(v1.entityType, v2.entityType)
                             : pkb.getAllParentRelationPairings(v1.entityType, v2.entityType);

            for (std::pair<int, int> stmtPair : stmtPairs) {
                resultTable.insertRow({std::to_string(stmtPair.first), std::to_string(stmtPair.second)});
            }
            return resultTable;
        }
    } else if (isV1Abstract && !isV2Abstract) { // e.g. parent(w, 4) where v1= w and v2 = 4
        resultTable = Table({v1.var});
        if (v1.entityType == WILDCARD) {
            res = isTransitive ? pkb.getParentStatementsT(stoi(v2.var)) : pkb.getParentStatements(stoi(v2.var));
        } else {
            res = isTransitive ? pkb.getParentStatementsT(stoi(v2.var), v1.entityType)
                               : pkb.getParentStatements(stoi(v2.var), v1.entityType);
        }
    } else if (!isV1Abstract && isV2Abstract) { // e.g. parent(4, w)
        resultTable = Table({v2.var});
        if (v2.entityType == WILDCARD) {
            res = isTransitive ? pkb.getChildrenStatementsT(stoi(v1.var)) : pkb.getChildrenStatements(stoi(v1.var));
        } else {
            res = isTransitive ? pkb.getChildrenStatementsT(stoi(v1.var), v2.entityType)
                               : pkb.getChildrenStatements(stoi(v1.var), v2.entityType);
        }
    }
    resultTable.insertSingleCol(convertIntSetToStrings(res));
    return resultTable;
}

Table QueryEvaluator::getTableFromUsesQuery(QueryParser::InputType v1, QueryParser::InputType v2) {
    PKB &pkb = PKB::Instance();

    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<std::string> res;

    // Note, these are already handled since the _ could really only refer to variable
    // Uses(p, _), Uses(s, _), Uses("procedureName", _), Uses(6, _)

    if (isV1Abstract && isV2Abstract) {
        if (v1.var == v2.var && v1.entityType != WILDCARD)
            return resultTable;
        if (v1.entityType == PROCEDURE) {
            std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> namePairs =
                pkb.getAllProcUsesVarRelationPair();
            if (v2.entityType == WILDCARD) { // e.g. uses(p, _)
                resultTable = Table({v1.var});
                for (std::pair<std::string, std::string> namePair : namePairs) {
                    resultTable.insertRow({namePair.first});
                }
            } else {
                resultTable = Table({v1.var, v2.var});
                for (std::pair<std::string, std::string> namePair : namePairs) {
                    resultTable.insertRow({namePair.first, namePair.second});
                }
            }
        } else {
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllStmtUsesVarRelationPair(v1.entityType);
            if (v2.entityType == WILDCARD) { // e.g. modifies(p, _)
                resultTable = Table({v1.var});
                for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                    resultTable.insertRow({std::to_string(stmtNamePair.first)});
                }
            } else {
                resultTable = Table({v1.var, v2.var});
                for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                    resultTable.insertRow({std::to_string(stmtNamePair.first), stmtNamePair.second});
                }
            }
        }
    } else if (isV1Abstract && !isV2Abstract) {
        resultTable = Table({v1.var});
        res = v1.entityType == PROCEDURE ? pkb.getUsesProc(v2.var) :                   // e.g. Uses(p, "v")
                  convertIntSetToStrings(pkb.getUsesStatement(v1.entityType, v2.var)); // e.g. Uses(s, "v")
        resultTable.insertSingleCol(res);
        return resultTable;
    } else if (!isV1Abstract && isV2Abstract) {
        resultTable = Table({v2.var});
        res = (v1.entityType == IDENTIFIER) ? pkb.getUsesVariableFromProc(v1.var) : // e.g. uses(PROCEDURE, v)
                  pkb.getUsesVariables(stoi(v1.var));                               // e.g. uses(5,v)
        resultTable.insertSingleCol(res);
        return resultTable;
    }
    return resultTable;
}

Table QueryEvaluator::getTableFromModifiesQuery(QueryParser::InputType v1, QueryParser::InputType v2) {
    PKB &pkb = PKB::Instance();

    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<std::string> res;

    // Note, these are already handled since the _ could really only refer to variable
    // Modifies(p, _), Modifies(s, _), Modifies("procedureName", _), Modifies(6, _)

    if (isV1Abstract && isV2Abstract) {
        if (v1.var == v2.var && v1.entityType != WILDCARD)
            return resultTable;
        if (v1.entityType == PROCEDURE) {
            std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> namePairs =
                pkb.getAllProcModifiesVarRelationPair();
            if (v2.entityType == WILDCARD) { // e.g. modifies(p, _)
                resultTable = Table({v1.var});
                for (std::pair<std::string, std::string> namePair : namePairs) {
                    resultTable.insertRow({namePair.first});
                }
            } else {
                resultTable = Table({v1.var, v2.var});
                for (std::pair<std::string, std::string> namePair : namePairs) {
                    resultTable.insertRow({namePair.first, namePair.second});
                }
            }
        } else {
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllStmtModifiesVarRelationPair(v1.entityType);
            if (v2.entityType == WILDCARD) { // e.g. modifies(p, _)
                resultTable = Table({v1.var});
                for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                    resultTable.insertRow({std::to_string(stmtNamePair.first)});
                }
            } else {
                resultTable = Table({v1.var, v2.var});
                for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                    resultTable.insertRow({std::to_string(stmtNamePair.first), stmtNamePair.second});
                }
            }
        }
    } else if (isV1Abstract && !isV2Abstract) {
        resultTable = Table({v1.var});
        res = v1.entityType == PROCEDURE ? pkb.getModifiesProc(v2.var) :                   // e.g. modifies(p, "v")
                  convertIntSetToStrings(pkb.getModifiesStatement(v1.entityType, v2.var)); // e.g. modifies(s, "v")
        resultTable.insertSingleCol(res);
        return resultTable;
    } else if (!isV1Abstract && isV2Abstract) {
        resultTable = Table({v2.var});
        res = (v1.entityType == IDENTIFIER) ? pkb.getModifiesVariableFromProc(v1.var) : // e.g. modifies(PROCEDURE, v)
                  pkb.getModifiesVariables(stoi(v1.var));                               // e.g. modifies(5,v)
        resultTable.insertSingleCol(res);
        return resultTable;
    }
    return resultTable;
}

Table QueryEvaluator::getTableFromCallsQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive) {
    PKB &pkb = PKB::Instance();

    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<std::string> res;

    if (isV1Abstract && isV2Abstract) {  // e.g. calls (p, q)
        if (v1.entityType == WILDCARD) { // e.g. calls(_, p)
            resultTable = Table({v2.var});
            res = pkb.getProceduresThatCalls("");
            resultTable.insertSingleCol(res);
            return resultTable;
        } else if (v2.entityType == WILDCARD) { // e.g. calls(p, _)
            resultTable = Table({v1.var});
            res = pkb.getProceduresCalledBy("");
            resultTable.insertSingleCol(res);
            return resultTable;
        }

        std::unordered_set<std::pair<std::string, std::string>, strPairHash, strPairComparator> namePairs =
            pkb.getCallsRelationPairings(isTransitive);
        if (v1.var == v2.var) {
            resultTable = Table({v1.var});
            resultTable = Table({v1.var, v2.var});
            for (std::pair<std::string, std::string> procPair : pkb.getCallsRelationPairings(isTransitive)) {
                if (procPair.first == procPair.second) {
                    resultTable.insertRow({procPair.first});
                }
            }
        } else {
            resultTable = Table({v1.var, v2.var});
            for (std::pair<std::string, std::string> procPair : pkb.getCallsRelationPairings(isTransitive)) {
                resultTable.insertRow({procPair.first, procPair.second});
            }
        }
        return resultTable;
    } else if (isV1Abstract && !isV2Abstract) { // e.g. calls(p, "main")
        resultTable = Table({v1.var});
        res = pkb.getProceduresThatCalls(v2.var, isTransitive);
        resultTable.insertSingleCol(res);
        return resultTable;
    } else if (!isV1Abstract && isV2Abstract) { // e.g. calls("main", p)
        resultTable = Table({v2.var});
        res = pkb.getProceduresCalledBy(v1.var, isTransitive);
        resultTable.insertSingleCol(res);
        return resultTable;
    }
    return resultTable;
}

Table QueryEvaluator::getTableFromNextQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive) {
    PKB &pkb = PKB::Instance();

    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<std::string> res;

    if (isV1Abstract && isV2Abstract) {  // e.g. next* (w, a)
        if (v1.entityType == WILDCARD) { // e.g. next(_, a)
            resultTable = Table({v2.var});
            res = convertIntSetToStrings(pkb.getNextStatements(v2.entityType));
            resultTable.insertSingleCol(res);
        } else if (v2.entityType == WILDCARD) { // e.g. next(a, _)
            resultTable = Table({v1.var});
            res = convertIntSetToStrings(pkb.getPreviousStatements(v1.entityType));
            resultTable.insertSingleCol(res);
        } else {
            std::unordered_set<std::pair<int, int>, intPairHash, intPairComparator> stmtPairs =
                isTransitive ? pkb.getNextRelationPairingsT(v1.entityType, v2.entityType)
                             : pkb.getNextRelationPairings(v1.entityType, v2.entityType);

            if (v1.var == v2.var) {
                resultTable = Table({v1.var});
                for (std::pair<int, int> stmtPair : stmtPairs) {
                    if (stmtPair.first == stmtPair.second) {
                        resultTable.insertRow({std::to_string(stmtPair.first)});
                    }
                }
            } else {
                resultTable = Table({v1.var, v2.var});

                for (std::pair<int, int> stmtPair : stmtPairs) {
                    resultTable.insertRow({std::to_string(stmtPair.first), std::to_string(stmtPair.second)});
                }
            }
            return resultTable;
        }
    } else if (isV1Abstract && !isV2Abstract) { // e.g. next(a, 10)
        resultTable = Table({v1.var});
        res = isTransitive ? convertIntSetToStrings(pkb.getPreviousStatementsT(stoi(v2.var), v1.entityType))
                           : convertIntSetToStrings(pkb.getPreviousStatements(stoi(v2.var), v1.entityType));
        resultTable.insertSingleCol(res);
        return resultTable;
    } else if (!isV1Abstract && isV2Abstract) { // e.g. next(10, a)
        resultTable = Table({v2.var});
        res = isTransitive ? convertIntSetToStrings(pkb.getNextStatementsT(stoi(v1.var), v2.entityType))
                           : convertIntSetToStrings(pkb.getNextStatements(stoi(v1.var), v2.entityType));
        resultTable.insertSingleCol(res);
        return resultTable;
    }
    return resultTable;
}

Table QueryEvaluator::getTableFromAffectsQuery(QueryParser::InputType v1, QueryParser::InputType v2, bool isTransitive) {
    PKB &pkb = PKB::Instance();

    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);

    Table resultTable;
    std::unordered_set<std::string> res;

    if (isV1Abstract && isV2Abstract) {  // e.g. affects* (s, a), affects(_, a)
        if (v1.entityType == WILDCARD) { // e.g. affects(_, a)
            resultTable = Table({v2.var});
            res = convertIntSetToStrings(pkb.getStatementsAffectedBy());
            resultTable.insertSingleCol(res);
        } else if (v2.entityType == WILDCARD) { // e.g. affects(a, _)
            resultTable = Table({v1.var});
            res = convertIntSetToStrings(pkb.getStatementsThatAffects());
            resultTable.insertSingleCol(res);
        } else {
            // resultTable = Table({v1.var, v2.var});
            std::unordered_set<std::pair<int, int>, intPairHash, intPairComparator> stmtPairs =
                isTransitive ? pkb.getAffectsRelationPairingsT() : pkb.getAffectsRelationPairings();

            if (v1.var == v2.var) {
                resultTable = Table({v1.var});
                for (std::pair<int, int> stmtPair : stmtPairs) {
                    if (stmtPair.first == stmtPair.second) {
                        resultTable.insertRow({std::to_string(stmtPair.first)});
                    }
                }
            } else {
                resultTable = Table({v1.var, v2.var});

                for (std::pair<int, int> stmtPair : stmtPairs) {
                    resultTable.insertRow({std::to_string(stmtPair.first), std::to_string(stmtPair.second)});
                }
            }
            return resultTable;
        }
    } else if (isV1Abstract && !isV2Abstract) { // e.g. affects(a, 10)
        resultTable = Table({v1.var});
        res = isTransitive ? convertIntSetToStrings(pkb.getStatementsThatAffectsT(stoi(v2.var)))
                           : convertIntSetToStrings(pkb.getStatementsThatAffects(stoi(v2.var)));
        resultTable.insertSingleCol(res);
        return resultTable;
    } else if (!isV1Abstract && isV2Abstract) { // e.g. affects(10, a)
        resultTable = Table({v2.var});
        res = isTransitive ? convertIntSetToStrings(pkb.getStatementsAffectedByT(stoi(v1.var)))
                           : convertIntSetToStrings(pkb.getStatementsAffectedBy(stoi(v1.var)));
        resultTable.insertSingleCol(res);
        return resultTable;
    }
    return resultTable;
}

Table QueryEvaluator::getTableFromWithClause(QueryParser::Clause withClause) {
    QueryParser::InputType lhs = withClause.firstVar;
    QueryParser::InputType rhs = withClause.secondVar;
    PKB &pkb = PKB::Instance();

    std::vector<std::string> lhsSynonymToAttr = parseAttributeInput(lhs.var);
    std::vector<std::string> rhsSynonymToAttr = parseAttributeInput(rhs.var);

    Table resultTable;
    std::unordered_set<std::string> names;
    std::unordered_set<int> stmts;

    if (rhs.entityType == IDENTIFIER || lhs.entityType == IDENTIFIER) {
        std::string synonym = rhs.entityType == IDENTIFIER ? lhsSynonymToAttr[0] : rhsSynonymToAttr[0];
        std::string attr = rhs.entityType == IDENTIFIER ? lhs.var : rhs.var;
        std::string entType = rhs.entityType == IDENTIFIER ? lhs.entityType : rhs.entityType;
        std::string valStr = rhs.entityType == IDENTIFIER ? rhs.var : lhs.var;
        resultTable = Table({synonym, attr});

        if (entType == PROCEDURE || entType == VARIABLE) {
            names = entType == PROCEDURE ? pkb.getAllProc() : pkb.getAllVar();
            if (names.find(valStr) == names.end())
                return resultTable;
            resultTable.insertRow({valStr, valStr});
            return resultTable;
        } else if (entType == READ) {
            stmts = pkb.getModifiesStatement(entType, valStr);
            if (stmts.size() == 0)
                return resultTable;
            for (int stmt : stmts) {
                resultTable.insertRow({std::to_string(stmt), valStr});
            }
            // resultTable.insertSingleCol(convertIntSetToStrings(stmts));
            return resultTable;
        } else if (entType == CALL) {
            stmts = pkb.getStatementsThatCalls(valStr);
            if (stmts.size() == 0)
                return resultTable;
            for (int stmt : stmts) {
                resultTable.insertRow({std::to_string(stmt), valStr});
            }
            return resultTable;
        } else if (entType == PRINT) {
            stmts = pkb.getUsesStatement(entType, valStr);
            if (stmts.size() == 0)
                return resultTable;
            for (int stmt : stmts) {
                resultTable.insertRow({std::to_string(stmt), valStr});
            }
            return resultTable;
        }
        return resultTable;
    } else if (rhs.entityType == NUMBER || lhs.entityType == NUMBER) { // e.g. s.stmtNum = 10
        std::string synonym = rhs.entityType == NUMBER ? lhsSynonymToAttr[0] : rhsSynonymToAttr[0];
        std::string entType = rhs.entityType == NUMBER ? lhs.entityType : rhs.entityType;
        std::string numStr = rhs.entityType == NUMBER ? rhs.var : lhs.var;
        std::string attr = rhs.entityType == NUMBER ? lhs.var : rhs.var;

        resultTable = Table({synonym, attr});

        if (entType == CONSTANT) {
            stmts = pkb.getAllConstants();
            if (stmts.find(stoi(numStr)) == stmts.end())
                return resultTable;
            // c.val = "10"
            // c | c.value
            resultTable.insertRow({numStr, numStr});
            return resultTable;
        } else if (entType == READ) {
            // rd.stmt# = 10
            // r | r.stmt#
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllStmtModifiesVarRelationPair(entType);
            for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                if (std::to_string(stmtNamePair.first) == numStr) {
                    resultTable.insertRow({numStr, numStr});
                }
            }
            return resultTable;
        } else if (entType == CALL) { // e.g. call.stmt# = 10
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllCallStmtWithProcPair();
            for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                if (std::to_string(stmtNamePair.first) == numStr) {
                    resultTable.insertRow({numStr, numStr});
                }
            }
            return resultTable;
        } else if (entType == PRINT) {
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllStmtUsesVarRelationPair(entType);
            for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                if (std::to_string(stmtNamePair.first) == numStr) {
                    resultTable.insertRow({numStr, numStr});
                }
            }
            return resultTable;
        } else {
            entType = entType == PROGLINE ? STMT : entType;
            std::unordered_set<int> allValidStmtNums = pkb.getAllStatements(entType);
            if (allValidStmtNums.find(stoi(numStr)) == allValidStmtNums.end()) {
                return resultTable;
            }
            resultTable.insertRow({numStr, numStr});
            return resultTable;
        }
        return resultTable;
    } else {
        Table lhsTable = getTableFromWithClauseHelper(lhs, lhsSynonymToAttr);
        Table rhsTable = getTableFromWithClauseHelper(rhs, rhsSynonymToAttr);

        std::string lhsHeader = lhsTable.getHeaders()[0];
        std::string rhsHeader = rhsTable.getHeaders()[0];

        // if colsToMatch = 0, then change the header of rhs to lhs, get joinTable,
        // then duplicate col with header of rhs
        if (lhsHeader == rhsHeader) {
            resultTable = lhsTable.getJoin(rhsTable);
        } else {
            rhsTable.duplicateColumn(rhs.var, lhs.var);
            resultTable = lhsTable.getJoin(rhsTable);
        }
    }
    return resultTable;
}

Table QueryEvaluator::getTableFromWithClauseHelper(QueryParser::InputType input, std::vector<std::string> synonymToAttr) {

    std::string synonym = synonymToAttr[0];
    std::string inputAttr = synonymToAttr.size() == 2 ? synonymToAttr[1] : "";

    PKB &pkb = PKB::Instance();
    if (inputAttr == "") { // it must be prog_line since the vec[0] and vec[1] are the same
        Table res = Table({synonym});
        res.insertSingleCol(convertIntSetToStrings(pkb.getAllStatements(STMT)));
        return res;
    } else if (inputAttr == "stmt#") {
        // stmt.stmt#, read.stmt#, print.stmt#, call.stmt#, while.stmt#, if.stmt#, assign.stmt#
        Table res = Table({synonym});
        res.insertSingleCol(convertIntSetToStrings(pkb.getAllStatements(input.entityType)));
        // e.g. {a, a.stmt#}
        res.duplicateColumn(synonym, synonym + "." + inputAttr);
        return res;
    } else if (inputAttr == "procName") {
        // procedure.procName, call.procName,
        if (input.entityType == PROCEDURE) {
            Table res = Table({synonym});
            res.insertSingleCol(pkb.getAllProc());
            res.duplicateColumn(synonym, synonym + "." + inputAttr);
            return res;
        } else if (input.entityType == CALL) {
            Table res = Table({synonym, synonym + "." + inputAttr});
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllCallStmtWithProcPair();
            for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                res.insertRow({std::to_string(stmtNamePair.first), stmtNamePair.second});
            }
            return res;
        }
    } else if (inputAttr == "varName") {
        // variable.varName, read.varName, print.varName
        if (input.entityType == VARIABLE) {
            Table res = Table({synonym});
            res.insertSingleCol(pkb.getAllVar());
            res.duplicateColumn(synonym, synonym + "." + inputAttr);
            return res;
        } else if (input.entityType == READ) {
            // res.insertSingleCol(pkb.getModifiesVariables(input.entityType));
            Table res = Table({synonym, synonym + "." + inputAttr});
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllStmtModifiesVarRelationPair(input.entityType);
            for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                res.insertRow({std::to_string(stmtNamePair.first), stmtNamePair.second});
            }
            return res;
        } else if (input.entityType == PRINT) {
            // res.insertSingleCol(pkb.getUsesVariables(input.entityType));
            Table res = Table({synonym, synonym + "." + inputAttr});
            std::unordered_set<std::pair<int, std::string>, intStrHash, intStrComparator> stmtNamePairs =
                pkb.getAllStmtUsesVarRelationPair(input.entityType);
            for (std::pair<int, std::string> stmtNamePair : stmtNamePairs) {
                res.insertRow({std::to_string(stmtNamePair.first), stmtNamePair.second});
            }
            return res;
        }
    } else if (inputAttr == VALUE) { // c.value
        Table res = Table({synonym});
        res.insertSingleCol(convertIntSetToStrings(pkb.getAllConstants()));
        res.duplicateColumn(synonym, synonym + "." + inputAttr);
        return res;
    }
    return Table();
}

std::unordered_set<std::string> QueryEvaluator::queryAllDesignEntHelper(QueryParser::InputType selectedEntity) {
    PKB &pkb = PKB::Instance();
    std::unordered_set<std::string> results;
    std::string selectedEntityReturnType = getReturnTypeForEntity(selectedEntity.entityType);
    std::vector<std::string> synonymToAttr = parseAttributeInput(selectedEntity.var);
    if (synonymToAttr.size() == 2 && (synonymToAttr[1] == "procName" || synonymToAttr[1] == "varName")) {
        selectedEntityReturnType = NAME;
    }
    if (selectedEntity.entityType == "boolean") {
        results = {"TRUE"};
    } else if (selectedEntityReturnType == STMT_NUM) {
        std::string designEnt = selectedEntity.entityType == PROGLINE ? STMT : selectedEntity.entityType;
        results = convertIntSetToStrings(pkb.getAllStatements(designEnt));
    } else if (selectedEntityReturnType == NAME) {
        if (selectedEntity.entityType == PROCEDURE) {
            results = pkb.getAllProc();
        } else if (selectedEntity.entityType == VARIABLE) {
            results = pkb.getAllVar();
        } else if (selectedEntity.entityType == READ) {
            results = pkb.getModifiesVariables(selectedEntity.entityType);
        } else if (selectedEntity.entityType == CALL) {
            results = pkb.getProceduresThatCalls("");
        } else if (selectedEntity.entityType == PRINT) {
            results = pkb.getUsesVariables(selectedEntity.entityType);
        }
    } else if (selectedEntityReturnType == VALUE) {
        results = convertIntSetToStrings(pkb.getAllConstants());
    }
    return results;
}

std::unordered_set<std::string> QueryEvaluator::queryAllDesignEntHelper(std::vector<QueryParser::InputType> selectedEntities) {
    Table resTable = getTableFromSelectedEntities(selectedEntities);
    std::vector<std::string> selectedHeaders;
    for (QueryParser::InputType se : selectedEntities) {
        selectedHeaders.push_back(se.var);
    }
    return resTable.getValuesFromColumns(selectedHeaders);
}

Table QueryEvaluator::getTableFromSelectedEntities(std::vector<QueryParser::InputType> selectedEntities) {
    PKB &pkb = PKB::Instance();
    Table resTable;
    for (int i = 0; i < selectedEntities.size(); i++) {
        QueryParser::InputType selectedEntity = selectedEntities[i];
        Table selectedEntTable = Table({selectedEntity.var});
        if (selectedEntity.entityType == "boolean") {
            selectedEntTable.insertSingleCol({"TRUE"});
        } else {
            std::unordered_set<std::string> results = queryAllDesignEntHelper(selectedEntity);
            selectedEntTable.insertSingleCol(results);
        }
        if (i == 0) {
            resTable = selectedEntTable;
        } else {
            resTable = resTable.getJoin(selectedEntTable);
        }
    }
    return resTable;
}

std::vector<QueryParser::InputType> QueryEvaluator::getInputTypeNotInTable(std::vector<std::string> synonymsNotInTable,
                                                                           std::vector<QueryParser::InputType> selectedEntities) {
    std::vector<QueryParser::InputType> res;
    std::unordered_set<std::string> synonymsNotInTableSet(synonymsNotInTable.begin(), synonymsNotInTable.end());
    for (QueryParser::InputType selectedEntity : selectedEntities) {
        if (synonymsNotInTableSet.find(selectedEntity.var) != synonymsNotInTableSet.end()) {
            res.push_back(selectedEntity);
        }
    }
    return res;
}

bool QueryEvaluator::getBooleanFromClauseHelper(QueryParser::Clause clause) {
    if (clause.clauseType == "suchThat") {
        return getBooleanFromSuchThatClause(clause);
    } else if (clause.clauseType == "assignPattern" || clause.clauseType == "ifPattern" || clause.clauseType == "whilePattern") {
        return getBooleanFromPatternClause(clause);
    } else if (clause.clauseType == "with") {
        return getBooleanFromWithClause(clause);
    }
}

Table QueryEvaluator::getTableFromClauseHelper(QueryParser::Clause clause) {
    if (clause.clauseType == "suchThat") {
        return getTableFromRelation(clause);
    } else if (clause.clauseType == "assignPattern" || clause.clauseType == "ifPattern" || clause.clauseType == "whilePattern") {
        return getTableFromPattern(clause);
    } else if (clause.clauseType == "with") {
        return getTableFromWithClause(clause);
    }
}

/**
 * Evaluates the truth condition of the relationship type between a pair of identifiers iif
 * isRelationBetweenIdentifiers(v1, v2) is true.
 */
bool QueryEvaluator::getBooleanFromSuchThatClause(QueryParser::Clause clause) {
    QueryParser::InputType v1 = clause.firstVar;
    QueryParser::InputType v2 = clause.secondVar;
    std::string relationType = stripDelimiter(clause.clauseVariable);
    bool isTransitive = isTransitiveRelation(clause.clauseVariable);

    // if (v1.var == v2.var && v1.entityType != WILDCARD) return false; // e.g. parent(2,2)
    PKB &pkb = PKB::Instance();

    if (!isAbstractVariable(v1) && !isAbstractVariable(v2)) { // e.g. Follows(5,6)
        if (!isRelationBetweenIdentifiers(clause))
            return false;
        if (relationType == FOLLOWS) {
            return pkb.isFollows(stoi(v1.var), stoi(v2.var), isTransitive);
        } else if (relationType == PARENT) {
            return pkb.isParent(stoi(v1.var), stoi(v2.var), isTransitive);
        } else if (relationType == USES) { // Uses ("procName", "varName") Uses(6, "var")

            return v1.entityType == IDENTIFIER ? pkb.isUses(v1.var, v2.var) : pkb.isUses(stoi(v1.var), v2.var);
        } else if (relationType == MODIFIES) {
            return v1.entityType == IDENTIFIER ? pkb.isModifies(v1.var, v2.var) : pkb.isModifies(stoi(v1.var), v2.var);
        } else if (relationType == CALLS) {
            return pkb.isCalls(v1.var, v2.var, isTransitive);
        } else if (relationType == NEXT) {
            return pkb.isNext(stoi(v1.var), stoi(v2.var), isTransitive);
        } else if (relationType == AFFECTS) {
            return pkb.isAffects(stoi(v1.var), stoi(v2.var), isTransitive);
        }
    } else {
        // special handling for NEXT and CALLS
        if (relationType == FOLLOWS) {
            if (v1.entityType == WILDCARD && v2.entityType == WILDCARD) {
                return isTransitive ? pkb.getAllParentRelationPairingsT(STMT, STMT).size() != 0
                                    : pkb.getAllParentRelationPairings(STMT, STMT).size() != 0;
            }
        } else if (relationType == FOLLOWS) {
            if (v1.entityType == WILDCARD && v2.entityType == WILDCARD) {
                return isTransitive ? pkb.getAllFollowsRelationPairingsT(STMT, STMT).size() != 0
                                    : pkb.getAllFollowsRelationPairings(STMT, STMT).size() != 0;
            }
        } else if (relationType == NEXT) {
            if (v1.entityType == WILDCARD && v2.entityType == WILDCARD) {
                return pkb.hasNext(STMT);
            } else if (v1.entityType == WILDCARD) {
                return v2.entityType == STMT_NUM ? pkb.hasPrevious(stoi(v2.var)) : pkb.hasPrevious(v2.entityType);
            } else if (v2.entityType == WILDCARD) {
                return pkb.hasNext(stoi(v1.var));
            }
        } else if (relationType == CALLS) {
            if (v1.entityType == WILDCARD && v2.entityType == WILDCARD) {
                return pkb.containsCallRelation();
            } else if (v1.entityType == WILDCARD && !isAbstractVariable(v2)) {
                return pkb.procedureIsCallee(v2.var);
            } else if (v2.entityType == WILDCARD && !isAbstractVariable(v1)) {
                return pkb.procedureIsCaller(v1.var);
            }
        } else if (relationType == AFFECTS) {
            if (v1.entityType == WILDCARD && v2.entityType == WILDCARD) {
                return pkb.containsAffectsRelation();
            }
        }
        Table res = getTableFromRelation(clause);
        return res.getCardinality() > 0;
    }
    return false;
}

bool QueryEvaluator::getBooleanFromPatternClause(QueryParser::Clause clause) {
    Table t = getTableFromPattern(clause);
    return t.getCardinality() > 0;
}

bool QueryEvaluator::getBooleanFromWithClause(QueryParser::Clause clause) {
    if ((clause.firstVar.entityType == IDENTIFIER && clause.secondVar.entityType == IDENTIFIER) ||
        (clause.firstVar.entityType == NUMBER && clause.secondVar.entityType == NUMBER)) {
        return clause.firstVar.var == clause.secondVar.var;
    }
    Table t = getTableFromWithClause(clause);
    return t.getCardinality() > 0;
}

std::unordered_set<std::string> QueryEvaluator::convertIntSetToStrings(std::unordered_set<int> statementNumbers) {
    std::unordered_set<std::string> res;
    if (statementNumbers.size() == 0)
        return res;
    for (const int &stmtNum : statementNumbers) {
        res.insert(std::to_string(stmtNum));
    }
    return res;
}

std::string QueryEvaluator::stripDelimiter(std::string relationType) {
    return relationType.substr(0, relationType.find(TRANSITIVE_DELIMITER));
}

/*
Takes in a string (e.g. s.stmt#), and returns a vector of size 2 [s, stmt#]. Returns the original string in the vector if
delimiter (.) is not found.
 */
std::vector<std::string> QueryEvaluator::parseAttributeInput(std::string s) {
    std::vector<std::string> res;
    std::string delimiter = ".";
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        res.push_back(token);
        s = s.substr(pos + delimiter.length());
    }
    res.push_back(s);
    return res;
}

bool QueryEvaluator::isTransitiveRelation(std::string relationType) {
    return relationType.find(TRANSITIVE_DELIMITER) != std::string::npos;
}

bool QueryEvaluator::isAbstractVariable(QueryParser::InputType var) {
    return ABSTRACT_REFERENCE_TYPES.find(var.entityType) != ABSTRACT_REFERENCE_TYPES.end();
}

std::list<std::string> QueryEvaluator::convertStringSetToList(std::unordered_set<std::string> input) {
    if (input.size() == 0)
        return std::list<std::string>();
    std::list<std::string> lst(input.begin(), input.end());
    return lst;
}

std::string QueryEvaluator::getReturnTypeForEntity(std::string designEntity) {
    std::unordered_map<std::string, std::string>::const_iterator designEntityToReturnType =
        DESIGN_ENTITY_TO_RETURN_TYPE.find(designEntity);
    if (designEntityToReturnType != DESIGN_ENTITY_TO_RETURN_TYPE.end())
        return designEntityToReturnType->second;
    return "No Return Type Found";
}

std::string QueryEvaluator::getReturnTypeForSuchThatClause(QueryParser::Clause clause) {
    QueryParser::InputType v1 = clause.firstVar;
    QueryParser::InputType v2 = clause.secondVar;
    std::string relationType = stripDelimiter(clause.clauseVariable);

    if (isRelationBetweenIdentifiers(clause))
        return "boolean";
    bool isV1Abstract = isAbstractVariable(v1);
    bool isV2Abstract = isAbstractVariable(v2);
    if (isV1Abstract && isV2Abstract)
        return "any";
    if (relationType == FOLLOWS || relationType == PARENT) {
        if (v1.entityType == v2.entityType && v2.entityType == WILDCARD)
            return "boolean";
        return STMT_NUM;
    } else if (relationType == USES || relationType == MODIFIES) {
        if ((v1.entityType == STMT_NUM && v2.entityType == VARIABLE) ||   // uses(6, v)
            (v1.entityType == IDENTIFIER && v2.entityType == VARIABLE) || // uses("functionX", v)
            (v1.entityType == STMT_NUM && v2.entityType == WILDCARD) ||   // uses(6, v)
            (v1.entityType == IDENTIFIER && v2.entityType == WILDCARD) || // uses("functionX", v)
            (v1.entityType == PROCEDURE && v2.entityType == IDENTIFIER)   // modifies(
        ) {
            return NAME;
        } else if ((v1.entityType == STMT || v1.entityType == PRINT || v1.entityType == CALL || v1.entityType == READ ||
                    v1.entityType == ASSIGN || v1.entityType == WHILE_STMT || v1.entityType == IF_STMT) &&
                   v2.entityType == IDENTIFIER) { // uses(s, "var")
            return STMT_NUM;
        }
    }
    return "No Return Type Found";
}

bool QueryEvaluator::isRelationBetweenIdentifiers(QueryParser::Clause clause) {
    QueryParser::InputType v1 = clause.firstVar;
    QueryParser::InputType v2 = clause.secondVar;
    std::string relationType = stripDelimiter(clause.clauseVariable);

    if (relationType == FOLLOWS || relationType == PARENT || relationType == AFFECTS || relationType == NEXT) {
        return v1.entityType == STMT_NUM && v1.entityType == v2.entityType || // e.g. follows(5,6)
               (v1.entityType == STMT_NUM && v2.entityType == WILDCARD) ||
               (v1.entityType == WILDCARD && v2.entityType == STMT_NUM) ||
               (v1.entityType == WILDCARD && v2.entityType == WILDCARD);
    } else if (relationType == USES || relationType == MODIFIES) {
        return (v1.entityType == STMT_NUM && v2.entityType == IDENTIFIER) ||    // e.g.uses(5, "varX")
               (v1.entityType == IDENTIFIER && v1.entityType == v2.entityType); // e.g. uses("functionA", "varX")
    } else if (relationType == CALLS) {
        return (v1.entityType == IDENTIFIER && v2.entityType == IDENTIFIER);
    } else if (relationType == WITH) {
        return (v1.entityType == IDENTIFIER && v2.entityType == IDENTIFIER) ||
               (v1.entityType == NUMBER && v2.entityType == NUMBER);

    } else {
        return false;
    }
}

/**
 * Checks if an expression contains a wildcard char at either the left or right (start or end)
 * @param  expression std::string to be validated
 * @param  position   "left" to check at the start and "right" to check at the end
 */
bool QueryEvaluator::hasWildCard(std::string expression, std::string position) {
    int len = expression.length();
    if (len == 0)
        return false;
    if (position == "left") {
        return (expression[0] == '_') == 1;
    } else if (position == "right") {
        return (expression[len - 1] == '_') == 1;
    }
    return false;
}

/**
 * Removes the wildcard char at both ends of the expression, if present.
 */
std::string QueryEvaluator::removeWildCard(std::string expression, bool hasLeftWildCard, bool hasRightWildCard) {
    int len = expression.length();
    std::string res;
    if (len == 0)
        return res;
    if (!hasLeftWildCard && !hasRightWildCard) {
        res = expression;
    } else if (hasLeftWildCard && hasRightWildCard) {
        res = expression.substr(1, len - 2);
    } else if (hasLeftWildCard && !hasRightWildCard) {
        res = expression.substr(1, len - 1);
    } else if (!hasLeftWildCard && hasRightWildCard) {
        res = expression.substr(0, len - 1);
    }

    // remove some redundant double quotes that were added lol
    if (res.find("\"") != std::string::npos) {
        res = res.substr(1, res.length() - 2);
    }
    return res;
}

std::vector<std::string> QueryEvaluator::getColsToMatch(QueryParser::Clause suchThatClause, QueryParser::Clause patternClause) {

    std::vector<std::string> colsToMatch;
    std::unordered_set<std::string> colsInSuchThat;
    std::unordered_set<std::string> colsInPattern;

    QueryParser::InputType st1 = suchThatClause.firstVar;
    QueryParser::InputType st2 = suchThatClause.secondVar;
    QueryParser::InputType lhs = patternClause.firstVar;
    std::string patternSynonym = patternClause.clauseVariable;

    if (isAbstractVariable(st1) && st1.entityType != WILDCARD) {
        colsInSuchThat.insert(st1.var);
    }

    if (isAbstractVariable(st2) && st2.entityType != WILDCARD) {
        colsInSuchThat.insert(st2.var);
    }

    colsInPattern.insert(patternSynonym);
    if (isAbstractVariable(lhs) && lhs.entityType != WILDCARD) {
        colsInPattern.insert(lhs.var);
    }

    for (const std::string &suchThatCol : colsInSuchThat) {
        if (colsInPattern.find(suchThatCol) != colsInPattern.end()) {
            colsToMatch.push_back(suchThatCol);
        }
    }
    return colsToMatch;
}

Table QueryEvaluator::handleAttributesInSelectedEntities(Table table, std::vector<QueryParser::InputType> selectedEntities) {
    PKB &pkb = PKB::Instance();
    std::unordered_map<std::string, std::string> synonymToEntityType;
    for (QueryParser::InputType selectedEntity : selectedEntities) {
        std::vector<std::string> synonymToAttr = parseAttributeInput(selectedEntity.var);
        if (synonymToAttr.size() == 1)
            continue;
        std::string attr = synonymToAttr[1];
        if (DESIGN_ENT_WITH_TWO_ATTRIBUTES.find(selectedEntity.entityType) != DESIGN_ENT_WITH_TWO_ATTRIBUTES.end() &&
            attr != "stmt#") {
            synonymToEntityType.insert({synonymToAttr[0], selectedEntity.entityType});
        } else {
            if (table.getIndexFromHeader(selectedEntity.var) == -1) {
                table.duplicateColumn(synonymToAttr[0], selectedEntity.var);
            }
        }
    }

    for (auto const &synonymEntPair : synonymToEntityType) {
        std::unordered_set<std::string> stmtNums = table.getValuesFromColumn(synonymEntPair.first);
        std::string entType = synonymEntPair.second;
        std::unordered_map<std::string, std::string> stmtNumToName;
        std::string attr = entType == CALL ? ".procName" : ".varName";
        if (entType == CALL) {
            for (std::string stmt : stmtNums) {
                stmtNumToName.insert({stmt, *pkb.getModifiesVariables(stoi(stmt)).begin()});
            }
        } else if (entType == READ) {
            for (std::string stmt : stmtNums) {
                stmtNumToName.insert({stmt, *pkb.getModifiesVariables(stoi(stmt)).begin()});
            }
        } else if (entType == PRINT) {
            for (std::string stmt : stmtNums) {
                stmtNumToName.insert({stmt, *pkb.getUsesVariables(stoi(stmt)).begin()});
            }
        }
        // std::cout << synonymEntPair.first + attr << std::endl;
        table.duplicateColumn(synonymEntPair.first, synonymEntPair.first + attr);
        table.replaceValuesForColumnWithMap(synonymEntPair.first + attr, stmtNumToName);
    }

    return table;
}

std::string QueryEvaluator::debugClause(QueryParser::Clause clause) {
    std::string relationType = stripDelimiter(clause.clauseVariable);
    bool isTransitive = isTransitiveRelation(clause.clauseVariable);
    QueryParser::InputType v1 = clause.firstVar;
    QueryParser::InputType v2 = clause.secondVar;

    std::string output = "clauseType: " + clause.clauseType + ", relationType: " + relationType +
                         ", isTransitive: " + (isTransitive ? "yes" : "no") + ", Input1 entityType: " + v1.entityType + "(" +
                         (isAbstractVariable(v1) ? "abstract" : "not abstract") + ")" + ", Input1 var" + v1.var +
                         ", Input2 entityType: " + v2.entityType + "(" + (isAbstractVariable(v2) ? "abstract" : "not abstract") +
                         ")" + ", Input2 var: " + v2.var;
    return output;
}
