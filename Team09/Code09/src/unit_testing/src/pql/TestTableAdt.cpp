#include "catch.hpp"

#include <stdio.h>
#include "Table.h"
#include "TestUtility.h"

using namespace std;

TEST_CASE("Test Table Joins") {
  Table resTable;
  std::unordered_set<std::string> ans;

  Table table1({"a", "v"});
  table1.insertRow({"5", "x"});
  table1.insertRow({"5", "y"});
  table1.insertRow({"6", "count"});
  table1.insertRow({"7", "cenX"});
  table1.insertRow({"7", "x"});
  table1.insertRow({"8", "cenY"});
  table1.insertRow({"8", "y"});
  table1.insertRow({"10", "count"});
  table1.insertRow({"12", "cenX"});
  table1.insertRow({"12", "count"});
  table1.insertRow({"13", "cenY"});
  table1.insertRow({"13", "count"});
  table1.insertRow({"14", "cenX"});
  table1.insertRow({"14", "cenY"});


  Table table2({"a", "v"});
  table2.insertRow({"1", "count"});
  table2.insertRow({"2", "cenX"});
  table2.insertRow({"3", "cenY"});
  table2.insertRow({"6", "count"});
  table2.insertRow({"7", "cenX"});
  table2.insertRow({"8", "cenY"});
  table2.insertRow({"11", "flag"});
  table2.insertRow({"12", "cenX"});
  table2.insertRow({"13", "cenY"});
  table2.insertRow({"14", "normSq"});

  resTable = table1.getJoin(table2);
  ans = resTable.getValuesFromColumn("a");
  REQUIRE(TestUtility::checkSet({"6", "7", "8", "12", "13"}, ans) == true); 
  ans = resTable.getValuesFromColumn("v");
  REQUIRE(TestUtility::checkSet({"count", "cenX", "cenY"}, ans) == true); 
  
  // resTable.print();

  Table table6({"ifs", "a"});
  table6.insertRow({"8", "10"});

  Table table7({"a", "pqr"});
  table7.insertRow({"10", "qwpeoi"});

  resTable = table6.getJoin(table7);
  ans = resTable.getValuesFromColumn("ifs");
  REQUIRE(TestUtility::checkSet({"8"}, ans) == true); 

  ans = resTable.getValuesFromColumn("a");
  REQUIRE(TestUtility::checkSet({"10"}, ans) == true); 

  ans = resTable.getValuesFromColumn("pqr");
  REQUIRE(TestUtility::checkSet({"qwpeoi"}, ans) == true); 

  resTable.duplicateColumn("ifs", "newCol");
  ans = resTable.getValuesFromColumn("newCol");
  REQUIRE(TestUtility::checkSet({"8"}, ans) == true); 
  

  // TODO: test different ordering of the cols
}

TEST_CASE("Test Table Duplicate Column") {
  std::unordered_set<std::string> ans;
  Table table1({"s3", "v1"});
  table1.insertRow({"5", "y"});
  table1.insertRow({"5", "z"});
  table1.insertRow({"7", "x"});
  table1.insertRow({"8", "y"});
  table1.insertRow({"2", "y"});

  table1.duplicateColumn("s3", "newCol");


  REQUIRE(TestUtility::checkSet(
      table1.getValuesFromColumn("s3"),
      table1.getValuesFromColumn("newCol")
    ) == true); 

  table1.duplicateColumn("newCol", "newCol2");

  REQUIRE(TestUtility::checkSet(
      table1.getValuesFromColumn("newCol"),
      table1.getValuesFromColumn("newCol2")
    ) == true); 

  table1.replaceHeaders({"col1", "col2", "col3", "col4"});
  table1.duplicateColumn("col2", "col5");

  REQUIRE(TestUtility::checkSet(
      table1.getValuesFromColumn("col2"),
      table1.getValuesFromColumn("col5")
    ) == true); 
}

TEST_CASE("Cross Product") {
  std::unordered_set<std::string> ans;
  Table table3({"s3", "v1"});
  table3.insertRow({"5", "y"});
  table3.insertRow({"5", "z"});
  table3.insertRow({"7", "x"});
  table3.insertRow({"8", "y"});
  table3.insertRow({"2", "y"});

  Table table4({"s3"});
  table4.insertRow({"6"});
  table4.insertRow({"5"});
  table4.insertRow({"7"});

  Table resTable = table3.getJoin(table4);
  ans = resTable.getValuesFromColumn("s3");
  REQUIRE(TestUtility::checkSet({"5", "7"}, ans) == true); 

  ans = resTable.getValuesFromColumn("v1");
  REQUIRE(TestUtility::checkSet({"x", "z", "y"}, ans) == true);

  Table table5({"s1", "s2"});
  table5.insertRow({"6", "20"});
  table5.insertRow({"3", "4"});
  table5.insertRow({"10", "15"});
  table5.insertRow({"8", "11"});

  resTable = resTable.getJoin(table5);
  ans = resTable.getValuesFromColumn("s3");
  REQUIRE(TestUtility::checkSet({"5", "7"}, ans) == true); 

  ans = resTable.getValuesFromColumn("v1");
  REQUIRE(TestUtility::checkSet({"x", "z", "y"}, ans) == true);

  ans = resTable.getValuesFromColumn("s1");
  REQUIRE(TestUtility::checkSet({"6", "3", "8", "10"}, ans) == true); 

  ans = resTable.getValuesFromColumn("s2");
  REQUIRE(TestUtility::checkSet({"20", "4", "11", "15"}, ans) == true); 
  
  Table table1({"s"});
  table1.insertRow({"1"});
  table1.insertRow({"2"});
  table1.insertRow({"3"});
  table1.insertRow({"4"});

  Table table2({"v"});
  table2.insertRow({"a"});
  table2.insertRow({"b"});
  table2.insertRow({"c"});

  resTable = table1.getJoin(table2);
  ans = resTable.getValuesFromColumn("s");
  REQUIRE(TestUtility::checkSet({"1", "2", "3", "4"}, ans) == true); 

  ans = resTable.getValuesFromColumn("v");
  REQUIRE(TestUtility::checkSet({"a", "b", "c"}, ans) == true); 
}

TEST_CASE("Multiple Queries (All Synonyms In Table)") {
  std::unordered_set<std::string> ans;
  std::unordered_set<std::string> res;

  Table table3({"s3", "v1"});
  table3.insertRow({"5", "y"});
  table3.insertRow({"5", "z"});
  table3.insertRow({"7", "x"});
  table3.insertRow({"8", "y"});
  table3.insertRow({"2", "y"});

  Table table4({"s3"});
  table4.insertRow({"6"});
  table4.insertRow({"5"});
  table4.insertRow({"7"});

  Table resTable = table3.getJoin(table4);
  ans = resTable.getValuesFromColumn("s3");
  REQUIRE(TestUtility::checkSet({"5", "7"}, ans) == true); 

  ans = resTable.getValuesFromColumn("v1");
  REQUIRE(TestUtility::checkSet({"x", "y", "z"}, ans) == true); 

  Table table5({"s1", "s2"});
  table5.insertRow({"6", "20"});
  table5.insertRow({"3", "4"});
  table5.insertRow({"10", "15"});
  table5.insertRow({"8", "11"});

  resTable = resTable.getJoin(table5);
  // resTable.print();

  res = resTable.getValuesFromColumns({"s3", "v1", "s1"});
  ans = {
    "7 x 8", "7 x 6", "7 x 10", "7 x 3", 
    "5 z 8", "5 z 10", "5 z 6", "5 z 3",
    "5 y 8", "5 y 10", "5 y 6", "5 y 3"
  };
  REQUIRE(TestUtility::checkSet(res, ans) == true);

  res = resTable.getValuesFromColumns({"s3", "v1"});
  ans = {"5 y", "5 z", "7 x"};
  REQUIRE(TestUtility::checkSet(res, ans) == true);

  res = resTable.getValuesFromColumns({"s3"});
  ans = {"5", "7"};
  REQUIRE(TestUtility::checkSet(res, ans) == true);
}

TEST_CASE("Projection of Columns") {
  Table table1({"s3", "v1", "a1"});
  table1.insertRow({"5", "y", "1"});
  table1.insertRow({"5", "z", "2"});
  table1.insertRow({"7", "x", "3"});
  table1.insertRow({"8", "y", "4"});
  table1.insertRow({"2", "y", "4"});
  table1.insertRow({"2", "y", "4"}); 
  // TODO: do we need to take care of duplicate insertions?
  // table1.print();
  // TestUtility::print(table1.getValuesFromColumns(table1.getHeaders()));

  Table resTable = table1.getProjection({"v1", "a1"});
  REQUIRE(resTable.getHeaders().size() == 2);
  // resTable.print();

}

TEST_CASE("Replace Value In Column") {
  std::unordered_map<std::string, std::string> oldNewMap = {
    {"1", "one"},
    {"2", "two"},
    {"3", "three"},
    {"4", "four"},
  };

  Table table1({"s3", "v1", "a1"});
  table1.insertRow({"5", "y", "1"});
  table1.insertRow({"5", "z", "2"});
  table1.insertRow({"7", "x", "3"});
  table1.insertRow({"8", "y", "4"});
  table1.insertRow({"2", "y", "4"});
  table1.insertRow({"2", "y", "4"}); 

  table1.replaceValuesForColumnWithMap("a1", oldNewMap);

  std::unordered_set<std::string> res = table1.getValuesFromColumn("a1");
  std::unordered_set<std::string> ans = {"one", "two", "three", "four"};
  REQUIRE(TestUtility::checkSet(res, ans) == true);

}
