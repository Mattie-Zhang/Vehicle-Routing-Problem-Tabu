#ifndef Matrix_H
#define Matrix_H

#include <string>

using namespace std;
class Matrix {
public:
	int numVertex;
	double** coordinate;
	Matrix(string fileName);
	int countRows(string fileName);
	void free();
};

#endif
