#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

#include "skiplist.h"
#include <algorithm>

using namespace std;

template<typename R>
void print_range(R& r) {
    for (auto&& x : r) {
        printf("%d ", x);
    }
    printf("\n");
}

int empty() { return 0; }

TEST_CASE("Exhaustive insert/remove/ordering test (no skipping)") {
    const int TS = 8;
    std::array<int,TS> input_perm;
    for (int i = 0; i < TS; ++i) {
        int val = i-(TS/2);
        if (val >= 0) { ++val; }
        input_perm[i] = val;
    }
    do {
        skip_list<int, 12, empty> l;
        std::set<int> baseline;
        REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
        for (auto x : input_perm) {
            if (x > 0) {
                baseline.insert(x);
                l.insert(x);
            }
            else {
                baseline.erase(x);
                l.erase(x);
            }
            REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
        }
    } while(next_permutation(begin(input_perm), end(input_perm)));
}

// return the 4 different heights in order,
// but every 4 invocations change the starting order...
int different_four_heights() {
    static int heights[4] = {0,3,6,9};
    static int to_ret = 0;

    if (to_ret == 4) {
        to_ret = 0;
        next_permutation(begin(heights), end(heights));
    }
    return heights[to_ret++];
}

TEST_CASE("helper test -- verify different_four_heights") {
    REQUIRE(different_four_heights() == 0);
    REQUIRE(different_four_heights() == 3);
    REQUIRE(different_four_heights() == 6);
    REQUIRE(different_four_heights() == 9);

    REQUIRE(different_four_heights() == 0);
    REQUIRE(different_four_heights() == 3);
    REQUIRE(different_four_heights() == 9);
    REQUIRE(different_four_heights() == 6);

    for (int i = 0; i < 22; ++i) {
        for (int j = 0; j < 4; ++j) {
            different_four_heights();
        }
    }
    REQUIRE(different_four_heights() == 0);
    REQUIRE(different_four_heights() == 3);
    REQUIRE(different_four_heights() == 6);
    REQUIRE(different_four_heights() == 9);

    REQUIRE(different_four_heights() == 0);
    REQUIRE(different_four_heights() == 3);
    REQUIRE(different_four_heights() == 9);
    REQUIRE(different_four_heights() == 6);

    // reset different_four_heights back to starting state.
    for (int i = 0; i < 22; ++i) {
        for (int j = 0; j < 4; ++j) {
            different_four_heights();
        }
    }
}

TEST_CASE("Exhaustive cases with different heights") {
    const int TS = 8;
    std::array<int, TS> input_perm;
    for (int i = 0; i < TS; ++i) {
        int val = i-(TS/2);
        if (val >= 0) { ++val; }
        input_perm[i] = val;
    }
    do {
        for (int i = 0; i < 24; ++i) {
            skip_list<int, 12, different_four_heights> l;
            std::set<int> baseline;
            REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
            for (auto x : input_perm) {
                if (x > 0) {
                    baseline.insert(x);
                    l.insert(x);
                }
                else {
                    baseline.erase(x);
                    l.erase(x);
                }
                REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
            }
        }
    } while(next_permutation(begin(input_perm), end(input_perm)));
}

// return the 4 different heights in order,
// but every 4 invocations change the starting order...
int different_four_heights_tight() {
    static int heights[4] = {2,3,4,5};
    static int to_ret = 0;

    if (to_ret == 4) {
        to_ret = 0;
        next_permutation(begin(heights), end(heights));
    }
    return heights[to_ret++];
}

TEST_CASE("Exhaustive cases with different heights (tight)") {
    const int TS = 8;
    std::array<int, TS> input_perm;
    for (int i = 0; i < TS; ++i) {
        int val = i-(TS/2);
        if (val >= 0) { ++val; }
        input_perm[i] = val;
    }
    do {
        for (int i = 0; i < 24; ++i) {
            skip_list<int, 12, different_four_heights_tight> l;
            std::set<int> baseline;
            REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
            for (auto x : input_perm) {
                if (x > 0) {
                    baseline.insert(x);
                    l.insert(x);
                }
                else {
                    baseline.erase(x);
                    l.erase(x);
                }
                REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
            }
        }
    } while(next_permutation(begin(input_perm), end(input_perm)));
}
