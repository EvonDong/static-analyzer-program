#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include "QueryParserHelper.h"

const std::string WHITESPACE = " \n\r\t\f\v";

QueryParserHelper::QueryParserHelper() = default;

bool QueryParserHelper::basicCheck(const std::string& query) {
    if ((query.length() <= 7) || (query.find("Select ") == std::string::npos)) {
        return false;
    } else {
        return true;
    }
}

std::vector<std::string> QueryParserHelper::splitQuery(std::string query) {
    std::vector<std::string> output;
    std::string delimiter = ";";
    size_t pos;
    std::string token;

    while ((pos = query.find(delimiter)) != std::string::npos) {
        token = QueryParserHelper::trim(query.substr(0, pos));
        output.push_back(token);
        query = QueryParserHelper::trim(query.erase(0, pos + delimiter.length()));
    }
    output.push_back(query);
    return output;
}

bool QueryParserHelper::isValidName(std::string s) {
    if (!isalpha(s[0])) {
        return false;
    }
    for (size_t i = 1; i < s.size(); i++) {
        if (!isalnum(s[i])){
            return false;
        }
    }
    return true;
}

bool QueryParserHelper::isValidValue(std::string s) {
    for (size_t i = 0; i < s.size(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

std::string leftTrim(const std::string& s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rightTrim(const std::string& s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string QueryParserHelper::trim(const std::string& s) {
    return rightTrim(leftTrim(s));
}