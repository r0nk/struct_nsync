#ifndef TREE_HPP
#define TREE_HPP

#include <algorithm>

struct node
{
	node* left = nullptr;
	node* right = nullptr;
	unsigned int key = 0;
	unsigned int left_weight = 0;
	unsigned int right_weight = 0;

	node(void) = default;
	node(unsigned int k);
	node(const node&);
	node(node&&)noexcept;
	node& operator=(const node&);
	node& operator=(node&&)noexcept;
	node* rotate_left(void);
	node* rotate_right(void);
	node* rotate_left_right(void);
	node* rotate_right_left(void);
	~node(void);
	static void swap(node&, node&);
};

class avl_tree
{
private:
	node* m_root = nullptr;

public:
	avl_tree(void) = default;
	avl_tree(const avl_tree&);
	avl_tree(avl_tree&&)noexcept;
	~avl_tree(void);

	avl_tree& operator=(const avl_tree&);
	avl_tree& operator=(avl_tree&&)noexcept;

	template<class... Args>
	void emplace_node(Args&&...);
	void add_node(const node&);

	void delete_node(unsigned int key);
	node* search(unsigned int key)const;

	void dump(void);

private:
	void i_check_balance(node** curr);
	void i_add_node(const node& n, node** dest);
	void i_add_node(node* n, node** dest);
	node* i_search(node* n, unsigned int key)const;
	node* i_delete_node(node* n, unsigned int key, unsigned int* = nullptr)const;
};

//templates must be defined in the header
template<class... Args>
void avl_tree::emplace_node(Args&&... args){
	i_add_node(node(std::forward<Args>(args)...), &m_root);
	i_check_balance(&m_root);
}

#endif
