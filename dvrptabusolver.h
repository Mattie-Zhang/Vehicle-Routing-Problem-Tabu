#ifndef DVRP_TABU_SOLVER_H//solve with tabu search solver
#define DVRP_TABU_SOLVER_H

#include "matrix.h"

class DVRPTabuSolver {
private:
	Matrix* matrix;
	int numVertex;
	int numRoute;

	int** s;
	double score;
	int** bestS;
	double bestScore;

	double tempBestScore;
	double tempBestScoreR1;
	double tempBestScoreR2;

	int K1, L1, K2, L2;//record the best treatment
	int* arrayRoute;//record the number of points in each path
	double* arrayScore;//record the score of each path

	int** tabuList;
	int tabuIndex;//recored the tabu index

public:
	DVRPTabuSolver(string fileName);
	void initialize();
	double getSubScore(int, int, int);

	void solve();
	void printBestS();
	bool getBestNearbySolution(int, int, int);

	void excute(int, int, int);
	void inverse(int, int, int);
	void insert(int, int, int, int, int);
	void change(int, int, int, int, int, int);

	bool checkTabu(int, int, int);
	void setTabu(int, int, int);
};

#endif
