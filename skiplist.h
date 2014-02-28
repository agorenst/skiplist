#ifndef _SKIP_LIST_H_
#define _SKIP_LIST_H_

#include <vector>
#include <time.h>
#include <algorithm>

template<class K, class V>
class slist {
    private:

        // This is a node in our linked list.
        class node {
            public:
                unsigned h; // height
                std::vector<node* > n; // links: our "next" nodes.
                std::pair<K,V> d; // the actual data our node holds.

                node(unsigned h, K k, V v): h(h), n(h, NULL), d(k,v) {}
                // straightforward copy, but we do NOT copy the links.
                node(const node* s): h(s->h), n(s->h, NULL), d(s->d) {}

                const K& key() { return d.first; }
                const V& val() { return d.second; }
        };
        // arbitrarily picked height---parameterize later.
        static const unsigned height = 10;

        // our actual fields.
        const node* tail;
        node* head;

        // an iterator for our container.
        // should inherit from iterator traits etc.
        class iter {
            public:
                node* n;
                iter(node* n): n(n) {}
                iter& operator++() {
                    n = n->n[0];
                    return *this;
                }
                const std::pair<K,V>& operator*() {
                    return n->d;
                }
                const std::pair<K,V>* operator->() const {
                    return &(n->d);
                }
                bool operator==(const iter& i) const {
                    // if they're both pointing to the same node.
                    // (in memory, of course)
                    return i.n == n;
                }
                bool operator!=(const iter& i) const {
                    return !((*this) == i);
                }
                bool operator<(K k) const {
                    return n->key() < k;
                }
        };
    public:
        // this is cheating! figure out how to make a real const_iterator.
        typedef iter const_iterator;

        slist();
        slist(const slist<K,V>& o); 
        ~slist(); 
        const_iterator begin() const;
        const_iterator end() const;
        bool empty() const ;
        bool has(K k) const ; 
        void insert(K k, V v);
        void remove(K k);
};



//
// These are the executable code.
//
//
unsigned rlevel(const unsigned height) {
    unsigned lvl = 1;
    while (static_cast<double>(rand())/RAND_MAX < (0.5) && lvl < height-1) { ++lvl; }
    return lvl;
}

template<class K, class V>
slist<K,V>::~slist() {
    node* prev = head;
    while (head != NULL) {
        head = head->n[0];
        delete prev;
        prev = head;
    }
}


template<class K, class V>
slist<K,V>::slist(const slist<K,V>& o): tail(NULL), head(new node(o.head)) {
    for (slist<K,V>::const_iterator it = o.begin(); it != o.end(); ++it) {
        insert(it->first,it->second);
    }
}

template<class K, class V>
slist<K,V>::slist(): tail(NULL), head(new node(height, K(), V())) { }

template<class K, class V>
typename slist<K,V>::const_iterator slist<K,V>::begin() const { return iter(head->n[0]); }

template<class K, class V>
typename slist<K,V>::const_iterator slist<K,V>::end() const { return iter(NULL); }

template<class K, class V>
bool slist<K,V>::empty() const {
    for (int i = 0; i < height; ++i) {
        if (head[i] != NULL) { return true; }
    }
    return false;
}

template<class K, class V>
bool slist<K,V>::has(K k) const {
    node* x = head;
    for (int i = height-1; i >= 0; --i) {
        while (x->n[i] != tail && x->n[i]->key() < k) {
            x = x->n[i];
        }
    }
    x = x->n[0];
    return (!(x == NULL || x->key() != k));
}



template<class K, class V>
void slist<K,V>::insert(K k, V v) {
    node* x = head;
    
    // this keeps track of the nodes that will
    // be pointing to our newly inserted node.
    std::vector<node*> update(height, NULL);

    // going from the highest to the lowest,
    for (int i = height-1; i >= 0; --i) {
        // if we find an intermediary node less than our key
        while (x->n[i] != tail && x->n[i]->key() < k) {
            // continue
            x = x->n[i];
        }
        // remember 
        update[i] = x;
    }

    x = x->n[0];
    if (x != NULL && x->key() == k) {
        return;
    }
    unsigned lvl = rlevel(height);
    node* y = new node(lvl, k, v);
    for (unsigned i = 0; i < lvl; ++i) {
        y->n[i] = update[i]->n[i];
        update[i]->n[i] = y;
    }
}
#endif
