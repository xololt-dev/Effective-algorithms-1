#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <thread>
#include <unordered_set>

bool operator==(const Cache& lhs, const Cache& rhs) {
	return (lhs.vertexCode == rhs.vertexCode) && (lhs.path.back() == rhs.path.back());//(lhs.path == rhs.path); //&& lhs.pathLength == rhs.pathLength);
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

		// mo�na stworzy� warto�ci na ka�dy w�tek bez problemu 
		
		std::vector<std::thread*> vectorOfThreadsInFlight;
		vectorOfThreadsInFlight.reserve(matrixSize - 1);
		orders.resize(matrixSize - 1);
		pathLengths.resize(matrixSize - 1, INT_MAX);
		std::vector<std::vector<int>>::iterator ordersIterator = orders.begin();
		std::vector<int>::iterator pathLengthsIterator = pathLengths.begin();

		// tworzenie w�tk�w dla zestaw�w permutacji
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
			// czy�cimy passOff, wype�niamy od nowa
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
		
		// sprawdzenie czy wszystkie w�tki si� wykona�y
		bool notFinished = 0;
		do {
			notFinished = 0;
			for (auto a : pathLengths) if (a == INT_MAX) {
				notFinished = 1;
				break;
			}
		} while (notFinished);
			
		// ustalenie najkr�tszej drogi
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
	/* dw�jki
	* tr�jki itd...
	* max size-2 ?
	*/

	// + vector do sprawdzenia, od najmniejszej do najwi�kszej (tuple: droga + odl.?)

	/* na stert� ka�d� wag� wierzcho�ku ko�cz�c� si� w 0 + droga	
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

	// cache prawdopodobnie przechowuje okropne ilo�ci duplikat�w
	do {
		// sprawdzenie czy istnieje wynik w cache
			// je�li nie, liczymy
			// je�li tak, u�ywamy cache i liczymy reszt� je�li co� zosta�o + zapis do cache
		// tempCache = findCachedResult(&cachedPaths, &permutationVector);

		// nie znaleziono
		if (tempCache.path == std::vector<int>{ -1 } || tempCache.pathLength == -1) {
			int previousVertex = 0;
			int currentPath = 0;
			int currentVertexNumber = permutationVector.size() - 1;
			int snippetLength = 0;

			std::vector<std::vector<Cache>> newCaches;

			// tutaj kolejno�� odwrotna (w por�wnaniu do brute force)
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

				// update �cie�ek w Cache
				addToCurrentIterationCache(&newCaches, temp, matrixSize, snippetLength, *permutationIterator);
	
				currentPath += snippetLength;

				previousVertex = *permutationIterator;
				currentVertexNumber--;
			} while (currentVertexNumber >= 0); //(permutationIterator != permutationVector.begin());

			// tutaj trzeba korekt�
			outerIter = pointerMat->begin();
			std::advance(outerIter, *permutationIterator);
			innerIter = (*outerIter).begin();
			currentPath += *innerIter;

			if (currentPath < shortestPath) {
				shortestPath = currentPath;
				this->vertexOrder = permutationVector;
			}

			// jesli cache pusty, trzeba go wype�ni�
			if (cachedPaths.empty()) {
				cachedPaths = newCaches;
			}
			// co� tutaj chyba skopane
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

void Algorithms::dP(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	int shortestPath = INT_MAX;
	this->pathLength = shortestPath;
	std::vector<std::vector<int>> matAddress = matrix->mat;
	const int matrixSize = matrix->size;
	int currentVertex = 0;
	int vertexCode = 1;
	std::vector<int> fastestPath, tempPath;

	for (int i = 1; i < matrixSize - 2; i++) vertexCode *= i;

	// cachedPaths.reserve(vertexCode);
	// std::cout << cachedPaths.bucket_count() << '\n';
	cachedPathsV.reserve(matrixSize - 2);
	
	for (std::vector<Cache> v : cachedPathsV) {

	}

	vertexCode = 1;

	for (int i = 1; i < matrixSize; i++) {
		// zaczyna od 1,2,3... matrixSize-1
		vertexCode *= 2;
		// rekurencyjnie dpHelp
		int tempLength = dpHelp(vertexCode, &tempPath, i, matrix);
		if (tempLength == INT_MAX) continue;
		// tempPath + droga do i
		tempLength += matAddress[i][0] + matAddress[tempPath.back()][i];
		// std::cout << "Koniec do 0: " << matAddress[i][0] << "\n";

		if (tempLength < this->pathLength) {
			this->pathLength = tempLength;
			tempPath.push_back(i);
			this->vertexOrder = tempPath;
		}
	}
	// fastestPath.push_back(0);
	std::reverse(this->vertexOrder.begin(), this->vertexOrder.end());
	this->vertexOrder.pop_back();

	// this->pathLength = shortestPath;
	// this->vertexOrder = fastestPath;
	this->executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);
	
	/*
	for (Cache c : cachedPaths) {
		for (int i : c.path) {
			std::cout << i << " ";
		}
		std::cout << "\n";
	}
	*/
	// std::cout << cachedPaths.size() << "\n";
	cachedPaths.clear();
	// cachedPaths.rehash(0);
	// cachedPaths.reserve(1);

	displayResults();
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
	// je�li cache pusty
	if (!cachedPaths->size()) return result;

	std::vector<int>::iterator iterC = permutationVector->end();
	// mo�e while loop
	
	// dla ka�dej kombinacji, zaczynamy od najd�u�szych
	/*
	for (std::vector<std::vector<Cache>>::iterator iter = cachedPaths->end(); iter != cachedPaths->begin(); iter--) {
		for (Cache cache : *iter) {
			// search
			iterC = std::search(permutationVector->begin(), permutationVector->end(), cache.path.begin(), cache.path.end(), mypredicate);

			// je�li wyst�puje
			if (iterC != permutationVector->end()) {
				// we� warto�� i wyjd�
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

			// je�li wyst�puje
			if (iterC != permutationVector->end()) {
				// we� warto�� i wyjd�
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

// vertexCode pokazuje kt�re wierzcho�ki odwiedzone (binarnie)
// orderQueue b�dzie odwrotne
int Algorithms::dpHelp(int vertexCode, std::vector<int>* orderQueue, int previousVertex, Matrix* matrix) {
	const int matrixSize = matrix->size;
	int tempVertexCode = vertexCode;
	int bestResult = INT_MAX;
	std::vector<int> toVisit;

	// sprawdzenie kt�re wierzcho�ki mo�na doda�
	for (int i = 0; i < matrixSize; i++) {
		int rest = fmodf((float)tempVertexCode, 2.0f);
		// je�li nie odwiedzone
		if (!rest) {
			toVisit.push_back(i);
		}
		tempVertexCode >>= 1;
	}

	// je�li pozostanie tylko ko�cowe
	if (toVisit.size() == 1) {
		orderQueue->push_back(0);
		return 0;
	}

	// iteracja przez te wierzcho�ki i sprawdzenie
	std::vector<int> bestPath, tempPath;
	int tempResult, pathBack = 0;
	Cache tempCache;

	for (int i : toVisit) {
		// unikamy 0
		if (i) {
			tempVertexCode = vertexCode + (int)pow(2, i);
			tempCache.path = { i };
			tempCache.vertexCode = tempVertexCode;

			std::unordered_set<Cache>::iterator iterCache = cachedPaths.begin();
			for (; iterCache != cachedPaths.end(); iterCache++) {
				if (iterCache->vertexCode == tempVertexCode && iterCache->path.back() == i) {
					/*
					std::cout << "Cache found\n";
					std::cout << iterCache->vertexCode << " == " << tempVertexCode << "\n"; // << "   " << iterCache->path.back() << " == " << i << "\n";
					for (int k : iterCache->path) {
						std::cout << k << " ";
					}
					std::cout << "   ";
					for (int j : toVisit) {
						std::cout << j << " ";
					}
					std::cout << "\n";
					*/
					tempPath = iterCache->path;
					tempResult = iterCache->pathLength;
					goto FOUND_CACHE;
				}
			}

			tempResult = dpHelp(tempVertexCode, &tempPath, i, matrix);
			if (tempResult == INT_MAX) continue;
			tempResult += matrix->mat[tempPath.back()][i];

			// przypisanie warto�ci cache
			tempCache.pathLength = tempResult;
			tempCache.path = tempPath;
			tempCache.vertexCode = tempVertexCode;
			tempCache.path.push_back(i);
			cachedPaths.insert(tempCache);

			if (tempResult > this->pathLength) continue;
			// tempResult + �cie�ka do i
			tempPath.push_back(i);

			FOUND_CACHE:
			int currentPathBack = matrix->mat[i][previousVertex];
			
			if (tempResult + currentPathBack < bestResult + pathBack) {
				bestResult = tempResult;
				pathBack = currentPathBack;
				bestPath = tempPath;
				// bestPath.push_back(i);
			}
		}
	}

	if (bestResult != INT_MAX) *orderQueue = bestPath;
	return bestResult;
}