#include "stdafx.h"
#include <string>
#include <iostream>
#include "BTree.h"

BTree::BTree(std::string filePath)
{
	BTreeFile.open(filePath, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

	if (BTreeFile.fail())
		cout << "The tree file was unable to be opened.\n";
}
