#include <iostream>
#include "skiplist.h"

using namespace std;


int main() {
    skiplist_multiset<int> test;
    for (int i = 0; i < 10; ++i) {
        test.insert(i);
    }
    for (int i = 0; i < 10; ++i) {
        test.insert(10-i);
    }
    test.print(cout);
    cout << endl;
}

