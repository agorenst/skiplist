#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <cstdio>
#include <iterator>


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

//template<typename T, int max_height, std::function<int()> rnd>
template<typename T, int max_height, int (*gen)(),
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
    int elt_compared = 0;

    void LOG_node_stepped() { node_stepped++; }
    void LOG_elt_comparison() { elt_compared++; }
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

    struct node {
        slice s;
        value_type e;
        node(slice s, const value_type& e): s(s), e(e) {}
        template<class... Args>
        node(slice s, Args&&... args): s(s), e{std::forward<Args>(args)...} {}
    };

    // This is the core lookup routine: find the "slice"
    // that would be the predecessors for element e.
    // TODO: have to use COMPARE, not operator<
    slice slice_preceeding(const T& e) {
        slice result{max_height, nullptr};
        node* prev_node = nullptr;
        slice* prev_nexts = &heads;

        for (int i = max_height - 1; i >= 0; --i) {
            LOG_HEIGHT_TRAVERSED
            if (!((*prev_nexts)[i])) {
                result[i] = prev_node;
            }
            else if ((*prev_nexts)[i]->e >= e) {
                LOG_ELT_COMPARISON;
                result[i] = prev_node;
            }
            else {
                do {
                    ASSERT((*prev_nexts)[i]->e < e);
                    LOG_NODE_STEP;
                    prev_node = (*prev_nexts)[i];
                    prev_nexts = &(prev_node->s);
                    LOG_CONDITIONAL((*prev_nexts)[i], LOG_ELT_COMPARISON);
                } while((*prev_nexts)[i] && (*prev_nexts)[i]->e < e);
                LOG_CONDITIONAL((*prev_nexts)[i], LOG_ELT_COMPARISON);

                result[i] = prev_node;
            }
        }
        //for_each(std::begin(result), std::end(result), [](node* n) {
        //    if (n) { cout << n->e << " "; }
        //    else { cout << "-" << " "; }
        //});
        //cout << endl;
        return result;
    }

    int generate_height() const {
        return std::min(gen(),max_height-1) + 1;
    }

public:
    // Not really sure this is correct. Either way, only works
    // for ints.
    void dbg_print() {
        const int N = this->size();
        for (int height = max_height; height >= 0; --height) {
            if (heads[height]) {
                printf("[]--");
            }
            else {
                printf("X------");
            }
            node* n = heads[0];
            while(n) {
                if (n->s.size() > height && n->s[height]) {
                    printf("[%d->%d]--", n->e, n->s[height]->e);
                }
                else if (n->s.size() > height) {
                    printf("[%d]-----", n->e);
                }
                else {//if (heads[height]) {
                    printf("-------");
                }
                //else {
                //}
                n = n->s[0];
            }
            printf("X\n");
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
        // maybe remove this if not necessary.
        // TODO: Figure out what's supposed to happen here. UB?
        //that.heads = std::vector<pnode>{0,nullptr};
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

    iterator insert(const_iterator hint, const value_type& value) {
        auto iter_and_flag = insert(value);
        return std::get<0>(iter_and_flag);
    }
    iterator insert(const_iterator hint, value_type&& value) {
        auto iter_and_flag = insert(value);
        return std::get<0>(iter_and_flag);
    }


    void stitch_up_node(const slice& predecessors, node* __restrict new_node) {
        DBG_PRINT("stitch_up_node: entering\n");
        const int new_height = new_node->s.size();
        ASSERT(new_height > 0); // otherwise memory leak...
        DBG_PRINT("new_height: %d\n", new_height);
        for (int i = 0; i < new_height; ++i) {
            if (predecessors[i] == nullptr) {
                //cout << "pointing " << new_node->e << " to " << (heads[i] ? heads[i]->e : -1) << endl;
                new_node->s[i] = heads[i];
                heads[i] = new_node;
            }
            else {
                //cout << "pointing " << new_node->e << " to " << (predecessors[i] ? predecessors[i]->e : -1) << endl;
                new_node->s[i] = predecessors[i]->s[i];
                predecessors[i]->s[i] = new_node;
            }
        }
    }

    // TODO: have to use COMPARE, not operator<
    bool preds_have_e(const slice& predecessors, const value_type& value) const {
        return (predecessors[0] &&
               predecessors[0]->s[0] &&
               predecessors[0]->s[0]->e == value) ||
               (!predecessors[0] && heads[0] && heads[0]->e == value);
    }

    slice generate_slice() const {
        return slice{(size_t)generate_height(), nullptr};
    }

    // TODO: Share code between the different inserts and emplace.
    // I'm deliberately waiting because I don't know all the design considerations
    // yet (multithreading, exception handling, debugging, etc.)

    // Mainly helpful for debugging.
    // Specify the height of the node without using the RNG
    std::pair<iterator,bool> insert(const value_type& value, const int height) {
        slice predecessors{slice_preceeding(value)};
        // TODO: watch for multiset functionality...
        // do we already have the element?
        if (preds_have_e(predecessors, value)) {
            return {predecessors[0], false};
        }

        ASSERT(height < max_height);

        int new_height = height;
        slice new_slice(new_height, nullptr);
        DBG_PRINT("insert(const value_type&, height): constructing new node\n");
        auto new_node = new node{new_slice, value};
        DBG_PRINT("insert(const value_type&, height): done constructing node\n");
        DBG_PRINT("new_node has height: %lu\n", new_node->s.size());

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    std::pair<iterator,bool> insert(value_type&& value) {
        slice predecessors{slice_preceeding(value)};
        // TODO: watch for multiset functionality...
        // do we already have the element?
        if (preds_have_e(predecessors, value)) {
            return {predecessors[0], false};
        }
        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        DBG_PRINT("insert(value_type&&): constructing new node\n");
        auto new_node = new node{new_slice, std::move(value)};
        DBG_PRINT("insert(value_type&&): done constructing node\n");

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    std::pair<iterator,bool> insert(const value_type& value) {
        slice predecessors{slice_preceeding(value)};
        // TODO: watch for multiset functionality...
        // do we already have the element?
        if (preds_have_e(predecessors, value)) {
            return {predecessors[0], false};
        }

        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        DBG_PRINT("insert(const value_type&): constructing new node\n");
        auto new_node = new node{new_slice, value};
        DBG_PRINT("insert(const value_type&): done constructing node\n");

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
    }

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        // We make the node first
        slice new_nexts{(size_t) generate_height(), nullptr};
        DBG_PRINT("emplace(args): constructing new node\n");
        auto new_node = new node{new_nexts, args...};
        DBG_PRINT("emplace(args): done constructing node\n");

        slice predecessors{slice_preceeding(new_node->e)};
        if (preds_have_e(predecessors, new_node->e)) {
            delete new_node;
            return {predecessors[0], false};
        }

        stitch_up_node(predecessors, new_node);
        return {new_node, true};
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
        node* to_del = nullptr;
        // the element is smaller than anything we actually have.
        if (!to_stitch[0] &&
            (!heads[0] || heads[0]->e != e)) {
            ASSERT(heads[0]->e > e);
            return;
        }
        // The element is the first in the set:
        else if (heads[0] &&
            heads[0]->e == e) {
            to_del = heads[0];

            for (int i = 0; i < max_height; ++i) {
                if (heads[i] == to_del) {
                    heads[i] = heads[i]->s[i];
                }
            }
            delete to_del;
            return;
        }
        // the element exists...
        else if (to_stitch[0] &&
            to_stitch[0]->s[0] &&
            to_stitch[0]->s[0]->e == e) {
            to_del = to_stitch[0]->s[0];
            for (int i = 0; i < max_height; ++i) {
                if (to_stitch[i] && to_stitch[i]->s[i] == to_del) {
                    to_stitch[i]->s[i] = to_stitch[i]->s[i]->s[i];
                }
                else if (heads[i] && heads[i] == to_del) {
                    heads[i] = nullptr;
                }
            }
            delete to_del;
            return;
        }
        // otherwise... return
        return;
        //if (to_stitch[0] &&
        //    to_stitch[0]->s[0] &&
        //    to_stitch[0]->s[0]->e == e) {

        //    to_del = to_stitch[0]->s[0];
        //    for (int i = max_height - 1; i >= 0; --i) {
        //        if (to_stitch[i] == to_del) {
        //            ASSERT(to_stitch[i]->s[i]->e == e);
        //            to_stitch[i]->s[i] = to_stitch[i]->s[i]->s[i];
        //        }
        //    }
        //}
        //slice to_stitch{slice_preceeding(e)};
        //node* to_del = nullptr;
        //if (to_stitch[0] && to_stitch[0]->s[0] && to_stitch[0]->s[0]->e == e) {
        //    to_del = to_stitch[0]->s[0];
        //}
        //for (int i = max_height - 1; i >= 0; --i) {
        //    if (to_stitch[i]) {
        //        ASSERT(to_stitch[i]->s[i]->e == e);
        //        to_stitch[i]->s[i] = to_stitch[i]->s[i]->s[i];
        //    }
        //    else if (heads[i] && heads[i]->e == e) {
        //        fprintf(stderr, "Restitching: %d %d %d\n", heads[i], heads[i]->e, e);
        //        fflush(stderr);
        //        ASSERT(heads[i]->s.size() >= i);
        //        if (!to_del) { to_del = heads[i]; }
        //        heads[i] = heads[i]->s[i];
        //    }
        //}
        //if (to_del) {
        //    delete to_del;
        //}
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
        T operator*() { return mynode->e; }
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
        T operator*() { return mynode->e; }
        bool operator==(const iterator& that) const { return mynode == that.mynode; }
        bool operator!=(const iterator& that) const { return !((*this) == that); }
    };

    iterator begin() {
        return iterator{heads[0]};
    }
    iterator end() {
        //printf("Making a new end iterator\n");
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
            while (curr && curr->e > e) {
                curr = curr->s[i];
            }
            --i;
        }
        if (curr && curr->e == e) {
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
            while (curr && curr->e > e) {
                curr = curr->s[i];
            }
            --i;
        }
        if (curr && curr->e == e) {
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
