#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "QueryParser.h"
#include <unordered_set>

class TestUtility {
public:
  static void print(std::unordered_set<std::string> setRes);

  static bool checkSet(std::unordered_set<std::string> s1, std::unordered_set<std::string> s2);

  static std::unordered_set<std::string> convertListToSet(std::list<std::string> list);
};
