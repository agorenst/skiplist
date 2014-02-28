#include <iostream>
#include "skiplist.h"

using namespace std;

int main(int argc, char* argv[]) {
	slist<int,int> l;
	for (int i = 0; i < 30; ++i) {
		l.insert(rand() % 100, 1);
	}
	for (auto it = l.begin(); it != l.end(); ++it) {
		cout << it->first << " ";
	}
	cout << endl;
	cout << "Test" << endl;
}
