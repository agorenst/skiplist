#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

#include "skiplist.h"
#include <algorithm>

// Tests confirming that skip_list meets basic definition of a set.
// Reference: http://en.cppreference.com/w/cpp/container/set/set
// I'm going to try to match the latest-possible standard, but some
// of the things (Compare?) are a bit too abstract right now...

using namespace std;

int empty() { return 0; }

TEST_CASE("constructor()") {
    skip_list<int, 12, empty> l;
}
TEST_CASE("constructor(InputIn first, InputIn last)") {
    {
    int basic_iterators[] = {9,10,2,5,4};
    skip_list<int, 12, empty> l(begin(basic_iterators), end(basic_iterators));

    std::sort(begin(basic_iterators), end(basic_iterators));
    REQUIRE(std::equal(begin(basic_iterators), end(basic_iterators),
                       begin(l), end(l)));
    }

    {
    std::set<int> nontrivial_iterators{8,9,1,4,3};
    skip_list<int, 12, empty> l(begin(nontrivial_iterators), end(nontrivial_iterators));
    REQUIRE(std::equal(begin(nontrivial_iterators), end(nontrivial_iterators),
                       begin(l), end(l)));
    }
}

TEST_CASE("constructor{initialization_list}") {
    int basic_case[] = {9,10,2,5,4};
    skip_list<int, 12, empty> l{9,10,2,5,4};

    std::sort(begin(basic_case), end(basic_case));
    REQUIRE(std::equal(begin(basic_case), end(basic_case),
                       begin(l), end(l)));
}

TEST_CASE("size()") {
    skip_list<int, 12, empty> l;
    REQUIRE(l.size() == 0);
    l.insert(1);
    REQUIRE(l.size() == 1);
    int lots[5] = {2,3,4,5,6};
    l.insert(begin(lots), end(lots));
    REQUIRE(l.size() == 6);
    l.insert(4);
    REQUIRE(l.size() == 6);
    l.erase(1);
    REQUIRE(l.size() == 5);
    l.erase(3);
    l.erase(2);
    REQUIRE(l.size() == 3);
    l.erase(4);
    l.erase(5);
    l.erase(6);
    REQUIRE(l.size() == 0);
}

TEST_CASE("empty()") {
    skip_list<int, 12, empty> l;
    REQUIRE(l.empty());
    l.insert(4);
    REQUIRE(!l.empty());
    l.erase(4);
    REQUIRE(l.empty());
}

TEST_CASE("insert(value)") {
    skip_list<int, 12, empty> l;
    l.insert(2);
    REQUIRE(l.find(2) != l.end());
}

TEST_CASE("find(const Key& key)") {
    skip_list<int, 12, empty> l;
    REQUIRE(l.find(-1) == l.end());
    REQUIRE(l.find(13) == l.end());
    l.insert(2);
    REQUIRE(l.find(1) == l.end());
    REQUIRE(l.find(3) == l.end());
    REQUIRE(l.find(2) != l.end());

    skip_list<int, 12, empty> r;

    r.insert(2);
    REQUIRE(r.find(2) != r.end());
    REQUIRE(*r.find(2) == 2);
    REQUIRE(r.size() == 1);
}

