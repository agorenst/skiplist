#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

#define LOGGING_INFO
#define DEBUG_MODE
#include "skiplist.h"
#include <algorithm>
#include <set>

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

TEST_CASE("move constructor") {
    skip_list<int, 12, empty> l;
    skip_list<int, 12, empty> r;
    l.insert(1);
    l.dbg_print();
    r.dbg_print();
    r = std::move(l);
    REQUIRE(r.find(1) != r.end());
}

#include <cstdio>
int COPY_COUNTER;
int MOVE_COUNTER;
class noisy {
    public:
    int x;
    noisy(const noisy& n): x(n.x+1) {
        printf("copy constructor %d %d\n", n.x, x);
        COPY_COUNTER++;
    }
    noisy(noisy&& n): x(n.x+1) {
        printf("move constructor %d %d\n", n.x, x);
        MOVE_COUNTER++;
    }
    noisy(): x(0) {
        printf("default constructor %d\n", x);
    }
    explicit noisy(int x): x(x) {
        printf("parameter constructor %d\n", x);
    }
    static void reset_state() {
        MOVE_COUNTER = 0;
        COPY_COUNTER = 0;
    }
    bool operator==(const noisy& n) const {
        return x == n.x;
    }
    bool operator>=(const noisy& n) const {
        return x >= n.x;
    }
    bool operator>(const noisy& n) const {
        return x > n.x;
    }
    bool operator<(const noisy& n) const {
        return x < n.x;
    }
    operator int() { return x; }
};

// If the iterator returns a copy, then it would
// invok ethe copy cosntructor and it's x would be 5
TEST_CASE("make sure iterator returns reference") {
    skip_list<noisy, 12, empty> l;
    l.emplace(4);
    auto iter = l.find(noisy(4));
    REQUIRE((*iter).x == 4);
}

TEST_CASE("emplace(Args&&... args)") {
    noisy::reset_state();
    skip_list<noisy, 12, empty> l;
    l.emplace(4);
    REQUIRE(MOVE_COUNTER == 0);
    REQUIRE(COPY_COUNTER == 0);
    auto has_four = l.find(noisy(4));
    REQUIRE(has_four != l.end());
    printf("has_four value: %d\n", (*has_four).x);
    REQUIRE((*has_four) == noisy(4));
    noisy::reset_state();
}

TEST_CASE("insert(value_type& v) only copies once") {
    noisy::reset_state();
    noisy tester(4);
    skip_list<noisy, 12, empty> l;
    l.insert(tester);
    REQUIRE(MOVE_COUNTER == 0);
    REQUIRE(COPY_COUNTER == 1);
    noisy::reset_state();
}

TEST_CASE("insert(value_type&&) move-constructs once, no copies") {
    printf("Starting RVO test...");
    noisy tester(4);
    skip_list<noisy, 12, empty> l;
    noisy::reset_state();
    l.insert(std::move(tester));
    REQUIRE(MOVE_COUNTER == 1);
    REQUIRE(COPY_COUNTER == 0);
    noisy::reset_state();
}

// This was just an older test that I used to explore
// emplace vs insert and so on...
TEST_CASE("emplace(Args&&... args) bigger test") {
    std::set<noisy> s;
    noisy big_noisy(10);
    printf("Standard set reaction:\n");
    printf("emplacing:\n");
    s.emplace(7);
    printf("inserting:\n");
    s.insert(big_noisy);
    printf("Now going to skiplist:\n");
    printf("emplace test\n");
    noisy tester(4);
    noisy::reset_state();
    skip_list<noisy, 12, empty> l;
    l.emplace(4);
    REQUIRE(MOVE_COUNTER == 0);
    REQUIRE(COPY_COUNTER == 0);
    noisy::reset_state();

    l.insert(big_noisy);
    REQUIRE(MOVE_COUNTER == 0);
    REQUIRE(COPY_COUNTER == 1);
    noisy::reset_state();
};

// More things to test:
// 1) Using template parameters (compare, allocators)
// 1a) Memory usage: make sure we're using the allocators in the way we expect.
// 2) Comparison operators
// 3) Exception safety (what's supposed to happen in OOM?)
// 4) Iterator const-ness and the like.
