#include "skiplist.h"

#include <random>
#include <algorithm>
#include <iostream>
#include <limits>
#include <climits>
#include <set>

// This test is *super* noisy on my machine.

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

// dead-stupid timer thanks to http://stackoverflow.com/questions/2808398/easily-measure-elapsed-time
#include <chrono>

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
    template<typename F, typename ...Args>
        static typename TimeT::rep execution(F&& func, Args&&... args)
        {
            auto start = std::chrono::steady_clock::now();
            std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
            auto duration = std::chrono::duration_cast< TimeT> 
                (std::chrono::steady_clock::now() - start);
            return duration.count();
        }
};

//int main() {
//    std::cout << measure<>::execution(functor(dummy)) << std::endl;
//}


template<typename C, int L>
void always_add_larger_permutation() {
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

template<typename C, int L>
void always_add_smaller_permutation() {
    vector<int> x(L);
    C container;
    for (int i = 0; i < L; ++i) {
        x[L-(i+1)] = i;
    }
    do {
        // sanity check
        //for_each(begin(x), end(x), [](int x) { 
        //    cout << x << " ";
        //});
        //cout << endl;
        container.insert(x);
    } while(prev_permutation(begin(x), end(x)));
    auto first_elt = *begin(container);
    for_each(begin(first_elt), end(first_elt), [](int x) {
        cout << x << " ";
    });
    cout << endl;
}

template<typename C, int L>
void increasing_permutation_insert() {
}

int main() {
    typedef std::set<vector<int>> baseline_type;
    typedef skip_list<vector<int>, 32, good_height_generator> skiplist_type;
    const int INPUT_SIZE=10;

    cout << "baseline type: " << endl;
    auto t = measure<>::execution(always_add_larger_permutation<baseline_type, INPUT_SIZE>);
    cout << "call 1: " << t << endl;
    t = measure<>::execution(always_add_smaller_permutation<baseline_type, INPUT_SIZE>);
    cout << "call 2: " << t << endl;

    cout << "skiplist type: " << endl;
    t = measure<>::execution(always_add_larger_permutation<skiplist_type, INPUT_SIZE>);
    cout << "call 1: " << t << endl;
    t = measure<>::execution(always_add_smaller_permutation<skiplist_type, INPUT_SIZE>);
    cout << "call 2: " << t << endl;
}
