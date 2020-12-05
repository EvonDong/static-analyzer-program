#include <iostream>

#include "CallGraph.h"
#include <stack>

// ===================== CallGraph ====================================

CallGraph::CallGraph() : hasCycle{false} {
}

void CallGraph::addProcedure(std::string procedureName) {
    if (callGraph.find(procedureName) != callGraph.end()) {
        std::string errorMessage = "Err: Cannot have 2 procedures of the same name.";
        throw std::invalid_argument(errorMessage);
    }
    std::unordered_set<std::string> newList;
    callGraph.insert({procedureName, newList});
}

void CallGraph::addProcedureCall(std::string procedureName, std::string procedureCalled) {
    callGraph.find(procedureName)->second.emplace(procedureCalled);
}

std::unordered_set<std::string> &CallGraph::getProcedureList(std::string procedureName) {
    return callGraph.find(procedureName)->second;
}

bool CallGraph::isCyclic() {
    setCycle(false);
    std::unordered_set<std::string> white;
    std::unordered_set<std::string> grey;
    std::unordered_set<std::string> black;

    for (std::pair<std::string, std::unordered_set<std::string>> element : callGraph) {
        white.insert(element.first);
    }

    for (std::pair<std::string, std::unordered_set<std::string>> element : callGraph) {
        if (white.find(element.first) != white.end()) {
            dfs(element.first, white, grey, black);
        }
    }
    return hasCycle;
}

void CallGraph::dfs(std::string procedure, std::unordered_set<std::string> &white, std::unordered_set<std::string> &grey,
                    std::unordered_set<std::string> &black) {
    white.erase(procedure);
    grey.insert(procedure);
    for (std::string neighbours : callGraph.find(procedure)->second) {
        if (callGraph.find(neighbours) == callGraph.end()) {
            throw std::invalid_argument("Err: Call to non-existent procedure");
        }
        if (white.find(neighbours) != white.end()) {
            dfs(neighbours, white, grey, black);
        }
        if (grey.find(neighbours) != grey.end()) {
            setCycle(true);
        }
    }
    black.insert(procedure);
    grey.erase(procedure);
}

void CallGraph::setCycle(bool b) {
    hasCycle = b;
}

void CallGraph::clear() {
    callGraph.clear();
}

bool CallGraph::isEmpty() {
    return callGraph.empty();
}
