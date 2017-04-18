/*AVL.cpp
Kaylee Gabus
EECS 2510 Spring 2017
Purpose: Creates a disk based AVL Tree. Not fully implemented, only supports insert.
			Keeps track of reads, writes, tree height, number of nodes, and number of items inserted.*/

#include "stdafx.h"
#include <string>
#include <iostream>
#include "DiskAVL.h"
//todo: comment avl cpp

DiskAVL::DiskAVL(string file)
{	//opens/creates the file that which will hold the tree
	filePath = file;
	AVLFile.open(filePath, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

	if (AVLFile.fail())
		cout << "The tree file was unable to be opened.\n";
}

DiskAVL::~DiskAVL()
{	//closes and deletes the tree file
	AVLFile.close();
	if (filePath != "")
		remove(filePath.c_str());
}

void DiskAVL::insert(char keyToInsert[50])
{
	insertKey(keyToInsert);
}

void DiskAVL::collectTreeMetrics()
{	//finds tree height, total word count, and file size
	//read, write, and node counts are all set during tree building
	if (treeRoot == 0) return;

	int reads = readCount;						//save the previous values so they only reflect tree building 
	int writes = writeCount;

	findTreeHeight(readNode(treeRoot), 1);		//sets total number of words and tree height
	
	AVLFile.seekg(0, ios::end);
	fileSize = AVLFile.tellg();					//returns file size in bytes

	readCount = reads;							//restore previous values
	writeCount = writes;
}

int DiskAVL::getTotalWordCount()
{
	return totalWordsCount;
}

int DiskAVL::getTreeHeight()
{
	return treeHeight;
}

int DiskAVL::getNodeCount()
{
	return nodeCount;
}

int DiskAVL::getReads()
{
	return readCount;
}

int DiskAVL::getWrites()
{
	return writeCount;
}

void DiskAVL::writeNode(DiskAVLNode node)
{	//writes the node to the tree's file 
	AVLFile.seekp(node.nodeNumber * sizeof(DiskAVLNode));		//find where to write to the file based on node size and number
	char *buffer = (char*)&node;
	AVLFile.write(buffer, sizeof(DiskAVLNode));				//write the node's info to the file
	AVLFile.flush();

	writeCount++;
}

DiskAVLNode DiskAVL::readNode(int readNodeNumber)
{	//reads the node from the tree's file
	DiskAVLNode node;
	if (readNodeNumber == 0)
		return node;

	AVLFile.seekg(readNodeNumber * sizeof(DiskAVLNode));
	AVLFile.read((char*)&node, sizeof(DiskAVLNode));

	readCount++;
	return node;
}

void DiskAVL::insertKey(char keyToInsert[50])
{	//inserts a value into the tree
	//if value isn't already present, create new node and adjust balance factors as needed 
	//if it is, increment it's count
	int insertedNodeNumber;							//saves the node number of the node to be added

	int lastBalancedNode, B, lastBalancedParent;	//these will be used to refer to nodes 
	int currentNode, currentNodeParent;				//though out the rotation and insert process
	int  displacement;								// displacement; Used to adjust BFs

	if (treeRoot == 0)
	{	//the tree is empty, make a new node and set it as the root
		treeRoot = 1;

		nodeA.nodeNumber = 1;
		nodeA.BF = 0;
		nodeA.freqCounts = 1;
		strcpy_s(nodeA.key, keyToInsert);
		nodeA.LeftChild = 0;
		nodeA.RightChild = 0;

		writeNode(nodeA);
		nodeCount++;
		return;
	}

	// Locate insertion point for the passed in key.
	// currentNodePointer scans through the tree until it falls off bottom
	// currentNodeParent is currentNodePointer’s parent
	// nodeToInsert will be added as either the Left or Right child of lastBalancedParent
	// lastBalancedPointer is the last parent above nodeToInsert with a BF of ±1 (before the insert)
	// lastBalancedParent is lastBalancedPointer’s parent

	lastBalancedParent = currentNodeParent = 0; // the parent pointers lag, so they start NULL
	lastBalancedNode = currentNode = treeRoot;

	nodeA = readNode(treeRoot);

	while (currentNode != 0)
	{// search tree for insertion point
		if (strcmp(keyToInsert, nodeA.key) == 0)
		{	//if the node if found, increment it's count
			nodeA.freqCounts++;
			writeNode(nodeA);
			return;
		}

		if (nodeA.BF != 0)
		{	//finds the last node to have a non-zero balance factor
			lastBalancedNode = currentNode;
			lastBalancedParent = currentNodeParent;
		}

		currentNodeParent = currentNode;

		currentNode = (strcmp(keyToInsert, nodeA.key) < 0) ? nodeA.LeftChild : nodeA.RightChild;
		nodeA = readNode(currentNode);
	}

	//currentNode is now NULL, but currentNodeParent points at the last node where the key should be inserted as a leaf
	
	strcpy_s(nodeA.key, keyToInsert);							//create and write a new node
	nodeA.LeftChild = 0;
	nodeA.RightChild = 0;
	nodeA.BF = 0;												// Leaves are always balanced by definition
	insertedNodeNumber = nodeA.nodeNumber = nodeCount + 1;		//save the value of the node that was just created for future use
	writeNode(nodeA);

	nodeCount++;

	nodeB = readNode(currentNodeParent);

	if (strcmp(keyToInsert, nodeB.key) < 0)	//determine whether to insert as a left or right child and insert
		nodeB.LeftChild = nodeA.nodeNumber;
	else nodeB.RightChild = nodeA.nodeNumber;

	writeNode(nodeB);

	nodeA = readNode(lastBalancedNode);				// lastBalancedNode was the LAST ancestor with a BF of ± 1, (or is still the root,
													// because we never FOUND a BF of ± 1), so ALL nodes BETWEEN it and currentNodeParent
													// must have a BF of 0, and will, therefore, BECOME ± 1.
													
	if (strcmp(keyToInsert, nodeA.key) > 0)			// If the key is inserted in the LEFT subtree of lastBalancedNode, then displacement = +1
	{ // determine and set the displacement
		B = currentNode = nodeA.RightChild;
		displacement = -1;
	}
	else
	{
		B = currentNode = nodeA.LeftChild;
		displacement = 1;
	} // B is identified as the last balanced node’s child


	while (currentNode != insertedNodeNumber)  //currentNode is now one node below lastBalancedNode. 
	{   //Adjust from here to the insertion point.  Don’t do anything to new node 
		nodeA = readNode(currentNode);

		if (strcmp(keyToInsert, nodeA.key) > 0)
		{	//adjust BF and move forward
			nodeA.BF = -1;
			currentNode = nodeA.RightChild;
		}
		else
		{
			nodeA.BF = 1;
			currentNode = nodeA.LeftChild;
		}

		writeNode(nodeA);
	}
	
	// Now we check the BF at A and see if we just pushed the tree INTO 
	// BALANCE, into an “unacceptable IMBALANCE”, or if it is still
	// “BALANCED ENOUGH”.
	nodeA = readNode(lastBalancedNode);
	if (nodeA.BF == 0)				// Tree WAS completely balanced before the insert.
	{								// The insert pushed it to slight (acceptable) imbalance
		nodeA.BF = displacement;    // Set the BF to +/- 1 (displacement tells direction)
		writeNode(nodeA);
		return;						// This is close enough to live with, so exit now
	}

	if (nodeA.BF == -displacement)	// If the tree had a slight imbalance the OTHER way, 
	{								// then did the insertion throw the tree INTO complete
		nodeA.BF = 0;				// balance? If so, set the BF to zero and return
		writeNode(nodeA);
		return;
	}
	//if neither return was reached, the tree is imbalanced enough to need correcting

	nodeB = readNode(B);		

	if (displacement == 1) 
	{	//this is a left imbalance.
		if (nodeB.BF == 1) // LL rotation
		{	//lastBalancedPointer's left child become's its grandchild's right child
			//its grandchild moves up to become it's left child
			nodeA.LeftChild = nodeB.RightChild;	// Change the child pointers to reflect the rotation
			nodeB.RightChild = nodeA.nodeNumber;
			nodeB.BF = nodeA.BF = 0;			// Adjust the BFs

			writeNode(nodeA);
			writeNode(nodeB);
		}
		else  
		{	//LR Rotation: three cases (structurally the same; BFs vary)
			nodeC= readNode(nodeB.RightChild); //C is B's right child

			nodeA.LeftChild = nodeC.RightChild;
			nodeB.RightChild = nodeC.LeftChild;
			nodeC.LeftChild = B;
			nodeC.RightChild = lastBalancedNode;


			switch (nodeC.BF)
			{	// Set the new BFs based on the BF at C.
			case 0: nodeA.BF = nodeB.BF = 0; break;
			case 1: nodeA.BF = -1; nodeB.BF = 0; break;
			case -1: nodeB.BF = 1; nodeA.BF = 0; break;
			}

			nodeC.BF = 0; // Regardless, C is now balanced

			writeNode(nodeA);		//write the adjustments to the file
			writeNode(nodeB);
			writeNode(nodeC);

			B = nodeC.nodeNumber;     // B is the root of the now-rebalanced subtree (recycle)
		} // end of else (LR Rotation)
	} // end of left rotations
	else 
	{	//d=-1.  This is a right imbalance
		if (nodeB.BF == -1) // RR rotation
		{
			nodeA.RightChild = nodeB.LeftChild;
			nodeB.LeftChild = nodeA.nodeNumber;
			nodeB.BF = nodeA.BF = 0;			// Adjust the BFs at A & B

			writeNode(nodeA);
			writeNode(nodeB);
		}
		else		// RL rotation
		{	//lastBalancedNode's right child become's its grandchild's left child
			//its grandchild moves up to become it's right child
			nodeC = readNode(nodeB.LeftChild); // C is B's right child

			nodeA.RightChild = nodeC.LeftChild;
			nodeB.LeftChild = nodeC.RightChild;
			nodeC.RightChild = B;
			nodeC.LeftChild = lastBalancedNode;

			switch (nodeC.BF)
			{	// Set the new BFs based on the BF at C.
			case 0: nodeA.BF = nodeB.BF = 0; break;
			case -1: nodeA.BF = 1; nodeB.BF = 0; break;
			case 1: nodeB.BF = -1; nodeA.BF = 0; break;
			}

			nodeC.BF = 0;			//Regardless, C is now balanced

			writeNode(nodeA);		//write the adjustments to the file
			writeNode(nodeB);
			writeNode(nodeC);

			B = nodeC.nodeNumber;	//B is the root of the now-rebalanced subtree (recycle)
		}
	}

	// Regardless, the subtree rooted at B has been rebalanced, and needs to
	// be the new child of lastBalancedParent.

	if (lastBalancedParent == 0) { treeRoot = B; return; }	//B is the tree’s new root
															  
	nodeC = readNode(lastBalancedParent);					//connect the balanced subtree to the rest of the tree
	if (lastBalancedNode == nodeC.LeftChild) { nodeC.LeftChild = B; writeNode(nodeC); return; }
	if (lastBalancedNode == nodeC.RightChild) { nodeC.RightChild = B; writeNode(nodeC); return; }
}

void DiskAVL::findTreeHeight(DiskAVLNode node, int height)
{	//recursively travels through the tree to find the longest path from root to leaf
	totalWordsCount += node.freqCounts;
	if (node.LeftChild != 0)
		findTreeHeight(readNode(node.LeftChild), height + 1);

	if (node.RightChild != 0)
		findTreeHeight(readNode(node.RightChild), height + 1);

	if (height > treeHeight)
		treeHeight = height;
}

