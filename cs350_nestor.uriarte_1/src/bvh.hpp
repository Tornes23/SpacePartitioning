/**
* @file	bvh.hpp 
* @author Nestor Uriarte ,540000817, nestor.uriarte@digipen.edu
* @date	Fri Oct 23 20:15:04 2020  
* @brief	Contains the definition of the BVH tree class and basic functionality The camera that will be used on the project.
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "pch.hpp"
#include "imgui.hpp"
#include "geometry.hpp"

namespace cs350
{
	template <typename T>
	struct Node
	{
		enum class NodeType
		{
			Node,
			Leaf
		};

		Node(const std::vector<T>& container);
		Node();
		Node* mLeft;
		Node* mRight;
		Node* mParent;
		bool mVisible;

		NodeType mType;
		std::vector<T> mInside;
	};

	template <typename T>
	class BVHTree
	{
	public:
		BVHTree();
		~BVHTree();
		void Initialize(const std::vector<T>& container);
		void Initialize(Node<T>** root);
		void InsertNode(Node<T>* parent, Node<T>** node, const std::vector<T>& container);
		void MergeNodes(Node<T>** toAdd, Node<T>** pos);
		void UpdateParent(Node<T>* parent);
		Node<T>* GetRoot();
		void clear();
	private:

		void CreateNode(Node<T>* parent, Node<T>** target, const std::vector<T>& container);
		void Clear(Node<T>** node);
		void FreeNode(Node<T>** node);

		Node<T>* mRoot;

	};

/**
* @brief	Default constructor of the class
**/
	template<typename T>
	BVHTree<T>::BVHTree()
	{
		//setting the root as null
		mRoot = nullptr;
	}

/**
* @brief	Destructor of the class
**/
	template<typename T>
	BVHTree<T>::~BVHTree()
	{
		//calling to clear the tree
		Clear(&mRoot);
	}

/**
* @brief	Initialiuzes the tree
* @param	const std::vector<T>& container
**/
	template<typename T>
	void BVHTree<T>::Initialize(const std::vector<T>& container)
	{
		//creates a node with the given data
		CreateNode(nullptr, &mRoot, container);
	}

/**
* @brief	Initializes the tree
* @param	Node<T>** root
**/
	template<typename T>
	void BVHTree<T>::Initialize(Node<T>** root)
	{
		//if the pointer is valid
		if (root != nullptr)
		{
			//setting the root
			mRoot = *root;
			mRoot->mParent = nullptr;
		}

	}

/**
* @brief	Creates a node
* @param	Node<T>* parent
* @param	Node<T>** target
* @param	const std::vector<T>& container
**/
	template<typename T>
	void BVHTree<T>::CreateNode(Node<T>* parent, Node<T>** target, const std::vector<T>& container)
	{
		//crate a new node
		Node<T>* new_node = new Node<T>(container);

		//checking is valid
		assert(new_node != nullptr);
	
		//setting the pointer
		*target = new_node;

		//if the parent is valid set it
		if (parent != nullptr)
			(*target)->mParent = parent;
		else
			(*target)->mParent = nullptr;

	}

/**
* @brief	Clears the tree
* @param	Node<T>** node
**/
	template<typename T>
	void BVHTree<T>::Clear(Node<T>** node)
	{
		//if the pointer is valid
		if (*node == nullptr)
			return;

		//calling to free the left and right ndes
		Clear(&((*node)->mLeft));
		Clear(&((*node)->mRight));
	
		//freenig the node
		FreeNode(node);
	}

/**
* @brief	Frees a node
* @param	Node<T>** node
**/
	template<typename T>
	void BVHTree<T>::FreeNode(Node<T>** node)
	{
		//if the pointer is valid
		if (*node == nullptr)
			return;

		//delete the allocated memory
		delete *node;

		//setting the node point to null
		*node = nullptr;
	}

/**
* @brief	Inserts a node
* @param	Node<T>* parent
* @param	Node<T>** node
* @param	const std::vector<T>& container
**/
	template<typename T>
	void BVHTree<T>::InsertNode(Node<T>* parent, Node<T>** node, const std::vector<T>& container)
	{
		//creates a node and sets it
		CreateNode(parent, node, container);
	
		//if the tree has no root initialize it with the given one
		if (mRoot == nullptr)
			Initialize(node);
	}

/**
* @brief	Merges two nodes
* @param	Node<T>** node
* @param	Node<T>** pos
**/
	template<typename T>
	void BVHTree<T>::MergeNodes(Node<T>** toAdd, Node<T>** pos)
	{
		
		//if the tree has no root initialize it with the given one
		if (mRoot == nullptr)
		{
			Initialize(toAdd);
			return;
		}

		//node to contain the new node
		Node<T>* node = nullptr;

		//parent node of the one in tne tree
		Node<T>* parent = (*pos)->mParent;

		//creating a new vector with the one to add and the one in the tree
		std::vector<T> merged = (*toAdd)->mInside;
		merged.insert(merged.end(), (*pos)->mInside.begin(), (*pos)->mInside.end());

		//creates a node
		CreateNode(parent, &node, merged);

		//setting the type of the node, the left and right
		node->mType = Node<T>::NodeType::Node;

		node->mLeft = *pos;
		node->mRight = *toAdd;

		//updating the parent of the affected nodes 
		(*pos)->mParent = node;
		(*toAdd)->mParent = node;

		//if the parent is valid
		if (parent != nullptr)
		{
			//check where to insert the new node
			if (parent->mLeft == *pos)
				parent->mLeft = node;
			else
				parent->mRight = node;
		}
		else//if the parent is null we want to add it to the root
			mRoot = node;

		//updating the parents up t this node
		UpdateParent(node->mParent);

	}

/**
* @brief	Update parents
* @param	Node<T>* parent
**/
	template<typename T>
	void BVHTree<T>::UpdateParent(Node<T>* parent)
	{
		//if the pointer is null
		if (parent == nullptr)
			return;

		//creating a new vector with the containers on the left and right nodes
		std::vector<T> newVec;

		newVec = parent->mLeft->mInside;
		newVec.insert(newVec.end(), parent->mRight->mInside.begin(), parent->mRight->mInside.end());

		//setting the container
		parent->mInside = newVec;

		//calling to update the parent
		UpdateParent(parent->mParent);
	}

/**
* @brief	Gets the root
**/
	template<typename T>
	Node<T>* BVHTree<T>::GetRoot()
	{
		//retuning the root
		return mRoot;
	}

/**
* @brief	Clears the tree
**/
	template<typename T>
	void BVHTree<T>::clear()
	{
		//calling to the private member clear
		Clear(&mRoot);
	}

/**
* @brief	Non Default Constructor of the class
* @param	const std::vector<T>& container
**/
	template<typename T>
	Node<T>::Node(const std::vector<T>& container)
	{
		//setting the values
		mLeft = nullptr;
		mRight = nullptr;
		mVisible = false;

		mType = Node<T>::NodeType::Leaf;
		
		mInside = container;
	}

/**
* @brief	Default Constructor of the class
**/
	template<typename T>
	inline Node<T>::Node()
	{
		//setting the values
		mLeft = nullptr;
		mRight = nullptr;
		mVisible = false;
		mParent = nullptr;

		mType = Node<T>::NodeType::Leaf;

	}
}

