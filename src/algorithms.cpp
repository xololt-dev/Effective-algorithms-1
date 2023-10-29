#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <thread>
#include <unordered_set>

bool operator==(const Cache& lhs, const Cache& rhs) {
	return (lhs.path == rhs.path); //&& lhs.pathLength == rhs.pathLength);
}

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

bool sortCache(Cache c1, Cache c2) {
	return (c1.path.front() <= c2.path.front()) && 
		(c1.path[1] <= c2.path[1]) &&
		(c1.pathLength < c2.pathLength);
}

bool checkPath(Cache c1, Cache c2) {
	if (c1.path == c2.path && c1.pathLength <= c2.pathLength) {
		return 1;
	}
	else {
		return 0;
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
	int shortestPath = INT_MAX;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<int> permutationVector;
	const int matrixSize = matrix->size;
	permutationVector.reserve(matrixSize - 1);

	for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);

	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<int>::iterator permutationIterator, innerIter = (*outerIter).begin();

	std::vector<std::vector<Cache>> cachedPaths;
	cachedPaths.reserve(matrixSize - 2);
	
	Cache tempCache;
	tempCache.pathLength = -1;

	// cache prawdopodobnie przechowuje okropne iloœci duplikatów
	do {
		// sprawdzenie czy istnieje wynik w cache
			// jeœli nie, liczymy
			// jeœli tak, u¿ywamy cache i liczymy resztê jeœli coœ zosta³o + zapis do cache
		// tempCache = findCachedResult(&cachedPaths, &permutationVector);

		// nie znaleziono
		if (tempCache.path == std::vector<int>{ -1 } || tempCache.pathLength == -1) {
			int previousVertex = 0;
			int currentPath = 0;
			int currentVertexNumber = permutationVector.size() - 1;
			int snippetLength = 0;

			std::vector<std::vector<Cache>> newCaches;

			// tutaj kolejnoœæ odwrotna (w porównaniu do brute force)
			permutationIterator = permutationVector.end();

			do {
				permutationIterator--;

				outerIter = pointerMat->begin();
				std::advance(outerIter, previousVertex);

				innerIter = (*outerIter).begin();
				std::advance(innerIter, *permutationIterator);
				snippetLength = *innerIter;

				Cache temp;
				temp.path = { *permutationIterator, previousVertex };
				temp.pathLength = snippetLength;

				// update œcie¿ek w Cache
				addToCurrentIterationCache(&newCaches, temp, matrixSize, snippetLength, *permutationIterator);
	
				currentPath += snippetLength;

				previousVertex = *permutationIterator;
				currentVertexNumber--;
			} while (currentVertexNumber >= 0); //(permutationIterator != permutationVector.begin());

			// tutaj trzeba korektê
			outerIter = pointerMat->begin();
			std::advance(outerIter, *permutationIterator);
			innerIter = (*outerIter).begin();
			currentPath += *innerIter;

			if (currentPath < shortestPath) {
				shortestPath = currentPath;
				this->vertexOrder = permutationVector;
			}

			// jesli cache pusty, trzeba go wype³niæ
			if (cachedPaths.empty()) {
				cachedPaths = newCaches;
			}
			// coœ tutaj chyba skopane
			else {
				std::vector<std::vector<Cache>>::iterator cacheIter = cachedPaths.begin();

				for (std::vector<Cache> vC : newCaches) {
					std::unordered_set<Cache> cache_set(cacheIter->begin(), cacheIter->end());

					for (Cache c : vC) {
						std::pair<std::unordered_set<Cache>::iterator, bool> check = cache_set.insert(c);
						
						if (!check.second) {
							/*
							std::cout << (check.first)->pathLength << "   " << c.pathLength << "\n";
							for (int i : (check.first)->path) {
								std::cout << i << " ";
							}
							std::cout << "   ";
							for (int i : c.path) {
								std::cout << i << " ";
							}
							std::cout << "\n";
							*/
							if ((check.first)->pathLength > c.pathLength
								&& (check.first)->path == c.path) {
								cache_set.erase(check.first);
								cache_set.insert(c);
							}
						}
					}

					*cacheIter = std::vector<Cache>(cache_set.begin(), cache_set.end());
					cacheIter++;
				}
			}
			newCaches.clear();
		}
		// znaleziono
		else {
			int position = 0;

			shortestPath = 1234567;
		}
	} while (std::next_permutation(permutationVector.begin(), permutationVector.end()));
	

	for (std::vector<Cache>& v : cachedPaths) {
		std::sort(v.begin(), v.end(), sortCache);
	}
	/*
	int len = 2;
	for (std::vector<Cache> v : cachedPaths) {
		std::cout << "Should be length " << len << "\n\n";
		for (Cache c : v) {
			std::cout << "Path: ";
			for (int i : c.path) {
				std::cout << i << " ";
			}
			std::cout << "\nPath length: " << c.pathLength << "\n";
		}
		len++;
	}
	*/
	std::cout << "\n\n\n\n";
	std::cout << shortestPath << "\n";
	for (int a : this->vertexOrder) {
		std::cout << a << " ";
	}
	std::cout << "\n";
}

bool mypredicate(int i, int j) {
	return (i == j);
}

void Algorithms::addToCurrentIterationCache(std::vector<std::vector<Cache>>* cache, Cache newEntry, int matrixSize, int snippetLength, int currentVertex) {
	std::vector<Cache> tempC = { newEntry };
	if (cache->empty()) {		
		cache->insert(cache->begin(), tempC);

		return;
	}

	std::vector<std::vector<Cache>>::iterator i = cache->end();
	std::advance(i, -1);
	if ((*i).front().path.size() < matrixSize - 2) {
		std::vector<Cache> tempCacheV = *i;
		updateCacheVector(tempCacheV, snippetLength, currentVertex);
		cache->push_back(tempCacheV);
	}

	i = cache->end();
	std::advance(i, -1);
	do {
		i--;
		if ((*i).front().path.size() >= matrixSize - 2) continue;

		std::vector<Cache> tempCacheV = *i;
		updateCacheVector(tempCacheV, snippetLength, currentVertex);
		i++;
		(*i).insert((*i).end(), std::make_move_iterator(tempCacheV.begin()),
			std::make_move_iterator(tempCacheV.end()));
		i--;
	} while (i != cache->begin());
	
	cache->begin()->push_back(newEntry);
}

void Algorithms::updateCacheVector(std::vector<Cache>& cache, int snippetLength, int currentVertex) {
	for (Cache& c : cache) {
		c.pathLength += snippetLength;
		c.path.insert(c.path.begin(), currentVertex);
	}
}

Cache Algorithms::findCachedResult(std::vector<std::vector<Cache>>* cachedPaths, std::vector<int>* permutationVector) {
	Cache result;
	result.path = { -1 };
	result.pathLength = -1;

	// sprawdzenie czy istnieje wynik w cache
	// jeœli cache pusty
	if (!cachedPaths->size()) return result;

	std::vector<int>::iterator iterC = permutationVector->end();
	// mo¿e while loop
	
	// dla ka¿dej kombinacji, zaczynamy od najd³u¿szych
	/*
	for (std::vector<std::vector<Cache>>::iterator iter = cachedPaths->end(); iter != cachedPaths->begin(); iter--) {
		for (Cache cache : *iter) {
			// search
			iterC = std::search(permutationVector->begin(), permutationVector->end(), cache.path.begin(), cache.path.end(), mypredicate);

			// jeœli wystêpuje
			if (iterC != permutationVector->end()) {
				// weŸ wartoœæ i wyjdŸ
				result = cache;
				break;
			}
		}
	}
	*/
	std::vector<std::vector<Cache>>::iterator iter = cachedPaths->end();
	while (iterC == permutationVector->end() && iter != cachedPaths->begin()) {
		for (Cache cache : *iter) {
			// search
			iterC = std::search(permutationVector->begin(), permutationVector->end(), cache.path.begin(), cache.path.end(), mypredicate);

			// jeœli wystêpuje
			if (iterC != permutationVector->end()) {
				// weŸ wartoœæ i wyjdŸ
				result = cache;
				break;
			}
		}
		
		iter--;
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