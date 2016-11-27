#define CATCH_CONFIG_MAIN
#include "Catch/single_include/catch.hpp"

TEST_CASE("skip_list(): skip_list(Compare()) {}") {
}
TEST_CASE("explicit skip_list(const Compare& comp, const Allocator& alloc = Allocator())") {
}
TEST_CASE("explicit skip_list(const Allocator& alloc)") {
}
TEST_CASE("skip_list(InputIt first, InputIt last, cosnt Allocator& alloc): skip_list(first, last, Compare(), alloc) {})") {
}
TEST_CASE("skip_list(const skip_list& other)") {
}
TEST_CASE("skip_list(const skip_list& other, const Allocator& alloc)") {
}
TEST_CASE("skip_list(skip_list&& other)") {
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
}
TEST_CASE("skip_list& operator=(skip_list&& that)") {
}
TEST_CASE("skip_list& operator=(std::initializer_list<value_type> ilist)") {
}
