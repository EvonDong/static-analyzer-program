// #pragma once

#include "catch.hpp"

#include <unordered_map>
#include <unordered_set>
#include <stdio.h>
#include <iostream>
#include <string>
#include "QueryEvaluator.h"

TEST_CASE("Test Return Type For Entity") {
  QueryEvaluator qe;
  REQUIRE(qe.getReturnTypeForEntity("stmt") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("read") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("print") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("call") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("while") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("if") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("assign") == "stmtNum");
  REQUIRE(qe.getReturnTypeForEntity("variable") == "name");
  REQUIRE(qe.getReturnTypeForEntity("procedure") == "name");
  REQUIRE(qe.getReturnTypeForEntity("constant") == "value");
}

TEST_CASE("Test Return Type for Clauses") {
  QueryEvaluator qe;
  QueryParser::Clause clause;
  QueryParser::InputType v1;
  QueryParser::InputType v2;

  // follows(5, 6)
  clause.clauseVariable = "Follows";
  clause.clauseType = "suchThat";
  clause.clauseType = "suchThat";
  v1.entityType = "stmtNum";
  v2.entityType = "stmtNum";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "boolean");

  // follows(a, 6)
  clause.clauseVariable = "Follows";
  clause.clauseType = "suchThat";
  v1.entityType = "assign";
  v2.entityType = "stmtNum";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "stmtNum");

  // follows(6, w)
  clause.clauseVariable = "Follows";
  clause.clauseType = "suchThat";
  v1.entityType = "stmtNum";
  v2.entityType = "while";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "stmtNum");  

  // follows(a, w)
  clause.clauseVariable = "Follows";
  clause.clauseType = "suchThat";
  v1.entityType = "assign";
  v2.entityType = "while";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "any");

  // Uses(a, v)
  clause.clauseVariable = "Uses";
  clause.clauseType = "suchThat";
  v1.entityType = "assign";
  v2.entityType = "variable";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "any");

  // Uses(s, "test")
  clause.clauseVariable = "Uses";
  clause.clauseType = "suchThat";
  v1.entityType = "stmt";
  v2.entityType = "identifier";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "stmtNum");

  // Uses(6, "test")
  clause.clauseVariable = "Uses";
  clause.clauseType = "suchThat";
  v1.entityType = "stmtNum";
  v2.entityType = "identifier";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "boolean");

  // Uses(6, v)
  clause.clauseVariable = "Uses";
  clause.clauseType = "suchThat";
  v1.entityType = "stmtNum";
  v2.entityType = "variable";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "name");

  // Uses(6, v)
  clause.clauseVariable = "InValid";
  clause.clauseType = "suchThat";
  v1.entityType = "stmtNum";
  v2.entityType = "variable";
  clause.firstVar = v1;
  clause.secondVar = v2;
  REQUIRE(qe.getReturnTypeForSuchThatClause(clause) == "No Return Type Found");
}

TEST_CASE("Test Utility Functions") {
  QueryEvaluator qe;
  std::string relationType, str1, str2;
  QueryParser::InputType v;

  relationType = "Follows*";
  REQUIRE(qe.isTransitiveRelation(relationType) == true);

  relationType = "Parent*";
  REQUIRE(qe.isTransitiveRelation(relationType) == true);

  relationType = "Parent";
  REQUIRE(qe.isTransitiveRelation(relationType) == false);

  // Test Delimiter
  relationType = "Parent*";
  str1 = qe.stripDelimiter(relationType);
  REQUIRE(qe.isTransitiveRelation(str1) == false);
  REQUIRE(qe.isTransitiveRelation(relationType) == true);

  relationType = "Parent";
  str1 = qe.stripDelimiter(relationType);
  REQUIRE(qe.isTransitiveRelation(str1) == false);
  REQUIRE(relationType == str1);

  // Test Abstract Variable Check
  v.entityType = "assign";
  REQUIRE(qe.isAbstractVariable(v) == true);

  v.entityType = "while";
  REQUIRE(qe.isAbstractVariable(v) == true);

  v.entityType = "stmtNum";
  REQUIRE(qe.isAbstractVariable(v) == false);

  v.entityType = "identifier";
  REQUIRE(qe.isAbstractVariable(v) == false);

  // Test Wild Card
  str1 = "_test_";
  bool hasLeft = qe.hasWildCard(str1, "left");
  bool hasRight = qe.hasWildCard(str1, "right");
  REQUIRE(hasLeft == true);
  REQUIRE(hasRight == true);
  REQUIRE(qe.removeWildCard(str1, hasLeft, hasRight) == "test");
  REQUIRE(str1 == "_test_");

  str1 = "test_";
  hasLeft = qe.hasWildCard(str1, "left");
  hasRight = qe.hasWildCard(str1, "right");
  REQUIRE(hasLeft == false);
  REQUIRE(hasRight == true);
  REQUIRE(qe.removeWildCard(str1, hasLeft, hasRight) == "test");
  REQUIRE(str1 == "test_");

  str1 = "_test";
  hasLeft = qe.hasWildCard(str1, "left");
  hasRight = qe.hasWildCard(str1, "right");
  REQUIRE(hasLeft == true);
  REQUIRE(hasRight == false);
  REQUIRE(qe.removeWildCard(str1, hasLeft, hasRight) == "test");
  REQUIRE(str1 == "_test");

  str1 = "test";
  hasLeft = qe.hasWildCard(str1, "left");
  hasRight = qe.hasWildCard(str1, "right");
  REQUIRE(hasLeft == false);
  REQUIRE(hasRight == false);
  REQUIRE(qe.removeWildCard(str1, hasLeft, hasRight) == "test");
  REQUIRE(str1 == "test");

  str1 = "";
  hasLeft = qe.hasWildCard(str1, "left");
  hasRight = qe.hasWildCard(str1, "right");
  REQUIRE(hasLeft == false);
  REQUIRE(hasRight == false);
}

TEST_CASE("Test Should Get Overlap") {
  QueryEvaluator qe;
  QueryParser::Clause suchThat;
  QueryParser::Clause pattern;
  QueryParser::InputType v1;
  QueryParser::InputType v2;
  QueryParser::InputType p1;

  // ----- No Overlapping Synonyms ----
  // Uses(6, v) pattern a ("xyz", _)
  v1.entityType = "stmtNum";
  v1.var = "6";
  v2.entityType = "variable";
  v2.var = "v";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "identifier";
  p1.var = "xyz";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 0);

  // ----- No Overlapping Synonyms But Same Type ----
  // Uses(6, v) pattern a ("xyz", _)
  v1.entityType = "stmtNum";
  v1.var = "6";
  v2.entityType = "assign";
  v2.var = "v";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "identifier";
  p1.var = "xyz";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 0);

    // ----- No Overlapping Synonyms  ----
  // follows(s, s1) pattern a (_, "asd")
  v1.entityType = "stmt";
  v1.var = "s";
  v2.entityType = "stmt";
  v2.var = "s1";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "wildcard";
  p1.var = "_";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 0);

  // ----- No Overlapping Synonyms  ----
  // follows(s, s1) pattern a (v, "asd")
  v1.entityType = "stmt";
  v1.var = "s";
  v2.entityType = "stmt";
  v2.var = "s1";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "variable";
  p1.var = "v";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 0);

  // ----- One Overlapping Synonyms ----
  // Parent(6, a) pattern a ("xyz", _)
  v1.entityType = "stmtNum";
  v1.var = "6";
  v2.entityType = "assign";
  v2.var = "a";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "identifier";
  p1.var = "xyz";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 1);

  // ----- One Overlapping Synonyms ----
  // Uses(a, v) pattern a (v1, _)
  v1.entityType = "assign";
  v1.var = "a";
  v2.entityType = "variable";
  v2.var = "v";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "variable";
  p1.var = "v1";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 1);

  // ----- One Overlapping Synonyms ----
  // Uses(A, v) pattern a (v, _)
  v1.entityType = "procedure";
  v1.var = "A";
  v2.entityType = "variable";
  v2.var = "v";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "variable";
  p1.var = "v";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 1);

  // ----- Two Overlapping Synonyms ----
  // Uses(a, v) pattern a (v, _)
  v1.entityType = "assign";
  v1.var = "a";
  v2.entityType = "variable";
  v2.var = "v";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "variable";
  p1.var = "v";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 2);

  // ----- Overlapping Identifier ----
  // Uses(s, "xyz") pattern a ("xyz", _)
  v1.entityType = "stmt";
  v1.var = "s";
  v2.entityType = "identifier";
  v2.var = "xyz";
  suchThat.firstVar = v1;
  suchThat.secondVar = v2;
  pattern.clauseVariable = "a";
  p1.entityType = "identifier";
  p1.var = "xyz";
  pattern.firstVar = p1;
  REQUIRE(qe.getColsToMatch(suchThat, pattern).size() == 0);
}

TEST_CASE("Test Input Parser") {
  QueryEvaluator qe;
  std::string originalStr = "sstmt#";
  
  // no delimiter
  std::vector<std::string> vec = qe.parseAttributeInput(originalStr);
  REQUIRE(vec[0] == originalStr);

  // 1 delimiter
  originalStr = "s.stmt#";
  vec = qe.parseAttributeInput(originalStr);
  REQUIRE(vec[0] == "s");
  REQUIRE(vec[1] == "stmt#");
}
