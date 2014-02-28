#include "skiplist.h"

using namespace std;

template<class K, class V>
slist<K,V>::~slist() {
	node* prev = head;
	while (head != NULL) {
		head = head->n[0];
		delete prev;
		prev = head;
	}
}
/*
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
		if (x->n[i] != tail && x->n[i]->key() < k) {
			x = x->n[i];
		}
		x = x->n[0];
		return (!(x == NULL || x->key() != k));
	}
	return false;
}

template<class K, class V>
void slist<K,V>::insert(K k, V v) {
	node* x = head;
	vector<node*> update(height, NULL);
	for (int i = height-1; i >= 0; --i) {
		while (x->n[i] != tail && x->n[i]->key() < k) {
			x = x->n[i];
		}
		update[i] = x;
	}
	x = x->n[0];
	if (x != NULL && x->key() == k) {
		return;
	}
	unsigned lvl = rlevel();
	node* y = new node(lvl, k, v);
	for (unsigned i = 0; i < lvl; ++i) {
		y->n[i] = update[i]->n[i];
		update[i]->n[i] = y;
	}
}
*/
