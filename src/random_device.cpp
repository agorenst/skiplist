#include <random>
#include <climits>
namespace skiplist_internal {
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

int number_of_time_3_divides(unsigned int x) {
    int count = 1;
    while (x % 3 == 0) { x /= 3; count++; }
    return count;
}

int good_height_generator() {
    auto x = dis(gen);
    //return number_of_time_3_divides(x);
    return ntz(x)+1;
}
};


