#include "matrix.h"//read matrix from a text file
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

#define TIME_THRESHOLD 30

Matrix::Matrix(string fileName) {
	std::cout << "File Name: " << fileName << endl;

	this->numVertex = countRows(fileName);
	this->coordinate = new double* [this->numVertex];

	ifstream ReadFile;
	ReadFile.open(fileName.c_str(), ios::in);//ios::in, read only
	if (ReadFile.is_open()) {
		int Row = 0, Col;
		string line;
		while (getline(ReadFile, line)) {
			Col = 0;
			coordinate[Row] = new double[numVertex];
			memset(coordinate[Row], 0, numVertex * sizeof(double));
			string item;
			for (unsigned int i = 0; i < line.length(); i++) {
				if (line[i] != ' ') {
					item += line[i];
				}
				else {
					coordinate[Row][Col] = stod(item);
					item.clear();
					Col++;
				}
			}
			Row++;
		}
		ReadFile.close();

		std::cout <<"The number of vertices: " << numVertex << endl;
		std::cout << "Matrix Size: " << Row << ", " << Col << endl;
	}
	else {
		std::cout << "Read error!" << endl;
		exit(0);
	}
		
}

int Matrix::countRows(string fileName)
{
	ifstream ReadFile;
	int n = 0;
	string temp;
	ReadFile.open(fileName, ios::in);//ios::in, read only
	if (ReadFile.fail()) {
		return 0;
	}
	else {
		while (getline(ReadFile, temp)) {
			n++;
		}
		return n;
	}
	ReadFile.close();
}

void Matrix::free() 
{
	for (int i = 0; i < numVertex; i++) {
		delete[] coordinate[i];
	}
	delete[] coordinate;
}
