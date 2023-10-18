#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <thread>

void Algorithms::bruteForce(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	
	if (1) {
		int maxThreadsPossible = std::thread::hardware_concurrency();

		std::vector<std::vector<int>> orders; // results dump
		std::vector<int> pathLengths; // results dump
		int matrixSize = matrix->size;

		std::vector<int> permutationVector;
		permutationVector.reserve(matrixSize);
		for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
		int previousVertex = 1;

		// mo¿na stworzyæ wartoœci na ka¿dy w¹tek bez problemu 
		if (matrixSize <= maxThreadsPossible && matrixSize > 3) {
			std::vector<std::thread*> vectorOfThreadsInFlight;
			vectorOfThreadsInFlight.reserve(matrixSize - 1);
			orders.resize(matrixSize);
			pathLengths.resize(matrixSize, INT_MAX);
			std::vector<std::vector<int>>::iterator ordersIterator = orders.begin();
			std::vector<int>::iterator pathLengthsIterator = pathLengths.begin();

			do {
				// czyœcimy passOff, wype³niamy od nowa
				if (previousVertex != permutationVector.front()) {
					vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithreadNew,
						&*ordersIterator,
						&*pathLengthsIterator,
						permutationVector,
						permutationVector.front(),
						matrix)
					);
					vectorOfThreadsInFlight.back()->detach();

					ordersIterator++;
					pathLengthsIterator++;
					previousVertex = permutationVector.front();
				}
			} while (std::next_permutation(permutationVector.begin(), permutationVector.end()));

			vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithreadNew,
				&*ordersIterator,
				&*pathLengthsIterator,
				permutationVector,
				permutationVector.front(),
				matrix)
			);
			vectorOfThreadsInFlight.back()->join();

			//join w¹tków
			/*
			for (auto& a : vectorOfThreadsInFlight) {
				a->join();
			}*/

			int place = 0;
			int currentPlace = 0;
			int minimum = INT_MAX;
			for (auto a : pathLengths) {
				if (a < minimum) {
					place = currentPlace;
					minimum = a;
				}
				currentPlace++;
			}

			this->pathLength = minimum;
			ordersIterator = orders.begin();
			std::advance(ordersIterator, place);
			this->vertexOrder = *ordersIterator;

			executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);

			for (auto& a : vectorOfThreadsInFlight) {
				delete a;
			}
			
			std::cout << pathLength << "\n";
			std::cout << executionTime.count() << "s\n";
			for (auto a : this->vertexOrder) std::cout << a << " ";
		}
		// problemy
		else {

		}
	}
	else {
		std::vector<int> order;
		this->pathLength = bruteHelperFunction(&order, matrix);
		order.push_back(0);
		order.insert(order.begin(), 0);

		executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);

		std::cout << pathLength << "\n";
		std::cout << executionTime.count() << "s\n";
		for (auto a : order) std::cout << a << " ";
	}
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

	permutationVector.reserve(matrixSize);
	
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

void Algorithms::bruteHelperMultithread(std::vector<int>* orderQueue, int* pathLength, std::vector<std::vector<int>> permutations, Matrix* matrix) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin(), permIter = permutations.end(); //permutations->end();
	std::vector<int>::iterator permutationIterator, innerIter = (*outerIter).begin();

	/*
	for (std::vector<int> a : permutations) {
		for (int n : a) {
			std::cout << n;
		}
		std::cout << " ";
	}
	std::cout << "\n";
	*/

	// dopóki mamy jakieœ dostarczone permutacje
	while (permIter != permutations.end()) {
		permutationIterator = (*permIter).begin();

		int previousVertex = 0;
		int currentPath = 0;
		int currentVertexNumber = 0;

		for (permutationIterator = (*permIter).begin(); currentVertexNumber < matrixSize - 1; permutationIterator++, currentVertexNumber++) {
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
			*orderQueue = *permIter;
		}

		permIter++;
	}

	// zamiast return
	*pathLength = shortestPath;
}

void Algorithms::bruteHelperMultithreadNew(std::vector<int>* orderQueue, int* pathLength, std::vector<int> permutation, int permutationNumber, Matrix* matrix) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<int>::iterator permutationIterator, innerIter = (*outerIter).begin();

	do {
		int previousVertex = 0;
		int currentPath = 0;
		int currentVertexNumber = 0;

		for (permutationIterator = permutation.begin(); currentVertexNumber < matrixSize - 1; permutationIterator++, currentVertexNumber++) {
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
			*orderQueue = permutation;
		}
	} while (std::next_permutation(permutation.begin(), permutation.end()) && permutation.front() == permutationNumber);

	// zamiast return
	*pathLength = shortestPath;
}