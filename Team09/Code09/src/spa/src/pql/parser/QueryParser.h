#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <utility>
#include <unordered_set>
#include <list>
#include <climits>
#include <map>

class QueryParser {
public:

    QueryParser();

    static std::list<std::string> parse(std::string query);
    struct InputType {
        std::string var;
        std::string entityType;

        inline bool operator == (const InputType& o) {
            return (var == o.var && entityType == o.entityType);
        }
    };

    struct Clause {
        std::string clauseType;
        std::string clauseVariable;
        InputType firstVar;
        InputType secondVar;

        inline bool operator == (const Clause& o) {
            return (clauseType == o.clauseType && clauseVariable == o.clauseVariable && firstVar == o.firstVar && secondVar == o.secondVar);
        }
    };

    static bool validateQuery(std::vector<std::string> queries);

    static std::pair<std::vector<QueryParser::InputType>, std::pair<std::vector<QueryParser::Clause>, std::pair<std::vector<QueryParser::Clause>, std::vector<QueryParser::Clause>>>> processQuery();

    static std::vector<Clause> processSuchThat(std::vector<std::string> suchThatClauses);

    static std::map<std::string, std::string> getDeclarationsMap();

    static void setDeclarationsMap(std::map<std::string, std::string>);

    static std::string getMainQuery();
    
    static void setMainQuery(std::string);

    static bool validateSuchThat(const Clause& clause);

    static std::vector<QueryParser::InputType> getDesiredVar();

    static std::vector<QueryParser::InputType> processDesiredVar(std::vector<std::string> selectVars);

    static std::vector<Clause> processPattern(std::vector<std::string> patternClauses);

    static bool validatePattern(const Clause& clause);

    static std::vector<Clause> parseWith(std::vector<std::string> withClauses);

    static QueryParser::Clause processWith(const Clause& clause);

    static bool isOfIntType(QueryParser::InputType input);

    static bool validIfPattern(size_t firstWildCardPos, size_t secondWildCardPos, size_t commaWildCardPos);

    static bool getIsBoolean();

    static void setIsBoolean(bool isBool);

    static std::string getErrorMsg();

    static void setErrorMsg(std::string q);
};


