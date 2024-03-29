#include "solution.h"//initialize the solution
#include <iostream>

using namespace std; 

#define TIME_THRESHOLD 20

Solution::Solution(Matrix* matrix)
{
	numVertex = matrix->numVertex;

	v = new int* [numVertex];
	initialize(matrix);

	std::cout << "The number of vertices: " << numVertex << endl;
	std::cout << "The number of routes: " << numRoute << endl;
	printPath();
}

void Solution::initialize(Matrix* matrix)
{
	int i = 0, j = 0;
	double score = 0;

	for (int n = 1; n < numVertex; n++) {
		if (j == 0) {
			v[i] = new int[numVertex];
			memset(v[i], 0, numVertex * sizeof(int));
			v[i][j] = n;
			score = matrix->coordinate[0][n];
			j++;
		}
		else {
			if (score + matrix->coordinate[n - 1][n] + matrix->coordinate[n][0] <= TIME_THRESHOLD) {
				v[i][j] = n;
				score += matrix->coordinate[n - 1][n];
				j++;
			}
			else {
				v[i][numVertex - 1] = j;
				i++;
				j = 0;
				n--;
			}
		}
	}
	v[i][numVertex - 1] = j;
	numRoute = i + 1;
}

void Solution::printPath()
{
	for (int i = 0; i < numRoute; i++) {
		for (int j = 0; j < numVertex; j++) {
			std::cout << v[i][j] << ",";
		}
		std::cout << endl;
	}
}

void Solution::free() {
	for (int i = 0; i < numVertex; i++) {
		delete[] v[i];
	}
	delete[] v;
}
