#include "stdafx.h"
#include <string>
#include <iostream>
#include "BTree.h"

using namespace std;

BTree::BTree(std::string filePath)
{

	BTreeFile.open(filePath, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

	if (BTreeFile.fail())
		cout << "The tree file was unable to be opened.\n";
}

BTree::~BTree()
{
	BTreeFile.close();
}

void BTree::insert(char insertWord[MAXWORDSIZE])
{
	insertKey(insertWord);
}

int BTree::getTotalWordCount()
{//todo: total word count
	return 0;
}

int BTree::getTreeHeight()
{//todo: tree height
	return 0;
}

int BTree::getNodeCount()
{//todo: node count
	return 0;
}

void BTree::writeNode(BTreeNode node)
{
	BTreeFile.seekp(node.nodeNumber * sizeof(BTreeNode));
	char * buffer = (char *)&node;
	BTreeFile.write(buffer, sizeof(BTreeNode));
	
	writeCount++;
	
	BTreeFile.flush();
}

BTreeNode BTree::readNode(int nodeNumber)
{	//todo: breaks here with access violation
	BTreeNode node;
	
	BTreeFile.seekg(nodeNumber * sizeof(BTreeNode));
	BTreeFile.read((char *)&node, sizeof(BTreeNode));
	
	readCount++;
	
	return node;
}

void BTree::insertKey(char insertKey[MAXWORDSIZE])
{	//todo: comment here
	

	if (treeRoot == 0)
	{	//if he tree is empty, make a new node, set it as the root, and return
		BTreeNode rootNode;

		rootNode.count[1] = 1;
		treeRoot = rootNode.nodeNumber = 1;
		rootNode.isLeaf = true;
		strcpy_s(rootNode.keys[1], insertKey);
		rootNode.keyCount++;
		
		writeNode(rootNode);
		
		nodeCount++;
		
		return;
	}

	BTreeNode rootNode = readNode(treeRoot);

	if (rootNode.keyCount == 2 * DEGREE - 1)
	{	//if the root is full, split it
		nodeCount++;

		BTreeNode newRoot;
		newRoot.nodeNumber = nodeCount;
		newRoot.isLeaf = false;
		newRoot.keyCount = 0;
		newRoot.children[1] = treeRoot;			//set the old tree root as this node's child
		treeRoot = newRoot.nodeNumber;

		splitChild(newRoot, 1);					//split the old root
		insertKeyNonFull(newRoot, insertKey);
	}
	else insertKeyNonFull(rootNode, insertKey);		//otherwise insert into the root 
}

void BTree::insertKeyNonFull(BTreeNode node, char insertKey[MAXWORDSIZE])
{	//todo: breaks here with stack overflow
	int keyPos = node.keyCount;

	if (node.isLeaf)
	{	//a leaf node is found, insert here
		while (keyPos >= 1 && (strcmp(insertKey, node.keys[keyPos]) < 0))
		{	//move all greater keys up one spot to make room for new key
			strcpy_s(node.keys[keyPos + 1], node.keys[keyPos]);
			node.count[keyPos + 1] = node.count[keyPos];
			keyPos--;
		}

		strcpy_s(node.keys[keyPos + 1], insertKey);		//add the key to the node
		node.count[keyPos + 1] = 1;
		node.keyCount++;
		writeNode(node);
	}
	else	//todo: somwhere in here the check for a duplicate key needs to happen
	{	//traverse until a leaf is found
		while (keyPos >= 1 && strcmp(insertKey, node.keys[keyPos]))
			keyPos--;			//while the key to insert is less than the current key, keep comparing unit it's not or we run out of keys

		keyPos++;

		// todo: this is probs where the duplicate check should happen
		if (readNode(node.children[keyPos]).keyCount == (2 * DEGREE - 1))
		{	//if the node is full, split it
			splitChild(node, keyPos);
			node = readNode(node.nodeNumber);
			if (strcmp(insertKey, node.keys[keyPos]) > 0)
				keyPos++;
		}
		insertKeyNonFull(readNode(node.children[keyPos]), insertKey);
	}
}

void BTree::splitChild(BTreeNode parentNode, int keyPosition)
{	//if a node is a capacity, this moves the central key into the node's parent 
	//and copies the upper half of it's keys into the lower half of a new node
	BTreeNode nodeToSplit = readNode(parentNode.children[keyPosition]);
	BTreeNode newNode;
	newNode.nodeNumber = nodeCount;
	newNode.isLeaf = nodeToSplit.isLeaf;
	newNode.keyCount = DEGREE - 1;			//this node will be half full as a result of gaining half of the other node's keys

	for (int keyPos = 1; keyPos <= DEGREE - 1; keyPos++)
	{	//copy the upper half of the node to be split's keys into the new node
		strcpy_s(newNode.keys[keyPos], nodeToSplit.keys[keyPos + DEGREE]);
		newNode.count[keyPos] = nodeToSplit.count[keyPos + DEGREE];
	}

	if (newNode.isLeaf == false)
	{	//copy over the children from the other node
		for (int childPos = 1; childPos <= DEGREE; childPos++)
			newNode.children[childPos] = nodeToSplit.children[DEGREE + childPos];
	}

	nodeToSplit.keyCount = DEGREE - 1;		//adjust the keycount to reflect the split
	//todo: maybe revise this when I'm not too much in a hurry to figure it out
	for (int childPos = parentNode.keyCount + 1; childPos >= keyPosition + 1; childPos--)
		parentNode.children[childPos + 1] = parentNode.children[childPos];		//move the child references to make room for the new reference

	parentNode.children[keyPosition + 1] = newNode.nodeNumber;		//add the new node as a child of the parent node

	for (int keyPos = parentNode.keyCount; keyPos >= keyPosition; keyPos--)
	{	//move the keys to make room for the one that get moved up into the parent
		strcpy_s(parentNode.keys[keyPos + 1], parentNode.keys[keyPos]);
		parentNode.count[keyPos + 1] = parentNode.count[keyPos];		//move key counts with keys
	}

	strcpy_s(parentNode.keys[keyPosition], nodeToSplit.keys[DEGREE]);	//move the center key into the parent node
	parentNode.count[keyPosition] = nodeToSplit.count[DEGREE];
	parentNode.keyCount++;

	writeNode(parentNode);			
	writeNode(nodeToSplit);
	writeNode(newNode);
}
