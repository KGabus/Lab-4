/*Lab 4 BTree.cpp
Kaylee Gabus
EECS 2510 Spring 2017
Purpose: Defines the entry point for the console application. 
			Program loops through file specified to create a disk based AVL tree and a BTree.
			Outputs time taken and select measurements to estimate work done both each trees.*/

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <ctime>
#include "DiskAVL.h"
#include "BTree.h"
//todo: comment everything
using namespace std;

string filePath = "C:\\Users\\Kaylee\\Desktop\\Text Files\\shakespeare.txt";		//set input file path here

double elapsedTime;
double overheadTime;
double AVLTime;
double BTreeTime;

clock_t startTime;

int main()
{
	char c;
	DiskAVL AVL("Gabus AVL Tree File.txt");
	BTree BTree("Gabus BTree File.txt");

	cout << "Current BTree Degree: " << DEGREE << endl;

	char chari[50]; // assumes no word is longer than 49
	int iPtr;
	bool IsDelimiter = false, WasDelimiter = false;

	for (int i = 0; i<sizeof(chari); i++) chari[i] = '\0';

	ifstream inFile(filePath, ios::binary);
	if (inFile.fail())
	{	//if the file is unable to be read, notify user and close the program
		cout << "Unable to open input file\n\n"
			<< "Program Exiting\n\nPress ENTER to exit\n";
		cin.get(c);
		exit(1);
	}

	cout << "Collecting overhead time: ";

	for (int i = 0; i < 3; i++)
	{	//for each of the three trees and a run to get overhead time

		inFile.clear();				//reset the stream to the begininging of the file
		inFile.seekg(0, ios::beg);

		iPtr = 0;

		startTime = clock();						//Start timer

		inFile.get(c);

		while (!inFile.eof())
		{
			IsDelimiter = (c == 32 || c == 10 || c == 13 || c == 9 || c == '.' || c == ',' || c == '!' || c == ';' || c == ':' || c == '(' || c == ')');
			if (IsDelimiter && !WasDelimiter)   // if THIS character is a delimiter and the last character WASN’T 
			{
				WasDelimiter = true;
				if (i == 0);		//do nothing, used to collect overhead time info
				if (i == 1) AVL.insert(chari);  	// insert it in the AVL Tree 
				if (i == 2) BTree.insert(chari);

				for (int i = 0; i < sizeof(chari); i++) chari[i] = '\0'; // zero the word         
				iPtr = 0;
			}
			else if (!IsDelimiter)  // if this isn’t a delimiter, keep going 
			{
				chari[iPtr] = c;
				iPtr++;
			}
			else if (IsDelimiter && WasDelimiter)
			{
				// Do nothing -- two consecutive delimiters. 
			}
			WasDelimiter = IsDelimiter;   // for the NEXT iteration     
			inFile.get(c);
		}
		
		elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;

		switch (i)
		{	//output runtimes for the trees
		case 0: overheadTime = elapsedTime; cout << overheadTime << "s\nStarting AVL: "; break;
		case 1: cout << elapsedTime - overheadTime << "s runtime. \nStarting BTree: "; AVL.collectTreeMetrics();  break;
		case 2: cout << elapsedTime - overheadTime << "s runtime." << endl; BTree.collectTreeMetrics(); break;
		}

	}
	inFile.close();

	cout << "\nTree Statistics:" << endl;			//output tree statistics
	cout << setw(14) << "Nodes:" << setw(10) << "Words:" << setw(10) << "Height:" << setw(10) << "Reads:" << setw(10) << "Writes:" << setw(19) << "Loading Factor:" << endl;
	cout << "AVL:   " << setw(7) << AVL.getNodeCount() << setw(10) << AVL.getTotalWordCount() << setw(10) << AVL.getTreeHeight() << setw(10) << AVL.getReads() << setw(10) << AVL.getWrites() << setw(19) << "n/a" << endl;
	cout << "BTree: " << setw(7) << BTree.getNodeCount() << setw(10) << BTree.getTotalWordCount() << setw(10) << BTree.getTreeHeight() << setw(10) << BTree.getReads() << setw(10) << BTree.getWrites() << setw(18) << setprecision(2) << BTree.getLoadingFactor() << "%" << endl;
	
	return 0;
}

