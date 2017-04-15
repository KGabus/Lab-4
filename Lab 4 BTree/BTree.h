#pragma once
#include <string>
#include <fstream>

#define DEGREE 3						//----------------------------Set degree of tree here
#define MAXWORDSIZE 50

struct BTreeNode
{
	int nodeNumber = 0;
	int keyCount = 0;
	char keys[2 * DEGREE][MAXWORDSIZE] = { 0 };
	int count[2 * DEGREE] = { 0 };
	int children[2 * DEGREE + 1] = { 0 };
	bool isLeaf = false;
};

using namespace std;

class BTree
{
public:
	BTree(std::string filePath);
	~BTree();
	
	void insert(char insertKey[MAXWORDSIZE]);
	int getTotalWordCount();
	int getTreeHeight();
	int getNodeCount();
	
private:
	int nodeCount = 0;
	int readCount = 0;
	int writeCount = 0;
	int totalWordsCount = 0;
	int treeHeight = 0;
	int currentHeight = 0;

	fstream BTreeFile;

	void writeNode(BTreeNode node);
	BTreeNode readNode(int nodeNumber);
	void insertKey(char insertKey[MAXWORDSIZE]);
	void splitChild(BTreeNode parentNode, int keyPosition);
};
