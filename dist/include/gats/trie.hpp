#if !defined(GUARD_trie_hpp)
#define GUARD_trie_hpp

/**	@file trie.hpp
	@author Garth Santor
	@date 2016-05-12
	@version 1.0.0
	@note Developed for C++11/vc14
	@breif trie<T> template implementation.
	*/

#include <cstddef>
#include <exception>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <initializer_list>
using namespace std;

class trie_node {
public:
	bool is_leaf;
	char val;
	trie_node* childArray[26];
	int children = 0; //keeps count of children in current node
	int leafs = 0; //keeps track of total leaf nodes (Word count)

	//ctor/dtor
	trie_node() :is_leaf(false), val(char(0)), childArray{ nullptr } {}
	trie_node(char ch) : is_leaf(false), val(ch), childArray{ nullptr } {}
	~trie_node() {
		for (int i = 0; i < children; i++)
			delete childArray[i];
	}

public:
	size_t size() const { return children; }

	//return a pointer to a child node that contains the value
	trie_node* subNode(char ch)
	{
		if (children > 0) {
			for (int j = 0; j < children; j++) {
				if (childArray[j]->val == ch)
					return childArray[j];
			}
		}
		return nullptr;
	}
};

template <class T>
class trie {
public:
	using value_type = T;
	using size_type = std::size_t;
	using reference = value_type&;
	using pointer = value_type*;
	using iterator = pointer;

	pointer begPtr;
	pointer endPtr;
	trie_node* root;

public:
	//ctors
	trie();
	~trie();

	//inserts a string into trie. Checks if string exists and if specific letters exists on node
	void operator [](std::string value) {
		if (search(value)) return;
		trie_node* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++)
		{
			trie_node* child = curr->subNode(*si);
			if (child != nullptr) {
				curr = child;
			}
			else {
				trie_node* newNode = new trie_node(*si);
				curr->childArray[curr->children] = newNode;
				curr->children++;
				curr = newNode;
				//change pointer of curr to point to new node
			}
		}
		curr->is_leaf = true;
		root->leafs++;
	}

	//searches the trie to see if a string already exists
	bool search(string value)
	{
		trie_node* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++)
		{
			curr = curr->subNode(*si);
			if (curr == nullptr)
				return false;
		}
		return curr->is_leaf == true;
	}

	//iterators
	iterator begin() { return iterator(begPtr); }
	iterator end() { return iterator(endPtr); }

	//capacity
	bool empty() const { return begPtr == endPtr; }

	size_type size() {
		return root->leafs;
	}

};

template<class T>
trie<T>::trie()
{
	begPtr = nullptr;
	endPtr = begPtr;
	root = new trie_node();
}


template<class T>
trie<T>::~trie() {
	delete root;
}

/*============================================================================

Revision History

Version 1.0.0: 2016-05-12
Initial development.


==============================================================================
Copyright Garth Santor, 2016

The copyright to the computer program(s) herein
is the property of Garth Santor, Canada.
The program(s) may be used and/or copied only with
the written permission of Garth Santor
or in accordance with the terms and conditions
stipulated in the agreement/contract under which
the program(s) have been supplied.
============================================================================*/
#endif // GUARD_trie_hpp