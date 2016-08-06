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

template<class T>
class trie_node {
public:
	bool is_leaf;
	std::string val;
	//trie_node* children[10];
	vector<trie_node*> children;
	
//public:
//	trie_node()
//	{
//		children.reserve(1);
//	}


public:
	size_t size() const { return children.size(); }
	void setChild(trie_node* node) const { children.push_back(node); }
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
	vector<trie_node<string>*> children;

	trie_node<string>* root;

	//needs to hold a node
	//call a add function []
	//the pointer points to node
	//add a new node and contents go into that node

public:
	//ctors
	trie();
	~trie();

	//insert item in trie
	void operator [](std::string value) { 
		trie_node<string> node;
		//trie_node* node = new trie_node();
		node.val = value;
		////begPtr = node*; 
		//root->setChild(node&);
		children.push_back(&node);
		//begPtr = &node;		
		//this can't be converted from trie_node<std::string>* to unsigned int *
		//root->children.push_back(&node);
		
	}

	//iterators
	iterator begin() { return iterator(begPtr); }
	iterator end() { return iterator(endPtr); }

	//capacity
	bool empty() const { return begPtr == endPtr; }
	size_type size() const { return children.size(); }
	//size_type size() const { return root->size(); }


};

template<class T>
trie<T>::trie()
{
	begPtr = nullptr;
	endPtr = begPtr;
}


template<class T>
trie<T>::~trie() {
	delete[] begPtr;
}


//The main tree should have a pointer to 1 root note
//each root note has children, beg ptr to it's parent and end ptr to children?

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