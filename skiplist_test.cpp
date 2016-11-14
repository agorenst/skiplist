#include "skiplist.h"

#include <random>
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1 << 30);

int ntz(unsigned x) {
    if (x == 0) { return 32; }
    int n = 1;
    if ((x & 0x0000FFFF) == 0) { n = n + 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n = n + 8; x >>= 8; }
    if ((x & 0x0000000F) == 0) { n = n + 4; x >>= 4; }
    if ((x & 0x00000003) == 0) { n = n + 2; x >>= 2; }
    return n - (x & 1);
}

int test_good() {
    int x = dis(gen);
    // hackers delight pg 12
    //x = x & (-x);
    return ntz(x);
    //int i = 0;
    //while (dis(gen)) ++i;
    //return i;
}
int test_bad() {
    return 0;
}

void performance_test() {
    const int perm_size = 9;
    vector<int> x(perm_size);
    skip_list<vector<int>, 12, test_good> l;
    for (int i = 0; i < perm_size; ++i) {
        x[i] = i;
    }
    long long i = 0;
    do {
        //++i;
        //l.LOG_reset_node_stepped();
        l.insert(x);
        //printf("%lld\t%d\n", i, l.LOG_get_node_stepped());
    } while (next_permutation(begin(x), end(x)));
//    for (auto&& x : l) {
//        for (auto&& e : x) { cout << e << " "; }
//        cout << endl;
//    }
}

int main() {
    performance_test();
}
