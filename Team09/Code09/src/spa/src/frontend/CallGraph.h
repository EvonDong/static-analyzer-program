#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

/*
 * CallGraph contains a mapping of procedures to procedures that are called within it.
 */
class CallGraph {
  private:
    std::unordered_map<std::string, std::unordered_set<std::string>> callGraph;
    bool hasCycle;

  public:
    //==================================================================
    // APIs
    //==================================================================

    /*
     * Description: Constructor for creating CallGraph
     */
    CallGraph();

    /*
     * Description: Adds a Procedure into the CallGraph.
     */
    void addProcedure(std::string procedureName);

    /*
     * Description: Adds a Procedure into the CallGraph.
     */
    void addProcedureCall(std::string procedureName, std::string procedureCalled);

    /*
     * Description: Returns a reference to the list of procedures called by procedureName.
     */
    std::unordered_set<std::string> &getProcedureList(std::string procedureName);

    /*
     * Description: Checks for the presence of cyclic calls in the CallGraph.
     */
    bool isCyclic();

    /*
     * Description: Helper function that performs DFS on the CallGraph.
     */
    void dfs(std::string procedure, std::unordered_set<std::string> &white, std::unordered_set<std::string> &grey,
             std::unordered_set<std::string> &black);

    void setCycle(bool b);

    /*
     * Description: Clears all elements in the callGraph.
     */
    void clear();

    /*
     * Description: Checks if the callGraph is empty.
     */
    bool isEmpty();
};
