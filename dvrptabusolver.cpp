#include "dvrptabusolver.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

#define TIME_THRESHOLD 50
#define CHARGE_TIME 20

#define TABU_LENGTH 10
#define NUM_INTERATION 10000
#define NUM_COUNT 50

DVRPTabuSolver::DVRPTabuSolver(string fileName) 
{
	matrix = new Matrix(fileName);
	numVertex = matrix->numVertex;

	initialize();

	tabuList = new int* [NUM_INTERATION];
	tabuIndex = 0;
}

void DVRPTabuSolver::initialize()
{
	s = new int* [numVertex];
	bestS = new int* [numVertex];

	arrayRoute = new int[numVertex];//store number of nonzero points in each route
	memset(arrayRoute, 0, numVertex * sizeof(int));

	arrayScore = new double[numVertex];//store total score of each route
	memset(arrayScore, 0, numVertex * sizeof(double));

	string fileName = "initPath.txt";
	ifstream ReadFile;
	ReadFile.open(fileName.c_str(), ios::in);

	int i = 0;
	double tempScore = 0;

	if (ReadFile.is_open()) {
		string line;
		while (getline(ReadFile, line)) {
			
			s[i] = new int[numVertex + 1];
			bestS[i] = new int[numVertex + 1];
			memset(s[i], 0, (numVertex + 1) * sizeof(int));

			int j = 1;
			string item;
			for (unsigned int l = 0; l < line.length(); l++) {
				if (line[l] != ' ') item += line[l];
				else {
					s[i][j] = stoi(item);
					bestS[i][j] = stoi(item);
					item.clear();
					j++;
				}
			}
			arrayRoute[i] = j - 1; //std::cout << arrayRoute[i] << endl;
			arrayScore[i] = getSubScore(i, 0, j + 1);
			tempScore = tempScore + arrayScore[i] + CHARGE_TIME;
			i++;
		}
	}
	else exit(0);
	
	numRoute = i; std::cout << "numRoute=" << i << endl;
	score = tempScore - CHARGE_TIME; std::cout << "initial score=" << score << endl;
	bestScore = tempScore - CHARGE_TIME;
}

double DVRPTabuSolver::getSubScore(int i, int k, int l)//calculate the score in the i-th row from k to l-1
{
	double tempScore = 0;
	for (int j = k; j < l - 1; j++) {
		tempScore += matrix->coordinate[s[i][j]][s[i][j + 1]];
	}
	return tempScore;
}

/*void DVRPTabuSolver::initialize()
{
	int i = 0, j = 1;
	double tempScore = 0;

	for (int n = 1; n < numVertex; n++) {
		if (j == 1) {
			s[i] = new int[numVertex + 1];
			bestS[i] = new int[numVertex + 1];

			memset(s[i], 0, (numVertex + 1) * sizeof(int));
			memset(bestS[i], 0, (numVertex + 1) * sizeof(int));

			s[i][j] = n;
			bestS[i][j] = n;

			tempScore = matrix->coordinate[0][n];
			j++;
		}
		else {
			if (tempScore + matrix->coordinate[n - 1][n] + matrix->coordinate[n][0] <= TIME_THRESHOLD) {
				s[i][j] = n;
				bestS[i][j] = n;
				tempScore += matrix->coordinate[n - 1][n];
				j++;
			}
			else {
				arrayRoute[i] = j - 1;
				arrayScore[i] = tempScore + matrix->coordinate[n - 1][0];
				i++; n--;
				j = 1;
			}
		}
	}
	arrayRoute[i] = j - 1;
	arrayScore[i] = tempScore + matrix->coordinate[numVertex - 1][0];
	numRoute = i + 1;
}*/

void DVRPTabuSolver::solve()
{
	for (int n = 0; n < NUM_INTERATION; n++)
	{
		if ((n % 100) == 0) {//print solution every 100 iterations
			std::cout << "The number of iteration is: " << n << endl;
			std::cout << "bestScore is: " << bestScore << endl;
		}
		
		srand(n);//srand((unsigned)time(NULL));
		int i1, i2;
		do {
			i1 = rand() % numRoute;//randomly choose two rows in each iteration check
			i2 = rand() % numRoute;
			
		} while ((i1 == i2) || (arrayRoute[i1] == 0) || (arrayRoute[i2] == 0));

		int c = n % 5;//randomly choose one type of transformation

		if (getBestNearbySolution(i1, i2, c) == true) {
			excute(i1, i2, c);
			setTabu(i1, i2, c);
			arrayScore[i1] = tempBestScoreR1;
			arrayScore[i2] = tempBestScoreR2;
			score = tempBestScore;

			if (((tempBestScore < score) && (checkTabu(i1, i2, c) == 1)) || (tempBestScore < bestScore)) {
				for (int i = 0; i < numRoute; i++)
					memcpy(bestS[i], s[i], (numVertex + 1) * sizeof(int));
				bestScore = tempBestScore;
			}
		}
		else continue;
	} 
	
	cout << "Best score : " << bestScore << endl;
	printBestS();
}

void DVRPTabuSolver::printBestS()
{
	for (int i = 0; i < numRoute; i++) {
		std::cout << "The " << i << "th Route:" << endl << "0 ";
		int j = 1;
		while (bestS[i][j] != 0) {
			std::cout << bestS[i][j] << " ";
			j++;
		}
		std::cout <<"0 "<< endl;
	}
}

bool DVRPTabuSolver::getBestNearbySolution(int i1, int i2, int c)
{
	bool flag = false;
	int k1, l1, k2, l2;
	double tempScore, tempScoreR1, tempScoreR2, subScore1, subScore2;
	int length1 = arrayRoute[i1];
	int length2 = arrayRoute[i2];
	tempBestScore = numeric_limits<double>::max();

	switch (c) { 
	case 0: {//insert k1,..,l1-1 of v1 before k2 in v2
		for (k1 = 1; k1 <= length1; k1++)
			for (l1 = k1 + 1; l1 <= length1 + 1; l1++) {

				subScore1 = getSubScore(i1, k1, l1);
				if ((k1 == 1) && (l1 == length1 + 1)) tempScoreR1 = 0;
				else tempScoreR1 = arrayScore[i1] - subScore1 - matrix->coordinate[s[i1][k1 - 1]][s[i1][k1]] - matrix->coordinate[s[i1][l1 - 1]][s[i1][l1]] + matrix->coordinate[s[i1][k1 - 1]][s[i1][l1]];
				
				for (k2 = 1; k2 <= length2 + 1; k2++) {
					tempScoreR2 = arrayScore[i2] + subScore1 - matrix->coordinate[s[i2][k2 - 1]][s[i2][k2]] + matrix->coordinate[s[i2][k2 - 1]][s[i1][k1]] + matrix->coordinate[s[i1][l1 - 1]][s[i2][k2]];
					if (length1 == l1 - k1) tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR2 - CHARGE_TIME;
					else tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR1 + tempScoreR2;

					if ((tempScoreR1 <= TIME_THRESHOLD) && (tempScoreR2 <= TIME_THRESHOLD) && (tempScore < tempBestScore)) {
						tempBestScore = tempScore;
						tempBestScoreR1 = tempScoreR1;
						tempBestScoreR2 = tempScoreR2; 
						K1 = k1;
						L1 = l1;
						K2 = k2;
						L2 = -1;
						flag = true;
					}
				}
			}
		break;
	}
	case 1: {//inverse k1,...,l1-1, and insert k1,..,l1-1 of v1 before k2 in v2
		for (k1 = 1; k1 <= length1; k1++)
			for (l1 = k1 + 1; l1 <= length1 + 1; l1++) {

				subScore1 = getSubScore(i1, k1, l1);
				if ((k1 == 1) && (l1 == length1 + 1)) tempScoreR1 = 0;
				else tempScoreR1 = arrayScore[i1] - subScore1 - matrix->coordinate[s[i1][k1 - 1]][s[i1][k1]] - matrix->coordinate[s[i1][l1 - 1]][s[i1][l1]] + matrix->coordinate[s[i1][k1 - 1]][s[i1][l1]];

				for (k2 = 1; k2 <= length2 + 1; k2++) {
					tempScoreR2 = arrayScore[i2] + subScore1 - matrix->coordinate[s[i2][k2 - 1]][s[i2][k2]] + matrix->coordinate[s[i2][k2 - 1]][s[i1][l1 - 1]] + matrix->coordinate[s[i1][k1]][s[i2][k2]];
					if (length1 == l1 - k1) tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR2 - CHARGE_TIME;
					else tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR1 + tempScoreR2;

					if ((tempScoreR1 <= TIME_THRESHOLD) && (tempScoreR2 <= TIME_THRESHOLD) && (tempScore < tempBestScore)) {
						tempBestScore = tempScore;
						tempBestScoreR1 = tempScoreR1;
						tempBestScoreR2 = tempScoreR2;
						K1 = k1;
						L1 = l1;
						K2 = k2;
						L2 = -1;
						flag = true;
					}
				}
			}
		break;
	}
	case 2: {//change k1, .., l1 - 1 of v1 and k2, .., l2 - 1 of v2
		for (k1 = 1; k1 <= length1; k1++)
			for (l1 = k1 + 1; l1 <= length1 + 1; l1++) {

				subScore1 = getSubScore(i1, k1, l1);

				for (k2 = 1; k2 <= length2; k2++)
					for (l2 = k2 + 1; (l2 <= length2 + 1) && (l2 <= l1 - k1 + k2); l2++) {//l1-k1>=l2-k2

						subScore2 = getSubScore(i2, k2, l2);
						tempScoreR1 = arrayScore[i1] - subScore1 + subScore2 - matrix->coordinate[s[i1][k1 - 1]][s[i1][k1]] - matrix->coordinate[s[i1][l1 - 1]][s[i1][l1]]
							+ matrix->coordinate[s[i1][k1 - 1]][s[i2][k2]] + matrix->coordinate[s[i2][l2 - 1]][s[i1][l1]];
						tempScoreR2 = arrayScore[i2] + subScore1 - subScore2 - matrix->coordinate[s[i2][k2 - 1]][s[i2][k2]] - matrix->coordinate[s[i2][l2 - 1]][s[i2][l2]]
							+ matrix->coordinate[s[i2][k2 - 1]][s[i1][k1]] + matrix->coordinate[s[i1][l1 - 1]][s[i2][l2]];
						tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR1 + tempScoreR2;

						if ((tempScoreR1 <= TIME_THRESHOLD) && (tempScoreR2 <= TIME_THRESHOLD) && (tempScore < tempBestScore)) {
							tempBestScore = tempScore;
							tempBestScoreR1 = tempScoreR1;
							tempBestScoreR2 = tempScoreR2;
							K1 = k1;
							L1 = l1;
							K2 = k2;
							L2 = l2;
							flag = true;
						}
					}
			}
		break;
	}
	case 3: {//inverse k1,...,l1-1, and change k1, .., l1 - 1 of v1 and k2, .., l2 - 1 of v2
		for (k1 = 1; k1 <= length1; k1++)
			for (l1 = k1 + 1; l1 <= length1 + 1; l1++) {

				subScore1 = getSubScore(i1, k1, l1);

				for (k2 = 1; k2 <= length2; k2++)
					for (l2 = k2 + 1; (l2 <= length2 + 1) && (l2 <= l1 - k1 + k2); l2++) {

						subScore2 = getSubScore(i2, k2, l2);
						tempScoreR1 = arrayScore[i1] - subScore1 + subScore2 - matrix->coordinate[s[i1][k1 - 1]][s[i1][k1]] - matrix->coordinate[s[i1][l1 - 1]][s[i1][l1]]
							+ matrix->coordinate[s[i1][k1 - 1]][s[i2][k2]] + matrix->coordinate[s[i2][l2 - 1]][s[i1][l1]];
						tempScoreR2 = arrayScore[i2] + subScore1 - subScore2 - matrix->coordinate[s[i2][k2 - 1]][s[i2][k2]] - matrix->coordinate[s[i2][l2 - 1]][s[i2][l2]]
							+ matrix->coordinate[s[i2][k2 - 1]][s[i1][l1 - 1]] + matrix->coordinate[s[i1][k1]][s[i2][l2]];
						tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR1 + tempScoreR2;

						if ((tempScoreR1 <= TIME_THRESHOLD) && (tempScoreR2 <= TIME_THRESHOLD) && (tempScore < tempBestScore)) {
							tempBestScore = tempScore;
							tempBestScoreR1 = tempScoreR1;
							tempBestScoreR2 = tempScoreR2;
							K1 = k1;
							L1 = l1;
							K2 = k2;
							L2 = l2;
							flag = true;
						}
					}
			}
		break;
	}
	default: {//inverse k1,...,l1-1 and k2,...,l2-1, and change k1, .., l1 - 1 of v1 and k2, .., l2 - 1 of v2
		for (k1 = 1; k1 <= length1; k1++)
			for (l1 = k1 + 1; l1 <= length1 + 1; l1++) {

				subScore1 = getSubScore(i1, k1, l1);

				for (k2 = 1; k2 <= length2; k2++)
					for (l2 = k2 + 1; (l2 <= length2 + 1) && (l2 <= l1 - k1 + k2); l2++) {

						subScore2 = getSubScore(i2, k2, l2);
						tempScoreR1 = arrayScore[i1] - subScore1 + subScore2 - matrix->coordinate[s[i1][k1 - 1]][s[i1][k1]] - matrix->coordinate[s[i1][l1 - 1]][s[i1][l1]]
							+ matrix->coordinate[s[i1][k1 - 1]][s[i2][l2 - 1]] + matrix->coordinate[s[i2][k2]][s[i1][l1]];
						tempScoreR2 = arrayScore[i2] + subScore1 - subScore2 - matrix->coordinate[s[i2][k2 - 1]][s[i2][k2]] - matrix->coordinate[s[i2][l2 - 1]][s[i2][l2]]
							+ matrix->coordinate[s[i2][k2 - 1]][s[i1][l1 - 1]] + matrix->coordinate[s[i1][k1]][s[i2][l2]];
						tempScore = score - arrayScore[i1] - arrayScore[i2] + tempScoreR1 + tempScoreR2;

						if ((tempScoreR1 <= TIME_THRESHOLD) && (tempScoreR2 <= TIME_THRESHOLD) && (tempScore < tempBestScore)) {
							tempBestScore = tempScore;
							tempBestScoreR1 = tempScoreR1;
							tempBestScoreR2 = tempScoreR2;
							K1 = k1;
							L1 = l1;
							K2 = k2;
							L2 = l2;
							flag = true;
						}
					}
			}
	}
	}
	return flag;
}

void DVRPTabuSolver::excute(int i1, int i2, int c)
{
	if (c < 2) {
		if (c == 1) inverse(i1, K1, L1);//inverse k1,...,l1-1
		insert(i1, K1, L1, i2, K2);//insert k1,..,l1-1 of v1 before k2 in v2
	}
	else {
		if ((c == 3) || (c == 4)) inverse(i1, K1, L1);//inverse k1,...,l1-1
		if (c == 4) inverse(i2, K2, L2);//inverse k1,...,l1-1 and k2,...,l2-1
		change(i1, K1, L1, i2, K2, L2);//exchange k1,...,l1-1 in v1 and k2,...,l2-1 in v2, L1-K1>=L2-K2
	}
}

void DVRPTabuSolver::inverse(int i, int k, int l)
{
	while ((k != l - 1) && (k != l)) {
		swap(s[i][k], s[i][l - 1]);
		k++; l--;
	}
}

void DVRPTabuSolver::insert(int i1, int k1, int l1, int i2, int k2)//insert k1, ..., l1-1 before k2
{
	for (int j = arrayRoute[i2] + l1 - k1; j > l1 - k1 + k2 - 1; j--) {
		s[i2][j] = s[i2][j - l1 + k1];
	}//copy the following length1-j terms of v2
	memcpy(s[i2] + k2, s[i1] + k1, (l1 - k1) * sizeof(int));//copy the middle l-k terms from v2
	arrayRoute[i2] = arrayRoute[i2] - k1 + l1;//assign the last term

	memcpy(s[i1] + k1, s[i1] + l1, (arrayRoute[i1] - l1 + 1) * sizeof(int));//copy the last length2-l terms from v2
	memset(s[i1] + arrayRoute[i1] + k1 - l1 + 1, 0, (l1 - k1) * sizeof(int));//assign the last l-k terms to be zero
	arrayRoute[i1] = arrayRoute[i1] + k1 - l1;//assign the last term 
}

void DVRPTabuSolver::change(int i1, int k1, int l1, int i2, int k2, int l2)//exchange k1,...,l1-1 terms in v1 and k2,...,l2-1 terms in v2, k1<l1, k2<l2, l1-k1>=l2-k2!!!rewrite
{
	int* temp = new int[l2 - k2];
	for (int i = 0; i < l2 - k2; i++) temp[i] = s[i2][i + k2];//memcpy(temp, s[i2] + k2, l2 - k2);

	for (int j = arrayRoute[i2] + l1 - k1 - l2 + k2; j > l1 - k1 + k2 - 1; j--) {
		s[i2][j] = s[i2][j + k1 - l1 - k2 + l2];
	}//copy the last lenth2-l2 terms of v[i2]
	memcpy(s[i2] + k2, s[i1] + k1, (l1 - k1) * sizeof(int));//copy the middle l1-k1 terms of v[i1]
	arrayRoute[i2] = arrayRoute[i2] + l1 - k1 - l2 + k2;//assign the last term

	memcpy(s[i1] + k1, temp, (l2 - k2) * sizeof(int));//copy the middle l2-k2 terms of v[i2] from temp
	memcpy(s[i1] + k1 + l2 - k2, s[i1] + l1, (arrayRoute[i1] - l1 + 1) * sizeof(int));//copy the last length1-l1 terms of v[i1]
	memset(s[i1] + arrayRoute[i1] - l1 + k1 + l2 - k2 + 1, 0, (l1 - k1 - l2 + k2) * sizeof(int));//assign the last l1-k1-l1+k1 terms to be zero
	arrayRoute[i1] = arrayRoute[i1] - l1 + k1 + l2 - k2;//assign the last term

	delete[]temp;
}

bool DVRPTabuSolver::checkTabu(int i1, int i2, int c)
{
	int T[7];
	if (i1 <= i2) {
		T[0] = i1; T[1] = K1; T[2] = L1; T[3] = i2; T[4] = K2; T[5] = L2; T[6] = c;//to make T[0]<=T[3]
	}
	else {
		T[0] = i2; T[1] = K2; T[2] = L2; T[3] = i1; T[4] = K1; T[5] = L1; T[6] = c;
	}
	for (int i = tabuIndex; (i >= 0) && (i > tabuIndex - TABU_LENGTH); i--)
		for (int j = 0; j < 7; j++) {
			if (tabuList[i][j] != T[j]) return 0;
		}
	return 1;
}

void DVRPTabuSolver::setTabu(int i1, int i2, int c)
{
	int T[7];
	if (i1 <= i2) {
		T[0] = i1; T[1] = K1; T[2] = L1; T[3] = i2; T[4] = K2; T[5] = L2; T[6] = c;//to make T[0]<=T[3]
	}
	else {
		T[0] = i2; T[1] = K2; T[2] = L2; T[3] = i1; T[4] = K1; T[5] = L1; T[6] = c;
	}
	tabuList[tabuIndex] = new int[7];
	memset(tabuList[tabuIndex], 0, 7 * sizeof(int));
	memcpy(tabuList[tabuIndex], T, 7 * sizeof(int));
	tabuIndex++;
}




