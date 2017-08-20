#include "tree.hpp"

#include <algorithm>
#include <queue>
#include <iostream>

template<class T>
template<class U>
avl_node<T>::avl_node(U&& u, unsigned int hash):
	key(hash),
	data(std::forward<U>(u)){}
template<class T>
avl_node<T>::avl_node(const avl_node& n):
	left(n.left ? new avl_node(*n.left) : nullptr),
	right(n.right ? new avl_node(*n.right) : nullptr),
	key(n.key),
	left_weight(n.left_weight),
	right_weight(n.right_weight),
	data(n.data){}
template<class T>
avl_node<T>::avl_node(avl_node&& n)noexcept:
	left(std::exchange(n.left, nullptr)),
	right(std::exchange(n.right, nullptr)),
	key(n.key),
	left_weight(n.left_weight),
	right_weight(n.right_weight),
	data(std::move(n.data)){}
template<class T>
avl_node<T>::~avl_node(void){
	delete left;
	delete right;
}
template<class T>
avl_node<T>& avl_node<T>::operator=(const avl_node& n){
	avl_node tmp(n);
	swap(*this, tmp);
	return *this;
}
template<class T>
avl_node<T>& avl_node<T>::operator=(avl_node&& n)noexcept{
	swap(*this, n);
	return *this;
}
template<class T>
avl_node<T>* avl_node<T>::rotate_left(void){
	avl_node* tmp = right;
	right = tmp->left;
	tmp->left = this;
	right_weight = tmp->left_weight;
	tmp->left_weight = left_weight + right_weight + 1;
	return tmp;
}
template<class T>
avl_node<T>* avl_node<T>::rotate_right(void){
	avl_node* tmp = left;
	left = tmp->right;
	tmp->right = this;
	left_weight = tmp->right_weight;
	tmp->right_weight = right_weight + left_weight + 1;
	return tmp;
}
template<class T>
avl_node<T>* avl_node<T>::rotate_left_right(void){
	if(left->right)
		left = left->rotate_left();
	return rotate_right();
}
template<class T>
avl_node<T>* avl_node<T>::rotate_right_left(void){
	if(right->left)
		right = right->rotate_right();
	return rotate_left();
}
template<class T>
void avl_node<T>::swap(avl_node& n1, avl_node& n2){
	std::swap(n1.left, n2.left);
	std::swap(n1.right, n2.right);
	std::swap(n1.key, n2.key);
	std::swap(n1.left_weight, n2.left_weight);
	std::swap(n1.right_weight, n2.right_weight);
}

template<class T>
avl_tree<T>::avl_tree(const hash_func_type& h):
	m_hash(h){}
template<class T>
avl_tree<T>::avl_tree(const avl_tree& b){
	if(b.m_root)
		m_root = new node_type(*b.m_root);
}
template<class T>
avl_tree<T>::avl_tree(avl_tree&& b)noexcept:
	m_root(std::exchange(b.m_root, nullptr)){}
template<class T>
avl_tree<T>::~avl_tree(void){
	delete m_root;
}

template<class T>
avl_tree<T>& avl_tree<T>::operator=(const avl_tree& b){
	avl_tree tmp(b);
	std::swap(m_root, tmp.m_root);
	return *this;
}
template<class T>
avl_tree<T>& avl_tree<T>::operator=(avl_tree&& b)noexcept{
	std::swap(m_root, b.m_root);
	return *this;
}
template<class T>
template<class... Args>
void avl_tree<T>::emplace_node(Args&&... args){
	T tmp(std::forward<Args>(args)...);
	i_add_node(node_type(tmp, m_hash(tmp)), &m_root);
	i_check_balance(&m_root);
}
template<class T>
void avl_tree<T>::add_node(const node_type& n){
	i_add_node(n, &m_root);
	i_check_balance(&m_root);
}
template<class T>
void avl_tree<T>::delete_node(unsigned int key){
	node_type* l;
	node_type* r;
	if(m_root->key == key){
		l = m_root->left;
		r = m_root->right;
		m_root->left = nullptr;
		m_root->right = nullptr;
		delete m_root;
		m_root = l;
		i_add_node(r, &m_root);
		i_check_balance(&m_root);
		return;
	}
	node_type* base = i_delete_node(m_root, key);
	if(!base)
		return;
	node_type* targ;
	if(base->left && base->left->key == key){
		targ = base->left;
		base->left = nullptr;
	}else if(base->right && base->right->key == key){
		targ = base->right;
		base->right = nullptr;
	}else{
		return;
	}
	r = targ->right;
	l = targ->left;
	targ->left = nullptr;
	targ->right = nullptr;
	delete targ;
	if(r)
		i_add_node(r, &m_root);
	if(l)
		i_add_node(l, &m_root);
	i_check_balance(&m_root);
}
template<class T>
auto avl_tree<T>::search(unsigned int key)const -> node_type*{
	return i_search(m_root, key);
}

//Gives general outline of tree, use gdb to be more accurate
template<class T>
void avl_tree<T>::dump(void){
	std::queue<node_type*> q;
	q.push(m_root);
	int level = 1;
	int p = 0;
	while(!q.empty()){
		node_type* curr = q.front();
		if(!curr){
			std::cout << " ";
		}else{
			std::cout << curr->key << " ";
			if(curr->left)
				q.push(curr->left);
			else
				q.push(nullptr);
			if(curr->right)
				q.push(curr->right);
			else
				q.push(nullptr);
		}
		q.pop();
		if(++p == level){
			level *= 2;
			p = 0;
			std::cout << std::endl;
		}
	}
	std::cout << "\n";
}

//What tf2 servers need: autobalance
template<class T>
void avl_tree<T>::i_check_balance(node_type** curr){
	if(!(*curr))
		return;
	if(!(*curr)->right_weight && !(*curr)->left_weight)
		return;
	if((*curr)->right_weight > (*curr)->left_weight){
		if((*curr)->right_weight - (*curr)->left_weight > 1){
			if((*curr)->right->right_weight > (*curr)->right->left_weight){
				*curr = (*curr)->rotate_right_left();
			}else{
				*curr = (*curr)->rotate_left();
			}
		}
	}
	else if((*curr)->left_weight > (*curr)->right_weight){
		if((*curr)->left_weight - (*curr)->right_weight > 1){
			if((*curr)->left->left_weight > (*curr)->left->right_weight)
				*curr = (*curr)->rotate_right();
			else
				*curr = (*curr)->rotate_left_right();
		}
	}
	i_check_balance(&(*curr)->right);
	i_check_balance(&(*curr)->left);
}
//Allocate new node
template<class T>
void avl_tree<T>::i_add_node(const node_type& n, node_type** dest){
	if(!(*dest))
		*dest = new node_type(n);
	else if(n.key < (*dest)->key){
		(*dest)->left_weight += n.left_weight + n.right_weight + 1;
		i_add_node(n, &((*dest)->left));
	}
	else{
		(*dest)->right_weight += n.left_weight + n.right_weight + 1;
		i_add_node(n, &((*dest)->right));
	}
}
//Add new node without allocating
template<class T>
void avl_tree<T>::i_add_node(node_type* n, node_type** dest){
	if(!(*dest))
		return;
	if(n->key < (*dest)->key){
		(*dest)->left_weight += n->left_weight + n->right_weight + 1;
		if(!(*dest)->left){
			(*dest)->left = n;
		}else{
			i_add_node(n, &(*dest)->left);
		}
	}else{
		(*dest)->right_weight += n->left_weight + n->right_weight + 1;
		if(!(*dest)->right){
			(*dest)->right = n;
		}else{
			i_add_node(n, &(*dest)->right);
		}
	}
}
//Find node with key or null if no result found
template<class T>
auto avl_tree<T>::i_search(node_type* n, unsigned int key)const -> node_type*{
	if(!n)
		return nullptr;
	if(key == n->key)
		return n;
	if(key < n->key)
		return i_search(n->left, key);
	return i_search(n->right, key);
}
//Remove node from tree, properly decrement weights during traversal.]
//Return null on fail
template<class T>
auto avl_tree<T>::i_delete_node(node_type* n, unsigned int key, unsigned int* dec)const -> node_type*{
	if(!n)
		return nullptr;
	if(n->left && n->left->key == key){
		n->left_weight = 0;
		if(dec)
			*dec = n->left->left_weight + n->left->right_weight + 1;
		return n;
	}
	if(n->right && n->right->key == key){
		n->right_weight = 0;
		if(dec)
			*dec = n->right->left_weight + n->right->right_weight + 1;
		return n;
	}
	if(key < n->key){
		unsigned int d = 0;
		node_type* tmp = i_delete_node(n->left, key, &d);
		n->left_weight -= d;
		return tmp;
	}
	unsigned int d = 0;
	node_type* tmp = i_delete_node(n->right, key, &d);
	n->right_weight -= d;
	return tmp;
}
