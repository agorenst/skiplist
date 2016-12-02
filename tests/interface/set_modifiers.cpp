#include "Catch/single_include/catch.hpp"

#include <initializer_list>
#include <cstdio>
#include <algorithm>
#include <set>

#include "NoisyClass.h"
int NoisyClass::COPY_COUNTER;
int NoisyClass::MOVE_COUNTER;
int NoisyClass::CONSTRUCTION_COUNTER;
int NoisyClass::LT_COUNTER;

#include "skiplist.h"

using namespace std;

template<typename InputIt>
void print_int_range(InputIt begin, InputIt end) {
    for_each(begin, end, [](int x) { printf("%d ", x); });
    printf("\n");
}
template<typename Container>
void print_int_range(const Container& c) {
    print_int_range(begin(c), end(c));
}

TEST_CASE("void clear()") {
    skip_list<int> l;
    REQUIRE(l.size() == 0);
    l.insert({1,2,3,4,5});
    print_int_range(l);
    REQUIRE(l.size() == 5);
    l.clear();
    REQUIRE(l.size() == 0);
}

TEST_CASE("std::pair<iterator,bool> insert(const value_type& value)") {
    auto n = NoisyClass(4);
    NoisyClass::reset_state();
    std::set<NoisyClass> s;

    auto set_pair = s.insert(n);
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 0);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 1);
    REQUIRE(set_pair.second);
    REQUIRE(set_pair.first->x == 4);

    NoisyClass::reset_state();
    set_pair = s.insert(n);
    REQUIRE(NoisyClass::LT_COUNTER == 2);
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 0);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    REQUIRE(!set_pair.second);
    REQUIRE(set_pair.first->x == 4);



    NoisyClass::reset_state();
    skip_list<NoisyClass> l;
    auto skip_pair = l.insert(n);
    REQUIRE(NoisyClass::LT_COUNTER == 0);
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 0);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 1);
    REQUIRE(skip_pair.second);
    REQUIRE(skip_pair.first->x == 4);

    NoisyClass::reset_state();
    skip_pair = l.insert(n);
    REQUIRE(NoisyClass::LT_COUNTER == 2);
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 0);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    REQUIRE(!skip_pair.second);
    REQUIRE(skip_pair.first->x == 4);
}
TEST_CASE("std::pair<iterator,bool> insert(value_type&& value)") {
    NoisyClass::reset_state();
    std::set<NoisyClass> s;

    auto set_pair = s.insert(NoisyClass(4));
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 1);
    REQUIRE(NoisyClass::MOVE_COUNTER == 1);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    REQUIRE(set_pair.second);
    REQUIRE(set_pair.first->x == 4);

    NoisyClass::reset_state();
    set_pair = s.insert(NoisyClass(4));
    REQUIRE(NoisyClass::LT_COUNTER == 2);
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 1);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    REQUIRE(!set_pair.second);
    REQUIRE(set_pair.first->x == 4);



    NoisyClass::reset_state();
    skip_list<NoisyClass> l;
    auto skip_pair = l.insert(NoisyClass(4));
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 1);
    REQUIRE(NoisyClass::MOVE_COUNTER == 1);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    REQUIRE(skip_pair.second);
    REQUIRE(skip_pair.first->x == 4);

    NoisyClass::reset_state();
    skip_pair = l.insert(NoisyClass(4));
    REQUIRE(NoisyClass::LT_COUNTER == 2);
    REQUIRE(NoisyClass::CONSTRUCTION_COUNTER == 1);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    REQUIRE(!skip_pair.second);
    REQUIRE(skip_pair.first->x == 4);
}
TEST_CASE("iterator insert(const_iterator hint, const value_type& value)") {
}
TEST_CASE("iterator insert(const_iterator hint, value_type&& value)") {
}
TEST_CASE("void insert(InputIt first, InputIt last)") {
    skip_list<int> l;
    int to_enter[] = {2,3,4,1,9,8};
    l.insert(begin(to_enter), end(to_enter));
    std::sort(begin(to_enter), end(to_enter));
    REQUIRE(std::equal(begin(l), end(l), begin(to_enter), end(to_enter)));
}
TEST_CASE("void insert(std::initializer_list<value_type> ilist)") {
    skip_list<int> l;
    auto to_enter = std::initializer_list<int>({2,3,4,1,9,8});
    l.insert(to_enter);
    std::set<int> baseline(to_enter);
    REQUIRE(std::equal(begin(l), end(l), begin(baseline), end(baseline)));
}
//TEST_CASE("insert_return_type insert(node_type&& nh)") {
//}
//TEST_CASE("iterator insert(const_iterator hint, node_type&& nh)") {
//}



// TODO: make sure we do the same number of copies and construction
// as a std::set, both when the element exists and when it doesn't.
TEST_CASE("std::pair<iterator,bool> emplace(Args&&... args)") {
    NoisyClass::reset_state();
    skip_list<NoisyClass> l;
    l.emplace(4);
    REQUIRE(NoisyClass::MOVE_COUNTER == 0);
    REQUIRE(NoisyClass::COPY_COUNTER == 0);
    auto has_four = l.find(NoisyClass(4));
    REQUIRE(has_four != l.end());
    printf("has_four value: %d\n", (*has_four).x);
    REQUIRE((*has_four) == NoisyClass(4));
    NoisyClass::reset_state();
}

TEST_CASE("std::pair<iterator,bool> emplace_hint(const_iterator hint, Args&&... args)") {
}

TEST_CASE("iterator erase(const_iterator pos)") {
}
TEST_CASE("iterator erase(const_iterator first, const_iterator last)") {
}
TEST_CASE("size_type erase(const key_type& key)") {
}

TEST_CASE("void swap(skip_list& other)") {
}
