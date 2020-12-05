#include "Table.h"

Table::Table() {
}

Table::Table(std::vector<std::string> headers) {
  for (int i = 0; i < headers.size(); i++) {
    headerToIndex.insert({headers[i], i});
  }
  table.push_back(headers);
}

std::vector<std::string> Table::getHeaders() {
  std::vector<std::string> res;
  if (table.size() != 0) res = table[0];
  return res;
}

// replaces the existing headers if any
void Table::replaceHeaders(std::vector<std::string> headers) {
  if (table.size() == 0 || table[0].size() != headers.size()) return;
  table[0] = headers;
  headerToIndex.clear();
  for (int i = 0; i < headers.size(); i++) {
    headerToIndex.insert({headers[i], i});
  }
} 

// gets the index of the col with respect to the row
int Table::getIndexFromHeader(std::string header) {
  if (headerToIndex.size() == 0) return -1;
  auto index = headerToIndex.find(header);
  return index != headerToIndex.end() ? index->second : -1;
}

// duplicates an existing col and adds a new col at the end of the table
void Table::duplicateColumn(std::string colToDup, std::string newColHeader) {
  if (colToDup == newColHeader) return; // cannot duplicate col with same header
  hashedRowValues.clear();
  int index = getIndexFromHeader(colToDup);
  insertHeader(newColHeader);
  std::vector<std::string> newHeaders = getHeaders();
  for (int i = 1; i <= getCardinality(); i++) {
    table[i].push_back(table[i][index]);
    hashedRowValues.insert(getHashFromRow(newHeaders, table[i]));
  }
  return;
}

void Table::insertHeader(std::string newColHeader) {
  headerToIndex.insert({newColHeader, headerToIndex.size()});
  table[0].push_back(newColHeader);
}

void Table::replaceValuesForColumnWithMap(std::string colName, std::unordered_map<std::string, std::string> oldNewMap) {
  int index = getIndexFromHeader(colName);
  for (int i = 1; i <= getCardinality(); i++) {
    std::vector<std::string> row = table[i];
    std::vector<std::string> headers = getHeaders();

    // remove old hash
    size_t hashedRowVal = getHashFromRow(headers, row);
    hashedRowValues.erase(hashedRowVal);

    std::unordered_map<std::string, std::string>::const_iterator oldNewPair =
      oldNewMap.find(row[index]);
    if (oldNewPair == oldNewMap.end()) continue;
    row[index] = oldNewPair->second;
    table[i] = row;

    // add new hash
    hashedRowValues.insert(getHashFromRow(headers, table[i]));
  }
}

std::unordered_set<std::string> Table::getValuesFromColumn(std::string colName) {
  std::unordered_set<std::string> res;
  int index = getIndexFromHeader(colName);
  if (table.size() <= 1 || index < 0) return res;
  for (int i = 1; i < table.size(); i++) {
    std::vector<std::string> row = table[i];
    res.insert(row[index]);
  }
  return res;
}

std::unordered_set<std::string> Table::getValuesFromColumns(std::vector<std::string> colNames) {
  std::unordered_set<std::string> res;
  if (table.size() <= 1) return res;
  for (int i = 1; i < table.size(); i++) {
    std::string rowValue = "";
    std::vector<std::string> row = table[i];
    // res.insert(row[index]);
    for (std::string colName : colNames) {
      int index = getIndexFromHeader(colName);
      rowValue += (rowValue == "" ? "" : " ") + row[index];
    }
    res.insert(rowValue);
  }
  return res;
}

Table Table::getProjection(std::vector<std::string> colsToProject) {
  Table projectionTable(colsToProject);
  if (getCardinality() == 0) return projectionTable;

  std::hash<std::string> hasher; 
  std::unordered_set<size_t> hashedValues;

  for (int i = 1; i < table.size(); i++) {
    std::vector<std::string> projectedRow;
    std::vector<std::string> row = table[i];
    std::string concatenatedRowValues = "";

    for (std::string colName : colsToProject) {
      int index = getIndexFromHeader(colName);
      projectedRow.push_back(row[index]);
      concatenatedRowValues += (row[index] + ";");
    }

    size_t hashKey = hasher(concatenatedRowValues);
    if (hashedValues.find(hashKey) == hashedValues.end()) {
      hashedValues.insert(hashKey);
      projectionTable.insertRow(projectedRow);
    }
  }

  return projectionTable;
}


void Table::insertRow(std::vector<std::string> row) {
  if (getHeaders().size() == row.size()) {
    size_t hashedRowVal = getHashFromRow(getHeaders(), row);
    if (hashedRowValues.find(hashedRowVal) == hashedRowValues.end()) {
      hashedRowValues.insert(hashedRowVal);
      table.push_back(row);
    }
  } else {
    std::cout << "Failed to insert row" << std::endl;
  }
}

void Table::insertSingleCol(std::unordered_set<std::string> elems) {
  for (const std::string& elem : elems) {
    std::vector<std::string> row = {elem};
    insertRow(row);
  }
}

int Table::getCardinality() {
  // exclude header
  return table.size() == 1 ? 0 : table.size() - 1;
}

std::vector<std::vector<std::string>> Table::getRows() {
  return table;
}

std::unordered_map<std::size_t, std::vector<int>> Table::getHashToRowIndices (std::vector<std::string> colNames) {
  std::unordered_map<std::size_t, std::vector<int>> hashToRowIndices;
  if (getCardinality() == 0) return hashToRowIndices;

  for (int i = 1; i <= getCardinality(); i++) {
    size_t hashKey = getHashFromRow(colNames, table[i]);
    auto entry = hashToRowIndices.find(hashKey);
    if (entry != hashToRowIndices.end()) {
      hashToRowIndices[hashKey].push_back(i);
    } else {
      hashToRowIndices.insert({hashKey, std::vector<int> {i}});
    }    
  }

  return hashToRowIndices;
}

size_t Table::getHashFromRow(std::vector<std::string> colsToMatch, std::vector<std::string> row) {
  std::vector<int> indices;
  for (std::string colName : colsToMatch) {
    indices.push_back(getIndexFromHeader(colName));
  }

  std::string concatenatedRowValues = "";
  std::hash<std::string> hasher; 
  for (int index : indices) {
    concatenatedRowValues += (row[index] + ";");
  }
  // std::cout << "concated " << concatenatedRowValues << std::endl;
  size_t hashKey = hasher(concatenatedRowValues);
  return hashKey;
}

Table Table::getCrossProduct(Table t2, std::vector<std::string> resultingHeaders) {
  Table resultingTable(resultingHeaders);
  for (int i = 1; i <= getCardinality(); i++) {
    std::vector<std::string> t1Row = table[i];
    for (int j = 1; j <= t2.getCardinality(); j++) {
      std::vector<std::string> combinedRow;
      std::vector<std::string> t2Row = (t2.getRows())[j];
      // populate the combinedRow
      for (int k = 0; k < resultingHeaders.size(); k++) {
        std::string headerName = resultingHeaders[k];
        int t1ColIndex = getIndexFromHeader(headerName);
        if (t1ColIndex != -1) {
          combinedRow.push_back(t1Row[t1ColIndex]);
        } else {
          int t2ColIndex = t2.getIndexFromHeader(headerName);
          combinedRow.push_back(t2Row[t2ColIndex]);
        }
      }
      resultingTable.insertRow(combinedRow);
    }
  }
  return resultingTable;
}

Table Table::getJoin(Table t2) {
  std::vector<std::string> resultingHeaders = getResultingHeaders(getHeaders(), t2.getHeaders());
  Table resultingTable(resultingHeaders);
  if (getCardinality() == 0 || t2.getCardinality() == 0) return resultingTable;
  std::vector<std::string> colsToMatch = getMatchingHeaders(t2);
  if (colsToMatch.size() == 0) return getCrossProduct(t2, resultingHeaders);

  if (t2.getCardinality() < getCardinality()) {
    return t2.getJoin(*this);
  }

  std::unordered_map<std::size_t, std::vector<int>> t1HashToRowIndices = getHashToRowIndices(colsToMatch);
  // std::unordered_map<std::size_t, std::vector<int>> t1HashToRowIndices;

  for (int i = 1; i <= t2.getCardinality(); i++) {
    std::vector<std::string> t2Row = (t2.getRows())[i];
    size_t t2RowHash = t2.getHashFromRow(colsToMatch, t2Row);
    // std::cout << "t2RowHash " << t2RowHash << " " << t2Row[0] << std::endl;
    auto t1HashRowIndicesPair = t1HashToRowIndices.find(t2RowHash);
    if (t1HashRowIndicesPair != t1HashToRowIndices.end()) {
      // std::cout << "has matching hash" << std::endl;
      std::vector<int> t1RowIndices = t1HashRowIndicesPair->second;

      for (int t1RowIndex : t1RowIndices) {
        std::vector<std::string> combinedRow;
        std::vector<std::string> t1Row = table[t1RowIndex];
        for (int j = 0; j < resultingHeaders.size(); j++) {
          std::string headerName = resultingHeaders[j];
          // std::cout << headerName << std::endl;
          int t1ColIndex = getIndexFromHeader(headerName);
          if (t1ColIndex != -1) {
            combinedRow.push_back(t1Row[t1ColIndex]);
          } else {
            int t2ColIndex = t2.getIndexFromHeader(headerName);
            combinedRow.push_back(t2Row[t2ColIndex]);
          }
        }
        resultingTable.insertRow(combinedRow);
      }
    }
  }
  return resultingTable;
}

std::vector<std::string> Table::getMatchingHeaders(Table t2) {
  std::vector<std::string> matchingHeaders;
  std::vector<std::string> h1 = getHeaders();
  std::vector<std::string> h2 = t2.getHeaders();

  if (h1.size() == 0 || h2.size() == 0) return matchingHeaders;

  std::unordered_set<std::string> resSet;

  for (std::string header : h2) {
    resSet.insert(header);
  }

  for (std::string header : h1) {
    if (resSet.find(header) != resSet.end()) {
      matchingHeaders.push_back(header);
    }
  }

  return matchingHeaders;
}

std::vector<std::string> Table::getResultingHeaders(
  std::vector<std::string> headers1, 
  std::vector<std::string> headers2
) {
  std::vector<std::string> res;
  std::unordered_set<std::string> resSet;
  for (std::string header : headers1) {
    if (resSet.find(header) == resSet.end()) {
      resSet.insert(header);
      res.push_back(header);
    }
  }

  for (std::string header : headers2) {
    if (resSet.find(header) == resSet.end()) {
      resSet.insert(header);
      res.push_back(header);
    }
  }

  return res;
}

void Table::print() {
  if (getCardinality() == 0) return;
  for (int i = 0; i < table[0].size(); i++) {
    std::cout << table[0][i] << (i != table[0].size() - 1 ? " | " : "");
  }
  std::cout << std::endl;
  for (int i = 1; i <= getCardinality(); i++) {
    for (int j = 0; j < table[i].size(); j++) {
      std::cout << table[i][j] << (j != table[i].size() - 1 ? " | " : "");
    }
    std::cout << std::endl;
  }
}

std::string Table::serialize() {
  std::string serialized = "";
  if (getCardinality() == 0) return serialized;
  for (int i = 0; i < table[0].size(); i++) {
    serialized +=  (table[0][i] +  (i != table[0].size() - 1 ? " , " : ""));
  }
  serialized+= "*EOL*";
  for (int i = 1; i <= getCardinality(); i++) {
    serialized += "";
    for (int j = 0; j < table[i].size(); j++) {
      serialized += (table[i][j] + (j != table[i].size() - 1 ? " , " : ""));
    }
    serialized += "*EOL*";
  }
  return serialized;
}
