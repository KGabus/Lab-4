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
	int freqCounts = 1;
	int LeftChild = 0;
	int RightChild = 0;
};

class DiskAVL
{
public:
	DiskAVL(string filePath);
	~DiskAVL();

	void insert(char keyToInsert[50]);
	void collectTreeMetrics();
	int getTotalWordCount();
	int getTreeHeight();
	int getNodeCount();
	int getReads();
	int getWrites();
	int getFileSize();

private:
	int treeRoot = 0;
	int nodeCount = 0;
	int readCount = 0;
	int writeCount = 0;
	int totalWordsCount = 0;
	int treeHeight = 0;
	unsigned int fileSize = 0;
	string filePath = "";

	DiskAVLNode nodeA;
	DiskAVLNode nodeB;
	DiskAVLNode nodeC;

	fstream AVLFile;

	DiskAVLNode readNode(int readNodeNumber);
	void writeNode(DiskAVLNode node);
	void insertKey(char keyToInsert[50]);
	void findTreeHeight(DiskAVLNode node, int height);
};