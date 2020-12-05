#include "FollowStorage.h"

FollowStorage *FollowStorage::inst_ = NULL;

FollowStorage *FollowStorage::getInstance() {
    if (inst_ == NULL) {
        inst_ = new FollowStorage();
    }
    return inst_;
}

void FollowStorage::setFollows(int stmt1, int stmt2) {
    if (stmt1 < stmt2) {
        followsMap[stmt1] = stmt2;
        followedMap[stmt2] = stmt1;
    }
}

void FollowStorage::setFollowsStar(int stmt1, int stmt2) {
    if (followsStarMap.find(stmt1) != followsStarMap.end()) {
        followsStarMap.at(stmt1).emplace(stmt2);
    } else {
        std::unordered_set<int> stmtList;
        stmtList.emplace(stmt2);
        followsStarMap.emplace(stmt1, stmtList);
    }

    if (followedStarMap.find(stmt2) != followedStarMap.end()) {
        followedStarMap.at(stmt2).emplace(stmt1);
    } else {
        std::unordered_set<int> stmtList;
        stmtList.emplace(stmt1);
        followedStarMap.emplace(stmt2, stmtList);
    }
}

bool FollowStorage::isFollows(int stmt1, int stmt2) {
    if (stmt1 < stmt2) {
        auto itr = followsMap.find(stmt1);
        return itr != followsMap.end() && itr->second == stmt2;
    }
    return false;
}

bool FollowStorage::isFollowsStar(int stmt1, int stmt2) {
    if (followsStarMap.find(stmt1) != followsStarMap.end()) {
        if (followsStarMap.at(stmt1).find(stmt2) != followsStarMap.at(stmt1).end()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

std::unordered_set<int> FollowStorage::getAllFollowsStar(int stmt) {
    if (followsStarMap.find(stmt) != followsStarMap.end()) {
        return followsStarMap.at(stmt);
    }
    std::unordered_set<int> empty;
    return empty;
}

std::unordered_set<int> FollowStorage::getAllFollows(int stmt) {
    if (followsMap.find(stmt) != followsMap.end()) {
        std::unordered_set<int> res;
        res.emplace(followsMap[stmt]);
        return res;
    }
    std::unordered_set<int> empty;
    return empty;
}

std::unordered_set<int> FollowStorage::getAllLeadingStar(int stmt) {
    if (followedStarMap.find(stmt) != followedStarMap.end()) {
        return followedStarMap.at(stmt);
    }
    std::unordered_set<int> empty;
    return empty;
}

std::unordered_set<int> FollowStorage::getAllLeading(int stmt) {
    if (followedMap.find(stmt) != followedMap.end()) {
        std::unordered_set<int> res;
        res.emplace(followedMap[stmt]);
        return res;
    }
    std::unordered_set<int> empty;
    return empty;
}

void FollowStorage::addEdge(int v, int w) {
    setFollowsStar(v, w);
}

void FollowStorage::DFSUtil(int curr, int v, std::unordered_set<int> visited) {
    visited.emplace(v);
    if (followsMap.find(v) != followsMap.end()) {
        if (visited.find(followsMap.at(v)) == visited.end()) {
            setFollowsStar(curr, followsMap.at(v));
            DFSUtil(curr, followsMap.at(v), visited);
        }
    }
}

/*
 * Description: Populates the followsStarMap
 * based on the followsMap.
 */
void FollowStorage::populateFollowsStarMap() {

    std::unordered_map<int, int>::iterator itr;
    for (itr = followsMap.begin(); itr != followsMap.end(); ++itr) {
        std::unordered_set<int> *visited = new std::unordered_set<int>;
        DFSUtil((*itr).first, (*itr).first, *visited);
    }
}

std::unordered_map<int, int> *FollowStorage::getAllFollowsRelationPair() {
    return &followsMap;
}

std::unordered_map<int, std::unordered_set<int>> *FollowStorage::getAllFollowsRelationPairT() {
    return &followsStarMap;
}

void FollowStorage::clear() {
    followsMap.clear();
    followsStarMap.clear();
    followedMap.clear();
    followedStarMap.clear();
}
