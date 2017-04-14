#pragma once
#include <string>
#include <fstream>

#define KEYCAPACITY 3						//----------------------------Set number of keys per node here
#define MAXWORDSIZE 50

struct BTreeNode
{
	int nodeNumber = 0;
	int keyCount = 0;
	char keys[2 * KEYCAPACITY][MAXWORDSIZE] = { 0 };
	int count[2 * KEYCAPACITY] = { 0 };
	int children[2 * KEYCAPACITY + 1] = { 0 };
	bool isLeaf = false;
};

class BTree
{
public:
	BTree(std::string filePath);
	~BTree();
	
	void insert(char input[MAXWORDSIZE]);
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

	void splitChild(BTreeNode parentNode, int keyPosition);
};
