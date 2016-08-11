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
#include <cassert>
#include <algorithm>
#include <initializer_list>
using namespace std;

template<class T>
class trie_node {
public:
	using value_type = T;

	bool is_leaf;
	char val;
	trie_node* parent;
	trie_node* childArray[26];
	int children = 0; //keeps count of children in current node
	int leafs = 0; //keeps track of total leaf nodes (Word count)
	value_type v_type;
	string key;

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
	trie_node<value_type>* subNode(char ch)
	{
		if (children > 0) {
			for (int j = 0; j < children; j++) {
				if (childArray[j]->val == ch)
					return childArray[j];
			}
		}
		return nullptr;
	}

	/*value_type operation= (value_type rhs)
	{
		v_type = rhs;
		return *this;
	}*/


};

template <class T>
class trie {
public:
	using value_type = T;
	using size_type = std::size_t;
	using reference = value_type&;
	using trieNode = trie_node<T>*;
	using pointer = trieNode;

	using key_type = std::string;
	using triePair = std::pair<key_type, T>;

	//trie_node<T>* begPtr;
	//trie_node<T>* endPtr;
	pointer begPtr;
	pointer endPtr;

	trie_node<value_type>* root;

	template<typename T>
	class node_iterator : public std::iterator<std::bidirectional_iterator_tag, pair<key_type,T>> {
	private:
		triePair p;
		key_type first;
		T second;
		//operator->
	public:
		node_iterator(trieNode n): first(n->key), second(n->v_type), p(triePair(n->key, n->v_type)) {}
		bool operator == (node_iterator const & rhs) const { return first == rhs.first; }
		bool operator != (node_iterator const & rhs) const { return first != rhs.first; }

//		dereference to get pair
		triePair* operator->()
		{
			//p = triePair(std::make_pair(first, second));
			//return &std::make_pair(first, second);
			return &p;
		}
	};

public:
	using iterator = node_iterator<T>;

	//compare iterator
	//bool operator==(iterator const & a, iterator const & b)
	//{

	//}

public:
	//ctors
	trie();
	~trie();


	//inserts a string into trie. Checks if string exists and if specific letters exists on node

	//leaf holds a key that represents the entire word
	value_type& operator [](std::string value) {
		string temp = "";
		if (search(value)) 
			return returnSearchNode(value)->v_type;
		trie_node<value_type>* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++)
		{
			trie_node<value_type>* child = curr->subNode(*si);
			if (child != nullptr) {
				//set child begPtr to &curr
				curr = child;
				temp += (*si);
			}
			else {
				trie_node<value_type>* newNode = new trie_node<value_type>(*si);
				curr->childArray[curr->children] = newNode;
				curr->children++;
				//sort the parent child array
				std::sort(curr->childArray, curr->childArray + curr->children,
					[](trieNode a, trieNode b) -> bool
				{ return a->val < b->val; });
				//change pointer of curr to point to new node
				newNode->parent = curr;
				curr = newNode;
				temp += (*si);
				//set the begPtr based on all nodes, takes into account latest additions
				begPtr = setBegPtr(root);
			}
		}
		curr->is_leaf = true;
		curr->key = temp;
		//get reference to parent of curr and increase its leaf count too
		root->leafs++;
		
		//endPtr = curr;
		//need to adjust begPtr and point end to nullptr

		//adjusting begPtr here, not Root, but search within for first real (first?)
		return curr->v_type;
	}

	trie_node<T>* setBegPtr(trie_node<T>* node)
	{
		if (node->children == 0)
			return node;
		else
			return setBegPtr(node->childArray[0]);
	}

	//counts how many times a word is found in trie
	size_type count(string value)
	{
		size_type count = 0;
		trie_node<value_type>* curr = root;
		trie_node<value_type>* temp;
		if (root->children != 0)
		{
			for (int i = 0; i < root->children; i++)
			{
				for (string::iterator si = value.begin(); si != value.end(); si++)
				{
					temp = curr->subNode(*si);
					if (temp != nullptr)
					{
						curr = temp;
					}
					else
					{
						return 0;
					}
				}
				if (curr->is_leaf == true)
					//have a method that takes a node and returns # of leafs inside
					return 1;
				else {
					return curr->children;
				}
			}
			return 0;
		}
		else {
			return 0;
		}
	}

	trie_node<T>* returnSearchNode(string value)
	{
		trie_node<value_type>* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++)
		{
			curr = curr->subNode(*si);
			if (curr == nullptr)
				return false;
		}
		return curr;
	}


	//searches the trie to see if a string already exists
	bool search(string value)
	{
		trie_node<value_type>* curr = root;
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
	root = new trie_node<T>();
	endPtr = root;
	begPtr = endPtr;
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