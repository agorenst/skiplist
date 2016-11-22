#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

//#define LOGGING_INFO
//#define SKIPLIST_DEBUG
#include "skiplist.h"
#include <algorithm>
#include <set>
#include <cstdio>

// Collecting specific cases that highlighted errors found in my implementation.
// It may be interesting to revisit these and see what sort of "invariants" I had
// neglected at certain points.

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
    return ntz(dis(gen))-1;
}

int empty() { return 0; }


int error_input_seq[] = {4 , 1 , 5 , 0 , 16 , 1 , 10 , 1 , 1 , 6 , 11 , 0 , 1 , 0 , 6 , 1 , 10 , 1 , 16 , 0 };
//int error_input_seq[] = {4 , 1 , 5 , 0 , 16 , 1 , 10 , 1 , 1 , 6 , 11 , 0 , 1 , 0 , 6 , 1 , 10 , 1 , 16 , 0 , 11 , 4 , 14 , 2 , 10 , 2 , 11 , 0 , 13 , 1 , 2 , 2 , 11 , 1 , 6 , 0 , 3 , 1 , 9 , 3 , 6 , 0 , 9 , 0 , 15 , 0 , 15 , 1 , 16 , 3 , 1 , 0 , 16 , 1 , 8 , 0 , 16 , 0 , 16 , 0 , 11 , 1 , 8 , 1 , 5 , 0 , 5 , 1 , 10 , 0 , 12 , 0 , 1 , 2 , 2 , 0 , 5 , 1 , 16 , 2 , 16 , 2 , 4 , 0 , 13 , 5 , 8 , 1 , 3 , 0 , 0 , 0 , 9 , 1 , 11 , 3 , 12 , 1 , 13 , 0 , 4 , 0 , 15 , 1 , 6 , 0 , 11 , 2 , 4 , 1 , 8 , 2 , 5 , 0 , 15 , 0 , 8 , 1 , 16 , 0 , 0 , 0 , 5 , 1 , 9 , 2 , 9 , 0 , 13 , 1 , 6 , 2 , 7 , 0 , 16 , 1 , 6 , 0 , 0 , 1 , 12 , 1 , 16 , 2 , 10 , 0 , 0 , 0 , 10 , 0 , 11 , 2 , 10 , 2 , 0 , 0 , 15 , 1 , 11 , 1 , 14 , 0 , 5 , 0 , 12 , 1 , 9 , 1 , 16 , 1 , 9 , 1 , 11 , 0 , 13 , 1 , 0 , 2 , 1 , 0 , 6 , 3 , 12 , 2 , 12 , 0 , 11 , 0 , 10 , 4 , 15 , 1 , 6 , 0 , 5 , 2 , 7 , 0 , 5 , 0};

// Examining a benchmark, valgrind detected a memory leak. We were really
// screwing up a few fundamental things, including allowing the insertion
// of elements at the beginning of the list even if they were equal,
// and other things like that...
TEST_CASE("flushing out memory leak") {
    skip_list<int, 32, good_height_generator> l;
    set<int> s;
    for (int i = 0; i < 10; ++i) {
        int value = error_input_seq[2*i];
        int height = error_input_seq[(2*i)+1];
        if (height <= 0) { height = 1; }
        l.insert(value, height);
        s.insert(value);
        //printf("Inserted: %d %d\n", value, height);
        //for_each(begin(l), end(l), [](int x) { printf("%d ", x); });
        //printf("\n");
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
    //for (int i = 0; i < 100; ++i) {
    //    int value = dis(gen) % 17;
    //    int height = good_height_generator()+1;
    //    printf("%d %d\n", value, height);
    //    l.insert(value, height);
    //}
}

// Unfortunately I neglected to comment this test properly,
// but I think it was a similar one in the vein of the next
// test case.
TEST_CASE("flushing out segfault") {
    skip_list<int, 32, good_height_generator> l;
    l.insert(8,1);
    l.insert(6,3);
    l.insert(12,1);
    l.insert(3,1);
    l.insert(5,1);
    l.insert(9,1);
    l.erase(14);
    REQUIRE(true);
}

// Say we're deleting the unique tallest element in the
// linked list. In that case, heads[i>0] points to it,
// but heads[j < i] may not.
// Because the predecessors slice never points to heads
// directly, we have to be sure that, after processing
// the contents of the predecessor slice, we continue on
// and check the heads[] slice as well.
// We neglected to do that here, so after removing 7 we
// left heads[2] still pointing to that node.
TEST_CASE("new segfault found via random_test") {
    skip_list<int, 32, good_height_generator> l;
    set<int> s;
    l.insert(0,2);
    l.insert(7,3);
    l.insert(8,2);
    l.insert(13,2);
    l.erase(7);
    l.insert(12,2);
}
