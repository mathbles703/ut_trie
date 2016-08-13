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
#include <cassert>
using namespace std;

/*Trie Node Class

  Represents a trie node inside a trie. Will hold at minimum a value representing the prefix, 
  bool to indicate if it's a leaf node (end of word)
  an array of trie_node pointers
  and a parent (excluding the root node)*/
template<class T>
class trie_node {
public:
	using value_type = T;

	bool is_leaf;						//indicates node is end of a word
	char val;							//the character prefix the node will hold
	trie_node* parent;					//pointer to parent (root wont have one)
	trie_node* childArray[26];			//array will hold all postfix nodes
	int children = 0;					//keeps count of children in current node
	int leafs = 0;						//keeps track of total leaf nodes (Word count)
	value_type nodeTypeValue{};			//node can be given a value of type T based on trie<T>
	string key;							//The key represents the whole word and is stored in leaf node

	//ctor/dtor
	trie_node() :is_leaf(false), val(char(0)), childArray{ nullptr }, parent(nullptr) {}
	trie_node(char ch) : is_leaf(false), val(ch), childArray{ nullptr }, parent(nullptr) {}
	//used to clean all children. Dtor will be called recursively
	~trie_node() {
		for (int i = 0; i < children; i++)
			delete childArray[i];
	}

public:
	//return the number of childen for that node
	size_t size() const { return children; }

	//return a pointer to a child node that contains the char value
	//returns nullptr if no match
	trie_node<value_type>* subNode(char ch) {
		if (children > 0) {
			for (int j = 0; j < children; j++) {
				if (childArray[j]->val == ch)
					return childArray[j];
			}
		}
		return nullptr;
	}
};


/*Trie Class

  Represents a trie class. It will have 1 root node 
  and 2 pointers to a begPtr and endPtr*/
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

	pointer begPtr;
	pointer endPtr;
	pointer trie::absEnd;
	trie_node<value_type>* root;

public:
	//ctor/dtors
	trie()
	{
		root = new trie_node<T>();				//holds ALL node children
		trie::absEnd = new trie_node<T>();		//indicates Absolute end of trie. Used in iteration
		endPtr = trie::absEnd;
		begPtr = endPtr;
	}

	~trie() {
		delete root;
		delete trie::absEnd;
	}



	/*Iterator Class
	
	  Used to iterate through a trie. Will hold a pointer to current Node,
	  a pointer to the absolute end pointer*/
	template<typename T>
	class node_iterator : public std::iterator<std::bidirectional_iterator_tag, pair<key_type, T>> {
	private:
		triePair p;
		trieNode currentNode;
		key_type first;
		T second;
		trieNode absEndPtr;

	public:
		//iterator initializers
		node_iterator(trieNode n, trieNode e) : first(n->key), second(n->nodeTypeValue), p(triePair(n->key, n->nodeTypeValue)), currentNode(n), absEndPtr(e) {}

		//sets internal pair to reflect the currentNode
		void setIteratorPair()
		{
			p = triePair(std::make_pair(currentNode->key, currentNode->nodeTypeValue));
		}

		//operators
		bool operator == (node_iterator const & rhs) const { 
			//this check is used to check if the iterator has reached the absolute end pointer
			if (currentNode == absEndPtr) 
				return true;
			else
				return second == rhs.second; 
		}

		bool operator != (node_iterator const & rhs) const { return second != rhs.second; }

		//dereference to get pair
		triePair* operator->() { return &p; }

		//pre-increment operator
		node_iterator& operator++() {
			currentNode = next(currentNode->parent, currentNode->val);
			if (currentNode == absEndPtr) {}
			else
				setIteratorPair();
			return *this;
		}

		//Uses the passed in node and checks 
		trieNode getLeafNode(trieNode node) {
			if (node->nodeTypeValue != T{})
				return node;
			else if (node->children > 0)
				return getLeafNode(node->childArray[0]);
			else
				return node->parent;
		}

		//used to iterate to the next leaf node
		//this method will take a lead node initially
		//it will crawl back up the tree until it hits a node that splits (has another different child/path)
		//the next child node is passed back to this method and it will crawl down that path
		//a method called GetLeafNode is used recursively
		//if it meets a leaf node, it will return a pointer to it
		//otherwise it will return the parent of the last node passed to the method
		trieNode next(trieNode parentNode, char value)
		{
			int nextNodeIndex = 0;
			trieNode returnNode;
			bool breakFunction = false; //used to break out of loop when next valid path/node is found

			if (parentNode->leafs == 1)
				return next(parentNode->parent, parentNode->val);
			else {

				for (int i = 0; i < parentNode->children; i++) {
					trieNode tempNode = parentNode->childArray[i];
					if (tempNode->val == value) {
						nextNodeIndex = i + 1;
						breakFunction = true;
					}

					//if we have exhausted all valid paths and we have reached the node, we return the absolute end pointer
					if ((nextNodeIndex >= parentNode->children) && (parentNode->val == char(0)))
						return absEndPtr;

					//if we have exhausted all paths, we will go up 1 level
					if (nextNodeIndex >= parentNode->children)
						return next(parentNode->parent, parentNode->val);
					
					if (breakFunction == true) break;
				}
				//the next valid leaf node
				returnNode = getLeafNode(parentNode->childArray[nextNodeIndex]);
				assert(returnNode->children == 0);	//ensure it is a real leaf node
			}
			return returnNode;
		}
	};

//late using initialized, needs to see node_iterator first
public:
	using iterator = node_iterator<T>;


	//When a leaf node in inserted into trie
	//this method will crawl up to the root node
	//and increase the leaf count on every node it touches
	void increaseLeafCount(trieNode node) {
		trieNode parent = node->parent;
		if (parent == nullptr)
			return;
		else {
			int parentLeafPreIncrement = parent->leafs;
			parent->leafs++;
			assert(parent->leafs == parentLeafPreIncrement + 1);	//ensure the leafs were incremented
			increaseLeafCount(parent);
		}
	}

	//inserts a string into trie.
	//Checks if string exists. If prefix exists, a pointer to the end of prefix is used for insert point for new node
	//leaf holds a key that represents the entire word
	value_type& operator [](std::string value) {
		string temp = "";
		if (search(value))
			return returnSearchNode(value)->nodeTypeValue;
		trie_node<value_type>* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++) {
			trie_node<value_type>* child = curr->subNode(*si);
			if (child != nullptr) {
				//set child begPtr to &curr
				curr = child;
				temp += (*si);
			}
			else {
				trie_node<value_type>* newNode = new trie_node<value_type>(*si);
				int childrenPreInsert = curr->children;	//used for assert test
				curr->childArray[curr->children] = newNode;
				curr->children++;
				assert(curr->children == childrenPreInsert + 1);
				//sort the parent child array by the char value
				std::sort(curr->childArray, curr->childArray + curr->children,
					[](trieNode a, trieNode b) -> bool
				{ return a->val < b->val; });
				//set the parent of the new node, and then make it the current node
				newNode->parent = curr;
				curr = newNode;
				temp += (*si);	//increment the string to be added to leaf node as key
			}
		}
		curr->is_leaf = true;
		curr->key = temp;			//used to hold whole word in leaf node
		increaseLeafCount(curr);	//crawler method will go from leaf to root and increase leaf by ++
		return curr->nodeTypeValue;
	}

	//returns first found node that has a nodeTypeValue. 
	trie_node<T>* getNodeWithValue(trie_node<T>* node) {
		if (node->nodeTypeValue != T{})
		{
			if(node->parent != nullptr)
				assert(node->val != char(0));	//ensure node has a value
			return node;
		}
		else if (node->children > 0)
			return getNodeWithValue(node->childArray[0]);
		else
			return root;
	}

	//counts how many times a word is found in trie
	size_type count(string value) {
		size_type count = 0;
		trie_node<value_type>* curr = root;
		trie_node<value_type>* temp;
		if (root->children != 0) {
			for (int i = 0; i < root->children; i++) { //searches every child of root
				for (string::iterator si = value.begin(); si != value.end(); si++) {
					temp = curr->subNode(*si); //searches character by character for each letter. As soon as one letter is NOT found, 0 is added to count
					if (temp != nullptr) {
						curr = temp;
					}
					else { return 0; }
				}
				if (curr->is_leaf == true)
					return 1;
				else { return curr->children; }
			}
			return 0;
		}
		else { return 0; } //if there are no children in root
	}

	//used to search for a whole word. Returns a pointer to the node at end of word
	//used in case user trying to insert word that already exists
	//used in element access/insert method
	trie_node<T>* returnSearchNode(string value) {
		trie_node<value_type>* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++) {
			curr = curr->subNode(*si);
			if (curr == nullptr)
				return false;
		}
		return curr;
	}


	//searches the trie to see if a string already exists
	//if entire word is found, we check if the last node is a leaf
	bool search(string value) {
		trie_node<value_type>* curr = root;
		for (string::iterator si = value.begin(); si != value.end(); si++) {
			curr = curr->subNode(*si);
			if (curr == nullptr)
				return false;
		}
		return curr->is_leaf == true;
	}

	//iterators
	iterator begin() { return iterator(getNodeWithValue(root), endPtr); }
	iterator end() { return iterator(endPtr, endPtr); }

	//capacity
	bool empty() const { return begPtr == endPtr; }

	//returns the number of distinct words in the trie
	size_type size() { return root->leafs; }
};


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