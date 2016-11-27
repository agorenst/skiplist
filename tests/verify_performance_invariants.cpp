#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

#define LOGGING_INFO

#include "skiplist.h"
#include <algorithm>
#include <climits>

// Tests confirming that the skip_list meets some expected
// sanity-checking performance guarantees.
// 
// This also has the effect of confirming our logging system.

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
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
    return ntz(dis(gen))-1;
}

TEST_CASE("inserting first element means 0 nodes") {
    skip_list<int, 32, good_height_generator> l;
    l.LOG_reset_node_stepped();
    l.insert(0);
    REQUIRE(l.LOG_get_node_stepped() == 0);
}

TEST_CASE("inserting in order assumes certain steps") {
    skip_list<int, 32, good_height_generator> l;
    l.LOG_reset_node_stepped();
    l.insert(5);
    REQUIRE(l.LOG_get_node_stepped() == 0);
    l.insert(4);
    REQUIRE(l.LOG_get_node_stepped() == 0);
    l.insert(3);
    REQUIRE(l.LOG_get_node_stepped() == 0);
    l.insert(2);
    REQUIRE(l.LOG_get_node_stepped() == 0);
    l.insert(1);
    REQUIRE(l.LOG_get_node_stepped() == 0);
    l.insert(0);
    REQUIRE(l.LOG_get_node_stepped() == 0);
}

// many more to do (consider deterministic height generator...)
