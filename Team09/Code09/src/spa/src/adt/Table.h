#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <list>
#include <functional> 
#include "QueryParser.h"
#include "Table.h"


class Table {
private:
    std::vector<std::vector<std::string>> table;
    std::unordered_map<std::string, int> headerToIndex;
    std::unordered_set<size_t> hashedRowValues;
public:
    Table(); // TODO: remove
    Table(std::vector<std::string> headers);

    std::vector<std::string> getHeaders();

    int getCardinality(); // excluding header

    void insertRow(std::vector<std::string> row);

    std::unordered_set<std::string> getValuesFromColumn(std::string colName);

    std::unordered_set<std::string> getValuesFromColumns(std::vector<std::string> colNames);

    void insertSingleCol(std::unordered_set<std::string> elems);

    void replaceHeaders(std::vector<std::string> headers);

    void insertHeader(std::string newColHeader);

    void duplicateColumn(std::string colToDup, std::string newColHeader);

    void replaceValuesForColumnWithMap(std::string colName, std::unordered_map<std::string, std::string> oldNewMap);

    int getIndexFromHeader(std::string header);

    Table getJoin(Table otherTable);

    Table getCrossProduct(Table t2, std::vector<std::string> resultingHeaders);

    Table getProjection(std::vector<std::string> colsToProject);

    void print();

    std::string serialize();

private:
    std::vector<std::vector<std::string>> getRows();

    std::unordered_map<std::size_t, std::vector<int>> getHashToRowIndices(std::vector<std::string> colNames);

    size_t getHashFromRow(std::vector<std::string> colsToMatch, std::vector<std::string> row);

    std::vector<std::string> getResultingHeaders(std::vector<std::string> headers1, std::vector<std::string> headers2);

    std::vector<std::string> getMatchingHeaders(Table t2);
};
