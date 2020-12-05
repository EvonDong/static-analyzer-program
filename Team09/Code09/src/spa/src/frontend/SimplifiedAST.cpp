#include "SimplifiedAST.h"
#include <iostream>

// ===================== StatementWrapper ====================================

StatementWrapper::StatementWrapper(int stmtNum, std::string type, std::vector<std::string> content)
    : statement{stmtNum, content, type} {
}

int StatementWrapper::getStatementNum() {
    return statement.stmtNum;
}

std::vector<std::string> &StatementWrapper::getStatementTokens() {
    return statement.content;
}

std::string StatementWrapper::getStatementType() {
    return statement.type;
}

std::vector<StatementWrapper> &StatementWrapper::getContainerStatementList() {
    return containerStatementList;
}

std::vector<StatementWrapper> &StatementWrapper::getIfStatementList() {
    return ifStatementList;
}

std::vector<StatementWrapper> &StatementWrapper::getElseStatementList() {
    return elseStatementList;
}

void StatementWrapper::setElseStatementList(std::vector<StatementWrapper> stmtList) {
    elseStatementList = stmtList;
}

void StatementWrapper::setIfStatementList(std::vector<StatementWrapper> stmtList) {
    ifStatementList = stmtList;
}

void StatementWrapper::setContainerStatementList(std::vector<StatementWrapper> stmtList) {
    containerStatementList = stmtList;
}

// ===================== PROCEDURE ====================================

Procedure::Procedure(std::string name) : name{name}, numStatements{0} {
}

std::string Procedure::getName() {
    return name;
}

std::vector<StatementWrapper> &Procedure::getStatementList() {
    return statementList;
}

int Procedure::getNumStatements() {
    return numStatements;
}

void Procedure::setNumStatements(int num) {
    numStatements = num;
}

void Procedure::addStatement(StatementWrapper &statement) {
    statementList.push_back(statement);
}

// ===================== SIMPLIFIED AST ===============================
void SimplifiedAST::addProcedure(Procedure &procedure) {
    this->procedureList.push_back(procedure);
}

std::vector<Procedure> &SimplifiedAST::getProcedureList() {
    return procedureList;
}

int SimplifiedAST::getNumProcedures() {
    return procedureList.size();
}
