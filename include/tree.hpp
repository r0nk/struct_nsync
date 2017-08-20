#ifndef TREE_HPP
#define TREE_HPP

#include <algorithm>
#include <cstdint>
#include <functional>

//FNV hash
template<class T>
unsigned int tmp_hash_fun(const T& t){
	const char* input = reinterpret_cast<const char*>(&t);
	unsigned int hash = 2166136261L;
	for(size_t i = 0;i < sizeof(t);i++)
		hash = (hash * 16777619) ^ input[i];
	return hash;
}

template<class T>
struct avl_node
{
	avl_node* left = nullptr;
	avl_node* right = nullptr;
	unsigned int key = 0;
	unsigned int left_weight = 0;
	unsigned int right_weight = 0;
	T data{};

	avl_node(void) = default;
	template<class U>
	avl_node(U&& u, unsigned int hash);
	avl_node(const avl_node&);
	avl_node(avl_node&&)noexcept;
	~avl_node(void);

	avl_node& operator=(const avl_node&);
	avl_node& operator=(avl_node&&)noexcept;

	avl_node* rotate_left(void);
	avl_node* rotate_right(void);
	avl_node* rotate_left_right(void);
	avl_node* rotate_right_left(void);
	static void swap(avl_node&, avl_node&);
};

template<class T>
class avl_tree
{
public:
	using node_type = avl_node<T>;
	using underlying_type = T;
	using hash_func_type = std::function<unsigned int(const T&)>;
private:
	hash_func_type m_hash{tmp_hash_fun<T>};
	node_type* m_root = nullptr;

public:
	avl_tree(void) = default;
	avl_tree(const hash_func_type&);
	avl_tree(const avl_tree&);
	avl_tree(avl_tree&&)noexcept;
	~avl_tree(void);

	avl_tree& operator=(const avl_tree&);
	avl_tree& operator=(avl_tree&&)noexcept;

	template<class... Args>
	void emplace_node(Args&&...);
	void add_node(const node_type&);

	void delete_node(unsigned int key);
	node_type* search(unsigned int key)const;

	void dump(void);

private:
	void i_check_balance(node_type** curr);
	void i_add_node(const node_type& n, node_type** dest);
	void i_add_node(node_type* n, node_type** dest);
	node_type* i_search(node_type* n, unsigned int key)const;
	node_type* i_delete_node(node_type* n, unsigned int key, unsigned int* = nullptr)const;
};

#include "tree.tpp"

#endif
