#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
#include <random>
#include <iostream>
#include <bitset>

#define LOGGING_INFO
#include "skiplist.h"

using namespace std;

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
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1 << 30);
int good_height_generator() {
    return ntz(dis(gen))-1;
}
//
//void benchmark_ntz() {
//}
//
//void benchmark_uniform_int_distribution() {
//}
void print_binary(unsigned int x) {
    cout << std::bitset<sizeof(unsigned int)*CHAR_BIT>(x);
}
void verify_ntz() {
    for (int i = 0; i < 256; ++i) {
        print_binary(i); cout << "\t" << ntz(i) << endl;
    }
}


template<int L>
void count_asymptotic_behavior() {
    vector<int> x(L);
    for (int i = 0; i < L; ++i) { x[i] = i; }
    skip_list<vector<int>, 32, good_height_generator> l;
    long long i = 0;
    do {
        ++i;
        l.LOG_reset_node_stepped();
        l.insert(x);
        auto c = l.LOG_get_node_stepped();
        printf("%lld\t%d\t%f\n", i, c, log2(i));
    } while (next_permutation(begin(x), end(x)));
}

// I'd expect this to ONLY be 1
// Other interesting question: number of elements compared?
template<int L>
void count_asymptotic_behavior_degenerate() {
    vector<int> x(L);
    for (int i = 0; i < L; ++i) { x[L-(i+1)] = i; }
    skip_list<vector<int>, 32, good_height_generator> l;
    long long i = 0;
    do {
        ++i;
        l.LOG_reset_node_stepped();
        l.LOG_reset_elt_comparisons();
        l.insert(x);
        auto c = l.LOG_get_node_stepped();
        auto d = l.LOG_get_elt_comparisons();
        printf("%lld\t%d\t%d\t%f\n", i, c, d, log2(i));
    } while (prev_permutation(begin(x), end(x)));
}

int main() {
    //count_asymptotic_behavior<9>();
    count_asymptotic_behavior_degenerate<10>();
}
