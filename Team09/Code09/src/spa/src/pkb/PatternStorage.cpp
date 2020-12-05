#include "PatternStorage.h"

PatternStorage *PatternStorage::inst_ = NULL;

PatternStorage *PatternStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new PatternStorage();
    }
    return inst_;
}

void PatternStorage::setStmtToPostfix(int stmtLine, std::string lhsVar, std::string pattern) {
    checkPatternValidity(lhsVar);
    std::string patternPostfix;
    try {
        patternPostfix = convertInfixToPostfix(pattern);
    } catch (std::exception &e) {
        throw e;
    }
    stmtNumsToPostfixMap[stmtLine] = patternPostfix;
    stmtNumsToVarAndPostfixMap[stmtLine] = std::pair<std::string, std::string>(lhsVar, patternPostfix);
    if (lhsVarToStmtsMap.find(lhsVar) != lhsVarToStmtsMap.end()) {
        lhsVarToStmtsMap.at(lhsVar).emplace(stmtLine);
    } else {
        std::unordered_set<int> stmtList;
        stmtList.emplace(stmtLine);
        lhsVarToStmtsMap.emplace(lhsVar, stmtList);
    }
}

std::unordered_set<int> PatternStorage::getPatternMatchStatement(std::string pattern, bool noLeft, bool noRight) {
    std::string inputPostfix;
    try {
        inputPostfix = convertInfixToPostfix(pattern);
    } catch (std::exception &e) {
        throw e;
    }
    std::unordered_set<int> stmtList;
    int length = inputPostfix.size();

    for (auto &itr : stmtNumsToPostfixMap) {
        if (noLeft && noRight) {
            if (itr.second.find(inputPostfix) != std::string::npos && itr.second.find(inputPostfix) == 0 &&
                length == itr.second.size()) {
                stmtList.emplace(itr.first);
            }
        } else if (noLeft) {
            if (itr.second.find(inputPostfix) != std::string::npos && itr.second.find(inputPostfix) == 0) {
                stmtList.emplace(itr.first);
            }
        } else if (noRight) {
            std::regex endsWithRegex("((" + inputPostfix + ")[-+*/%]*)$");
            if (regex_search(itr.second, endsWithRegex)) {
                stmtList.emplace(itr.first);
            }
        } else {
            if (itr.second.find(inputPostfix) != std::string::npos) {
                stmtList.emplace(itr.first);
            }
        }
    }
    return stmtList;
}

std::unordered_set<int> PatternStorage::getPatternAndVarMatchStatement(std::string lhsVar, std::string pattern, bool noLeft,
                                                                       bool noRight) {
    checkPatternValidity(lhsVar);
    std::string inputPostfix;
    try {
        inputPostfix = convertInfixToPostfix(pattern); // convert to postfix
    } catch (std::exception &e) {
        throw e;
    }

    if (lhsVarToStmtsMap.find(lhsVar) != lhsVarToStmtsMap.end()) {
        return checkAndReturnMatchingStatements(lhsVarToStmtsMap.at(lhsVar), inputPostfix, noLeft, noRight);
    }
    std::unordered_set<int> res;
    return res;
}

std::unordered_set<int> PatternStorage::checkAndReturnMatchingStatements(std::unordered_set<int> stmtList, std::string pattern,
                                                                         bool noLeft, bool noRight) {
    std::unordered_set<int> res;
    int length = pattern.size();
    for (auto &itr : stmtList) {
        if (noLeft && noRight) {
            if (stmtNumsToPostfixMap.find(itr) != stmtNumsToPostfixMap.end()) {
                if (stmtNumsToPostfixMap.at(itr).find(pattern) != std::string::npos &&
                    stmtNumsToPostfixMap.at(itr).find(pattern) == 0 && length == stmtNumsToPostfixMap.at(itr).size()) {
                    res.emplace(itr);
                }
            }
        } else if (noLeft) {
            if (stmtNumsToPostfixMap.at(itr).find(pattern) != std::string::npos &&
                stmtNumsToPostfixMap.at(itr).find(pattern) == 0) {
                res.emplace(itr);
            }
        } else if (noRight) {
            // when checking noRight is to check no more letters on the right but can have symbols
            if (pattern.size() == 2 && stmtNumsToPostfixMap.at(itr).find(pattern) != std::string::npos) {
                int i = stmtNumsToPostfixMap.at(itr).find(pattern) + 1;
                int allSymbols = true;
                for (; static_cast<size_t>(i) < stmtNumsToPostfixMap.at(itr).size(); i++) {
                    if (isalpha(stmtNumsToPostfixMap.at(itr)[i])) {
                        allSymbols = false;
                        break;
                    }
                }
                if (allSymbols) {
                    res.emplace(itr);
                }
            }
            if (pattern.size() != 1 && stmtNumsToPostfixMap.at(itr).find(pattern) != std::string::npos &&
                stmtNumsToPostfixMap.at(itr).find(pattern) + length == stmtNumsToPostfixMap.at(itr).size()) {
                res.emplace(itr);
            }
        } else {
            if (stmtNumsToPostfixMap.at(itr).find(pattern) != std::string::npos) {
                res.emplace(itr);
            }
        }
    }
    return res;
}

int PatternStorage::preced(char ch) {
    if (ch == '+' || ch == '-') {
        return 1; // Precedence of + or - is 1
    } else if (ch == '*' || ch == '/' || ch == '%') {
        return 2; // Precedence of * or / is 2
    } else {
        return 0;
    }
}

// do pattern input validation
std::string PatternStorage::convertInfixToPostfix(std::string infix) {
    checkPatternValidity(infix);
    try {
        std::stack<char> stk;
        stk.push('#');            // add some extra character to avoid underflow
        std::string postfix = ""; // initially the postfix string is empty
        std::string::iterator it;
        bool isStartOfVarConst = true;
        for (it = infix.begin(); it != infix.end(); it++) {
            if (isspace(*it)) {
                // skip space characters
                if (postfix.size() >= 1 && isalnum(postfix.back())) { // add space to indicate end of var for postfix
                    postfix += "# ";
                    isStartOfVarConst = true;
                }
                continue;
            } else if (isalnum(char(*it))) {
                if (isStartOfVarConst) { // add space to beginning of var
                    postfix += "#";
                    isStartOfVarConst = false;
                }
                postfix += *it; // add to postfix when character is letter or number
            } else if (*it == '(') {
                if (postfix.size() >= 1 && isalnum(postfix.back())) { // add space to indicate end of var for postfix
                    postfix += "# ";
                    isStartOfVarConst = true;
                }
                stk.push('(');
            } else if (*it == ')') {
                bool foundMatchingBracket = false;
                if (postfix.size() >= 1 && isalnum(postfix.back())) { // add space to indicate end of var for postfix
                    postfix += "# ";
                    isStartOfVarConst = true;
                }
                while (!stk.empty() && stk.top() != '#') {
                    if (stk.top() == '(') {
                        foundMatchingBracket = true;
                        stk.pop();
                        break;
                    } else {
                        postfix += stk.top(); // store and pop until ( has found
                        stk.pop();
                    }
                }
                if (!foundMatchingBracket) { // invalid pattern with no matching opening bracket
                    throw std::runtime_error("Invalid pattern");
                }
            } else if (*it == '+' || *it == '-' || *it == '/' || *it == '*' || *it == '%') {
                if (postfix.size() >= 1 && isalnum(postfix.back())) {
                    postfix += "# ";
                    isStartOfVarConst = true;
                }
                if (!stk.empty() && preced(*it) > preced(stk.top())) {
                    stk.push(*it); // push if precedence is high
                } else {
                    while (!stk.empty() && stk.top() != '#' && preced(*it) <= preced(stk.top())) {
                        postfix += stk.top(); // store and pop until higher precedence is found
                        stk.pop();
                    }
                    stk.push(*it);
                }
            } else {
                throw std::runtime_error("Invalid pattern.");
            }
        }

        if (postfix.size() >= 1 && isalnum(postfix.back())) {
            postfix += "# ";
        }

        while (!stk.empty() && stk.top() != '#') {
            postfix += stk.top(); // store and pop until stack is not empty.
            stk.pop();
        }
        checkPostfixValidity(postfix); // check opening brackets have matching closing brackets
        return postfix;
    } catch (std::exception &e) {
        throw std::runtime_error("Invalid pattern.");
    }
}

void PatternStorage::checkPostfixValidity(std::string postfix) {
    int validityCounter = 0;
    std::string::iterator it;
    for (it = postfix.begin(); it != postfix.end(); it++) {
        if (char(*it) == 32) {
            validityCounter++;
        } else if (*it == '+' || *it == '-' || *it == '/' || *it == '*' || *it == '%') {
            validityCounter -= 2;
            if (validityCounter < 0) {
                throw std::runtime_error("Invalid pattern.");
            }
            validityCounter++;
        } else if (char(*it) != '#' && !isalnum(char(*it))) {
            throw std::runtime_error("Invalid pattern.");
        }
    }
    if (validityCounter != 1 && postfix != "") {
        throw std::runtime_error("Invalid pattern.");
    }
}

void PatternStorage::checkPatternValidity(std::string pattern) {
    std::string word = "";
    bool isNotCheckedAgainstRegex = true;
    for (auto x : pattern) {
        if (x == '+' || x == '-' || x == '/' || x == '*' || x == ')' || x == '%') {
            isNotCheckedAgainstRegex = false;
            //^[\\s]*([\\w]+)[\\s]*
            std::regex reg1("^[\\s]*([a-zA-Z]+)([\\w]*)[\\s]*"); // LETTER(LETTER|DIGIT)
            std::regex reg2("^[\\s]*[\\d]+[\\s]*");              // all digits
            if (!word.empty() && !regex_match(word, reg2) && !regex_match(word, reg1)) {
                throw std::runtime_error("Invalid pattern variable naming convention.");
            }
            word = "";
        } else if (x == '(') {
            continue;
        } else if (!isspace(x)) {
            word += x;
        }
    }

    if (isNotCheckedAgainstRegex) {
        std::regex reg1("^[\\s]*([a-zA-Z]+)([\\w]*)[\\s]*"); // LETTER(LETTER|DIGIT)
        std::regex reg2("^[\\s]*[\\d]+[\\s]*");              // all digits
        std::regex reg3("^[\\s]*[0]+[\\d]+[\\s]*");
        if (!word.empty() && !regex_match(word, reg2) && !regex_match(word, reg1)) {
            throw std::runtime_error("Invalid pattern variable naming convention.");
        }
        if (regex_match(word, reg3)) {
            throw std::runtime_error("Invalid pattern variable naming convention.");
        }
    }
}

void PatternStorage::setConditionPattern(int stmtNum, std::unordered_set<std::string> varsUsed, std::string designEnt) {
    if (designEnt == IF_STMT) {
        for (auto &itr : varsUsed) {
            if (varToIfStmtsMap.find(itr) != varToIfStmtsMap.end()) {
                varToIfStmtsMap.at(itr).emplace(stmtNum);
            } else {
                std::unordered_set<int> stmtNums;
                stmtNums.emplace(stmtNum);
                varToIfStmtsMap.emplace(itr, stmtNums);
            }
            ifStmtToVarPairSet.emplace(std::pair<int, std::string>{stmtNum, itr});
        }
        ifStmtToVarMap.emplace(stmtNum, varsUsed);
    } else if (designEnt == WHILE_STMT) {
        for (auto &itr : varsUsed) {
            if (varToWhileStmtsMap.find(itr) != varToWhileStmtsMap.end()) {
                varToWhileStmtsMap.at(itr).emplace(stmtNum);
            } else {
                std::unordered_set<int> stmtNums;
                stmtNums.emplace(stmtNum);
                varToWhileStmtsMap.emplace(itr, stmtNums);
            }
            whileStmtToVarPairSet.emplace(std::pair<int, std::string>{stmtNum, itr});
        }
        whileStmtToVarMap.emplace(stmtNum, varsUsed);
    }
}

std::unordered_set<int> PatternStorage::getVarMatchConditionStatements(std::string var, std::string designEnt) {
    checkPatternValidity(var);
    if (designEnt == IF_STMT) {
        if (varToIfStmtsMap.find(var) != varToIfStmtsMap.end()) {
            return varToIfStmtsMap.at(var);
        } else {
            std::unordered_set<int> empty;
            return empty;
        }
    } else if (designEnt == WHILE_STMT) {
        if (varToWhileStmtsMap.find(var) != varToWhileStmtsMap.end()) {
            return varToWhileStmtsMap.at(var);
        } else {
            std::unordered_set<int> empty;
            return empty;
        }
    }
    std::unordered_set<int> empty;
    return empty;
}
std::unordered_set<int> PatternStorage::getConditionStatementsWithAnyVar(std::string designEnt) {
    if (designEnt == IF_STMT) {
        std::unordered_set<int> stmts;
        for (std::pair<int, std::unordered_set<std::string>> element : ifStmtToVarMap) {
            stmts.emplace(element.first);
        }
        return stmts;
    } else if (designEnt == WHILE_STMT) {
        std::unordered_set<int> stmts;
        for (std::pair<int, std::unordered_set<std::string>> element : whileStmtToVarMap) {
            stmts.emplace(element.first);
        }
        return stmts;
    }
    std::unordered_set<int> empty;
    return empty;
}

IntStringPairResultSet PatternStorage::getVarMatchConditionStatementPairs(std::string designEnt) {
    if (designEnt == IF_STMT) {
        return ifStmtToVarPairSet;
    } else if (designEnt == WHILE_STMT) {
        return whileStmtToVarPairSet;
    }

    IntStringPairResultSet empty;
    return empty;
}

void PatternStorage::clear() {
    stmtNumsToVarAndPostfixMap.clear();
    stmtNumsToPostfixMap.clear();
    lhsVarToStmtsMap.clear();
    varToIfStmtsMap.clear();
    varToWhileStmtsMap.clear();
    ifStmtToVarMap.clear();
    whileStmtToVarMap.clear();
    ifStmtToVarPairSet.clear();
    whileStmtToVarPairSet.clear();
}
