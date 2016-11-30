#include "noisy_allocator.h"

#include <set>
#include "skiplist.h"

#include <cstdio>

using namespace std;

int main() {
    set<int,std::less<int>, NoisyAlloc<int>> s;
    for (int i = 0; i < 100; ++i) {
        s.insert(i);
    }
    printf("Total memory allocated: %u\n", s.get_allocator().total_allocated);
    printf("%d\n", *(s.begin()));

    skip_list<int,32,skiplist_internal::good_height_generator, std::less<int>, NoisyAlloc<int>> l;
    for (int i = 0; i < 100; ++i) {
        l.insert(i);
    }
    printf("Total memory allocated: %u\n", l.get_allocator().total_allocated);
    printf("%d\n", *(l.begin()));
}
