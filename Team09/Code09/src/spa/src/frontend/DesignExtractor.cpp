#include "DesignExtractor.h"
#include <iostream>

DesignExtractor::DesignExtractor(SimplifiedAST ast, CallGraph &callGraph) : ast{ast}, pkb{PKB::Instance()}, callGraph{callGraph} {
}

void DesignExtractor::extract() {
    for (Procedure p : ast.getProcedureList()) {

        std::vector<StatementWrapper> &stmts = p.getStatementList();

        populatePKBWithProcedureInfo(p);

        // Follows relation
        processFollowsRelations(stmts);

        // FollowsStar relation
        std::vector<int> prevStmtList;
        processFollowsStarRelations(stmts, prevStmtList);

        // Parent relation
        processParentRelations(stmts, 0);

        // ParentStar relation
        std::vector<int> parentList;
        processParentStarRelations(stmts, parentList);

        // Uses relation
        processUsesRelations(p);

        // Modifies relation
        processModifiesRelations(p);

        // Calls relation
        processCallsRelations(stmts, p.getName());

        populatePKBWithStatements(stmts);

        populatePKBWithAssignmentAndConditionalPattern(stmts);

        // processCallsUsesAndModifies(p);
    }

    pkb.startPopulateCallsStarInfo();
    // populatePKBWithProcedureUsesAndModifiesVar();
    std::unordered_map<int, std::set<int>> cfg = createCFG(ast);
    pkb.addNextAdjacencyList(cfg);
}

void DesignExtractor::populatePKBWithProcedureInfo(Procedure &procedure) {
    int startLine = procedure.getStatementList()[0].getStatementNum();
    int endLine = getEndLineOfProcedure(procedure.getStatementList());
    pkb.addProcInfo(procedure.getName(), startLine, endLine);
}

int DesignExtractor::getEndLineOfProcedure(std::vector<StatementWrapper> &stmts) {
    StatementWrapper &last = stmts.back();
    if (last.getStatementType() == IF_STMT) {
        return getEndLineOfProcedure(last.getElseStatementList());
    } else if (last.getStatementType() == WHILE_STMT) {
        return getEndLineOfProcedure(last.getContainerStatementList());
    } else {
        return last.getStatementNum();
    }
}

void DesignExtractor::processFollowsRelations(std::vector<StatementWrapper> &stmts) {
    int prevStmtNum = 0;

    for (StatementWrapper stmt : stmts) {

        // for first statement
        if (prevStmtNum != 0) {
            pkb.addFollowsRelation(prevStmtNum, stmt.getStatementNum());
        }
        prevStmtNum = stmt.getStatementNum();

        // check for container, if and else
        if (!stmt.getContainerStatementList().empty()) {
            processFollowsRelations(stmt.getContainerStatementList());
        } else if (!stmt.getIfStatementList().empty()) {
            processFollowsRelations(stmt.getIfStatementList());
            processFollowsRelations(stmt.getElseStatementList());
        }
    }
}

void DesignExtractor::processFollowsStarRelations(std::vector<StatementWrapper> &stmts, std::vector<int> prevStmtList) {
    for (StatementWrapper stmt : stmts) {

        for (int prev : prevStmtList) {
            pkb.addFollowsStarRelation(prev, stmt.getStatementNum());
        }
        prevStmtList.push_back(stmt.getStatementNum());

        // check for container, if and else
        std::vector<int> newPrevStmtList;
        if (!stmt.getContainerStatementList().empty()) {
            processFollowsStarRelations(stmt.getContainerStatementList(), newPrevStmtList);
        } else if (!stmt.getIfStatementList().empty()) {
            processFollowsStarRelations(stmt.getIfStatementList(), newPrevStmtList);
            processFollowsStarRelations(stmt.getElseStatementList(), newPrevStmtList);
        }
    }
}

void DesignExtractor::processParentRelations(std::vector<StatementWrapper> &stmts, int parent) {
    for (StatementWrapper stmt : stmts) {

        // for base procedure (no parents)
        if (parent != 0) {
            pkb.addParentRelation(parent, stmt.getStatementNum());
        }

        // check for container, if and else
        if (!stmt.getContainerStatementList().empty()) {
            processParentRelations(stmt.getContainerStatementList(), stmt.getStatementNum());
        } else if (!stmt.getIfStatementList().empty()) {
            processParentRelations(stmt.getIfStatementList(), stmt.getStatementNum());
            processParentRelations(stmt.getElseStatementList(), stmt.getStatementNum());
        }
    }
}

void DesignExtractor::processParentStarRelations(std::vector<StatementWrapper> &stmts, std::vector<int> parentList) {
    for (StatementWrapper stmt : stmts) {

        // add statement to parent first, if any
        for (int parent : parentList) {
            pkb.addParentStarRelation(parent, stmt.getStatementNum());
        }

        // check for container, if and else
        if (!stmt.getContainerStatementList().empty()) {
            parentList.push_back(stmt.getStatementNum());
            processParentStarRelations(stmt.getContainerStatementList(), parentList);
            if (parentList.size() != 0) {
                parentList.pop_back();
            }
        } else if (!stmt.getIfStatementList().empty()) {
            parentList.push_back(stmt.getStatementNum());
            processParentStarRelations(stmt.getIfStatementList(), parentList);
            processParentStarRelations(stmt.getElseStatementList(), parentList);
            if (parentList.size() != 0) {
                parentList.pop_back();
            }
        }
    }
}

void DesignExtractor::processUsesRelations(Procedure &procedure) {
    std::vector<StatementWrapper> &stmts = procedure.getStatementList();
    std::unordered_set<std::string> procedureVarSet = getUsesVarSet(stmts);
    if (!procedureVarSet.empty()) {
        pkb.addProcUsesRelation(procedure.getName(), procedureVarSet);
    }
}

std::unordered_set<std::string> DesignExtractor::getUsesVarSet(std::vector<StatementWrapper> &stmts) {
    std::unordered_set<std::string> totalVarSet;

    for (StatementWrapper stmt : stmts) {

        if (stmt.getStatementType() == ASSIGN) {
            std::unordered_set<std::string> varSet = extractUsesVarFromStmt(stmt);

            if (!varSet.empty()) {
                // update PKB
                pkb.addStmtUsesRelation(stmt.getStatementNum(), ASSIGN, varSet);

                // add to the total set
                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        } else if (stmt.getStatementType() == PRINT) {
            std::unordered_set<std::string> varSet = extractUsesVarFromStmt(stmt);

            if (!varSet.empty()) {
                // update PKB
                pkb.addStmtUsesRelation(stmt.getStatementNum(), PRINT, varSet);

                // add to the total set
                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        } else if (stmt.getStatementType() == IF_STMT) {
            std::unordered_set<std::string> varSet = extractUsesVarFromStmt(stmt);

            // get var sets from container
            std::unordered_set<std::string> varSetIf = getUsesVarSet(stmt.getIfStatementList());
            std::unordered_set<std::string> varSetElse = getUsesVarSet(stmt.getElseStatementList());

            // combine the 3 varSets together
            varSet.insert(varSetIf.begin(), varSetIf.end());
            varSet.insert(varSetElse.begin(), varSetElse.end());

            if (!varSet.empty()) {
                // update the PKB
                pkb.addStmtUsesRelation(stmt.getStatementNum(), IF_STMT, varSet);

                // add to the total set
                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        } else if (stmt.getStatementType() == WHILE_STMT) {
            std::unordered_set<std::string> varSet = extractUsesVarFromStmt(stmt);

            // get var sets from container
            std::unordered_set<std::string> varSetContainer = getUsesVarSet(stmt.getContainerStatementList());

            // combine 2 the varSets together
            varSet.insert(varSetContainer.begin(), varSetContainer.end());

            if (!varSet.empty()) {
                // update the PKB
                pkb.addStmtUsesRelation(stmt.getStatementNum(), WHILE_STMT, varSet);

                // add to the total set
                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        }
    }
    return totalVarSet;
}

std::unordered_set<std::string> DesignExtractor::extractUsesVarFromStmt(StatementWrapper &stmt) {
    std::unordered_set<std::string> varSet;
    std::vector<std::string> &tokens = stmt.getStatementTokens();

    if (stmt.getStatementType() == ASSIGN) {
        bool RHS = false;
        for (std::string t : tokens) {
            if (t == "=") {
                RHS = true;
                continue;
            }
            if (RHS && std::isalpha(t[0])) {
                varSet.insert(t);
            }
        }
    } else if (stmt.getStatementType() == PRINT) {
        // fast and lazy way
        varSet.insert(tokens[1]);

        // assume token list may not be correct
        /*
        bool RHS = false;
        for (std::string t : tokens) {
            if (t == "print") {
                RHS = true;
                continue;
            }
            if (RHS && std::isalpha(t[0])) {
                varSet.insert(t);
            }
        }
        */
    } else if (stmt.getStatementType() == IF_STMT || stmt.getStatementType() == WHILE_STMT) {
        int bracesToMatch = 1;
        for (std::size_t i = 2; i < tokens.size(); i++) {
            if (bracesToMatch == 0) {
                break;
            }

            if (tokens[i] == ")") {
                bracesToMatch -= 1;
            } else if (tokens[i] == "(") {
                bracesToMatch += 1;
            } else {
                if (std::isalpha(tokens[i][0])) {
                    varSet.insert(tokens[i]);
                }
            }
        }
    }
    return varSet;
}

void DesignExtractor::processModifiesRelations(Procedure &procedure) {
    std::vector<StatementWrapper> &stmts = procedure.getStatementList();
    std::unordered_set<std::string> procedureVarSet = getModifiesVarSet(stmts);
    if (!procedureVarSet.empty()) {
        pkb.addProcModifiesRelation(procedure.getName(), procedureVarSet);
    }
}

std::unordered_set<std::string> DesignExtractor::getModifiesVarSet(std::vector<StatementWrapper> &stmts) {
    std::unordered_set<std::string> totalVarSet;

    for (StatementWrapper stmt : stmts) {

        if (stmt.getStatementType() == ASSIGN) {
            std::unordered_set<std::string> varSet = extractModifiesVarFromStmt(stmt);

            if (!varSet.empty()) {
                // update PKB
                pkb.addStmtModifiesRelation(stmt.getStatementNum(), ASSIGN, varSet);
                // add to total set
                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        } else if (stmt.getStatementType() == READ) {
            std::unordered_set<std::string> varSet = extractModifiesVarFromStmt(stmt);

            if (!varSet.empty()) {
                // update PKB
                pkb.addStmtModifiesRelation(stmt.getStatementNum(), READ, varSet);
                // add to total set
                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        } else if (stmt.getStatementType() == IF_STMT) {
            std::unordered_set<std::string> varSet;
            std::unordered_set<std::string> varSetIf = getModifiesVarSet(stmt.getIfStatementList());
            std::unordered_set<std::string> varSetElse = getModifiesVarSet(stmt.getElseStatementList());

            varSet.insert(varSetIf.begin(), varSetIf.end());
            varSet.insert(varSetElse.begin(), varSetElse.end());

            if (!varSet.empty()) {
                pkb.addStmtModifiesRelation(stmt.getStatementNum(), IF_STMT, varSet);

                totalVarSet.insert(varSet.begin(), varSet.end());
            }
        } else if (stmt.getStatementType() == WHILE_STMT) {
            std::unordered_set<std::string> varSetContainer = getModifiesVarSet(stmt.getContainerStatementList());

            if (!varSetContainer.empty()) {
                pkb.addStmtModifiesRelation(stmt.getStatementNum(), WHILE_STMT, varSetContainer);

                totalVarSet.insert(varSetContainer.begin(), varSetContainer.end());
            }
        }
    }
    return totalVarSet;
}

std::unordered_set<std::string> DesignExtractor::extractModifiesVarFromStmt(StatementWrapper &stmt) {
    std::unordered_set<std::string> varSet;
    std::vector<std::string> &tokens = stmt.getStatementTokens();

    if (stmt.getStatementType() == ASSIGN) {
        // assume token list is already correct
        // var_name = expr;
        varSet.insert(tokens[0]);
    } else if (stmt.getStatementType() == READ) {
        // assume token list is already correct
        // READ var_name
        varSet.insert(tokens[1]);
    }
    return varSet;
}

void DesignExtractor::processCallsRelations(std::vector<StatementWrapper> &stmts, std::string procedureName) {

    for (StatementWrapper stmt : stmts) {
        if (stmt.getStatementType() == CALL) {
            pkb.addCallsRelation(procedureName, stmt.getStatementTokens()[1], stmt.getStatementNum());
        } else if (stmt.getStatementType() == IF_STMT) {
            processCallsRelations(stmt.getIfStatementList(), procedureName);
            processCallsRelations(stmt.getElseStatementList(), procedureName);
        } else if (stmt.getStatementType() == WHILE_STMT) {
            processCallsRelations(stmt.getContainerStatementList(), procedureName);
        }
    }
}

std::unordered_set<std::string> DesignExtractor::getAssignVar(std::vector<std::string> tokenList) {
    std::unordered_set<std::string> varSet;
    for (std::string t : tokenList) {
        if (std::isalpha(t[0])) {
            varSet.insert(t);
        }
    }
    return varSet;
}

std::unordered_set<std::string> DesignExtractor::getPrintReadCallVar(std::vector<std::string> tokenList) {
    std::unordered_set<std::string> varSet{tokenList[1]};
    return varSet;
}

std::unordered_set<std::string> DesignExtractor::getIfWhileVar(std::vector<std::string> tokenList) {
    std::unordered_set<std::string> varSet;
    int bracesToMatch = 1;

    // know that first 2 tokens are "if/while", "("
    for (std::size_t i = 2; i < tokenList.size(); i++) {
        if (bracesToMatch == 0) {
            break;
        }

        if (tokenList[i] == ")") {
            bracesToMatch -= 1;
        } else if (tokenList[i] == "(") {
            bracesToMatch += 1;
        } else {
            if (std::isalpha(tokenList[i][0])) {
                varSet.insert(tokenList[i]);
            }
        }
    }
    return varSet;
}

std::unordered_set<int> DesignExtractor::getConstantList(std::vector<std::string> tokenList) {
    std::unordered_set<int> constSet;
    for (std::string t : tokenList) {
        if (std::isdigit(t[0])) {
            constSet.insert(std::stoi(t));
        }
    }
    return constSet;
}

std::string DesignExtractor::getAssignLHS(std::vector<std::string> tokenList) {
    return tokenList[0];
}

std::string DesignExtractor::getAssignRHS(std::vector<std::string> tokenList) {
    std::stringstream ss;
    bool RHS = false;
    bool firstItemInStream = true;
    for (std::size_t i = 0; i < tokenList.size(); i++) {
        if (tokenList[i] == "=") {
            RHS = true;
            continue;
        }

        if (RHS && tokenList[i] != ";") {
            if (firstItemInStream) {
                ss << tokenList[i];
                firstItemInStream = false;
            } else {
                ss << " " << tokenList[i];
            }
        }
    }
    return ss.str();
}

void DesignExtractor::populatePKBWithStatements(std::vector<StatementWrapper> &stmts) {
    for (StatementWrapper stmt : stmts) {
        std::unordered_set<std::string> varSet;

        if (stmt.getStatementType() == ASSIGN) {
            varSet = getAssignVar(stmt.getStatementTokens());
        } else if (stmt.getStatementType() == PRINT) {
            varSet = getPrintReadCallVar(stmt.getStatementTokens());
        } else if (stmt.getStatementType() == READ) {
            varSet = getPrintReadCallVar(stmt.getStatementTokens());
        } else if (stmt.getStatementType() == IF_STMT) {
            varSet = getIfWhileVar(stmt.getStatementTokens());
            populatePKBWithStatements(stmt.getIfStatementList());
            populatePKBWithStatements(stmt.getElseStatementList());
        } else if (stmt.getStatementType() == WHILE_STMT) {
            varSet = getIfWhileVar(stmt.getStatementTokens());
            populatePKBWithStatements(stmt.getContainerStatementList());
        } else if (stmt.getStatementType() == CALL) {
            // PKB Doesnt want
            // varSet = getPrintReadCallVar(stmt.getStatementTokens());
        }

        std::unordered_set<int> constSet = getConstantList(stmt.getStatementTokens());
        pkb.storeStatement(stmt.getStatementNum(), stmt.getStatementType(), varSet, constSet);
    }
}

void DesignExtractor::populatePKBWithAssignmentAndConditionalPattern(std::vector<StatementWrapper> &stmts) {
    for (StatementWrapper stmt : stmts) {
        if (stmt.getStatementType() == ASSIGN) {
            std::string LHS = getAssignLHS(stmt.getStatementTokens());
            std::string RHS = getAssignRHS(stmt.getStatementTokens());
            pkb.storeAssignPattern(stmt.getStatementNum(), LHS, RHS);
        } else if (stmt.getStatementType() == IF_STMT) {
            std::unordered_set<std::string> varSet = getIfWhileVar(stmt.getStatementTokens());
            pkb.addConditionPattern(stmt.getStatementNum(), varSet, IF_STMT);
            populatePKBWithAssignmentAndConditionalPattern(stmt.getIfStatementList());
            populatePKBWithAssignmentAndConditionalPattern(stmt.getElseStatementList());
        } else if (stmt.getStatementType() == WHILE_STMT) {
            std::unordered_set<std::string> varSet = getIfWhileVar(stmt.getStatementTokens());
            pkb.addConditionPattern(stmt.getStatementNum(), varSet, WHILE_STMT);
            populatePKBWithAssignmentAndConditionalPattern(stmt.getContainerStatementList());
        }
    }
}

std::unordered_map<int, std::set<int>> DesignExtractor::createCFG(SimplifiedAST ast) {
    std::unordered_map<int, std::set<int>> adjList;

    for (Procedure p : ast.getProcedureList()) {
        std::vector<StatementWrapper> &stmts = p.getStatementList();
        CFGHelper(stmts, 0, 0, adjList);
    }

    return adjList;
}

void DesignExtractor::CFGHelper(std::vector<StatementWrapper> &stmts, int prev, int containerOutNext,
                                std::unordered_map<int, std::set<int>> &adjList) {

    for (std::size_t i = 0; i < stmts.size(); i++) {
        StatementWrapper &stmt = stmts[i];

        if (prev != 0) {
            std::unordered_map<int, std::set<int>>::iterator it = adjList.find(prev);

            // if not found
            if (it == adjList.end()) {
                adjList.emplace(prev, std::set<int>{stmt.getStatementNum()});
            } else {
                it->second.insert(stmt.getStatementNum());
            }
        }

        prev = stmt.getStatementNum();

        if ((i == stmts.size() - 1) && (containerOutNext != 0) && stmt.getStatementType() != IF_STMT) {
            // last item in statement, link back to ContainerOutNext
            // for if/else, is the statement after IF/else block OR if inside a while loop, to the while cond
            // for while, is the while statement itself
            std::unordered_map<int, std::set<int>>::iterator it = adjList.find(stmt.getStatementNum());

            // if not found
            if (it == adjList.end()) {
                adjList.emplace(stmt.getStatementNum(), std::set<int>{containerOutNext});
            } else {
                it->second.insert(containerOutNext);
            }
        }

        if (stmt.getStatementType() == IF_STMT) {
            // try to get next
            int nextStmt = 0;

            if (i < stmts.size() - 1) {
                StatementWrapper &temp = stmts[i + 1];
                nextStmt = temp.getStatementNum();
            }

            // reset prev because next stmt in stmt list should not have next relation to if
            prev = 0;

            if (nextStmt == 0 && containerOutNext != 0) {
                // if no stmts after IF and is inside a while loop
                CFGHelper(stmt.getIfStatementList(), stmt.getStatementNum(), containerOutNext, adjList);
                CFGHelper(stmt.getElseStatementList(), stmt.getStatementNum(), containerOutNext, adjList);
            } else {
                // not in a while loop, so just go ahead normally
                CFGHelper(stmt.getIfStatementList(), stmt.getStatementNum(), nextStmt, adjList);
                CFGHelper(stmt.getElseStatementList(), stmt.getStatementNum(), nextStmt, adjList);
            }
        } else if (stmt.getStatementType() == WHILE_STMT) {
            CFGHelper(stmt.getContainerStatementList(), stmt.getStatementNum(), stmt.getStatementNum(), adjList);
        }
    }
}

std::unordered_map<int, std::set<int>> DesignExtractor::createBipsCFG(SimplifiedAST &ast) {
    std::unordered_map<int, std::set<int>> adjList;

    for (Procedure p : ast.getProcedureList()) {
        std::vector<StatementWrapper> &stmts = p.getStatementList();
        bipsCFGHelper(stmts, 0, 0, 0, adjList, ast);
    }

    return adjList;
}

void DesignExtractor::bipsCFGHelper(std::vector<StatementWrapper> &stmts, int prev, int containerOutNext, int procedureReturn,
                                    std::unordered_map<int, std::set<int>> &adjList, SimplifiedAST &ast) {

    for (std::size_t i = 0; i < stmts.size(); i++) {
        StatementWrapper &stmt = stmts[i];
        if (prev != 0) {
            if (((i >= 1) && (stmts[i - 1].getStatementType() != CALL)) || (i == 0)) {
                std::unordered_map<int, std::set<int>>::iterator it = adjList.find(prev);

                // if not found
                if (it == adjList.end()) {
                    adjList.emplace(prev, std::set<int>{stmt.getStatementNum()});
                } else {
                    it->second.insert(stmt.getStatementNum());
                }
            }
        }

        prev = stmt.getStatementNum();

        if ((i == stmts.size() - 1) && (procedureReturn != 0) && (containerOutNext == 0) &&
            stmt.getStatementType() != IF_STMT) {
            if (stmt.getStatementType() == CALL) {
                std::string name = stmt.getStatementTokens()[1];
                for (Procedure p : ast.getProcedureList()) {
                    if (p.getName() == name) {
                        bipsCFGHelper(p.getStatementList(), stmt.getStatementNum(), containerOutNext, procedureReturn, adjList,
                                      ast);
                    }
                }
            } else {
                // last item in statement, link back to procedure it is called from
                std::unordered_map<int, std::set<int>>::iterator it = adjList.find(stmt.getStatementNum());

                // if not found
                if (it == adjList.end()) {
                    adjList.emplace(stmt.getStatementNum(), std::set<int>{procedureReturn});
                } else {
                    it->second.insert(procedureReturn);
                }
            }
        }

        if ((i == stmts.size() - 1) && (containerOutNext != 0) && (procedureReturn == 0) &&
            stmt.getStatementType() != IF_STMT && stmt.getStatementType() != CALL) {
            // last item in statement, link back to ContainerOutNext
            // for if/else, is the statement after IF/else block OR if inside a while loop, to the while cond
            // for while, is the while statement itself
            std::unordered_map<int, std::set<int>>::iterator it = adjList.find(stmt.getStatementNum());

            // if not found
            if (it == adjList.end()) {
                adjList.emplace(stmt.getStatementNum(), std::set<int>{containerOutNext});
            } else {
                it->second.insert(containerOutNext);
            }
        }

        if (stmt.getStatementType() == IF_STMT) {
            // try to get next
            int nextStmt = 0;

            if (i < stmts.size() - 1) {
                StatementWrapper &temp = stmts[i + 1];
                nextStmt = temp.getStatementNum();
            }

            // reset prev because next stmt in stmt list should not have next relation to if
            prev = 0;

            if (nextStmt == 0 && containerOutNext != 0) {
                // if no stmts after IF and is inside a while loop
                bipsCFGHelper(stmt.getIfStatementList(), stmt.getStatementNum(), containerOutNext, procedureReturn, adjList, ast);
                bipsCFGHelper(stmt.getElseStatementList(), stmt.getStatementNum(), containerOutNext, procedureReturn, adjList,
                              ast);
            } else {
                // not in a while loop, so just go ahead normally
                bipsCFGHelper(stmt.getIfStatementList(), stmt.getStatementNum(), nextStmt, procedureReturn, adjList, ast);
                bipsCFGHelper(stmt.getElseStatementList(), stmt.getStatementNum(), nextStmt, procedureReturn, adjList, ast);
            }
        } else if (stmt.getStatementType() == WHILE_STMT) {
            bipsCFGHelper(stmt.getContainerStatementList(), stmt.getStatementNum(), stmt.getStatementNum(), procedureReturn,
                          adjList, ast);
        } else if (stmt.getStatementType() == CALL) {
            std::string name = stmt.getStatementTokens()[1];
            int nextStmt = 0;
            if (i < stmts.size() - 1) {
                StatementWrapper &temp = stmts[i + 1];
                nextStmt = temp.getStatementNum();
                for (Procedure p : ast.getProcedureList()) {
                    if (p.getName() == name) {
                        bipsCFGHelper(p.getStatementList(), stmt.getStatementNum(), 0, nextStmt, adjList, ast);
                    }
                }
            } else if ((i == stmts.size() - 1) && (procedureReturn == 0)) {
                for (Procedure p : ast.getProcedureList()) {
                    if (p.getName() == name) {
                        bipsCFGHelper(p.getStatementList(), stmt.getStatementNum(), containerOutNext, 0, adjList, ast);
                    }
                }
            }
        }
    }
}
