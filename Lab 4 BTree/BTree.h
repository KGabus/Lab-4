/*BTree.h
Kaylee Gabus
EECS 2510 Spring 2017
Purpose: Header file for Btree.*/

#pragma once
#include <string>
#include <fstream>

#define DEGREE 5						//----------------------------Set degree of tree here
#define MAXWORDSIZE 50					//MAXWORDSIZE - 1 = the largest number of characters per each key. 
															//if changed, AVLNode char array size should also be adjusted
struct BTreeNode
{
	int nodeNumber = 0;
	int keyCount = 0;
	char keys[2 * DEGREE][MAXWORDSIZE] = { 0, 0 };
	int freqCounts[2 * DEGREE] = { 0 };
	int children[2 * DEGREE + 1] = { 0 };
	bool isLeaf = false;
};

using namespace std;

class BTree
{
public:
	BTree(std::string file);
	~BTree();
	
	void insert(char insertKey[MAXWORDSIZE]);
	int getTotalWordCount();
	int getTreeHeight();
	int getNodeCount();
	int getReads();
	int getWrites();
	int getFileSize();
	double getLoadingFactor();
	void collectTreeMetrics();
	
private:
	int treeRoot = 0;

	int nodeCount = 0;
	int readCount = 0;
	int writeCount = 0;
	int uniqueWords = 0;
	int totalWordsCount = 0;
	int treeHeight = 0;
	double loadingFactor = 0;
	unsigned int fileSize = 0;
	string filePath = "";

	fstream BTreeFile;

	void writeNode(BTreeNode node);
	BTreeNode readNode(int nodeNumber);
	void insertKey(char insertKey[MAXWORDSIZE]);
	void insertKeyNonFull(BTreeNode node, char insertKey[MAXWORDSIZE]);
	void splitChild(BTreeNode parentNode, int keyPosition);
	void findTreeHeight(BTreeNode node, int height);
};
