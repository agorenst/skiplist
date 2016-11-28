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

int error_input_seq[] = {4 , 1 , 5 , 0 , 16 , 1 , 10 , 1 , 1 , 6 , 11 , 0 , 1 , 0 , 6 , 1 , 10 , 1 , 16 , 0 };
// Examining a benchmark, valgrind detected a memory leak. We were really
// screwing up a few fundamental things, including allowing the insertion
// of elements at the beginning of the list even if they were equal,
// and other things like that...
TEST_CASE("flushing out memory leak") {
    skip_list<int> l;
    set<int> s;
    for (int i = 0; i < 10; ++i) {
        int value = error_input_seq[2*i];
        int height = error_input_seq[(2*i)+1];
        if (height <= 0) { height = 1; }
        l.insert(value, height);
        s.insert(value);
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
}

// Unfortunately I neglected to comment this test properly,
// but I think it was a similar one in the vein of the next
// test case.
TEST_CASE("flushing out segfault") {
    skip_list<int> l;
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
    skip_list<int> l;
    set<int> s;
    l.insert(0,2);
    l.insert(7,3);
    l.insert(8,2);
    l.insert(13,2);
    l.erase(7);
    l.insert(12,2);
}


// I tried to be clever and streamline some comparisons, and
// I got caught in negating the <, <> vs equality, etc.
// A simple error in "erase", yet again...
TEST_CASE("assertion failure after changing to Compare") {
    skip_list<int> l;
    set<int> s;
    int i = 0;
    int latest_error[] = { 0, 2, 1,     0, 1, -1,     -999};
    while(latest_error[i] != -999) {
        int value = latest_error[i++];
        int height = latest_error[i++];
        int to_insert = latest_error[i++];
        if (to_insert == 1) {
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


// I measured that, in a series of random insertion both over
// a small and large domain, my skip list took about 1.8 comparisons
// for ever 1 comparison std::set used.
//
// In an experiment to see if it would help, I started trying to implement
// a binary-search (rather than linear search) technique for insertion.
// Not obvious it would help, hence, experiment.
//
// Along the way, I found that a certain series of insertions and removals
// would violate the invariant that each "stack" of links would be contiguous
// non-null, then contiguous-null. This input, somehow, lead to head
// have null in the [1] position, but non-null in the [0], [2] positions.
//
// The cause: a small error in erasing, where I would always set "head[i]" to
// nullptr. I really need to refactor how finding predecessors notates
// that the heads list is the predecessor. Nontrivial design question.
TEST_CASE("invariant violation") {
    int invariant_violation[] = {0, 1, 1,
        3, 1, 1,
        4, 3, 1,
        2, 2, 1,
        0, 1, 1,
        1, 1, 1,
        3, 2, 1,
        1, 1, 1,
        3, 3, 1,
        2, 2, 1,
        2, 2, -1,
        15, 1, 1,
        -999};
    skip_list<int> l;
    set<int> s;
    int i = 0;
    while(invariant_violation[i] != -999) {
        int value = invariant_violation[i++];
        int height = invariant_violation[i++];
        int to_insert = invariant_violation[i++];
        printf("%d %d %d\n", value, height, to_insert);
        l.dbg_print();
        if (to_insert == 1) {
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
