#include "Catch/single_include/catch.hpp"

#include "skiplist.h"

#include <algorithm>

using namespace std;

TEST_CASE("skip_list(): skip_list(Compare()) {}") {
}
TEST_CASE("explicit skip_list(const Compare& comp, const Allocator& alloc = Allocator())") {
}
TEST_CASE("explicit skip_list(const Allocator& alloc)") {
}
TEST_CASE("skip_list(InputIt first, InputIt last, const Compare& comp = Compare(), const Allocator& alloc)") {
}
TEST_CASE("skip_list(InputIt first, InputIt last, const Allocator& alloc): skip_list(first, last, Compare(), alloc) {})") {
}
TEST_CASE("skip_list(const skip_list& other)") {
    skip_list<int> l{1,2,3,4,5,6};
    skip_list<int> r(l);
    REQUIRE(equal(begin(l), end(l), begin(r), end(r)));
}
TEST_CASE("skip_list(const skip_list& other, const Allocator& alloc)") {
}
TEST_CASE("skip_list(skip_list&& other)") {
    int seq[] = {1,2,3,4,5,6};
    skip_list<int> l(begin(seq), end(seq));
    skip_list<int> r(std::move(l));
    REQUIRE(equal(begin(l), end(l), begin(seq), end(seq)));
}
TEST_CASE("skip_list(skip_list&& other, const Allocator& alloc)") {
}
TEST_CASE("skip_list(std::initializer_list<value_type> init, const Compare& comp = Compare(), const Allocator& alloc = Allocator())") {
}
TEST_CASE("skip_list(std::initializer_list<value_type> init, const Allocator& alloc = Allocator()): skip_list(init, Compare(), alloc) {}") {
}
TEST_CASE("~skip_list()") {
}

TEST_CASE("skip_list& operator=(const skip_list& that)") {
    int seq[] = {1,2,3,4,5,6};
    skip_list<int> l{begin(seq), end(seq)};
    auto r = l;
    REQUIRE(equal(begin(l), end(l), begin(r), end(r)));
}
TEST_CASE("skip_list& operator=(skip_list&& that)") {
    int seq[] = {1,2,3,4,5,6};
    skip_list<int> l{begin(seq), end(seq)};
    auto r = std::move(l);
    REQUIRE(equal(begin(r), end(r), begin(seq), end(seq)));
}
TEST_CASE("skip_list& operator=(std::initializer_list<value_type> ilist)") {
    std::initializer_list<int> il = {1,2,3,4,5,6,7,8,9,10};
    skip_list<int> r{1,2,3};
    skip_list<int>& s = (r = il);
    REQUIRE(&s == &r);
    REQUIRE(equal(begin(il), end(il), begin(r), end(r)));
}
