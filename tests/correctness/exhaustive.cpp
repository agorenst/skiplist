#include "Catch/single_include/catch.hpp"

#include "skiplist.h"
#include <array>
#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

template<class Iter>
bool next_input_sequence(Iter start, Iter finish) {
    if (start == finish) { return false; }
    for (;;) {
        if (*start == -1) {
            *start = 1;
            return true;
        }
        else if (*start == 4) {
            *start = -4;
            ++start;
            if (start == finish) {
                return false;
            }
        }
        else {
            (*start)++;
            return true;
        }
    }
}

template<class Iter>
bool next_height_sequence(Iter start, Iter finish) {
    if (start == finish) { return false; }
    for (;;) {
        if (*start == 4) {
            *start = 1;
            ++start;
            if (start == finish) {
                return false;
            }
        }
        else {
            (*start)++;
            return true;
        }
    }
}

// I really want N = 7, but that's a lot... Maybe about 30 minutes to run.
const int N = 5;
TEST_CASE("Exhaustive case 1") {
    array<int,N> input_sequence;
    fill(begin(input_sequence), end(input_sequence), -4);
    array<int,N> height_sequence;
    fill(begin(height_sequence), end(height_sequence), 1);
    long long int count = 0;
    bool no_failure = true;
    do {
        if (input_sequence[0] < 0) { continue; }
        //for_each(begin(input_sequence), end(input_sequence), [](int x) {
        //        cout << x << " ";
        //        });
        //cout << endl;
        int positive_count = count_if(begin(input_sequence), end(input_sequence),
                [](int x) { return x > 0; });
        do {
            ++count;
        //    cout << "\t";
        //    for_each(begin(height_sequence), end(height_sequence), [](int x) {
        //            cout << x << " ";
        //            });
        //    cout << endl;

            skip_list<int> l;
            set<int> s;
            int i = 0;
            int j = 0;
            while (i < N) {
                int val_to_insert = input_sequence[i++];
                if (val_to_insert < 0) {
                    val_to_insert = abs(val_to_insert);
                    l.erase(val_to_insert);
                    s.erase(val_to_insert);
                }
                else {
                    l.insert(val_to_insert, height_sequence[j++]);
                    s.insert(val_to_insert);
                }
                no_failure &= equal(begin(l), end(l), begin(s), end(s));
            }
            if (count % 1000000 == 0) { cout << count << endl; }
        }
        while (next_height_sequence(begin(height_sequence), begin(height_sequence)+positive_count));
    } while (next_input_sequence(begin(input_sequence), end(input_sequence)));
    REQUIRE(no_failure);
    cout << "Count: " << count << endl;
}
