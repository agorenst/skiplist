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

TEST_CASE("operator=(const skip_list& that") {
    skip_list<int, 4, empty> l;
    skip_list<int, 4, empty> r;
    l.insert(1);
    r.insert(2);
    l = r;
    REQUIRE(l.size() == 1);
    REQUIRE(l.find(2) != l.end());
    REQUIRE(r == l);
}

TEST_CASE("insert(value)") {
    skip_list<int, 12, empty> l;
    l.insert(2);
    REQUIRE(l.find(2) != l.end());
}
TEST_CASE("insert(move)") {
    skip_list<std::vector<int>, 12, empty> l;
    std::vector<int> test_input{1,2,3,4,5,6,7};
    std::vector<int> checker(test_input);
    auto result = l.insert(std::move(test_input));
    // Checking the basic results.
    REQUIRE(std::get<1>(result));
    REQUIRE(l.find(checker) != l.end());
    REQUIRE(std::get<0>(result) == l.find(checker));
}
TEST_CASE("insert(begin, end)") {
    skip_list<int, 12, empty> l;
    int to_enter[] = {2,3,4,1,9,8};
    l.insert(begin(to_enter), end(to_enter));
    std::sort(begin(to_enter), end(to_enter));
    REQUIRE(std::equal(begin(l), end(l), begin(to_enter), end(to_enter)));
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


TEST_CASE("operator== function") {
    skip_list<int, 12, empty> l;
    skip_list<int, 12, empty> r;
    l.insert(1);
    l.insert(10);
    l.insert(5);
    r.insert(1);
    r.insert(5);
    r.insert(10);
    REQUIRE(l == r);
    r.insert(11);
    REQUIRE(l != r);
}

