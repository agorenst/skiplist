#include "Catch/single_include/catch.hpp"

#include "skiplist.h"
#include <array>
#include <algorithm>
#include <iostream>

using namespace std;

// My rough draft implementation of "find" was incredibly buggy. Here, we're going to spell it out.
//
TEST_CASE("failure one") {
    skip_list<int> l;
    l.insert(0, 1);
    l.insert(1, 1);
    REQUIRE(l.find(0) != l.end());
    REQUIRE(l.find(1) != l.end());
}

TEST_CASE("Linear find test (using randomness)") {
    const int N = 2000;
    skip_list<int> l;
    for (int i = 0; i < N; ++i) {
        auto h = l.generate_height();
        //cout << i  << ", " << h << ", ";
        l.insert(i, h);
        for (int j = 0; j <= i; ++j) {
            auto it = l.find(j);
            REQUIRE(it != l.end());
            REQUIRE(*it == j);
        }
    }
}
