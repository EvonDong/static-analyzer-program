#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <utility>
#include <unordered_set>
#include <list>
#include <climits>
#include <map>

class QueryParserHelper {
public:
    QueryParserHelper();

    static bool isValidValue(std::string s);

    static bool isValidName(std::string s);

    static std::string trim(const std::string& s);

    static bool basicCheck(const std::string& query);

    static std::vector<std::string> splitQuery(std::string query);

};