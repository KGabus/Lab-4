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

void BTree::writeNode(BTreeNode node)
{
	BTreeFile.seekp(node.nodeNumber * sizeof(BTreeNode));
	char * buffer = (char *)&node;
	BTreeFile.write(buffer, sizeof(BTreeNode));
	
	writeCount++;
	
	BTreeFile.flush();
}

BTreeNode BTree::readNode(int nodeNumber)
{
	BTreeNode node;
	
	BTreeFile.seekg(nodeNumber * sizeof(BTreeNode));
	BTreeFile.read((char *)&node, sizeof(BTreeNode));
	
	readCount++;
	
	return node;
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
