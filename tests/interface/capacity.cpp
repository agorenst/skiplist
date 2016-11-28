#include "Catch/single_include/catch.hpp"

#include "skiplist.h"

TEST_CASE("bool empty() const") {
    skip_list<int> l;
    REQUIRE(l.empty());
    l.insert(4);
    REQUIRE(!l.empty());
    l.erase(4);
    REQUIRE(l.empty());
}

TEST_CASE("size_type size() const") {
}

TEST_CASE("size_type max_size() const") {
}
