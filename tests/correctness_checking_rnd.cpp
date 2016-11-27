#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

#define LOGGING_INFO
//#define SKIPLIST_DEBUG
#include "skiplist.h"
#include <algorithm>
#include <set>
#include <iostream>

// This complements the small, exhaustive tests we have in other files.
// This does a larger series of inserts and removals and makes sure
// that on each operation the skip_list still looks like a set.
//
// We output the random seed for repeatability, in the case that this
// random test does in fact fail.

using namespace std;

std::random_device rd;
std::mt19937 gen;
std::uniform_int_distribution<> dis(0, 1 << 30);

// From hacker's delight
int ntz(unsigned int x) {
    static_assert(sizeof(unsigned int) == 4, "Assuming 4-byte integers");
    static_assert(CHAR_BIT == 8, "Assuming 8-bit bytes");
    if (x == 0) { return 32; }
    int n = 1;
    if ((x & 0x0000FFFF) == 0) { n = n + 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n = n + 8; x >>= 8; }
    if ((x & 0x0000000F) == 0) { n = n + 4; x >>= 4; }
    if ((x & 0x00000003) == 0) { n = n + 2; x >>= 2; }
    return n - (x & 1);
}

int good_height_generator() {
    return ntz(dis(gen))+1;
}

typedef skip_list<int,31, good_height_generator> test_type;
TEST_CASE("Test over small range") {
    auto seed = rd();
    cout << "Random seed: " << seed << endl;
    gen.seed(seed);
    const int N = 17;
    const int M = 17;
    test_type l;
    set<int> s;
    for (int i = 0; i < 10000000; ++i) {
        int value = dis(gen) % N;
        int height = min(29, good_height_generator()+1);
        if (dis(gen) % M) {
            l.insert(value, height);
            s.insert(value);
        }
        else {
            l.erase(value);
            s.erase(value);
        }
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
}
TEST_CASE("Test over large range") {
    auto seed = rd();
    cout << "Random seed: " << seed << endl;
    gen.seed(seed);
    const int N = 15485867;
    const int M = 17;
    test_type l;
    set<int> s;
    for (int i = 0; i < 1000; ++i) {
        int value = dis(gen) % N;
        int height = min(29, good_height_generator()+1);
        if (dis(gen) % M) {
            l.insert(value, height);
            s.insert(value);
        }
        else {
            l.erase(value);
            s.erase(value);
        }
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
}
