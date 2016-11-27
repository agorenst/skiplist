#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <cstdio>
#include <iterator>
#include <climits>


// Are we compiling in logging mode?
#ifdef LOGGING_INFO
#define LOG_CONDITIONAL(C, A) if (C) { A; }
#define LOG_HEIGHT_TRAVERSED
#define LOG_NODE_STEP LOG_node_stepped()
#define LOG_ELT_COMPARISON LOG_elt_comparison()
#else
#define LOG_CONDITIONAL(C, A)
#define LOG_HEIGHT_TRAVERSED
#define LOG_NODE_STEP
#define LOG_ELT_COMPARISON
#endif

// Are we compiling in debug mode? (with asserts)
#ifdef SKIPLIST_DEBUG
#define ASSERT(x) assert(x)
#define DBG_PRINT(...) printf(__VA_ARGS__);
#else
#define ASSERT(x)
#define DBG_PRINT(...)
#endif

#include <iostream>
using namespace std;


#include <random>


// Setting "sane" defaults to save typing -- will refine defaults
// as performance testing solidifies.
namespace skiplist_internal {
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
    auto x = dis(gen);
    return ntz(x)+1;
}
};

template<typename T,
    // TODO: can max_height be a constexpr of generator.max()?
    int max_height = 32,
    int (*gen)() = skiplist_internal::good_height_generator,
    class Compare = std::less<T>,
    class Allocator = std::allocator<T>>
class skip_list {
    private:
    struct node;
    typedef node* pnode;

    // TODO: Allocation -- maybe put a pool in front?
    typedef std::vector<pnode, Allocator> slice;

    // the only field, and we initialize it.
    slice heads{max_height, nullptr};

#ifdef LOGGING_INFO
    // Logging information to track performance.
    // Currently single-threaded...
    int node_stepped = 0;
    mutable int elt_compared = 0;

    void LOG_node_stepped() { node_stepped++; }
    void LOG_elt_comparison() const { elt_compared++; }
public:
    int LOG_get_node_stepped() const { return node_stepped; }
    void LOG_reset_node_stepped() { node_stepped = 0; }
    int LOG_get_elt_comparisons() const { return elt_compared; }
    void LOG_reset_elt_comparisons() { elt_compared = 0; }
#endif

    public:
    typedef T                                               key_type;
    typedef T                                               value_type;
    typedef std::size_t                                     size_type;
    typedef std::ptrdiff_t                                  difference_type;
    typedef Compare                                         key_compare;
    typedef Compare                                         value_compare;
    typedef Allocator                                       allocator_type;
    typedef value_type&                                     reference;
    typedef const value_type&                               const_reference;
    typedef typename std::allocator_traits<Allocator>::pointer       pointer;
    typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
    struct iterator;
    struct const_iterator;

    private:
    bool compare(const value_type& t1, const value_type& t2) const {
        // TODO: figure out if comp is supposed to be a field, or have
        // other restrictions.
        value_compare comp;
        LOG_ELT_COMPARISON;
        return comp(t1, t2);
    }

    struct node {
        slice s;
        value_type e;
        node(slice s, const value_type& e): s(s), e(e) {}
        template<class... Args>
        node(slice s, Args&&... args): s(s), e{std::forward<Args>(args)...} {}
    };

    template<typename Iter>
    int find_i_to_fwd(const T& E, Iter first, Iter last) {
        return std::upper_bound(first, last, E, [this](const T& e, node* n) {
            return !(n && compare(n->e, e));
        }) - first;
    }

    // This is the core lookup routine: find the "slice"
    // that would be the predecessors for element e.
    slice slice_preceeding(const T& e, const size_type height = max_height) {
        slice result{height, nullptr};
        node* prev_node = nullptr;
        slice* prev_nexts = &heads;
        for (int i = height - 1; i >= 0; --i) {
            LOG_HEIGHT_TRAVERSED;
            while((*prev_nexts)[i] && compare((*prev_nexts)[i]->e, e)) {
                LOG_NODE_STEP;
                prev_node = (*prev_nexts)[i];
                prev_nexts = &(prev_node->s);
            }
            result[i] = prev_node;
        }
        return result;
    }

    int generate_height() const {
        auto height = std::min(gen(), max_height-1);
        ASSERT(0 < height && height < max_height);
        return height;
    }

    bool check_simple_invariants() const {
        // find the first head that's null
        // everything after the first null entry must also be null.
        auto it = std::find(std::begin(heads), std::end(heads), nullptr);
        if (!std::all_of(it, std::end(heads), [](node* n) { return !n; })) {
            return false;
        }

        if (heads[0]) {
            node* n = heads[0];
            while (n) {
                auto it = std::find(std::begin(n->s), std::end(n->s), nullptr);
                if (!std::all_of(it, std::end(n->s), [](node* n) { return !n; })) {
                    return false;
                }
                n = n->s[0];
            }
        }

        node* n = heads[0];
        while (n) {
            // for all our slice, each non-null element should not be less
            // than the next slice element
            for (int i = n->s.size() - 2; i >= 0; i--) {
                if (n->s[i+1]) {
                    if (compare(n->s[i+1]->e, n->s[i]->e)) {
                        return false;
                    }
                }
            }
            // we better be less than the element we precede.
            if (n->s[0]) {
                if (compare(n->s[0]->e, n->e)) {
                    return false;
                }
            }
            // assuming that compare is transitive, at this point our stack
            // is consistently sorted.
            n = n->s[0];
        }
        return true;
    }


public:
    std::vector<int> tree_measure() {
        vector<int> counter(max_height,0);
        for (auto it = this->begin(); it != this->end(); ++it) {
            auto n = it.mynode;
            counter[n->s.size()]++;
        }
        return counter;
    }
    void dbg_print() {
        for (int height = max_height-1; height >= 0; --height) {
            if (heads[height]) {
                if (height > 9) {
                    printf("[%d]--", height);
                }
                else {
                    printf("[0%d]--", height);
                }
            }
            else { continue ;} // don't print a height that was never reached...?
            node* n = heads[0];
            while (n) {
                if (n->s.size() > height) {
                    if (n->e > 9) {
                        printf("[%d]", n->e);
                    }
                    else {
                        printf("[0%d]", n->e);
                    }
                }
                else {
                    printf("----");
                }
                printf("--");
                n = n->s[0];
            }
            printf("|\n");
        }
    }
    
    skip_list() = default;

    template<class ITER>
    skip_list(ITER start, ITER finish) {
        for (; start != finish; ++start) {
            insert(*start);
        }
    }

    skip_list(const std::initializer_list<T>& l) {
        for (auto&& x : l) {
            insert(x);
        }
    }

    ~skip_list() {
        clear();
    }

    skip_list& operator=(const skip_list& that) {
        if (this == &that) { return *this; }
        // for now this is very inefficient, but we'll do this.
        this->clear();
        this->insert(std::begin(that), std::end(that));
        return *this;
    }
    skip_list& operator=(skip_list&& that) {
        // is that right? I don't think this is possible...?
        if (this == &that) { return *this; }
        this->clear();
        this->heads = that.heads;
        // I have to do this, otherwise if it gets destructed
        // it will remove the memory out from under us.
        std::fill(std::begin(that.heads), std::end(that.heads), nullptr);
        return *this;
    }
    skip_list& operator=(std::initializer_list<T> l) {
        this->clear();
        insert(begin(l), end(l));
    }

    // Very basic destructor.
    void clear() {
        node* curr = heads[0];
        while (curr) {
            auto to_del = curr;
            curr = curr->s[0];
            delete to_del;
        }
        std::fill(std::begin(heads), std::end(heads), nullptr);
    }

    // for now we're just bailing on using the "hint"
    iterator insert(const_iterator hint, const value_type& value) {
        auto iter_and_flag = insert(value);
        return std::get<0>(iter_and_flag);
    }
    iterator insert(const_iterator hint, value_type&& value) {
        auto iter_and_flag = insert(value);
        return std::get<0>(iter_and_flag);
    }


    void stitch_up_node(const slice& predecessors, node* new_node) {
        const int new_height = new_node->s.size();
        for (int i = 0; i < new_height; ++i) {
            if (predecessors[i] == nullptr) {
                new_node->s[i] = heads[i];
                heads[i] = new_node;
            }
            else {
                new_node->s[i] = predecessors[i]->s[i];
                predecessors[i]->s[i] = new_node;
            }
        }
        ASSERT(check_simple_invariants());
    }

    bool are_equal(const value_type& v1, const value_type& v2) const {
        if (!compare(v1, v2)) {
            return !compare(v2, v1);
        }
        return false;
    }

    bool preds_have_e(const slice& predecessors, const value_type& value) const {
        if (predecessors[0]) {
            return predecessors[0]->s[0] && are_equal(predecessors[0]->s[0]->e, value);
        }
        else {
            return heads[0] && are_equal(heads[0]->e, value);
        }
    }

    // TODO: Share code between the different inserts and emplace.
    // I'm deliberately waiting because I don't know all the design considerations
    // yet (multithreading, exception handling, debugging, etc.)
    // TODO: Use allocator properly.
    // TODO: Lots of asserts?
    // TODO: Logging (to help unit tests verify that we have the coverage we expect)
    // TODO: Watch for multiset functionality.

    std::pair<iterator,bool> insert(const value_type& value, const int height) {
        slice predecessors{slice_preceeding(value)};
        if (preds_have_e(predecessors, value)) {
            return {predecessors[0], false};
        }
        int new_height = height;
        slice new_slice(new_height, nullptr);
        auto new_node = new node{new_slice, value};

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    std::pair<iterator,bool> insert(value_type&& value) {
        slice predecessors{slice_preceeding(value)};
        if (preds_have_e(predecessors, value)) {
            return {predecessors[0], false};
        }
        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        auto new_node = new node{new_slice, std::move(value)};

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    std::pair<iterator,bool> insert(const value_type& value) {
        slice predecessors{slice_preceeding(value)};
        if (preds_have_e(predecessors, value)) {
            return {predecessors[0], false};
        }

        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        auto new_node = new node{new_slice, value};

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        // We make the node first
        slice new_nexts{(size_t) generate_height(), nullptr};
        auto new_node = new node{new_nexts, args...};

        slice predecessors{slice_preceeding(new_node->e)};
        if (preds_have_e(predecessors, new_node->e)) {
            delete new_node;
            return {predecessors[0], false};
        }

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    void insert(std::initializer_list<value_type> ilist) {
        insert(std::begin(ilist), std::end(ilist));
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        for (; first != last; first++) {
            insert(*first);
        }
    }


    // insert_return_type insert(node_type&& nh); // C++17
    // iterator insert(const_iterator hint, node_type&& nh); // C++17

    void erase(const T& e) {
        slice to_stitch{slice_preceeding(e)};
        node* pre_to_del = to_stitch[0];

        // either equal to or less than heads[0]->e
        if (!pre_to_del && heads[0]) {
            // less than
            if (compare(e, heads[0]->e)) {
                return;
            }
            // equal to
            else if (!compare(heads[0]->e, e)) {
                node* to_del = heads[0];

                for (int i = 0; i < max_height; ++i) {
                    if (heads[i] == to_del) {
                        heads[i] = heads[i]->s[i];
                    }
                }
                delete to_del;
                ASSERT(check_simple_invariants());
                return;
            }
        }
        // if the element is in the list other than the head
        else if (pre_to_del && pre_to_del->s[0] && are_equal(pre_to_del->s[0]->e, e)) {
            node* to_del = pre_to_del->s[0];
            for (int i = 0; i < max_height; ++i) {
                if (i < to_del->s.size()) {
                    ASSERT(to_stitch[i] || heads[i]);
                    ASSERT(heads[i] == to_del || to_stitch[i]->s[i] == to_del);
                    if (to_stitch[i]) {
                        ASSERT(to_stitch[i]->s[i]->s[i] == to_del->s[i]);
                    }
                    else {
                        ASSERT(heads[i]->s[i] == to_del->s[i]);
                    }
                }
                else {
                    ASSERT(to_stitch[i] != to_del);
                }
                if (to_stitch[i] && to_stitch[i]->s[i] == to_del) {
                    to_stitch[i]->s[i] = to_stitch[i]->s[i]->s[i];
                }
                else if (heads[i] && heads[i] == to_del) {
                    heads[i] = heads[i]->s[i];
                }
            }
            delete to_del;
            ASSERT(check_simple_invariants());
            return;
        }
        // otherwise, it's not here.
        return;
    }
    
// Definitely temporary, just getting things hooked up.
    struct const_iterator : public std::iterator<std::forward_iterator_tag,
                                    T,
                                    int,
                                    T*,
                                    T&>{
        const node* mynode;
        const_iterator(node* n): mynode(n) {}
        const_iterator& operator++() {
            //printf("increment mynode from %p\n", mynode);
            if (mynode) { mynode = mynode->s[0]; }
            //printf("now: %p\n", mynode);
            return *this;
        }
        const_iterator& operator++(int) {
            return ++(*this);
        }
        const T& operator*() { return mynode->e; }
        bool operator==(const const_iterator& that) const { return mynode == that.mynode; }
        bool operator!=(const const_iterator& that) const { return !((*this) == that); }
    };
// Definitely temporary, just getting things hooked up.
    struct iterator : public std::iterator<std::forward_iterator_tag,
                                    T,
                                    int,
                                    T*,
                                    T&>{
        node* mynode;
        iterator(const const_iterator& that): mynode(that.mynode) {}
        iterator(node* n): mynode(n) {}
        iterator& operator++() {
            //printf("increment mynode from %p\n", mynode);
            if (mynode) { mynode = mynode->s[0]; }
            //printf("now: %p\n", mynode);
            return *this;
        }
        T& operator*() { return mynode->e; }
        bool operator==(const iterator& that) const { return mynode == that.mynode; }
        bool operator!=(const iterator& that) const { return !((*this) == that); }
    };

    iterator begin() {
        return iterator{heads[0]};
    }
    iterator end() {
        return iterator{nullptr};
    }
    const_iterator begin() const {
        return const_iterator{heads[0]};
    }
    const_iterator end() const {
        return const_iterator{nullptr};
    }

    bool empty() const noexcept {
        return !heads[0];
    }
    // very slow for now!
    size_type size() const noexcept {
        return std::distance(begin(), end());
    }


    // Basic find operation. Returns an iterator containing
    // the node with the value equal to e, or end()
    // TODO: Can probably clean up the looping/conditional logic.
    // TODO: Can't I share code by constructing a const_iterator
    // from iterator?
    iterator find(const T& e) {
        int i = max_height - 1;

        // find the highest non-null element.
        while (i >= 0 && !heads[i]) { --i; };
        if (i < 0) { return end(); }
        auto curr = heads[i];
        ASSERT(curr);
        while (i > 0) {
            while (curr && compare(e, curr->e)) {
                curr = curr->s[i];
            }
            --i;
        }
        if (curr && are_equal(curr->e,e)) {
            return iterator{curr};
        }
        else {
            return end();
        }
    }
    const_iterator find(const T& e) const {
        int i = max_height - 1;

        // find the highest non-null element.
        while (i >= 0 && !heads[i]) { --i; };
        if (i < 0) { return end(); }
        auto curr = heads[i];
        ASSERT(curr);
        while (i > 0) {
            while (curr && compare(e, curr->e)) {
                curr = curr->s[i];
            }
            --i;
        }
        if (curr && are_equal(curr->e,e)) {
            return const_iterator{curr};
        }
        else {
            return end();
        }
    }
    //template<class K> iterator find(const K& k);
    //template<class K> const_iterator find(const K& k) const;
};

template<class Key, int max_height, int(*gen)(),
         class Compare, class Alloc>
bool operator==(const skip_list<Key, max_height, gen, Compare, Alloc>& lhs,
                const skip_list<Key, max_height, gen, Compare, Alloc>& rhs) {
    return std::equal(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

template<class Key, int max_height, int(*gen)(),
         class Compare, class Alloc>
bool operator!=(const skip_list<Key, max_height, gen, Compare, Alloc>& lhs,
                const skip_list<Key, max_height, gen, Compare, Alloc>& rhs) {
    auto result = std::mismatch(begin(lhs), end(lhs), begin(rhs), end(rhs));
    return std::get<0>(result) != lhs.end() || std::get<1>(result) != rhs.end();
}
template<class Key, int max_height, int(*gen)(),
         class Compare, class Alloc>
bool operator<(const skip_list<Key, max_height, gen, Compare, Alloc>& lhs,
                const skip_list<Key, max_height,gen, Compare, Alloc>& rhs) {
    auto result = std::mismatch(begin(lhs), end(lhs), begin(rhs), end(rhs));
    auto first_iter = std::get<0>(result);
    auto second_iter = std::get<1>(result);

    // They're actually the same sequence!
    if (first_iter == lhs.end() && second_iter == rhs.end()) {
        return false;
    }
    // lhs is a "substring" of the rhs
    if (first_iter == lhs.end()) {
        return true;
    }
    // rhs is a substring of the lhs
    if (second_iter == rhs.end()) {
        return false;
    }
    return *first_iter < *second_iter;
}
template<class Key, int max_height, int(*gen)(),
         class Compare, class Alloc>
bool operator<=(const skip_list<Key, max_height, gen, Compare, Alloc>& lhs,
                const skip_list<Key, max_height, gen, Compare, Alloc>& rhs) {
    return !(lhs > rhs);
}
template<class Key, int max_height, int(*gen)(),
         class Compare, class Alloc>
bool operator>(const skip_list<Key, max_height,  gen, Compare, Alloc>& lhs,
                const skip_list<Key, max_height, gen, Compare, Alloc>& rhs) {
    auto result = std::mismatch(begin(lhs), end(lhs), begin(rhs), end(rhs));
    auto first_iter = std::get<0>(result);
    auto second_iter = std::get<1>(result);

    // They're actually the same sequence!
    if (first_iter == lhs.end() && second_iter == rhs.end()) {
        return false;
    }
    // lhs is a "substring" of the rhs
    if (first_iter == lhs.end()) {
        return false;
    }
    // rhs is a substring of the lhs
    if (second_iter == rhs.end()) {
        return true;
    }
    return *first_iter > *second_iter;
}
template<class Key, int max_height, int(*gen)(),
         class Compare, class Alloc>
bool operator>=(const skip_list<Key, max_height, gen, Compare, Alloc>& lhs,
                const skip_list<Key, max_height, gen, Compare, Alloc>& rhs) {
    return !(lhs < rhs);
}
