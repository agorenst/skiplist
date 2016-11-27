#define SKIPLIST_PERF
#include "skiplist.h"

#include <random>
#include <algorithm>
#include <iostream>
#include <limits>
#include <climits>
#include <set>

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

// insert a billion elements, in order
template<typename Container>
void insert_integer_range(int low, int high) {
    Container c;
    for (int i = low; i < high; i++) {
        c.insert(i);
    }
    // make sure we use the result.
    cout << (*begin(c)) << endl;
}

template<typename Container>
void insert_reverse_integer_range(int low, int high) {
    Container c;
    for (int i = high-1; i >= low; i--) {
        c.insert(i);
    }
    // make sure we use the result.
    cout << (*begin(c)) << endl;
}

template<typename Container>
void insert_random_integers(int count, int range) {
    Container c;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rnd(0,range);
    for (int i = 0; i < count; i++) {
        c.insert(rnd(gen));
    }
    // make sure we use the result.
    cout << (*begin(c)) << endl;
}

template<typename Container>
void insert_random_integers_many_overlap(int count, int range) {
    insert_random_integers<Container>(count, range);
}

int main() {
    typedef std::set<int> baseline_type;
    typedef skip_list<int> skiplist_type;
    const int N = 10000000;

    cout << "std::set<int>" << endl;
    auto t = measure<>::execution(insert_integer_range<baseline_type>, 0, N);
    cout << "time: " << t << endl;
    t = measure<>::execution(insert_reverse_integer_range<baseline_type>, 0, N);
    cout << "time: " << t << endl;
    t = measure<>::execution(insert_random_integers<baseline_type>, N, N);
    cout << "time: " << t << endl;

    cout << "skiplist type: " << endl;
    t = measure<>::execution(insert_integer_range<skiplist_type>, 0, N);
    cout << "time: " << t << endl;
    t = measure<>::execution(insert_reverse_integer_range<skiplist_type>, 0, N);
    cout << "time: " << t << endl;
    t = measure<>::execution(insert_random_integers<skiplist_type>, N, N);
    cout << "time: " << t << endl;
}

