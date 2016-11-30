#include <algorithm>
#include <cstdio>
#include <cassert>

template<class T>
class NoisyAlloc {
    public:
    unsigned total_allocated = 0;
    unsigned memory_in_use = 0;

    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template<class U> struct rebind { typedef NoisyAlloc<U> other; };

    NoisyAlloc() { printf("constructing NoisyAlloc\n"); }
    NoisyAlloc(const NoisyAlloc& other):
        total_allocated(other.total_allocated),
        memory_in_use(other.memory_in_use) {
            printf("copy-constructing NoisyAlloc\n");
        }
    template<class U>
    NoisyAlloc(const NoisyAlloc<U>& other):
        total_allocated(other.total_allocated),
        memory_in_use(other.memory_in_use) {
            printf("template copy-constructing NoisyAlloc\n");
        }

    T* allocate(std::size_t n) {
        printf("requesting n=%lu elements, of size %lu\n", n, sizeof(T));
        T* m = (T*) ::operator new(n*sizeof(T));
        printf("memory allocated at %p\n", m);
        total_allocated += n*sizeof(T);
        memory_in_use += n*sizeof(T);
        return m;
    }
    void deallocate(pointer p, size_type n) {
        //printf("Freeing pointer p=%p with n=%lu\n", p, n);
        assert(memory_in_use >= sizeof(T)*n);
        memory_in_use -= sizeof(T)*n;
        delete p;
    }
};
