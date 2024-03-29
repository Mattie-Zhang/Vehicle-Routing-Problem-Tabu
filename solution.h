#ifndef SOLUTION_H
#define SOLUTION_H
#include "matrix.h"

class Solution {
public:
	int** v;
	int numVertex;
	int numRoute;

	Solution(Matrix*);
	void initialize(Matrix*);
	void printPath();
	void free();
};

#endif
