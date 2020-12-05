#pragma once

#include <functional>
#include <string>
#include <utility>

/*
        These self-defined hashers are used by the PKB to hash pair containers to be stored
        in unordered_set. They use Szudzik's function found in
        https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
*/
struct intPairHash {
  public:
    template <typename T, typename U> size_t operator()(const std::pair<T, U> &x) const {
        T a = x.first;
        U b = x.second;

        return a ^ b;
    }
};

struct strIntHash {
  public:
    template <typename T, typename U> size_t operator()(const std::pair<T, U> &x) const {
        std::hash<std::string> strHasher;

        auto a = strHasher(x.first);
        U b = x.second;

        return a ^ b;
    }
};

struct intStrHash {
  public:
    template <typename T, typename U> size_t operator()(const std::pair<T, U> &x) const {
        std::hash<std::string> strHasher;

        T a = x.first;
        auto b = strHasher(x.second);

        return a ^ b;
    }
};

struct strPairHash {
  public:
    template <typename T, typename U> size_t operator()(const std::pair<T, U> &x) const {
        std::hash<std::string> strHasher;

        auto a = strHasher(x.first);
        auto b = strHasher(x.second);
        return a ^ b;
    }
};

struct comparator {
  public:
    template <typename T1, typename T2> bool operator()(const std::pair<T1, T2> &lhs, const std::pair<T1, T2> &rhs) const {
        if (lhs.first == rhs.first)
            return lhs.second < rhs.second;
        else
            return lhs.first < rhs.first;
    }
};

struct intPairComparator {
  public:
    bool operator()(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) const {
        return (lhs.first == rhs.first) && (lhs.second == rhs.second);
    }
};

struct strIntComparator {
  public:
    bool operator()(const std::pair<std::string, int> &lhs, const std::pair<std::string, int> &rhs) const {
        return (lhs.first == rhs.first) && (lhs.second == rhs.second);
    }
};

struct intStrComparator {
  public:
    bool operator()(const std::pair<int, std::string> &lhs, const std::pair<int, std::string> &rhs) const {
        return (lhs.first == rhs.first) && (lhs.second == rhs.second);
    }
};

struct strPairComparator {
  public:
    bool operator()(const std::pair<std::string, std::string> &lhs, const std::pair<std::string, std::string> &rhs) const {
        return (lhs.first == rhs.first) && (lhs.second == rhs.second);
    }
};

struct ProcRangeComparator {
  public:
    bool operator()(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) const {
        return lhs.first < rhs.first;
    }
};
