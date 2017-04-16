/*AVL.h
Kaylee Gabus
EECS 2510 Spring 2017
Purpose: Header file for disk based AVL tree.*/



#pragma once
#include <string>
#include <fstream>

using namespace std;

struct DiskAVLNode
{
	int nodeNumber = 0;
	char key[50];
	int BF = 0;
	int counts = 1;
	int LeftChild = 0;
	int RightChild = 0;

};

class DiskAVL
{
public:
	int treeRoot = 0;

	DiskAVL(string filePath);
	~DiskAVL();

	void insert(char keyToInsert[50]);
	int getTotalWordCount();
	int getTreeHeight();
	int getNodeCount();

	//todo: comment avl header
private:

	int nodeCount = 0;
	int readCount = 0;
	int writeCount = 0;
	int totalWordsCount = 0;
	int treeHeight = 0;
	int currentHeight = 0;

	DiskAVLNode nodeA;
	DiskAVLNode nodeB;
	DiskAVLNode nodeC;

	fstream AVLFile;

	DiskAVLNode readNode(int readNodeNumber);
	void writeNode(DiskAVLNode node);
	void traverseAndSum(DiskAVLNode *node);
	void insertKey(char keyToInsert[50]);
	void findTreeHeight(DiskAVLNode node, int height);
};