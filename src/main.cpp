#include <stdio.h>

#include "tree.hpp"

struct test_struct
{
	test_struct(int o, int i):
		x(i), y(o){}
	int x;
	int y;
};

int main(){
	auto tmp = tmp_hash_fun(test_struct(1, 2));
	avl_tree<test_struct> t;
	t.emplace_node(1, 2);
	t.emplace_node(3, 4);
	t.emplace_node(5, 6);
	t.emplace_node(7, 8);
	t.emplace_node(9, 10);
	t.emplace_node(11, 12);
	printf("lol\n");
}
