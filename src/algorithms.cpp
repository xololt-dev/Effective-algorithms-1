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
		// dodaæ d³ugoœæ œcie¿ki, usun¹æ wierzcho³ek, iœæ dalej
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
* jeœli brak wierzcho³ków, ³¹czymy z startem, zwracamy drogê do startu + wektor z wierzcho³kiem startu
* inaczej recursive, sprawdzamy odleg³osci, zwracamy najkrótsz¹ drogê + do wektora dodajemy wierzcho³ek najkrótszej drogi
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
	
	// póki s¹ permutacje, analizujemy (do while, aby pierwsza permutacja by³a niezmieniona)
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