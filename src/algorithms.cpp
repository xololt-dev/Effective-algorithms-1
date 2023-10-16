#include "algorithms.hpp"

void Algorithms::bruteForce(Matrix* matrix) {
	int startPoint = 0, currentPoint = startPoint, shortestPath = INT_MAX;
	std::vector<std::vector<int>>::iterator mainIter = matrix->mat.begin();
	mainIter++;
	currentPoint++;

	for (; mainIter != matrix->mat.end(); mainIter++) {
		// doda� d�ugo�� �cie�ki, usun�� wierzcho�ek, i�� dalej
		std::vector<int> pointsLeft;
		for (int i = 0; i < matrix->mat.size(); i++) {
			if (i != startPoint && i != currentPoint) pointsLeft.push_back(i);
		}

		// std::remove(pointsLeft.begin(), pointsLeft.end(), 1);
		std::min(shortestPath, 1);

		currentPoint;
	}
}

/*
* je�li brak wierzcho�k�w, ��czymy z startem, zwracamy drog� do startu + wektor z wierzcho�kiem startu
* inaczej recursive, sprawdzamy odleg�osci, zwracamy najkr�tsz� drog� + do wektora dodajemy wierzcho�ek najkr�tszej drogi
*/

int bruteRecursiveHelperFunction(std::vector<int> pointsLeft, std::vector<int>* orderQueue, Matrix* matrix) {
	int shortestPath = INT_MAX;
	int vertexToAdd = 0;

	// ?
	for (auto a : pointsLeft) {
		std::vector<int> pointsOutput = pointsLeft;
		std::remove(pointsOutput.begin(), pointsOutput.end(), &a);

		if (!pointsOutput.size()) {
			// do zrodla
			std::vector<std::vector<int>>::iterator mainIter = matrix->mat.begin();
			std::vector<int>::iterator innerIter = (*mainIter).begin();
			// znajdz ostatni wierzcholek
			std::advance(innerIter, &a);
			// wartosc sciezki do zrodla, zamkniecie petli
			shortestPath = *innerIter;
		}
		else {
			int result = bruteRecursiveHelperFunction(pointsOutput, orderQueue, matrix);
			
			if (result < shortestPath) {
				vertexToAdd = a;
				shortestPath = result;
			}
		}
	}
	
	return shortestPath;
}