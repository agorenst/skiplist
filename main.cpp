#include <map>
#include "skiplist.h"
#include <string>
#include <iostream>


using namespace std;

int main() {
    slist<int,string> sl;
    sl.insert(10, "hello");
    sl.insert(4, "goodbye");
    for (auto it = sl.begin(); it != sl.end(); ++it) {
        cout << it->second << endl;
    }
}
