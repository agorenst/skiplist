#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <cstdio>
#include <iterator>
#include <climits>

// need to patch this up.
using namespace std;


// This is used for tracing calls and the like.
// May be extended to use some logging mechanism in
// unit tests.
#ifdef SKIPLIST_TRACE
#include <string>
struct invocation_tracer {
    std::string funcname;
    explicit invocation_tracer(std::string&& s): funcname(s) {
        printf("entering %s\n", funcname.c_str());
    }
    ~invocation_tracer() {
        printf("leaving %s\n", funcname.c_str());
    }
};
#define SL_TRACE_CALL invocation_tracer do_trace(__PRETTY_FUNCTION__);
#else
#define SL_TRACE_CALL
#endif



// TODO: I really have to make sure we don't have to link to a obj file
// in order to use a skiplist. Shove this state elsewhere (constructor parameter?)
#include <random>
namespace skiplist_internal {
    extern std::random_device rd;
    extern std::mt19937 gen;
    extern std::uniform_int_distribution<> dis;
    int good_height_generator();
};

// Setting "sane" defaults to save typing -- will refine defaults
// as performance testing solidifies.
// TODO: NOEXCEPT
template<typename T,
    // TODO: can max_height be a constexpr of generator.max()?
    int max_height = 32,
    int (*gen)() = skiplist_internal::good_height_generator,
    class Compare = std::less<T>,
    class Allocator = std::allocator<T>>

class skip_list {
// Run all of our asserts.
#ifdef SKIPLIST_CORRECTNESS
    // This helps make sure that our invariants
    // are maintained in non-const methods.
struct invariant_checker {
    typedef skip_list<T, max_height, gen, Compare, Allocator> parent;
    const parent& mine;
    explicit invariant_checker(const parent& p): mine(p) {
        assert(mine.check_simple_invariants());
    }
    ~invariant_checker() {
        assert(mine.check_simple_invariants());
    }
};
#define ASSERT(x) assert(x)
#define INVARIANT_GUARD invariant_checker do_invariants(*this);
#else
#define ASSERT(x)
#define INVARIANT_GUARD
#endif

// This is used for keeping track of measurement
// counters for performance.
// NOTE NOTE NOTE: we don't swap, move, copy, etc. these fields.
#ifdef SKIPLIST_MEASURES
public:
mutable int elt_comparison = 0;
mutable int slice_preceeding_iteration = 0;
mutable int same_ptr_cmp = 0;
mutable int node_step = 0;
mutable int equality_cmp = 0;
mutable int preds_have_e_counter = 0;
mutable int ptr_creation = 0;
#define ELT_COMPARISON elt_comparison++
#define SLICE_PRECEEDING_ITERATION slice_preceeding_iteration++
#define SAME_PTR_CMP same_ptr_cmp++
#define NODE_STEP node_step++
#define EQUALITY_CMP equality_cmp++
#define PREDS_HAVE_E preds_have_e_counter++
#define PTR_CREATION(x) ptr_creation += x
private:
#else
#define ELT_COMPARISON
#define SLICE_PRECEEDING_ITERATION
#define SAME_PTR_CMP
#define NODE_STEP
#define EQUALITY_CMP
#define PREDS_HAVE_E
#define PTR_CREATION(x)
#endif

private:
    struct node;
    typedef node* pnode;

    // TODO: Allocation -- maybe put a pool in front?
    typedef std::vector<pnode, Allocator> slice;



    // the only field, and we initialize it.
    slice heads{max_height, nullptr};

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
    struct node {
        slice s;
        value_type e;
        node(slice s, const value_type& e): s(s), e(e) {}
        template<class... Args>
        node(slice s, Args&&... args): s(s), e{std::forward<Args>(args)...} {}
    };
    bool compare(const value_type& t1, const value_type& t2) const {
        // TODO: figure out if comp is supposed to be a field, or have
        // other restrictions.
        value_compare comp;
        ELT_COMPARISON;
        return comp(t1, t2);
    }
    // This is the core lookup routine: find the "slice"
    // that would be the predecessors for element e.
    slice slice_preceeding(const T& e) const {
        SL_TRACE_CALL;

        slice result{max_height, nullptr};
        node* prev_node = nullptr;
        node* next = nullptr;
        const slice* prev_nexts = &heads;
        int i = max_height - 1;

        for (;;) {
            SLICE_PRECEEDING_ITERATION;
            while ((*prev_nexts)[i] == next) {
                SAME_PTR_CMP;
                result[i--] = prev_node;
                if (i < 0) { return result; }
            }
            next = (*prev_nexts)[i];
            while (next && compare(next->e, e)) {
                NODE_STEP;
                prev_node = next;
                prev_nexts = &(next->s);
                next = (*prev_nexts)[i];
            }
        }
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

    void stitch_up_node(const slice& predecessors, node* new_node) {
        INVARIANT_GUARD;
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
    }

    bool are_equal(const value_type& v1, const value_type& v2) const {
        EQUALITY_CMP;
        if (!compare(v1, v2)) {
            return !compare(v2, v1);
        }
        return false;
    }
    bool preds_have_e(const slice& predecessors, const value_type& value) const {
        PREDS_HAVE_E;
        if (predecessors[0]) {
            return predecessors[0]->s[0] && !compare(value, predecessors[0]->s[0]->e);
        }
        else {
            return heads[0] && !compare(value, heads[0]->e);
        }
    }

    
public:
    std::vector<int> tree_measure() const {
        vector<int> counter(max_height,0);
        for (auto it = this->begin(); it != this->end(); ++it) {
            auto n = it.mynode;
            counter[n->s.size()]++;
        }
        return counter;
    }
    void dbg_print() {}


//////////////////////////////////////////////////////////////////////////
// Main set interface defined below.
//////////////////////////////////////////////////////////////////////////
    skip_list() : skip_list(Compare()) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
    }
    explicit skip_list(const Compare comp, const Allocator& alloc = Allocator()) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
    }
    template<class InputIt>
    skip_list(InputIt first, InputIt last,
              const Compare& comp = Compare(),
              const Allocator& alloc = Allocator()) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    // TODO: simply copy nodes directly?
    skip_list(const skip_list& that) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        insert(std::begin(that), std::end(that));
    }
    skip_list(const skip_list& that, const Allocator& alloc) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        insert(std::begin(that), std::end(that));
    }

    skip_list(skip_list&& that) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        // this should invoke the operator=(skip_list&& that) method.
        (*this) = that;
    }
    skip_list(skip_list&& that, const Allocator& alloc) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        // this should invoke the operator=(skip_list&& that) method.
        (*this) = that;
    }


    skip_list(const std::initializer_list<T>& l,
              const Compare& comp = Compare(),
              const Allocator& alloc = Allocator()) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        for (auto&& x : l) {
            insert(x);
        }
    }
    skip_list(const std::initializer_list<T>& l,
              const Allocator& alloc) : skip_list(l, Compare(), alloc) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        for (auto&& x : l) {
            insert(x);
        }
    }

    ~skip_list() {
        SL_TRACE_CALL;
        clear();
    }

    skip_list& operator=(const skip_list& that) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        if (this == &that) { return *this; }
        // for now this is very inefficient, but we'll do this.
        this->clear();
        this->insert(std::begin(that), std::end(that));
        return *this;
    }
    skip_list& operator=(skip_list&& that) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        // TODO: Resolve this confusion I have.
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
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        this->clear();
        insert(std::begin(l), std::end(l));
        return *this;
    }

    // Wait, how do data structures actually use allocators?
    allocator_type get_allocator() const {
        SL_TRACE_CALL;
        return allocator_type();
    }

    // Very basic destructor.
    void clear() {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        node* curr = heads[0];
        while (curr) {
            auto to_del = curr;
            curr = curr->s[0];
            delete to_del;
        }
        std::fill(std::begin(heads), std::end(heads), nullptr);
    }

    // TODO: use hint to our advantage.
    // See also: emplace_hint.
    iterator insert(const_iterator hint, const value_type& value) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        auto iter_and_flag = insert(value);
        return std::get<0>(iter_and_flag);
    }
    iterator insert(const_iterator hint, value_type&& value) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        auto iter_and_flag = insert(value);
        return std::get<0>(iter_and_flag);
    }



    // TODO: Share code between the different inserts and emplace.
    // I'm deliberately waiting because I don't know all the design considerations
    // yet (multithreading, exception handling, debugging, etc.)
    // TODO: Use allocator properly.
    // TODO: Lots of asserts?
    // TODO: Logging (to help unit tests verify that we have the coverage we expect)
    // TODO: Watch for multiset functionality.

    std::pair<iterator,bool> insert(const value_type& value, int height) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        slice predecessors{slice_preceeding(value)};
        if (preds_have_e(predecessors, value)) {
            if (predecessors[0]) { return { predecessors[0]->s[0], false }; }
            else { return { heads[0], false }; }
        }
        int new_height = height;
        PTR_CREATION(new_height);
        slice new_slice(new_height, nullptr);
        auto new_node = new node{new_slice, value};

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    std::pair<iterator,bool> insert(value_type&& value) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        //printf("std::pair<iterator,bool> insert(value_type&& value)\n");
        slice predecessors{slice_preceeding(value)};
        if (preds_have_e(predecessors, value)) {
            if (predecessors[0]) { return { predecessors[0]->s[0], false }; }
            else { return { heads[0], false }; }
        }
        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        PTR_CREATION(new_height);
        auto new_node = new node{new_slice, std::move(value)};

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    std::pair<iterator,bool> insert(const value_type& value) {
        INVARIANT_GUARD;
        SL_TRACE_CALL;
        //printf("std::pair<iterator,bool> insert(const value_type& value)\n");
        slice predecessors{slice_preceeding(value)};
        if (preds_have_e(predecessors, value)) {
            if (predecessors[0]) { return { predecessors[0]->s[0], false }; }
            else { return { heads[0], false }; }
        }

        int new_height = generate_height();
        PTR_CREATION(new_height);
        slice new_slice(new_height, nullptr);
        auto new_node = new node{new_slice, value};

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    void insert(std::initializer_list<value_type> ilist) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        insert(std::begin(ilist), std::end(ilist));
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        for (; first != last; first++) {
            insert(*first);
        }
    }

    // insert_return_type insert(node_type&& nh); // C++17
    // iterator insert(const_iterator hint, node_type&& nh); // C++17

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        // We make the node first
        slice new_nexts{(size_t) generate_height(), nullptr};
        auto new_node = new node{new_nexts, args...};

        slice predecessors{slice_preceeding(new_node->e)};
        if (preds_have_e(predecessors, new_node->e)) {
            delete new_node;
            if (predecessors[0]) { return { predecessors[0]->s[0], false }; }
            else { return { heads[0], false }; }
        }

        PTR_CREATION(new_nexts.size());
        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    // TODO: use hint to our advantage.
    // For now we ignore the hint.
    template<class... Args>
    std::pair<iterator, bool> emplace_hint(const_iterator hint, Args&&... args) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        // We make the node first
        slice new_nexts{(size_t) generate_height(), nullptr};
        auto new_node = new node{new_nexts, args...};

        slice predecessors{slice_preceeding(new_node->e)};
        if (preds_have_e(predecessors, new_node->e)) {
            delete new_node;
            if (predecessors[0]) { return { predecessors[0]->s[0], false }; }
            else { return { heads[0], false }; }
        }

        PTR_CREATION(new_nexts.size());
        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    // TODO FOR ERASE: we're really depending on having a good
    // iterator story here.
    iterator erase(const_iterator pos) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        erase(*pos);
        return end(); // TODO: NOT CORRECT, supposed to be std::next(pos);
    }
    iterator erase(iterator pos) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        erase(*pos);
        return end(); // TODO: NOT CORRECT
    }
    iterator erase(const_iterator first, const_iterator last) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        for (; first != last; ++first) {
            erase(*first);
        }
        return end(); // TODO: NOT CORRECT
    }

    size_type erase(const key_type& key) {
        SL_TRACE_CALL;
        INVARIANT_GUARD;
        slice to_stitch{slice_preceeding(key)};
        if (preds_have_e(to_stitch, key)) {
            node* to_del = to_stitch[0] ? to_stitch[0]->s[0] : heads[0];
            for (int i = max_height - 1; i >= 0; i--) {
                if (to_stitch[i] && to_stitch[i]->s[i] == to_del) {
                    ASSERT(to_stitch[i]->s[i] == to_del);
                    to_stitch[i]->s[i] = to_del->s[i];
                }
                else if (heads[i] && heads[i] == to_del) {
                    ASSERT(heads[i] == to_del);
                    heads[i] = heads[i]->s[i];
                }
            }
            delete to_del;
            return 1;
        }
        return 0;
    }

    void swap(skip_list& other) {
        SL_TRACE_CALL;
        // TODO: I anticipate having more fields.
        // Depends on allocator type!
        std::swap(this->heads, other.heads);
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
        const pointer operator->() { return &(mynode->e); }
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
        pointer operator->() { return &(mynode->e); }
        bool operator==(const iterator& that) const { return mynode == that.mynode; }
        bool operator!=(const iterator& that) const { return !((*this) == that); }
    };

    iterator begin() {
        SL_TRACE_CALL;
        return iterator{heads[0]};
    }
    iterator end() {
        SL_TRACE_CALL;
        return iterator{nullptr};
    }
    const_iterator begin() const {
        SL_TRACE_CALL;
        return const_iterator{heads[0]};
    }
    const_iterator end() const {
        SL_TRACE_CALL;
        return const_iterator{nullptr};
    }
    const_iterator cbegin() const {
        SL_TRACE_CALL;
        return const_iterator{heads[0]};
    }
    const_iterator cend() const {
        SL_TRACE_CALL;
        return const_iterator{nullptr};
    }

    // TODO: I DON'T ACTUALLY HAVE REVERSE ITERATORS.
    // THESE ARE WRONG.
    iterator rbegin() {
        SL_TRACE_CALL;
        return iterator{heads[0]};
    }
    iterator rend() {
        SL_TRACE_CALL;
        return iterator{nullptr};
    }
    const_iterator rbegin() const {
        SL_TRACE_CALL;
        return const_iterator{heads[0]};
    }
    const_iterator rend() const {
        SL_TRACE_CALL;
        return const_iterator{nullptr};
    }
    const_iterator crbegin() const {
        SL_TRACE_CALL;
        return const_iterator{heads[0]};
    }
    const_iterator crend() const {
        SL_TRACE_CALL;
        return const_iterator{nullptr};
    }

    bool empty() const noexcept {
        SL_TRACE_CALL;
        return !heads[0];
    }
    // very slow for now!
    size_type size() const noexcept {
        SL_TRACE_CALL;
        return std::distance(begin(), end());
    }
    size_type max_size() const noexcept {
        SL_TRACE_CALL;
        // TODO: determine a better value. This is just a silly
        // placeholder.
        return 10000000;
    }


    // Basic find operation. Returns an iterator containing
    // the node with the value equal to e, or end()
    // TODO: Can probably clean up the looping/conditional logic.
    // TODO: Can't I share code by constructing a const_iterator
    // from iterator?
    iterator find(const T& e) {
        SL_TRACE_CALL;
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
        SL_TRACE_CALL;
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

    // TODO: NONE OF THESE ARE CORRECT
    template<class K> iterator find(const K& k) {
        SL_TRACE_CALL;
        return this->end();
    }
    template<class K> const_iterator find(const K& k) const {
        SL_TRACE_CALL;
        return this->end();
    }

    // TODO: NONE OF THESE ARE CORRECT
    std::pair<iterator,iterator> equal_range(const key_type& key) {
        SL_TRACE_CALL;
        return std::make_pair(begin(), begin());
    }
    std::pair<const_iterator,const_iterator> equal_range(const key_type& key) const {
        SL_TRACE_CALL;
        return std::make_pair(begin(), begin());
    }
    template<class K>
    std::pair<iterator,iterator> equal_range(const K& x) {
        SL_TRACE_CALL;
        return std::make_pair(begin(), begin());
    }
    template<class K>
    std::pair<const_iterator,const_iterator> equal_range(const K& x) const {
        SL_TRACE_CALL;
        return std::make_pair(begin(), begin());
    }

    // TODO: NONE OF THESE ARE CORRECT
    iterator lower_bound(const key_type& key) {
        SL_TRACE_CALL;
        return end();
    }
    const_iterator lower_bound(const key_type& key) const {
        SL_TRACE_CALL;
        return end();
    }
    template<class K>
    iterator lower_bound(const K& x) {
        SL_TRACE_CALL;
        return end();
    }
    template<class K>
    const_iterator lower_bound(const K& x) const {
        SL_TRACE_CALL;
        return end();
    }

    // TODO: NONE OF THESE ARE CORRECT
    iterator upper_bound(const key_type& key) {
        SL_TRACE_CALL;
        return end();
    }
    const_iterator upper_bound(const key_type& key) const {
        SL_TRACE_CALL;
        return end();
    }
    template<class K>
    iterator upper_bound(const K& x) {
        SL_TRACE_CALL;
        return end();
    }
    template<class K>
    const_iterator upper_bound(const K& x) const {
        SL_TRACE_CALL;
        return end();
    }

    // TODO: NONE OF THESE ARE CORRECT
    key_compare key_comp() const {
        SL_TRACE_CALL;
        return end();
    }
    value_compare value_comp() const {
        SL_TRACE_CALL;
        return end();
    }
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

// TODO: Make this right...
//template< class Key, class Compare, class Alloc >
//void swap(...)

#endif
