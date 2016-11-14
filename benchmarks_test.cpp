#include "skiplist.h"

#include <random>
#include <algorithm>
#include <iostream>
#include <limits>
#include <climits>
#include <set>

using namespace std;

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

int good_height_generator() {
    return ntz(dis(gen))-1;
}

template<typename C, int L>
void increasing_permutation_insert() {
    vector<int> x(L);
    C container;
    for (int i = 0; i < L; ++i) {
        x[i] = i;
    }
    do {
        container.insert(x);
    } while(next_permutation(begin(x), end(x)));
    auto first_elt = *begin(container);
    for_each(begin(first_elt), end(first_elt), [](int x) {
        cout << x << " ";
    });
    cout << endl;
}

int main() {
    typedef std::set<vector<int>> baseline_type;
    typedef skip_list<vector<int>, 32, good_height_generator> skiplist_type;

#ifdef STD_SET
    increasing_permutation_insert<baseline_type, 6>();
#elif SKIPLIST_SET
    increasing_permutation_insert<skiplist_type, 6>();
#endif
}
