#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
#include <random>
#include <iostream>
#include <bitset>
#include <set>

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
    int max_comparisons = 0;
    do {
        ++i;
        l.LOG_reset_node_stepped();
        l.LOG_reset_elt_comparisons();
        l.insert(x);
        auto c = l.LOG_get_node_stepped();
        auto d = l.LOG_get_elt_comparisons();
        max_comparisons = max(max_comparisons, d);
        printf("%lld\tsteps: %d\tcompares: %d\t%f\n", i, c, d, log2(i));
    } while (next_permutation(begin(x), end(x)));
    printf("max comparisons: %d\n", max_comparisons);
}

// I'd expect this to ONLY be 1
// Other interesting question: number of elements compared?
template<int L>
void count_asymptotic_behavior_degenerate() {
    vector<int> x(L);
    for (int i = 0; i < L; ++i) { x[L-(i+1)] = i; }
    skip_list<vector<int>, 32, good_height_generator> l;
    long long i = 0;
    int max_comparisons = 0;
    do {
        ++i;
        l.LOG_reset_node_stepped();
        l.LOG_reset_elt_comparisons();
        l.insert(x);
        auto c = l.LOG_get_node_stepped();
        auto d = l.LOG_get_elt_comparisons();
        max_comparisons = max(max_comparisons, d);
        printf("%lld\tsteps: %d\tcompares: %d\t%f\n", i, c, d, log2(i));
    } while (prev_permutation(begin(x), end(x)));
    printf("max comparisons: %d\n", max_comparisons);
}


long long logging_comparer_counter = 0;
struct logging_comparer {
    bool operator()(const int& a, const int& b) {
        logging_comparer_counter++;
        return a < b;
    }
};

void measure_compare_counts() {
    std::set<int, logging_comparer> s;
    for (int i = 0; i < 1000000; ++i) {
        s.insert(i);
    }
    cout << "Logging comparer for std::set: " << logging_comparer_counter << endl;
    logging_comparer_counter = 0;
    skip_list<int,32,good_height_generator,logging_comparer> l;
    for (int i = 0; i < 1000000; ++i) {
        l.insert(i);
    }
    cout << "Logging comparer for skiplist: " << logging_comparer_counter << endl;
}

int main() {
    measure_compare_counts();
    //count_asymptotic_behavior_degenerate<10>();
}
