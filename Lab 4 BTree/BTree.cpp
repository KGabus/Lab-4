/*BTree.cpp
Kaylee Gabus
EECS 2510 Spring 2017
Purpose: Creates a BTree. Not fully implemented, only supports insert.
			Keeps track of reads, writes, tree height, number of nodes, and number of items inserted.*/

#include "stdafx.h"
#include <string>
#include <iostream>
#include "BTree.h"

using namespace std;

BTree::BTree(std::string file)
{	//opens/creates the file which will hold the tree
	filePath = file;
	BTreeFile.open(filePath, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

	if (BTreeFile.fail())
		cout << "The tree file was unable to be opened.\n";
}

BTree::~BTree()
{	//closes and deletes the file that holds the tree
	BTreeFile.close();
	if (filePath != "")
		remove(filePath.c_str());		
}

void BTree::insert(char keyToInsert[MAXWORDSIZE])
{
	insertKey(keyToInsert);
}

int BTree::getTotalWordCount()
{
	return totalWordsCount;
}

int BTree::getTreeHeight()
{
	if (treeRoot == 0 && totalWordsCount == 0) return 0;
	
	findTreeHeight(readNode(treeRoot), 0);
	return treeHeight;
}

int BTree::getNodeCount()
{
	return nodeCount;
}

int BTree::getReads()
{
	return readCount;
}

int BTree::getWrites()
{
	return writeCount;
}

double BTree::getLoadingFactor()
{	//once the tree has been built, 
	//finds and returns the percentage of keys used vs. the tree's total key capacity
	if (treeRoot == 0) return 0;

	double loadingFactor = ((DEGREE * 2) - 1) * nodeCount;
	loadingFactor = (uniqueWords / loadingFactor) * 100;
	return loadingFactor;
}

void BTree::collectTreeMetrics()
{	//finds tree height, and file size
	//read, write, total word, and node counts are all set during tree building
	if (treeRoot == 0) return;

	int reads = readCount;						//save the previous values so they only reflect tree building 
	int writes = writeCount;

	findTreeHeight(readNode(treeRoot), 1);		//sets total number of words and tree height

	BTreeFile.seekg(0, ios::end);
	fileSize = BTreeFile.tellg();				//returns file size in bytes

	readCount = reads;							//restore previous values
	writeCount = writes;
}

void BTree::writeNode(BTreeNode node)
{	//writes the node to the tree's file
	BTreeFile.seekp(node.nodeNumber * sizeof(BTreeNode));
	char * buffer = (char *)&node;
	BTreeFile.write(buffer, sizeof(BTreeNode));
	
	writeCount++;
	
	BTreeFile.flush();
}

BTreeNode BTree::readNode(int nodeNumber)
{	//reads a node from the tree's file
	BTreeNode node;
	
	BTreeFile.seekg(nodeNumber * sizeof(BTreeNode));
	BTreeFile.read((char *)&node, sizeof(BTreeNode));
	
	readCount++;
	
	return node;
}

void BTree::insertKey(char keyToInsert[MAXWORDSIZE])
{	//inserts a key into the tree 
	//if the tree is empty, creates a new node, sets it as the root, and returns
	//if the root is full, checks to see if key is present in root
		//if it is, increment it's count
		//if it's not, split the root and insert into proper child using insertKeyNonFull
	//if the key doesn't belong in the root, calls insertKeyNonFull
	totalWordsCount++;

	if (treeRoot == 0)
	{	//if he tree is empty, make a new node, set it as the root, and return
		BTreeNode rootNode;

		rootNode.freqCounts[1] = 1;
		treeRoot = rootNode.nodeNumber = 1;
		rootNode.isLeaf = true;
		strcpy_s(rootNode.keys[1], keyToInsert);
		rootNode.keyCount++;
		
		writeNode(rootNode);
		
		nodeCount++;
		
		return;
	}

	BTreeNode rootNode = readNode(treeRoot);

	for (int keyPos = rootNode.keyCount; keyPos >= 1; keyPos--)
	{	//check to see if key is present in root
		if (strcmp(rootNode.keys[keyPos], keyToInsert) == 0)
		{	//increment its count and return
			rootNode.freqCounts[keyPos]++;
			writeNode(rootNode);
			return;
		}
	}

	if (rootNode.keyCount == 2 * DEGREE - 1)
	{	//if the root is full, split it
		BTreeNode newRoot;
		
		nodeCount++;
				
		newRoot.nodeNumber = nodeCount;
		newRoot.isLeaf = false;
		newRoot.keyCount = 0;
		newRoot.children[1] = treeRoot;			//set the old tree root as this node's child
		treeRoot = newRoot.nodeNumber;

		splitChild(newRoot, 1);					//split the old root
		newRoot = readNode(newRoot.nodeNumber); //read the adjusted node back into RAM
		insertKeyNonFull(newRoot, keyToInsert);
	}
	else insertKeyNonFull(rootNode, keyToInsert);		//otherwise insert into the root 
}

void BTree::insertKeyNonFull(BTreeNode node, char keyToInsert[MAXWORDSIZE])
{	//handles a majority of insert responsibilities
	//traverses the tree until a leaf node is found
		//if it encounters a full node, it checks if the insert key is a duplicate and increments its count
			//otherwise it splits the node
	//once a leaf node is found, if the key is a duplicate its count is incremented 
		//if its not, it is inserted as a new key
	int keyPos = node.keyCount;

	if (node.isLeaf)
	{	//a leaf node is found, insert here
		while (keyPos >= 1 && (strcmp(keyToInsert, node.keys[keyPos]) < 0))
		{	//move all greater keys up one spot to make room for new key
			strcpy_s(node.keys[keyPos + 1], node.keys[keyPos]);
			node.freqCounts[keyPos + 1] = node.freqCounts[keyPos];
			keyPos--;
		}

		strcpy_s(node.keys[keyPos + 1], keyToInsert);		//add the key to the node
		node.freqCounts[keyPos + 1] = 1;
		node.keyCount++;
		writeNode(node);

		uniqueWords++;
	}
	else
	{	//traverse until a leaf is found
		while (keyPos >= 1 && strcmp(keyToInsert, node.keys[keyPos]) < 0)
			keyPos--;			//while the key to insert is less than the current key, keep comparing until it's not or we run out of keys

		keyPos++;

		BTreeNode possibleInsertNode = readNode(node.children[keyPos]);
		for (int keyPos = possibleInsertNode.keyCount; keyPos >= 1; keyPos--)
		{	//check to see if key is present in node
			if (strcmp(possibleInsertNode.keys[keyPos], keyToInsert) == 0)
			{	//increment its count and return
				possibleInsertNode.freqCounts[keyPos]++;
				writeNode(possibleInsertNode);
				return;
			}
		}

		if (possibleInsertNode.keyCount == (2 * DEGREE - 1))
		{	//if the node is full, split it
			splitChild(node, keyPos);
			node = readNode(node.nodeNumber);
			if (strcmp(keyToInsert, node.keys[keyPos]) > 0)
				keyPos++;
		}
		insertKeyNonFull(readNode(node.children[keyPos]), keyToInsert);
	}
}

void BTree::splitChild(BTreeNode parentNode, int keyPosition)
{	//if a node is a capacity, this moves the central key into the node's parent 
	//and copies the upper half of it's keys (and their freq. counts) into the lower half of a new node and adjusts both nodes' key counts
	//the keys and their freq. counts remain in the original node but are inaccessable due to the adjusted key count
	BTreeNode nodeToSplit = readNode(parentNode.children[keyPosition]);
	BTreeNode newNode;

	nodeCount++;

	newNode.nodeNumber = nodeCount;
	newNode.isLeaf = nodeToSplit.isLeaf;
	newNode.keyCount = DEGREE - 1;			//this node will be half full as a result of gaining half of the other node's keys

	for (int keyPos = 1; keyPos <= DEGREE - 1; keyPos++)
	{	//copy the upper half of the node to be split's keys into the new node
		strcpy_s(newNode.keys[keyPos], nodeToSplit.keys[keyPos + DEGREE]);
		newNode.freqCounts[keyPos] = nodeToSplit.freqCounts[keyPos + DEGREE];
	}

	if (newNode.isLeaf == false)
	{	//copy over the children from the other node
		for (int childPos = 1; childPos <= DEGREE; childPos++)
			newNode.children[childPos] = nodeToSplit.children[DEGREE + childPos];
	}

	nodeToSplit.keyCount = DEGREE - 1;		//adjust the keycount to reflect the split

	for (int childPos = parentNode.keyCount + 1; childPos >= keyPosition + 1; childPos--)
		parentNode.children[childPos + 1] = parentNode.children[childPos];		//move the child references to make room for the new reference

	parentNode.children[keyPosition + 1] = newNode.nodeNumber;					//add the new node as a child of the parent node

	for (int keyPos = parentNode.keyCount; keyPos >= keyPosition; keyPos--)
	{	//move the keys to make room for the one that get moved up into the parent
		strcpy_s(parentNode.keys[keyPos + 1], parentNode.keys[keyPos]);
		parentNode.freqCounts[keyPos + 1] = parentNode.freqCounts[keyPos];		//move key counts with keys
	}

	strcpy_s(parentNode.keys[keyPosition], nodeToSplit.keys[DEGREE]);			//move the center key into the parent node
	parentNode.freqCounts[keyPosition] = nodeToSplit.freqCounts[DEGREE];
	parentNode.keyCount++;

	writeNode(parentNode);			//write the adjusted nodes back to the file
	writeNode(nodeToSplit);
	writeNode(newNode);
}

void BTree::findTreeHeight(BTreeNode node, int height)
{	//finds the height of the tree by following the first child pointer of nodes until a leaf is reached
	treeHeight = 0;

	while (!node.isLeaf)
	{	//since all leaf nodes have the same height, this doesn't need to be recursive
		node = readNode(node.children[1]);
		treeHeight++;
	}

	treeHeight++;		//counts leaf level
}
