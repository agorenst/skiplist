#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

//#define LOGGING_INFO
//#define SKIPLIST_DEBUG
#include "skiplist.h"
#include <algorithm>
#include <set>
#include <cstdio>

// Tests confirming that skip_list meets basic definition of a set.
// Reference: http://en.cppreference.com/w/cpp/container/set/set
// I'm going to try to match the latest-possible standard, but some
// of the things (Compare?) are a bit too abstract right now...

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1 << 30);

// From hacker's delight
int ntz(unsigned int x) {
    static_assert(sizeof(unsigned int) == 4, "Assuming 4-byte integers");
    if (x == 0) { return 32; }
    int n = 1;
    if ((x & 0x0000FFFF) == 0) { n = n + 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n = n + 8; x >>= 8; }
    if ((x & 0x0000000F) == 0) { n = n + 4; x >>= 4; }
    if ((x & 0x00000003) == 0) { n = n + 2; x >>= 2; }
    return n - (x & 1);
}

int good_height_generator() {
    return ntz(dis(gen));
}

int empty() { return 0; }


TEST_CASE("Random set of insertions and deletions.") {
    const int N = 17;
    const int M = 17;
    skip_list<int, 32, good_height_generator> l;
    set<int> s;
    for (int i = 0; i < 1000000; ++i) {
        int value = dis(gen) % N;
        int height = good_height_generator()+1;
        //printf("%d, %d, ", value, height);
        if (dis(gen) % M) {
            //printf("1,\n");
            l.insert(value, height);
            s.insert(value);
        }
        else {
            //printf("-1,\n");
            l.erase(value);
            s.erase(value);
        }
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
}

