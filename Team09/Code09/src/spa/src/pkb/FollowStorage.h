#pragma once

#include "constants.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

class FollowStorage {
  private:
    static FollowStorage *inst_;

    std::unordered_map<int, std::unordered_set<int>> followsStarMap;
    std::unordered_map<int, int> followsMap;
    std::unordered_map<int, int> followedMap;
    std::unordered_map<int, std::unordered_set<int>> followedStarMap;

    void addEdge(int v, int w);
    void DFSUtil(int curr, int v, std::unordered_set<int> visited);

  public:
    static FollowStorage *getInstance();

    void setFollows(int stmt1, int stmt2);
    void setFollowsStar(int stmt1, int stmt2);
    bool isFollows(int stmt1, int stmt2);
    bool isFollowsStar(int stmt1, int stmt2);
    void populateFollowsStarMap();

    std::unordered_set<int> getAllFollowsStar(int stmt);
    std::unordered_set<int> getAllFollows(int stmt);
    std::unordered_set<int> getAllLeadingStar(int stmt);
    std::unordered_set<int> getAllLeading(int stmt);

    std::unordered_map<int, int> *getAllFollowsRelationPair();
    std::unordered_map<int, std::unordered_set<int>> *getAllFollowsRelationPairT();

    void clear();
};
