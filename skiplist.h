#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <cstdio>
#include <iterator>

//template<typename T, int max_height, std::function<int()> rnd>
template<typename T, int max_height, int (*gen)()>
class skip_list {
    private:
    struct node;
    typedef std::vector<std::shared_ptr<node>> slice;
    //typedef std::vector<node*> slice;

    struct node {
        T e;
        slice s;
        node(T e, slice s): e(e), s(s) {}
        node(T&& e, slice&& s): e(e), s(s) {}
    };

    void dbg_print_slice(const slice& s) {
        //std::for_each(std::rbegin(s), std::rend(s), [](const auto& m){
        //    printf("\t(%p)", m.get());
        //    printf("\n");
        //});
    }

    void dbg_print_node(node* n) {
        //printf("node (%p)\n", n);
        //dbg_print_slice(n->s);
    }

    slice heads{max_height, nullptr};

    slice slice_preceeding(T e, int height = max_height) {
        slice result{max_height, nullptr};
        slice* curr = &heads;
        for (int i = height - 1; i >= 0; --i) {
            // TODO revisit the >= in light of multiset.
            if ((*curr)[i] == nullptr || (*curr)[i]->e >= e) {
                result[i] = nullptr;
            }
            else {
                auto candidate_node = (*curr)[i];
                while (candidate_node->s[i] && candidate_node->s[i]->e < e) {
                    candidate_node = candidate_node->s[i];
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

    void insert(T e) {
        int new_height = generate_height();
        slice new_slice(new_height, nullptr);
        auto new_node = std::make_shared<node>(e, new_slice);
        //auto new_node = new node(e, new_slice);

        slice predecessors = slice_preceeding(e, new_height);
        //printf("found predecessor slice for %d:\n", e);
        dbg_print_slice(predecessors);
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

    bool contains(T e) const {
        auto curr = heads[max_height];
        int i = max_height - 2;
        for (; i >= 0; --i) {
            while (curr == nullptr || curr->e > e) {
                curr = heads[i];
            }
        }
        if (curr == nullptr) {
            assert(i == 0);
            return false;
        }
        while (i > 0) {
            while (curr && curr->e > e) {
                curr = curr->s[i];
            }
            --i;
        }
        return curr && curr->e == e;
    }

    void erase(T e) {
        slice to_stitch = slice_preceeding(e, max_height);
        //printf("erase: found predecessor slice for %d:\n", e);
        dbg_print_slice(to_stitch);
        for (int i = max_height - 1; i >= 0; --i) {
            if (to_stitch[i]) {
                assert(to_stitch[i]->s[i]->e == e);
                to_stitch[i]->s[i] = to_stitch[i]->s[i]->s[i];
            }
            else if (heads[i]) {
                assert(heads[i]->e == e);
                heads[i] = heads[i]->s[i];
            }
        }
    }
    
    void dbg_print_list() {
        //auto curr = heads[0];
        //printf("head:\n");
        //dbg_print_slice(heads);
        //while (curr) {
        //    dbg_print_node(curr.get());
        //    curr = curr->s[0];
        //}
    }


// Definitely temporary, just getting things hooked up.
    struct iterator {
        node* mynode;
        iterator& operator++() {
            //printf("increment mynode from %p\n", mynode);
            if (mynode) { mynode = mynode->s[0].get(); }
            //printf("now: %p\n", mynode);
            return *this;
        }
        T operator*() { return mynode->e; }
        bool operator==(const iterator& that) const { return mynode == that.mynode; }
        bool operator!=(const iterator& that) const { return !((*this) == that); }
    };

    iterator begin() {
        //printf("Making a new iterator with %p\n", heads[0].get());
        return iterator{heads[0].get()};
    }
    iterator end() {
        //printf("Making a new end iterator\n");
        return iterator{nullptr};
    }

};
