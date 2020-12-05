#include <cstdio>
#include <string>
#include <vector>
#include <utility>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <limits.h>
#include <regex>

#include "QueryParser.h"
#include "QueryParserHelper.h"
#include "QueryEvaluator.h"
#include "PKB.h"

const std::string WHITESPACE = " \n\r\t\f\v";
const std::unordered_set<std::string> allEntityType = {"stmt", "procedure", "prog_line", "while", "if", "assign", "print", "variable", "constant", "read", "call", "stmtLst"};
const std::unordered_set<std::string> allRelation = {"Follows", "Follows*", "Parent", "Parent*", "Uses", "Modifies", "Calls", "Calls*", "Next", "Next*", "NextBip", "NextBip*", "Affects", "Affects*", "AffectsBip", "AffectsBip*"};
const std::unordered_set<std::string> allInputFollowNext = {"prog_line", "stmt", "while", "if", "read", "print", "assign", "call"};
const std::unordered_set<std::string> generalInput = {"prog_line", "stmt", "while", "if", "read", "print", "assign", "call"};
const std::unordered_set<std::string> allFirstInputParent = {"prog_line", "stmt", "while", "if"};
const std::unordered_set<std::string> allFirstInputModifies = {"procedure", "prog_line", "stmt", "read", "while", "if", "call", "assign"};  // print not included
const std::unordered_set<std::string> allFirstInputUses = {"procedure", "prog_line", "stmt", "print", "while", "if", "call", "assign"};   // read not included
const std::unordered_set<std::string> allSecondInputUses = {"variable", "wildcard"};
const std::unordered_set<std::string> allSecondInputModifies = {"variable", "wildcard"};
const std::unordered_set<std::string> allInputCall = {"procedure", "wildcard"};
const std::unordered_set<std::string> allInputAffects = {"prog_line", "stmt", "assign", "wildcard"};

const std::unordered_set<std::string> intEntityTypeRefStmtNum = {"stmt", "read", "print", "call", "while", "if", "assign"};
const std::unordered_set<std::string> intEntityTypeRefValue = {"constant"};
const std::unordered_set<std::string> intEntityTypeWithoutRef = {"prog_line"};
const std::unordered_set<std::string> nameEntityTypeRefVarName = {"variable", "read", "print"};
const std::unordered_set<std::string> nameEntityTypeRefProcName = {"procedure", "call"};

const std::string SUCH = "such ";
const std::string THAT = "that ";
const std::string SUCH_THAT = "such that ";
const std::string PATTERN = "pattern ";
const std::string WITH = "with ";
const std::string AND = "and ";

std::map<std::string, std::string> declarationsMap;
std::string mainQuery;
bool isBoolean = false;
std::string errorMsg;

QueryParser::QueryParser() = default;

std::map<std::string, std::string> QueryParser::getDeclarationsMap() {
    return declarationsMap;
}

void QueryParser::setDeclarationsMap(std::map<std::string, std::string> input) {
    declarationsMap = std::move(input);
}

std::string QueryParser::getMainQuery() {
    return mainQuery;
}

void QueryParser::setMainQuery(std::string q) {
    mainQuery = std::move(q);
}

bool QueryParser::getIsBoolean() {
    return isBoolean;
}

void QueryParser::setIsBoolean(bool isBool) {
    isBoolean = std::move(isBool);
}

std::string QueryParser::getErrorMsg() {
    return errorMsg;
}

void QueryParser::setErrorMsg(std::string q) {
    errorMsg = std::move(q);
}

std::list<std::string> QueryParser::parse(std::string query) {
    std::list<std::string> resultList;

    query = QueryParserHelper::trim(query);
    if (!QueryParserHelper::basicCheck(query)) {
        return resultList;
    }

    std::vector<std::string> declarationsAndClauses = QueryParserHelper::splitQuery(query);
    if (!(validateQuery(declarationsAndClauses))) {
        return resultList;
    }

    size_t suchPos = mainQuery.find(SUCH);
    size_t patternPos = mainQuery.find(PATTERN);
    size_t withPos = mainQuery.find(WITH);

    if (suchPos == std::string::npos && patternPos == std::string::npos && withPos == std::string::npos) {
        size_t pos = mainQuery.find_first_of(WHITESPACE);
        std::string var;

        if (pos != std::string::npos) {
            std::vector<Clause> suchThatClauses;
            std::vector<Clause> patternClauses;
            std::vector<Clause> withClauses;

            isBoolean = false;
            std::string errorMsg = "";

            try {
                std::vector<QueryParser::InputType> selectVarVec = getDesiredVar();
                if (isBoolean && selectVarVec.size() == 1) {
                    resultList.emplace_back("TRUE");
                    return resultList;
                }
                resultList = QueryEvaluator::evaluateQuery(selectVarVec, suchThatClauses, patternClauses, withClauses);
                PKB& pkb = PKB::Instance();
                pkb.clearCache();
                return resultList;
            } catch (std::exception& e) {
                return resultList;
            }
        } else {
            return resultList;
        }
    }

    std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> queryResult;
    std::vector<Clause> suchThatClauses;
    std::vector<Clause> patternClauses;
    std::vector<Clause> withClauses;
    InputType entity;
    std::vector<Clause> allClauses;
    std::vector<InputType> allEntities;

    try {
        isBoolean = false;
        std::string errorMsg = "";
        queryResult = processQuery();
        allEntities = queryResult.first;
        suchThatClauses = queryResult.second.first;
        patternClauses = queryResult.second.second.first;
        withClauses = queryResult.second.second.second;

        for (int i = 0; i < suchThatClauses.size(); i++) {
            allClauses.emplace_back(suchThatClauses[i]);
        }

        for (int i = 0; i < patternClauses.size(); i++) {
            allClauses.emplace_back(patternClauses[i]);
        }

        for (int i = 0; i < withClauses.size(); i++) {
            allClauses.emplace_back(withClauses[i]);
        }

        resultList = QueryEvaluator::evaluateQuery(allEntities, suchThatClauses, patternClauses, withClauses);
        PKB& pkb = PKB::Instance();
        pkb.clearCache();
    } catch (std::exception& e) {
        if (isBoolean && errorMsg != "") {
            resultList.emplace_back("FALSE");
        }
        return resultList;
    }
    return resultList;
}

bool QueryParser::validateQuery(std::vector<std::string> declarationsAndClauses) {
    declarationsMap.clear();

    if (declarationsAndClauses.size() < 1) {
        return false;
    }
    mainQuery = "";
    mainQuery = declarationsAndClauses.back();

    if (mainQuery.find("Select ") != 0) {
        return false;
    }

    std::string current;
    for (int i = 0; i < declarationsAndClauses.size() - 1; i++) {
        current = declarationsAndClauses[i];
        size_t pos = current.find_first_of(WHITESPACE);
        std::string entityType = QueryParserHelper::trim(current.substr(0, pos));
        std::string remaining = current.substr(pos);

        std::string delimiter = ",";
        size_t nextComma = remaining.find(delimiter);
        int begin, nextEnd;
        if (nextComma != std::string::npos) {

            begin = 0;
            nextEnd = remaining.find(delimiter);
            std::string nextVar;

            while (nextEnd != std::string::npos) {
                int len = nextEnd - begin;
                nextVar = QueryParserHelper::trim(remaining.substr(begin, len));
                begin = nextEnd + 1;
                declarationsMap.insert(make_pair(nextVar, entityType));
                nextEnd = remaining.find(delimiter, begin);
            }

            declarationsMap.insert(make_pair(QueryParserHelper::trim(remaining.substr(begin)), QueryParserHelper::trim(entityType)));
        }
        else {
            declarationsMap.insert(make_pair(QueryParserHelper::trim(remaining), QueryParserHelper::trim(entityType)));
        }
    }

    for (auto const& x: declarationsMap) {
        if (allEntityType.find(x.second) != allEntityType.end()) {
            return  QueryParserHelper::isValidName(x.first);
        } else {
            return false;
        }
    }
    return true;
}

std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> QueryParser::processQuery() {
    std::vector<QueryParser::InputType> desiredVar = getDesiredVar();

    std::vector<std::string> suchThatClauses;
    std::vector<std::string> patternClauses;
    std::vector<std::string> withClauses;

    std::string previousClause;
    std::string current = mainQuery;

    size_t suchPos;
    size_t patternPos;
    size_t withPos;
    size_t andPos;
    size_t nextPos = INT_MAX;
    size_t nextSecondPos = INT_MAX;

    do {
        suchPos = current.find(SUCH);
        patternPos = current.find(PATTERN);
        withPos = current.find(WITH);
        andPos = current.find(AND);
        nextSecondPos = INT_MAX;

        std::vector<int> positions;
        positions.emplace_back(suchPos);
        positions.emplace_back(patternPos);
        positions.emplace_back(withPos);
        positions.emplace_back(andPos);

        for (auto pos: positions) {
            if(pos == 0) {
                nextPos = pos;
            } else if (pos != std::string::npos && pos < nextSecondPos) {
                nextSecondPos = pos;
            }
        }

        if (nextPos != 0) {
            throw std::runtime_error("No valid clauses");
        }

        if (suchPos == 0) {
            std::string trimmedSuch = QueryParserHelper::trim(current.substr(4));
            size_t thatPos = trimmedSuch.find(THAT);
            if (thatPos == 0) {
                suchPos = current.find(SUCH, 4);
                if (suchPos != std::string::npos && suchPos < nextSecondPos) {
                    nextSecondPos = suchPos;
                }
                suchThatClauses.emplace_back(QueryParserHelper::trim(current.substr(current.length() - trimmedSuch.length() + 4, nextSecondPos - (current.length() - trimmedSuch.length() + 4))));
                previousClause = "such";
            } else {
                throw std::runtime_error("Invalid such format");
            }
        } else if (patternPos == 0) {
            patternPos = current.find(PATTERN, 6);
            if (patternPos != std::string::npos && patternPos < nextSecondPos) {
                nextSecondPos = patternPos;
            }
            if (nextSecondPos == INT_MAX) {
                patternClauses.emplace_back(QueryParserHelper::trim(current.substr(7, nextSecondPos)));
            } else {
                patternClauses.emplace_back(QueryParserHelper::trim(current.substr(7, nextSecondPos - 7)));
            }

            previousClause = "pattern";
        } else if (withPos == 0) {
            withPos = current.find(WITH, 4);
            if (withPos != std::string::npos && withPos < nextSecondPos) {
                nextSecondPos = withPos;
            }
            withClauses.emplace_back(QueryParserHelper::trim(current.substr(4, nextSecondPos - 4)));
            previousClause = "with";
        } else if (andPos == 0) {
            andPos = current.find(AND, 3);
            if (andPos != std::string::npos && andPos < nextSecondPos) {
                nextSecondPos = andPos;
            }
            if (previousClause == "such") {
                suchThatClauses.emplace_back(QueryParserHelper::trim(current.substr(3, nextSecondPos - 3)));
                previousClause = "such";
            } else if (previousClause == "pattern") {
                patternClauses.emplace_back(QueryParserHelper::trim(current.substr(3, nextSecondPos - 3)));
                previousClause = "pattern";
            } else if (previousClause == "with") {
                withClauses.emplace_back(QueryParserHelper::trim(current.substr(3,nextSecondPos - 3)));
                previousClause = "with";
            } else {
                throw std::runtime_error("Unknown 'and' clauses");
            }
        } else {
            throw std::runtime_error("Unknown clauses");
        }

        if (nextSecondPos != INT_MAX) {
            current = QueryParserHelper::trim(current.substr(nextSecondPos));
        } else {
            current = "";
        }

    } while (current.length() > 0 && nextPos != INT_MAX);

    std::vector<Clause> suchThatVector;
    std::vector<Clause> patternVector;
    std::vector<Clause> withVector;

    if (!(suchThatClauses.empty())) {
        suchThatVector = processSuchThat(suchThatClauses);
    }
    if (!(patternClauses.empty())) {
        patternVector = processPattern(patternClauses);
    }

    if (!withClauses.empty()) {
        withVector = parseWith(withClauses);
    }

    return make_pair(desiredVar, make_pair(suchThatVector, make_pair(patternVector, withVector)));
}

std::vector<QueryParser::InputType> QueryParser::getDesiredVar() {
    size_t pos1;
    size_t pos2 = 0;
    std::string var;
    std::vector<std::string> allVar;

    pos1 = mainQuery.find_first_of(WHITESPACE);
    if (pos1 != std::string::npos) {
        mainQuery = QueryParserHelper::trim(mainQuery.substr(pos1 + 1));
        pos2 = mainQuery.find_first_of(WHITESPACE);

        if (mainQuery.find("<") == 0 && mainQuery.find(">") != std::string::npos) { // for tuple
            size_t endPos = mainQuery.find(">") - 1;
            var = QueryParserHelper::trim(mainQuery.substr(1, endPos));

            size_t commaPos = var.find(",");
            while (commaPos != std::string::npos) {
                std::string splitVar = QueryParserHelper::trim(var.substr(0, commaPos));
                allVar.emplace_back(splitVar);
                var = QueryParserHelper::trim(var.substr(commaPos + 1));
                commaPos = var.find(",");
            }
            mainQuery = QueryParserHelper::trim(mainQuery.substr(endPos + 2));

            if ((mainQuery != "") && !((mainQuery.find(SUCH) == 0) ||
              (mainQuery.find(PATTERN) == 0) || (mainQuery.find(WITH) == 0))) {
                throw std::runtime_error("Invalid words after select clause");
            }
            allVar.emplace_back(var);
        } else if (pos2 != std::string::npos) {
            var = QueryParserHelper::trim(mainQuery.substr(0, pos2));
            mainQuery = QueryParserHelper::trim(mainQuery.erase(0, pos2 + 1));
            if ((mainQuery != "") && !((mainQuery.find(SUCH) == 0) ||
                (mainQuery.find(PATTERN) == 0) || (mainQuery.find(WITH) == 0))) {
                throw std::runtime_error("Invalid words after select clause");
            }
            allVar.emplace_back(var);
        } else {
            var = QueryParserHelper::trim(mainQuery);
            allVar.emplace_back(var);
        }
    } else {
        throw std::runtime_error("Cannot find any space");
    }

    std::vector<QueryParser::InputType> processedVar = processDesiredVar(allVar);
    return processedVar;
}

std::vector<QueryParser::InputType> QueryParser::processDesiredVar(std::vector<std::string> selectVars) {
    if (selectVars.size() == 0) {
        throw std::runtime_error("No select clauses");
    }

    std::vector<QueryParser::InputType> selectVarVector;
    for (int i = 0; i < selectVars.size(); i++) {
        std::string var = selectVars[i];
        QueryParser::InputType selectInput;

        size_t dotPos = var.find('.');
        if (dotPos != std::string::npos) {
            std::string synonym = var.substr(0, dotPos);
            std::string ref = var.substr(dotPos + 1);
            std::string type;

            if (declarationsMap.find(synonym) == declarationsMap.end()) {
                throw std::runtime_error("Select var not declared");
            } else {
                type = declarationsMap.at(synonym);
            }

            if (((intEntityTypeRefStmtNum.find(type) != intEntityTypeRefStmtNum.end()) && (ref == "stmt#"))
                || ((intEntityTypeRefValue.find(type) != intEntityTypeRefValue.end()) && (ref == "value"))
                || ((nameEntityTypeRefVarName.find(type) != nameEntityTypeRefVarName.end()) && (ref == "varName"))
                || ((nameEntityTypeRefProcName.find(type) != nameEntityTypeRefProcName.end()) && (ref == "procName"))) {
                selectInput.var = var;
                selectInput.entityType = type;
            } else {
                throw std::runtime_error("Select var invalid reference");
            }
        } else if (declarationsMap.find(var) != declarationsMap.end()) {
            selectInput.var = var;
            selectInput.entityType = declarationsMap.at(var);
        } else if (i == 0 && var == "BOOLEAN") {
            if (selectVars.size() == 1) {
                isBoolean = true;
                QueryParser::InputType selectInput;
                selectInput.var = "BOOLEAN";
                selectInput.entityType = "boolean";
                selectVarVector.emplace_back(selectInput);
                return selectVarVector;
            } else {
                throw std::runtime_error("Multiple select boolean variable");
            }
        } else {
            throw std::runtime_error("Select var invalid, not declared");
        }
        selectVarVector.emplace_back(selectInput);
    }

    return selectVarVector;
}

std::vector<QueryParser::Clause> QueryParser::processSuchThat(std::vector<std::string> suchThatClauses) {
    std::vector<QueryParser::Clause> output;

    for(int i = 0; i < suchThatClauses.size(); i++) {
        std::string exactClause = suchThatClauses[i];

        size_t frontBracPos, commaPos, backBracPos;
        frontBracPos = exactClause.find("(");
        commaPos = exactClause.find(",");
        backBracPos = exactClause.find(")");
        if ((frontBracPos == std::string::npos) ||
            (commaPos == std::string::npos) ||
            (backBracPos == std::string::npos)) {
            throw std::runtime_error("Cannot find all brackets, comma");
        }

        if (QueryParserHelper::trim(exactClause.substr(backBracPos + 1)).length() != 0) {
            throw std::runtime_error("Invalid/unknown words at back");
        }

        Clause resultClause;
        resultClause.clauseType = "suchThat";

        std::string relation = QueryParserHelper::trim(exactClause.substr(0, frontBracPos));
        if (allRelation.find(relation) != allRelation.end()) {
            resultClause.clauseVariable = relation;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            throw std::runtime_error("Unknown relation");
        }

        InputType input1;
        InputType input2;

        std::string firstInput = QueryParserHelper::trim(exactClause.substr(frontBracPos + 1, commaPos - frontBracPos - 1)); //relook here!!
        std::string secondInput = QueryParserHelper::trim(exactClause.substr(commaPos + 1, backBracPos - commaPos - 1));

        // First input
        size_t firstIndex, lastIndex;
        firstIndex = firstInput.find_first_of('"');
        lastIndex = firstInput.find_last_of('"');

        if ((firstIndex != std::string::npos) ||
            (lastIndex != std::string::npos)) {
            if (firstIndex == lastIndex) {
                throw std::runtime_error("Wrong format for first input");
            } else {
                input1.var = QueryParserHelper::trim(firstInput.substr(1, firstInput.length() - 2));
                input1.entityType = "identifier";
                resultClause.firstVar = input1;
            }
        } else {
            input1.var = firstInput;
            if ((QueryParserHelper::isValidName(firstInput)) && (declarationsMap.find(firstInput) != declarationsMap.end())) {
                input1.entityType = declarationsMap.at(firstInput);
            } else if (QueryParserHelper::isValidValue(firstInput)) {
                input1.entityType = "stmtNum";
            } else if (firstInput == "_") {
                input1.entityType = "wildcard";
            } else {
                throw std::runtime_error("Invalid first input");
            }
            resultClause.firstVar = input1;
        }

        // Second input
        size_t firstIndex2, lastIndex2;
        firstIndex2 = secondInput.find_first_of('"');
        lastIndex2 = secondInput.find_last_of('"');
        if ((firstIndex2 != std::string::npos) ||
            (lastIndex2 != std::string::npos)) {
            if (firstIndex2 == lastIndex2) {
                throw std::runtime_error("Wrong format for second input");
            } else {
                input2.var = QueryParserHelper::trim(secondInput.substr(1, secondInput.length() - 2));
                input2.entityType = "identifier";
                resultClause.secondVar = input2;
            }
        } else {
            input2.var = secondInput;
            if ((QueryParserHelper::isValidName(secondInput)) && (declarationsMap.find(secondInput) != declarationsMap.end())) {
                input2.entityType = declarationsMap.at(secondInput);
            } else if (QueryParserHelper::isValidValue(secondInput)) {
                input2.entityType = "stmtNum";
            } else if (secondInput == "_") {
                input2.entityType = "wildcard";
            } else {
                throw std::runtime_error("Invalid second input");
            }
            resultClause.secondVar = input2;
        }

        if (!(validateSuchThat(resultClause))) {
            throw std::runtime_error("Validation error for such that clause");
        }
        output.push_back(resultClause);
    }
    return output;
}

bool QueryParser::validateSuchThat(const Clause& clause) {
    std::string relation = clause.clauseVariable;
    InputType input1 = clause.firstVar;
    InputType input2 = clause.secondVar;

    if(relation == "Follows" || relation == "Follows*") {
        if((input1.entityType == "stmtNum" && input2.entityType == "stmtNum") &&
           (stoi(input1.var) >= stoi(input2.var) || stoi(input1.var) < 0)) {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        } else if ((input1.entityType == "wildcard" || (allInputFollowNext.find(input1.entityType) != allInputFollowNext.end()) ||
                    (input1.entityType == "stmtNum")) &&
                   (input2.entityType == "wildcard" || (allInputFollowNext.find(input2.entityType) != allInputFollowNext.end() ||
                    (input2.entityType == "stmtNum")))) {
            if((input1.entityType != "wildcard" && input2.entityType != "wildcard") && (input1.var == input2.var)) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                return false;
            }
            if((input1.entityType == "stmtNum" && stoi(input1.var) < 0) || (input2.entityType == "stmtNum" && stoi(input2.var) < 0)) {
                return false;
            }
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else if(relation == "Parent" || relation == "Parent*") {
        if((input1.entityType == "stmtNum" && input2.entityType == "stmtNum") &&
           (stoi(input1.var) > stoi(input2.var) || stoi(input1.var) < 0)) {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        } else if ((input1.entityType == "wildcard" || (allFirstInputParent.find(input1.entityType) != allFirstInputParent.end()) ||
                     (input1.entityType == "stmtNum")) &&
                    (input2.entityType == "wildcard" || (input2.entityType == "stmtNum") ||
                    (generalInput.find(input2.entityType) != generalInput.end()))) {
            if((input1.entityType != "wildcard" && input2.entityType != "wildcard") && (input1.var == input2.var)) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                return false;
            }
            if((input1.entityType == "stmtNum" && stoi(input1.var) < 0) || (input2.entityType == "stmtNum" && stoi(input2.var) < 0)) {
                return false;
            }
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else if (relation == "Uses") {
        if(input2.entityType == "stmtNum" || (input1.entityType == "stmtNum" && stoi(input1.var) < 0)) {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        } else if ((allFirstInputUses.find(input1.entityType) != allFirstInputUses.end() || (input1.entityType == "stmtNum")
            || (input1.entityType == "identifier" && QueryParserHelper::isValidName(input1.var)))
            &&
            ((input2.entityType == "identifier" && QueryParserHelper::isValidName(input2.var)) || (allSecondInputUses.find(input2.entityType) != allSecondInputUses.end()))) {
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else if (relation == "Modifies") {
        if(input2.entityType == "stmtNum" || (input1.entityType == "stmtNum" && stoi(input1.var) < 0)) {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        } else if ((allFirstInputModifies.find(input1.entityType) != allFirstInputModifies.end() || (input1.entityType == "stmtNum")
            || (input1.entityType == "identifier" && (QueryParserHelper::isValidName(input1.var))))
            &&
            ((input2.entityType == "identifier"  && QueryParserHelper::isValidName(input2.var))
            || (allSecondInputModifies.find(input2.entityType) != allSecondInputModifies.end()))) {
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } if(relation == "Next" || relation == "Next*" || relation == "NextBip" || relation == "NextBip*") {
        if((input1.entityType == "stmtNum" && input2.entityType == "stmtNum") &&
           (stoi(input1.var) < 0 || stoi(input2.var) < 0)) {
            return false;
        } else if ((input1.entityType == "wildcard" || (allInputFollowNext.find(input1.entityType) != allInputFollowNext.end()) ||
                    (input1.entityType == "stmtNum")) &&
                   (input2.entityType == "wildcard" || (allInputFollowNext.find(input2.entityType) != allInputFollowNext.end() ||
                   (input2.entityType == "stmtNum")))) {
            if((input1.entityType == "stmtNum" && stoi(input1.var) < 0) || (input2.entityType == "stmtNum" && stoi(input2.var) < 0)) {
                return false;
            }
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else if (relation == "Calls" || relation == "Calls*") {
        if (((allInputCall.find(input1.entityType) != allInputCall.end())
            || (input1.entityType == "identifier" && (QueryParserHelper::isValidName(input1.var))))
            &&
            ((allInputCall.find(input2.entityType) != allInputCall.end())
            || (input2.entityType == "identifier" && (QueryParserHelper::isValidName(input2.var))))) {
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else if(relation == "Affects" || relation == "Affects*" || relation == "AffectsBip" || relation == "AffectsBip*") {
        if((input1.entityType == "stmtNum" && input2.entityType == "stmtNum") &&
           (stoi(input1.var) < 0 || stoi(input2.var) < 0)) {
            return false;
        } else if (((allInputAffects.find(input1.entityType) != allInputAffects.end()) || (input1.entityType == "stmtNum")) &&
                   ((allInputAffects.find(input2.entityType) != allInputAffects.end()) ||
                                                        (input2.entityType == "stmtNum"))) {
            if((input1.entityType == "stmtNum" && stoi(input1.var) < 0) || (input2.entityType == "stmtNum" && stoi(input2.var) < 0)) {
                return false;
            }
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        return false;
    }
}

std::vector<QueryParser::Clause> QueryParser::processPattern(std::vector<std::string> patternClauses) {
    std::vector<QueryParser::Clause> output;

    for(int i = 0; i < patternClauses.size(); i++) {
        std::string exactClause = QueryParserHelper::trim(patternClauses[i]);

        size_t frontBracPos, commaPos, backBracPos;
        frontBracPos = exactClause.find("(");
        commaPos = exactClause.find(",");
        backBracPos = exactClause.find_last_of(")");
        if ((frontBracPos == std::string::npos) || (commaPos == std::string::npos) ||
            (backBracPos == std::string::npos)) {
            throw std::runtime_error("Cannot find all comma, brackets");
        }

        if (QueryParserHelper::trim(exactClause.substr(backBracPos + 1)).length() != 0) {
            throw std::runtime_error("Invalid/unknown words at back");
        }

        Clause resultClause;

        std::string relation = QueryParserHelper::trim(exactClause.substr(0, frontBracPos));
        if (declarationsMap.find(relation) != declarationsMap.end()) {
            std::string stmtType = declarationsMap.at(relation);
            if (stmtType == "assign") {
                resultClause.clauseType = "assignPattern";
            } else if (stmtType == "if") {
                resultClause.clauseType = "ifPattern";
            } else if (stmtType == "while") {
                resultClause.clauseType = "whilePattern";
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid desired var type for pattern clause");
            }
            resultClause.clauseVariable = relation;
        } else {
            throw std::runtime_error("Cannot find desired var for pattern clause");
        }

        InputType input1;
        InputType input2;
        // First input
        std::string firstInput = QueryParserHelper::trim(exactClause.substr(frontBracPos + 1, commaPos - frontBracPos - 1)); //relook here!!
        std::string secondInput = QueryParserHelper::trim(exactClause.substr(commaPos + 1, backBracPos - commaPos - 1));
        size_t firstIndex;
        firstIndex = firstInput.find_first_of('"');
        if (firstIndex == 0) {
            input1.var = QueryParserHelper::trim(firstInput.substr(1, firstInput.length() - 2));
            input1.entityType = "identifier";
            resultClause.firstVar = input1;
        } else {
            input1.var = QueryParserHelper::trim(firstInput);
            if (QueryParserHelper::isValidName(input1.var)) {
                if (declarationsMap.find(input1.var) != declarationsMap.end()) {
                    input1.entityType = declarationsMap.at(firstInput);
                } else {
                    if (isBoolean) {
                        errorMsg = "semantic error";
                    }
                    throw std::runtime_error("Undeclared first input");
                }
            } else if (input1.var == "_") {
                input1.entityType = "wildcard";
            } else {
                throw std::runtime_error("Unknown/invalid first input");
            }
        }
        resultClause.firstVar = input1;

        // Second input
        firstIndex = secondInput.find_first_of('"');
        input2.var = QueryParserHelper::trim(secondInput);
        size_t firstWildCardPos = input2.var.find_first_of('_');
        size_t secondWildCardPos = input2.var.find_last_of('_');
        size_t commaWildCardPos = input2.var.find_first_of(',');
        int len = input2.var.length();
        if ((firstIndex == 0 || ((QueryParserHelper::trim(input2.var.substr(1))[0] == '"') && input2.var[0] == '_'))) {
            // Remove first "
            if (firstIndex == 0) {
                input2.var = QueryParserHelper::trim(input2.var.substr(1));
            } else if ((QueryParserHelper::trim(input2.var.substr(1))[0] == '"') && input2.var[0] == '_') {
                input2.var = "_" + QueryParserHelper::trim(input2.var.substr(firstIndex + 1));
            }
            // Remove last "
            len = input2.var.length();
            if (input2.var[len - 1] == '"') {
                input2.var = QueryParserHelper::trim(input2.var.substr(0, len - 1));
            } else if (input2.var[len - 1] == '_' && QueryParserHelper::trim(input2.var.substr(0, len - 1)).back() == '"') {
                size_t lastIndex = input2.var.find_last_of('"');
                input2.var = QueryParserHelper::trim(input2.var.substr(0, lastIndex)) + "_";
            } else {
                throw std::runtime_error("Missing closing double quotes");
            }

            std::regex re("\\s{2,}");
            std::string fmt = " ";
            input2.var = regex_replace(input2.var, re, fmt);
            input2.entityType = "expression";
            resultClause.secondVar = input2;
        } else if (input2.var == "_" || input2.var == "_,_") {
                input2.entityType = "wildcard";
        } else if (validIfPattern(firstWildCardPos, secondWildCardPos, commaWildCardPos)) {
            input2.entityType = "wildcard";
            input2.var = "_,_";
        } else {
            throw std::runtime_error("Unknown/invalid second input");
        }
        resultClause.secondVar = input2;

        if (!(validatePattern(resultClause))) {
            throw std::runtime_error("Validation error for pattern clause");
        }
        output.push_back(resultClause);
    }
    return output;
}

bool QueryParser::validatePattern(const Clause& clause) {
    std::string var = clause.clauseVariable;
    InputType input1 = clause.firstVar;
    InputType input2 = clause.secondVar;

    std::string stmtType;
    if(declarationsMap.find(var) != declarationsMap.end()) {
        stmtType = declarationsMap.at(var);
    } else {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        return false;
    }

    // Validation for first input
    if ((input1.var == "_" && input1.entityType == "wildcard") ||
            (input1.entityType == "identifier" && QueryParserHelper::isValidName(input1.var)) ||
        ((declarationsMap.find(input1.var) != declarationsMap.end()) && declarationsMap.at(input1.var) == "variable")) {
    } else {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        return false;
    }

    // Validation for 2nd input
    if (stmtType == "assign") {
        if (input2.var == "_") {
            return true;
        } else if ((input2.entityType == "expression") || (input2.var == "_" && input2.entityType == "wildcard")) {
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else if (stmtType == "if") { //input2.var.find(",") != std::string::npos
        size_t firstWildCardPos = input2.var.find_first_of('_');
        size_t secondWildCardPos = input2.var.find_last_of('_');
        size_t commaWildCardPos = input2.var.find_first_of(',');
        if (input2.var == "_,_" || validIfPattern(firstWildCardPos, secondWildCardPos, commaWildCardPos)) {
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } if (stmtType == "while") {
        if (input2.var == "_") {
            return true;
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            return false;
        }
    } else {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        return false;
    }
}

bool QueryParser::validIfPattern(size_t firstWildCardPos, size_t secondWildCardPos, size_t commaWildCardPos) {
    if (firstWildCardPos == std::string::npos || secondWildCardPos == std::string::npos || commaWildCardPos == std::string::npos) {
        return false;
    } else if ((firstWildCardPos < commaWildCardPos) && (secondWildCardPos > commaWildCardPos)) {
        return true;
    } else {
        return false;
    }
}

std::vector<QueryParser::Clause> QueryParser::parseWith(std::vector<std::string> withClauses) {
    std::vector<QueryParser::Clause> output;

    for (int i = 0; i < withClauses.size(); i++) {
        std::string exactClause = QueryParserHelper::trim(withClauses[i]);

        size_t equalPos;
        equalPos = exactClause.find("=");
        if (equalPos == std::string::npos) {
            throw std::runtime_error("Cannot find an equal sign");
        }

        Clause resultClause;
        resultClause.clauseType = "with";
        resultClause.clauseVariable = "equal";

        std::string leftInput = QueryParserHelper::trim(exactClause.substr(0, equalPos));
        std::string rightInput = QueryParserHelper::trim(exactClause.substr(equalPos + 1));

        if (leftInput == "" || rightInput == "") {
            throw std::runtime_error("Missing inputs");
        }

        InputType input1;
        input1.var = leftInput;
        resultClause.firstVar = input1;

        InputType input2;
        input2.var = rightInput;
        resultClause.secondVar = input2;

        Clause newClause = processWith(resultClause);
        output.push_back(newClause);
    }
    return output;
}

QueryParser::Clause QueryParser::processWith(const Clause& clause) {
    std::string assignmentVar = clause.clauseType;
    InputType input1 = clause.firstVar;
    std::string input1Var = input1.var;

    InputType input2 = clause.secondVar;
    std::string input2Var = input2.var;

    Clause newClause;
    InputType newInput1;
    InputType newInput2;

    newInput1.var = input1Var;
    newInput2.var = input2Var;

    // For first input
    if (QueryParserHelper::isValidValue(input1Var)) {
        newInput1.entityType = "num";
    } else if (QueryParserHelper::isValidName(input1Var)) {
        if (declarationsMap.find(input1Var) != declarationsMap.end()) {
            newInput1.entityType = declarationsMap.at(input1Var);
            if (intEntityTypeWithoutRef.find(newInput1.entityType) == intEntityTypeWithoutRef.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("1st input is not of a valid type");
            }
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            throw std::runtime_error("Invalid name for 1st input");
        }
    } else if (input1Var[0] == '"') {
        if (input1Var[input1Var.length() - 1] != '"') {
            throw std::runtime_error("Invalid format for 1st input");
        }
        newInput1.var = QueryParserHelper::trim(input1Var.substr(1, input1Var.length() - 2));
        if (!QueryParserHelper::isValidName(newInput1.var)) {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            throw std::runtime_error("Invalid identifier name for 1st input");
        }
        newInput1.entityType = "identifier";
    } else if ((input1Var.find(".value") != std::string::npos) || (input1Var.find(".stmt#") != std::string::npos)
        || (input1Var.find(".procName") != std::string::npos) || (input1Var.find(".varName") != std::string::npos)) {
        size_t valPos = input1Var.find(".value");
        size_t stmtPos = input1Var.find(".stmt#");
        size_t procPos = input1Var.find(".procName");
        size_t varPos = input1Var.find(".varName");

        if (valPos != std::string::npos) {
            std::string exactVar = input1Var.substr(0, valPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput1.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 1st input");
            }
            if (intEntityTypeRefValue.find(newInput1.entityType) == intEntityTypeRefValue.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 1st input");
            }
        } else if (stmtPos != std::string::npos) {
            std::string exactVar = input1Var.substr(0, stmtPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput1.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 1st input");
            }
            if (intEntityTypeRefStmtNum.find(newInput1.entityType) == intEntityTypeRefStmtNum.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 1st input");
            }
        } else if (procPos != std::string::npos) {
            std::string exactVar = input1Var.substr(0, procPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput1.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 1st input");
            }
            if (nameEntityTypeRefProcName.find(newInput1.entityType) == nameEntityTypeRefProcName.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 1st input");
            }
        } else if (varPos != std::string::npos) {
            std::string exactVar = input1Var.substr(0, varPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput1.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 1st input");
            }
            if (nameEntityTypeRefVarName.find(newInput1.entityType) == nameEntityTypeRefVarName.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 1st input");
            }
        }
    } else {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        throw std::runtime_error("Invalid 1st input");
    }


    // For second input
    if (QueryParserHelper::isValidValue(input2Var)) {
        newInput2.entityType = "num";
    } else if (QueryParserHelper::isValidName(input2Var)) {
        if (declarationsMap.find(input2Var) != declarationsMap.end()) {
            newInput2.entityType = declarationsMap.at(input2Var);
            if (intEntityTypeWithoutRef.find(newInput2.entityType) == intEntityTypeWithoutRef.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 2nd input");
            }
        } else {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            throw std::runtime_error("Invalid name for 2nd input");
        }
    } else if (input2Var[0] == '"') {
        if (input2Var[input2Var.length() - 1] != '"') {
            throw std::runtime_error("Invalid format for 1st input" + std::to_string(input2Var.length() - 1));
        }
        newInput2.var = QueryParserHelper::trim(input2Var.substr(1, input2Var.length() - 2));
        if (!QueryParserHelper::isValidName(newInput2.var)) {
            if (isBoolean) {
                errorMsg = "semantic error";
            }
            throw std::runtime_error("Invalid identifier name for 2nd input");
        }
        newInput2.entityType = "identifier";
    } else if ((input2Var.find(".value") != std::string::npos) || (input2Var.find(".stmt#") != std::string::npos)
               || (input2Var.find(".procName") != std::string::npos) || (input2Var.find(".varName") != std::string::npos)) {
        size_t valPos = input2Var.find(".value");
        size_t stmtPos = input2Var.find(".stmt#");
        size_t procPos = input2Var.find(".procName");
        size_t varPos = input2Var.find(".varName");

        if (valPos != std::string::npos) {
            std::string exactVar = input2Var.substr(0, valPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput2.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 2nd input");
            }
            if (intEntityTypeRefValue.find(newInput2.entityType) == intEntityTypeRefValue.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 2nd input");
            }
        } else if (stmtPos != std::string::npos) {
            std::string exactVar = input2Var.substr(0, stmtPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput2.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 2nd input");
            }
            if (intEntityTypeRefStmtNum.find(newInput2.entityType) == intEntityTypeRefStmtNum.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 2nd input");
            }
        } else if (procPos != std::string::npos) {
            std::string exactVar = input2Var.substr(0, procPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput2.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 2nd input");
            }
            if (nameEntityTypeRefProcName.find(newInput2.entityType) == nameEntityTypeRefProcName.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 2nd input");
            }
        } else if (varPos != std::string::npos) {
            std::string exactVar = input2Var.substr(0, varPos);
            if(declarationsMap.find(exactVar) != declarationsMap.end()) {
                newInput2.entityType = declarationsMap.at(exactVar);
            } else {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid entity type for 2nd input");
            }
            if (nameEntityTypeRefVarName.find(newInput2.entityType) == nameEntityTypeRefVarName.end()) {
                if (isBoolean) {
                    errorMsg = "semantic error";
                }
                throw std::runtime_error("Invalid reference for 2nd input");
            }
        }
    } else {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        throw std::runtime_error("Invalid 2nd input");
    }

    if ((!isOfIntType(newInput1) && isOfIntType(newInput2)) || (isOfIntType(newInput1) && !isOfIntType(newInput2))) {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        throw std::runtime_error("Non-matching input types");
    }

    if (newInput1.entityType == "num" && newInput2.entityType == "num" && (newInput1.var != newInput2.var)) {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        throw std::runtime_error("Wrong with clause");
    }

    if (newInput1.entityType == "identifier" && newInput2.entityType == "identifier" && (newInput1.var != newInput2.var)) {
        if (isBoolean) {
            errorMsg = "semantic error";
        }
        throw std::runtime_error("Wrong with clause");
    }

    newClause.firstVar = newInput1;
    newClause.secondVar = newInput2;
    newClause.clauseType = "with";
    newClause.clauseVariable = "equal";

    return newClause;
}

bool QueryParser::isOfIntType(QueryParser::InputType input) {
    if ((input.var.find(".procName") != std::string::npos) || (input.var.find(".varName") != std::string::npos)
        || input.entityType == "identifier") {
        return false;
    }
    return true;
}
