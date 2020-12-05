#include "TestUtility.h"

void TestUtility::print(std::unordered_set<std::string> setRes) {
    for (auto it = setRes.begin(); it != setRes.end(); ++it){
        std::cout << "ans: " << it->c_str() << std::endl;
    }
}

bool TestUtility::checkSet(std::unordered_set<std::string> s1, std::unordered_set<std::string> s2) {
    // std::unordered_set<std::string> s1 = convertListToSet(s1List);
    if (s1.size() != s2.size()) return false;
    for (auto it = s1.begin(); it != s1.end(); ++it){
        std::string str1 = it->c_str();
        if (s2.find(str1) == s2.end()) return false;
    }
    return true;
}

std::unordered_set<std::string> TestUtility::convertListToSet(std::list<std::string> list) {
    std::unordered_set<std::string> set;
    for (std::string elem : list) set.insert(elem);
    return set;
}
