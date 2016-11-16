#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <cstdio>
#include <iterator>

#ifdef LOGGING_INFO
#define LOG_NODE_STEP LOG_node_stepped()
#define LOG_ELT_COMPARISON LOG_elt_comparison()
#else
#define LOG_NODE_STEP
#define LOG_ELT_COMPARISON
#endif

//template<typename T, int max_height, std::function<int()> rnd>
template<typename T, int max_height, int (*gen)(),
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class skip_list {
    private:
    struct node;
    typedef node* pnode;
    typedef std::vector<pnode> slice;
    // the only field, and we initialize it.
    slice heads{max_height, nullptr};

#ifdef LOGGING_INFO
    // Logging information to track performance.
    // Currently single-threaded...
    int node_stepped = 0;
    int elt_compared = 0;

    void LOG_node_stepped() {
        node_stepped++;
    }
    void LOG_elt_comparison() {
        elt_compared++;
    }
public:
    int LOG_get_node_stepped() const {
        return node_stepped;
    }
    void LOG_reset_node_stepped() {
        node_stepped = 0;
    }
    int LOG_get_elt_comparisons() const {
        return elt_compared;
    }
    void LOG_reset_elt_comparisons() {
        elt_compared = 0;
    }
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
        T e;
        slice s;
        node(T e, slice s): e(e), s(s) {}
        node(T&& e, slice&& s): e(e), s(s) {}
    };


    // A very core helper function. This is how we navigate
    slice slice_preceeding(const T& e) {
        slice result{max_height, nullptr};
        slice* curr = &heads;

        for (int i = max_height - 1; i >= 0; --i) {
            // TODO revisit the >= in light of multiset.
            if ((*curr)[i]) { LOG_ELT_COMPARISON; }
            if ((*curr)[i] == nullptr || (*curr)[i]->e >= e) {
                result[i] = nullptr;
            }
            else {
                auto candidate_node = (*curr)[i];
                if (candidate_node->s[i]) { LOG_ELT_COMPARISON; }
                while (candidate_node->s[i] && candidate_node->s[i]->e < e) {
                    curr = &(candidate_node->s);
                    candidate_node = candidate_node->s[i];
                    if (candidate_node->s[i]) { LOG_ELT_COMPARISON; }
                    LOG_NODE_STEP;
                }
                result[i] = candidate_node;
            }
        }
        return result;
    }

    int generate_height() {
        return std::min(gen(),max_height-1) + 1;
    }

public:
    
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
        this->clear();
        this->heads = that->heads;
        // maybe remove this if not necessary.
        that->heads = std::vector<pnode>{0,nullptr};
    }
    skip_list& operator=(std::initializer_list<T> l) {
        this->clear();
        insert(begin(l), end(l));
    }


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

    std::pair<iterator,bool> insert(value_type&& value) {
        value_type v2(value);
        return insert(v2);
    }
    // TODO the parameter is not right.
    std::pair<iterator,bool> insert(const value_type& value) {
        slice predecessors{slice_preceeding(value)};
        // TODO: watch for multiset functionality...
        // do we already have the element?
        if (predecessors[0] &&
            predecessors[0]->s[0] &&
            predecessors[0]->s[0]->e == value) {
            return {predecessors[0],false};
        }

        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        auto new_node = new node{value, new_slice};

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

    void erase(T e) {
        slice to_stitch{slice_preceeding(e)};
        node* to_del = nullptr;
        if (to_stitch[0] && to_stitch[0]->s[0]) {
            to_del = to_stitch[0]->s[0];
        }
        for (int i = max_height - 1; i >= 0; --i) {
            if (to_stitch[i]) {
                assert(to_stitch[i]->s[i]->e == e);
                to_stitch[i]->s[i] = to_stitch[i]->s[i]->s[i];
            }
            else if (heads[i] && heads[i]->e == e) {
                if (!to_del) { to_del = heads[i]; }
                heads[i] = heads[i]->s[i];
            }
        }
        if (to_del) {
            delete to_del;
        }
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
    iterator find(const T& e) {
        int i = max_height - 1;

        // find the highest non-null element.
        while (i >= 0 && !heads[i]) { --i; };
        if (i < 0) { return end(); }
        auto curr = heads[i];
        assert(curr);
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
        assert(curr);
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
