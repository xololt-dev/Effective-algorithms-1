#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <thread>

void Algorithms::bruteForce(Matrix* matrix, int multithread) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	
	if (multithread) {
		// int maxThreadsPossible = std::thread::hardware_concurrency();

		std::vector<std::vector<int>> orders; // results dump
		std::vector<int> pathLengths; // results dump
		int matrixSize = matrix->size;

		std::vector<int> permutationVector;
		permutationVector.reserve(matrixSize);
		for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
		int previousVertex = 1;

		// mo¿na stworzyæ wartoœci na ka¿dy w¹tek bez problemu 
		
		std::vector<std::thread*> vectorOfThreadsInFlight;
		vectorOfThreadsInFlight.reserve(matrixSize - 1);
		orders.resize(matrixSize - 1);
		pathLengths.resize(matrixSize - 1, INT_MAX);
		std::vector<std::vector<int>>::iterator ordersIterator = orders.begin();
		std::vector<int>::iterator pathLengthsIterator = pathLengths.begin();

		// tworzenie w¹tków dla zestawów permutacji
		for (int i = 0; i < permutationVector.size() - 1; i++) {
			std::swap(permutationVector[0], permutationVector[i + 1]);

			vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
				&*ordersIterator,
				&*pathLengthsIterator,
				permutationVector,
				permutationVector.front(),
				matrix)
			);
			vectorOfThreadsInFlight.back()->detach();

			ordersIterator++;
			pathLengthsIterator++;
		}

		/*
		do {
			// czyœcimy passOff, wype³niamy od nowa
			if (previousVertex != permutationVector.front()) {
				vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
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
		*/

		vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
			&*ordersIterator,
			&*pathLengthsIterator,
			permutationVector,
			permutationVector.front(),
			matrix)
		);
		vectorOfThreadsInFlight.back()->join();
		
		// sprawdzenie czy wszystkie w¹tki siê wykona³y
		bool notFinished = 0;
		do {
			notFinished = 0;
			for (auto a : pathLengths) if (a == INT_MAX) {
				notFinished = 1;
				break;
			}
		} while (notFinished);
			
		// ustalenie najkrótszej drogi
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

		this->executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);

		for (auto& a : vectorOfThreadsInFlight) {
			delete a;
		}
			
		displayResults();
	}
	else {
		this->pathLength = bruteHelperFunction(&this->vertexOrder, matrix);
		this->executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);
		
		displayResults();
	}
}

void Algorithms::dynamicProgramming(Matrix* matrix) {
	/* dwójki
	* trójki itd...
	* max size-2 ?
	*/

	// + vector do sprawdzenia, od najmniejszej do najwiêkszej (tuple: droga + odl.?)

	/* na stertê ka¿d¹ wagê wierzcho³ku koñcz¹c¹ siê w 0 + droga	
	*/
	std::vector<int> permutationVector;
	const int matrixSize = matrix->size;
	permutationVector.reserve(matrixSize);

	for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);

	std::vector<std::vector<Cache>> cachedPaths;
	cachedPaths.reserve(matrixSize - 2);

	do {
		// sprawdzenie czy istnieje wynik w cache
			// jeœli nie, liczymy
			// jeœli tak, u¿ywamy cache i liczymy resztê jeœli coœ zosta³o + zapis do cache
		

	} while (std::next_permutation(permutationVector.begin(), permutationVector.end()));
}

bool mypredicate(int i, int j) {
	return (i == j);
}

Cache Algorithms::findCachedResult(std::vector<std::vector<Cache>>* cachedPaths, std::vector<int>* permutationVector)
{
	Cache result;

	// sprawdzenie czy istnieje wynik w cache
	// jeœli cache pusty
	if (!cachedPaths->size()) {
		result.path = { 0 };
		result.pathLength = -1;

		return result;
	}

	// dla ka¿dej kombinacji, zaczynamy od najd³u¿szych
	for (std::vector<std::vector<Cache>>::iterator iter = cachedPaths->end(); iter != cachedPaths->begin(); iter--) {
		for (auto cache : *iter) {
			// jeœli wystêpuje
			if (std::search(permutationVector->begin(), permutationVector->end(), 
				cache.path.begin(), cache.path.end()), 
				mypredicate) std::cout << "";
		}
	}

	return result;
}

void Algorithms::displayResults() {
	std::cout << "\nDlugosc sciezki: " << pathLength << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : this->vertexOrder) std::cout << a << " ";
	std::cout << "0\nCzas trwania algorytmu: " << executionTime.count() << "s\n";
}

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

			if (currentPath > shortestPath) goto ZA_DUZO;

			previousVertex = *permutationIterator;
		}

		outerIter = pointerMat->begin();
		innerIter = (*outerIter).begin();
		std::advance(innerIter, previousVertex);
		currentPath += *innerIter;

		ZA_DUZO:
		if (currentPath < shortestPath) {
			shortestPath = currentPath;
			*orderQueue = permutationVector;
		}
	} while (std::next_permutation(permutationVector.begin(), permutationVector.end()));

	return shortestPath;
}

void Algorithms::bruteHelperMultithread(std::vector<int>* orderQueue, int* pathLength, std::vector<int> permutation, int permutationNumber, Matrix* matrix) {
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

			// jezeli liczona sciezka jest dluzsza niz obecnie najkrotsza, wczesniej konczymy
			if (currentPath > shortestPath) goto ZA_DUZO;

			previousVertex = *permutationIterator;
		}

		outerIter = pointerMat->begin();
		innerIter = (*outerIter).begin();
		std::advance(innerIter, previousVertex);
		currentPath += *innerIter;
	ZA_DUZO:
		if (currentPath < shortestPath) {
			shortestPath = currentPath;
			*orderQueue = permutation;
		}
	} while (std::next_permutation(permutation.begin(), permutation.end()) && permutation.front() == permutationNumber);

	// zamiast return
	*pathLength = shortestPath;
}