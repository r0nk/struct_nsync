#include "tree.hpp"

#include <algorithm>
#include <queue>
#include <iostream>

node::node(unsigned int k):
	key(k){}
node::node(const node& n):
	left(n.left ? new node(*n.left) : nullptr),
	right(n.right ? new node(*n.right) : nullptr),
	key(n.key),
	left_weight(n.left_weight),
	right_weight(n.right_weight){}
node::node(node&& n)noexcept:
	left(std::exchange(n.left, nullptr)),
	right(std::exchange(n.right, nullptr)),
	key(n.key),
	left_weight(n.left_weight),
	right_weight(n.right_weight){}
node::~node(void){
	delete left;
	delete right;
}
node& node::operator=(const node& n){
	node tmp(n);
	swap(*this, tmp);
	return *this;
}
node& node::operator=(node&& n)noexcept{
	swap(*this, n);
	return *this;
}
node* node::rotate_left(void){
	node* tmp = right;
	right = tmp->left;
	tmp->left = this;
	right_weight = tmp->left_weight;
	tmp->left_weight = left_weight + right_weight + 1;
	return tmp;
}
node* node::rotate_right(void){
	node* tmp = left;
	left = tmp->right;
	tmp->right = this;
	left_weight = tmp->right_weight;
	tmp->right_weight = right_weight + left_weight + 1;
	return tmp;
}
node* node::rotate_left_right(void){
	if(left->right)
		left = left->rotate_left();
	return rotate_right();
}
node* node::rotate_right_left(void){
	if(right->left)
		right = right->rotate_right();
	return rotate_left();
}
void node::swap(node& n1, node& n2){
	std::swap(n1.left, n2.left);
	std::swap(n1.right, n2.right);
	std::swap(n1.key, n2.key);
	std::swap(n1.left_weight, n2.left_weight);
	std::swap(n1.right_weight, n2.right_weight);
}

avl_tree::avl_tree(const avl_tree& b):
	m_root(new node(b.m_root ? *b.m_root : 0)){}
avl_tree::avl_tree(avl_tree&& b)noexcept:
	m_root(std::exchange(b.m_root, nullptr)){}
avl_tree::~avl_tree(void){
	delete m_root;
}

avl_tree& avl_tree::operator=(const avl_tree& b){
	avl_tree tmp(b);
	std::swap(m_root, tmp.m_root);
	return *this;
}
avl_tree& avl_tree::operator=(avl_tree&& b)noexcept{
	std::swap(m_root, b.m_root);
	return *this;
}
void avl_tree::add_node(const node& n){
	i_add_node(n, &m_root);
	i_check_balance(&m_root);
}
void avl_tree::delete_node(unsigned int key){
	node* l;
	node* r;
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
	node* base = i_delete_node(m_root, key);
	if(!base)
		return;
	node* targ;
	if(base->left && base->left->key == key){
		targ = base->left;
		base->left = nullptr;
	}else if(base->right && base->right->key == key){
		targ = base->right;
		base->right = nullptr;
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
node* avl_tree::search(unsigned int key)const{
	return i_search(m_root, key);
}

//Gives general outline of tree, use gdb to be more accurate
void avl_tree::dump(void){
	std::queue<node*> q;
	q.push(m_root);
	int level = 1;
	int p = 0;
	node empty;
	while(!q.empty()){
		node* curr = q.front();
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
void avl_tree::i_check_balance(node** curr){
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
void avl_tree::i_add_node(const node& n, node** dest){
	if(!(*dest))
		*dest = new node(n);
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
void avl_tree::i_add_node(node* n, node** dest){
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
node* avl_tree::i_search(node* n, unsigned int key)const{
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
node* avl_tree::i_delete_node(node* n, unsigned int key, unsigned int* dec)const{
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
		node* tmp = i_delete_node(n->left, key, &d);
		n->left_weight -= d;
		return tmp;
	}
	unsigned int d = 0;
	node* tmp = i_delete_node(n->right, key, &d);
	n->right_weight -= d;
	return tmp;
}
