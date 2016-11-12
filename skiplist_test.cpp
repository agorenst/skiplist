#include "skiplist.h"

#include <random>
#include <algorithm>
#include <iostream>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1);

int test_good() {
    int i = 0;
    while (dis(gen)) ++i;
    return i;
}
int test_bad() {
    return 0;
}

void big_test() {
    skip_list<int, 12, test_bad> l;
    l.insert(3);
    l.insert(2);
    l.insert(7);
    l.insert(5);
    l.insert(6);
    l.insert(4);
    l.dbg_print_list();
    printf("About to remove 7\n");
    l.erase(7);
    l.dbg_print_list();
    printf("About to remove 2\n");
    l.erase(2);
    l.dbg_print_list();
    printf("About to remove 5\n");
    l.erase(5);
    l.dbg_print_list();

    for (auto&& x : l) {
        cout << x << " ";
    }
    cout << endl;
}
//
//void small_delete_test() {
//    skip_list<int, 12> l;
//    l.insert(2);
//    l.dbg_print_list();
//    l.erase(2);
//    l.dbg_print_list();
//}
//

void performance_test() {
    const int perm_size = 7;
    vector<int> x(perm_size);
    skip_list<vector<int>, 12, test_good> l;
    for (int i = 0; i < perm_size; ++i) {
        x[i] = i;
    }
    do {
        l.insert(x);
    } while (next_permutation(begin(x), end(x)));
    for (auto&& x : l) {
        for (auto&& e : x) { cout << e << " "; }
        cout << endl;
    }
}

int main() {
    performance_test();
}
