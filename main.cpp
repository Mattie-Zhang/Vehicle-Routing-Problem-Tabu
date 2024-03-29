#include <iostream>
#include "dvrptabusolver.h"
using namespace std;

int main(int argc, char* argv[]) {
	 
	DVRPTabuSolver solver1("matrix.txt");
	solver1.solve();

	return 1;
}
