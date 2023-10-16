#include "util.hpp"
#include <algorithm>
#include <iostream>

void Algorithms::bruteForce(Matrix* matrix) {
	/*
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
	*/
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	std::vector<int> order;

	this->pathLength = bruteHelperFunction(&order, matrix);
	order.push_back(0);
	order.insert(order.begin(), 0);

	executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);

	std::cout << pathLength << "\n";
	std::cout << executionTime.count() << "s\n";
	for (auto a : order) std::cout << a << " ";	
}

/*
* je�li brak wierzcho�k�w, ��czymy z startem, zwracamy drog� do startu + wektor z wierzcho�kiem startu
* inaczej recursive, sprawdzamy odleg�osci, zwracamy najkr�tsz� drog� + do wektora dodajemy wierzcho�ek najkr�tszej drogi
*/

int Algorithms::bruteHelperFunction(std::vector<int>* orderQueue, Matrix* matrix) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<int> permutationVector;
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<int>::iterator permutationIterator, innerIter = (*outerIter).begin();

	permutationVector.reserve(pointerMat->size());
	
	for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
	
	// p�ki s� permutacje, analizujemy (do while, aby pierwsza permutacja by�a niezmieniona)
	do {
		int previousVertex = 0;
		int currentPath = 0;
		int currentVertexNumber = 0;

		for (permutationIterator = permutationVector.begin(); currentVertexNumber < matrixSize - 1; permutationIterator++, currentVertexNumber++) {
			/* np pierwsza iteracja(od zrodla)
			* outerIter = pierwszy wierzcholek permutationVector
			* inner = previousVertex (czyli zrodlo, czyli 0)
			*/
			
			outerIter = pointerMat->begin();
			std::advance(outerIter, *permutationIterator);
			
			innerIter = (*outerIter).begin();
			std::advance(innerIter, previousVertex);
			currentPath += *innerIter;
			previousVertex = *permutationIterator;			
		}

		outerIter = pointerMat->begin();
		innerIter = (*outerIter).begin();
		std::advance(innerIter, previousVertex);
		currentPath += *innerIter;

		if (currentPath < shortestPath) {
			shortestPath = currentPath;
			*orderQueue = permutationVector;
		}

	} while (std::next_permutation(permutationVector.begin(), permutationVector.end()));

	return shortestPath;
}