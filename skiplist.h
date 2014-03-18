
#ifndef _SKIP_LIST_H_
#define _SKIP_LIST_H_

#include <vector>
#include <time.h>
#include <algorithm>
#include <memory>
#include <iostream>

/*
* The goal here is to look like a first-class C++ object.
* We can make multisets, and from there lists, (multi)maps,
* and regular sets are more straightforward.
*
* This is largely for my own edification, but we also have
* the goal of very carefully, explicitly explaining what
* goes into a C++ container (assuming, of course, that I learn it!).
*/

const int height = 10;

template<class T, class Compare = std::less<T>, class Alloc = std::allocator<T>>
class skiplist_multiset {
    public:
    const static int H = 10;
    // represents the links in the skiplist.
    // zero-indexed, with vector[0] being the lowest level,
    // vector[height-1] being the highest.

    class node;

    typedef std::vector<std::shared_ptr<node>> edge_list;

    class link_slice {
        protected:
        std::vector<std::shared_ptr<node>> links;
    };

    class prior_slice : public link_slice {
        private:
        bool invariant_element_ordering() const {
            // say our data is 8
            // height[0] is the closest element, 7
            // height[1] is the next-closest, 6
            // height[2] may be 5 (or 6), or it may be NULL;

            for (unsigned i = 0; i < links.size()-1; ++i) {
                if (links[i] != NULL) {
                    assert(links[i+1] == NULL || links[i+1]->data <= links[i]->data);
                }
            }
        }
        bool invariant_null_ordering() const {
            for (unsigned i = 0; i < links.size()-1; ++i) {
                if (links[i] == NULL) {
                    assert(links[i+1] == NULL);
                    // if a higher height (i+1) is not null, then "at least"
                    // links[i] = links[i+1], so this is a contradiction.
                }
            }
        }
        bool invariant_prior_validity(const T& data) const {
            for (unsigned i = 0; i < links.size(); ++i) {
                if (p != NULL) {
                    assert(p->data <= data && (p->next[i] == NULL || p->next[i]->data > data));
                }
            }
        }
    };


    class successor_slice : public link_slice {
        private:
        bool invariant_element_ordering() const {
            for (unsigned i = 0; i < links.size()-1; ++i) {
                // intuitively, links[0] is the next-largest element ---
                // links[1] is something a little larger, or NULL
                if (links[i] != NULL) {
                    assert(links[i+1] == NULL || links[i+1]->data >= links[i]->data);
                }
            }
        }
        bool invariant_null_ordering() const {
            for (unsigned i = 0; i < links.size()-1; ++i) {
                if (links[i] == NULL) {
                    // intuitively, links[0] is the next-largest element ---
                    // if anything is non-null, this is
                    assert(links[i+1] == NULL);
                }
            }
        }
        bool invariant_prior_validity(const T& data) conts {
            for (unsigned i = 0; i < links.size(); ++i) {
                if (p != NULL) {
                    assert(p->data > data);
                }
            }
        }
    };

    // This represents the node in our linked list
    class node {
        public:
        edge_list edges;
        T data;
        node(const T& data, int height): edges(height, NULL), data(data) {}
    };

    static int get_max_prior_height(const edge_list& e, const T& data) {
        for (int i = H-1; i >= 0; --i) {
            if (e[i] != NULL && e[i]->data < data) {
                return i;
            }
        }
        return -1;
    }

    // when we proceed through the skiplist, we want to remember all possible predecessors,
    static edge_list get_priors(const edge_list& e, const T& data, bool equality = false) {
        edge_list priors(H, NULL);
        int height = get_max_prior_height(e, data);
        if (height >= 0) {
            std::shared_ptr<node> n = e[height];
            for (; height >= 0; --height) {
                // this can probably be phrased better. The only different in cases is the <= vs <
                if (!equality) {
                    while (n->edges[height] != NULL && n->edges[height]->data < data) {
                        n = n->edges[height];
                    }
                }
                if (equality) {
                    while (n->edges[height] != NULL && n->edges[height]->data <= data) {
                        n = n->edges[height];
                    }
                }
                priors[height] = n;
            }
        }
        return priors;
    }


    int rheight() {
        int height = 1;
        while (rand() % 2 == 0 && height < H) {
            ++height;
        }
        return height;
    }

    edge_list head;
    public:
    skiplist_multiset(): head(H,NULL) {}

    // http://stackoverflow.com/questions/7758580/writing-your-own-stl-container/7759622#7759622
    // design decision: may want to save the "priors", so can be more easily used for "erase".
    class const_iterator : public std::iterator<std::forward_iterator_tag,T> {
        private:
        const node* ptr;
        public:
        // point of confusion: is there actually a difference between these operators?
        // the SO site says that operator-> returns T* instead. Confusing.
        const T& operator*() const { return ptr->data; }
        const T& operator->() const { return ptr->data; }
        bool operator==(const const_iterator& c) { return ptr == c.ptr; }
        bool operator!=(const const_iterator& c) { return ptr != c.ptr; }
        const_iterator& operator=(const const_iterator&c ) { ptr = c.ptr; }
        const_iterator(): ptr(NULL) {}
        const_iterator(const const_iterator& c): ptr(c.ptr) {}
        const_iterator(node* ptr): ptr(ptr) {}
    };

    const_iterator begin() const { return const_iterator(head[0]); }
    const_iterator end() const { return const_iterator(); }

    const_iterator find(const T& data) const {
        auto priors = get_priors(head, data);
        if (priors[0] != NULL && // is there any element we have less than data?
            priors[0]->edges[0] != NULL && // if so, is there an element right after?
            priors[0]->edges[0]->data == data) { // if so, is it actually our element?
            return const_iterator(priors[0]->edges[0]);
        }

        // edge case: if data is the smallest element, it may be head:
        if (head[0] != NULL && head[0]->data == data) {
            return const_iterator(head[0]);
        }
        return end();
    }

    bool insert(const T& data) {
        // according to http://en.cppreference.com/w/cpp/container/multiset/insert,
        // this should insert *after* everything else. Hence we use equality.
        auto priors = get_priors(head, data, true);
        int node_height = rheight();
        std::shared_ptr<node> to_insert(new node(data,height));

        for (unsigned i = 0; i < node_height; ++i) {
            if (priors[i] == NULL) {
                std::cout << "For i = " << i << ", priors[i] is NULL" << std::endl;
                to_insert->edges[i] = head[i];
                head[i] = to_insert;
            }
            else {
                std::cout << "For i = " << i << ", priors[i] points to " << priors[i]->data << std::endl;
                to_insert->edges[i] = priors[i]->edges[i];
                priors[i]->edges[i] = to_insert;
            }
        }
        std::cout << "Is head[0] null? " << (head[0] == NULL) << std::endl;
        return true;
    }

    // erasing all the values with k is tricky---the priors vector may behave strangely over
    // erasing multiple elements.
    // Need some kind of backtracking process.
    bool erase(const T& data) {
        auto priors = get_priors(head, data);

    }

    void print(std::ostream& o) {
        std::shared_ptr<node> n = head[0];
        o << "Is the first n equal to null? " << (n == NULL) << "\n";
        while (n != NULL) {
            o << n->data << " ";
            n = n->edges[0];
        }
    }
};

#endif
