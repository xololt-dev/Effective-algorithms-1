#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <bitset>

/*
bool operator==(const Cache& lhs, const Cache& rhs) {
	return (lhs.vertexCode == rhs.vertexCode) && (lhs.path.back() == rhs.path.back());//(lhs.path == rhs.path); //&& lhs.pathLength == rhs.pathLength);
}
*/

void Algorithms::bruteForce(Matrix* matrix, int multithread) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	
	if (multithread) {
		// int maxThreadsPossible = std::thread::hardware_concurrency();

		std::vector<std::vector<short>> orders; // results dump
		std::vector<int> pathLengths; // results dump
		int matrixSize = matrix->size;

		std::vector<short> permutationVector;
		permutationVector.reserve(matrixSize);
		for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
		int previousVertex = 1;

		// mo¿na stworzyæ wartoœci na ka¿dy w¹tek bez problemu 
		
		std::vector<std::thread*> vectorOfThreadsInFlight;
		vectorOfThreadsInFlight.reserve(matrixSize - 1);
		orders.resize(matrixSize - 1);
		pathLengths.resize(matrixSize - 1, INT_MAX);
		std::vector<std::vector<short>>::iterator ordersIterator = orders.begin();
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

void Algorithms::dP(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	int shortestPath = INT_MAX;
	this->pathLength = shortestPath;
	std::vector<std::vector<int>> matAddress = matrix->mat;
	const int matrixSize = matrix->size;
	int currentVertex = 0;
	int vertexCode = 1;
	std::vector<short> fastestPath, tempPath;

	for (int i = 1; i < matrixSize - 3; i++) vertexCode *= i;

	// cachedPaths.reserve(vertexCode);
	// std::cout << cachedPaths.bucket_count() << '\n';
	// cachedPathsV.reserve(matrixSize - 2);
	// cachedPathsV = new std::vector<Cache>[matrixSize - 2];
	
	for (int i = matrixSize - 3; i >= 0; i--) {
		cachedPathsV[i].reserve(vertexCode);
		vertexCode /= 2;
	}

	vertexCode = 1;

	for (int i = 1; i < matrixSize; i++) {
		// zaczyna od 1,2,3... matrixSize-1
		vertexCode *= 2;
		// rekurencyjnie dpHelp
		int tempLength = dpHelp(vertexCode, &tempPath, i, matrix);
		// std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now).count() << "\n";
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
	// cachedPaths.clear();
	// delete[] cachedPathsV;

	displayResults();
}

void Algorithms::newDP(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	const int matrixSize = matrix->size;
	int result = INT_MAX, tempResult = 0;
	std::unordered_map<int, Cache> insideTempMap;
	cachedPathsNew.resize(matrixSize - 1, insideTempMap); //insideTempVec);
	for (auto& a : cachedPathsNew) 
		a.reserve(pow(2, matrixSize - 2));
	std::vector<short> vertexOrder, tempOrder;

	// odwiedz kazdy wierzcholek oprócz startu (0)
	for (int i = 1; i < matrixSize; i++) {
		tempResult = newDPHelper((1 << matrixSize) - 1 - (int)pow(2, i), i, &tempOrder, matrix);
		
		if (tempResult + matrix->mat[i][0] < result) {
			vertexOrder = tempOrder;
			result = tempResult + matrix->mat[i][0];
		}
	}

	std::reverse(vertexOrder.begin(), vertexOrder.end());
	vertexOrder.pop_back();
	this->pathLength = result;
	this->vertexOrder = vertexOrder;

	this->executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);
	displayResults();

	/*
	for (std::unordered_map<int, Cache>& vc : cachedPathsNew) {
	//	for (auto c : vc) {
		std::cout << vc.size() << " " << vc.max_size() << "\n";//c.first << ": " << c.second.pathLength << "  ";
//		}
	//	std::cout << "\n";
	}
	*/
	
	// delete[] cachedPathsV;
	/*
	int totalS = 0, totalI = 0;
	for (auto i : cachedPathsNew) {
		totalS += i.bucket_count() * (sizeof(void*) + sizeof(size_t)) // bucket index
			* 1.5; // estimated allocation overheads
		totalI += i.bucket_count() * (sizeof(void*) + sizeof(size_t)) // bucket index
			* 1.5; // estimated allocation overheads
		for (auto a : i) {
			totalS += a.second.path.size() * sizeof(short) + sizeof(Cache) + sizeof(void*); // data list
			// (i.size() * (sizeof(Cache) + sizeof(void*)) 
			totalI += a.second.path.size() * sizeof(int) + sizeof(Cache) + sizeof(void*); // data list
		}
	}
	std::cout << "\nTotal short: " << totalS/8 << "\nTotal int: " << totalI/8 << "\n";
	std::cout << "Smaller by: " << (double)totalS / (double)totalI << "\n";
	*/

	cachedPathsNew.clear();
}

void Algorithms::newDPV(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	const int matrixSize = matrix->size;
	int result = INT_MAX, tempResult;
	// cachedPathsV = new std::vector<Cache>[matrixSize - 1];
	Cache defaultCache;
	defaultCache.path = { 0 };
	defaultCache.pathLength = 0;
	// defaultCache.vertexCode = 0;
	std::vector<Cache> insideTempVec((int)(1 << matrixSize), defaultCache);
	cachedPathsV.resize(matrixSize - 1, insideTempVec);
	std::vector<short> vertexOrder, tempOrder;

	// odwiedz kazdy wierzcholek oprócz startu (0)
	for (int i = 1; i < matrixSize; i++) {
		tempResult = newDPHelperV((1 << matrixSize) - 1 - (int)pow(2, i), i, &tempOrder, matrix);

		if (tempResult + matrix->mat[i][0] < result) {
			vertexOrder = tempOrder;
			result = tempResult + matrix->mat[i][0];
		}
	}

	std::reverse(vertexOrder.begin(), vertexOrder.end());
	vertexOrder.pop_back();
	this->pathLength = result;
	this->vertexOrder = vertexOrder;

	this->executionTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);
	displayResults();

	/*
	for (std::unordered_map<int, Cache> vc : cachedPathsNew) {
		for (auto c : vc) {
			std::cout << c.first << ": " << c.second.pathLength << "  ";
		}
		std::cout << "\n";
	}
	*/

	cachedPathsV.clear();
	// cachedPathsNew.clear();
}

bool mypredicate(int i, int j) {
	return (i == j);
}

void Algorithms::displayResults() {
	std::cout << "\nDlugosc sciezki: " << pathLength << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : this->vertexOrder) std::cout << a << " ";
	std::cout << "0\nCzas trwania algorytmu: " << executionTime.count() << "s\n";
}

int Algorithms::bruteHelperFunction(std::vector<short>* orderQueue, Matrix* matrix) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<short> permutationVector;
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<short>::iterator permutationIterator;
	std::vector<int>::iterator innerIter = (*outerIter).begin();

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

void Algorithms::bruteHelperMultithread(std::vector<short>* orderQueue, int* pathLength, std::vector<short> permutation, int permutationNumber, Matrix* matrix) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<short>::iterator permutationIterator;
	std::vector<int>::iterator innerIter = (*outerIter).begin();

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

// vertexCode pokazuje które wierzcho³ki odwiedzone (binarnie)
// orderQueue bêdzie odwrotne
int Algorithms::dpHelp(int vertexCode, std::vector<short>* orderQueue, int previousVertex, Matrix* matrix) {
	const int matrixSize = matrix->size;
	// jeœli pozostanie tylko koñcowe
	if (vertexCode == pow(2, matrixSize) - 2) {
		orderQueue->push_back(0);
		return 0;
	}
	
	int tempVertexCode = vertexCode;
	int bestResult = INT_MAX;
	std::vector<int> toVisit;

	// sprawdzenie które wierzcho³ki mo¿na dodaæ
	for (int i = 0; i < matrixSize; i++) {
		int rest = fmodf((float)tempVertexCode, 2.0f);
		// jeœli nie odwiedzone
		if (!rest)
			toVisit.push_back(i);
		tempVertexCode >>= 1;
	}

	/*
	// jeœli pozostanie tylko koñcowe
	if (toVisit.size() == 1) {
		orderQueue->push_back(0);
		return 0;
	}
	*/
	// iteracja przez te wierzcho³ki i sprawdzenie
	std::vector<short> bestPath, tempPath;
	int tempResult, pathBack = 0;
	Cache tempCache;
	bool cacheExists = 0;

	for (int i : toVisit) {
		// unikamy 0
		if (i) {
			tempVertexCode = vertexCode + (int)pow(2, i);
			tempCache.path = { (short) i };
			//tempCache.vertexCode = tempVertexCode;
			
			for (int j = 0; j < matrixSize - 2; j++) {
				for (Cache c : cachedPathsV[j]) {
					if (/*c.vertexCode == tempVertexCode &&*/ c.path.back() == i) {
						tempPath = c.path;
						tempResult = c.pathLength;
						goto FOUND_CACHE;
					}
				}
			}

			/*
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
					
					tempPath = iterCache->path;
					tempResult = iterCache->pathLength;
					goto FOUND_CACHE;
				}
			}
			*/

			tempResult = dpHelp(tempVertexCode, &tempPath, i, matrix);
			if (tempResult == INT_MAX) continue;
			tempResult += matrix->mat[tempPath.back()][i];

			// przypisanie wartoœci cache
			tempCache.pathLength = tempResult;
			tempCache.path = tempPath;
			// tempCache.vertexCode = tempVertexCode;
			tempCache.path.push_back(i);
			// cachedPaths.insert(tempCache);
			cacheExists = 0;
			
			// sprawdzenie czy cache istnieje
			if (!cachedPathsV[tempCache.path.size() - 2].empty()) {
				for (Cache c : cachedPathsV[tempCache.path.size() - 2]) {
					if (c.pathLength == tempCache.pathLength
						/* && c.vertexCode == tempCache.vertexCode */
						&& c.path.front() == tempCache.path.front()
						&& c.path.back() == tempCache.path.back()) {
						cacheExists = 1;
						break;
					}
				}
			}
			else cacheExists = 1;

			if (!cacheExists) 
				cachedPathsV[tempCache.path.size()].push_back(tempCache);

			if (tempResult > this->pathLength) continue;
			// tempResult + œcie¿ka do i
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

int Algorithms::newDPHelper(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix) {
	// sprawdzenie maski
	// jesli bit 0 i bit previousVertex jest zaznaczony, to odwiedzilismy wszystko inne
	if (maskCode == 1) {
		//std::cout << "Ostatnie 1 + koniec " << maskCode << " " << ((1 << currentVertex) | 1) << "\n";
		vertexOrder->push_back(0);
		vertexOrder->push_back(currentVertex);
		return matrix->mat[0][currentVertex];
	}

	// sprawdzenie cache
	// jesli bylo poprzednio obliczone, wez gotowy wynik
	/*
	if (cachedPathsNew[currentVertex - 1][maskCode].pathLength != 0) {
		//std::cout << "Cache hit " << maskCode << " " << cachedPathsNew[currentVertex][maskCode].pathLength << "\n";
		*vertexOrder = cachedPathsNew[currentVertex - 1][maskCode].path;
		return cachedPathsNew[currentVertex - 1][maskCode].pathLength;
	}
	*/
	std::unordered_map<int, Cache>::iterator cacheHit = cachedPathsNew[currentVertex - 1].find(maskCode);
	if (cacheHit != cachedPathsNew[currentVertex - 1].end()) {
		*vertexOrder = cacheHit->second.path;
		//std::cout << "Cache hit:" << maskCode << "\n";
		return cacheHit->second.pathLength;
	}

	// obliczanie "reczne"
	// jesli brak cache, przejdz do mniejszych problemów
	int result = INT_MAX, tempResult;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>& toMatrix = (matrix->mat);
	std::vector<short> resultOrder, tempOrder;

	for (int i = 0; i < matrixSize; i++) {
		// nie odnosimy siê do tego samego vertexu lub
		// nie odnosimy siê do startu 
		// plus maskCode wykrywa "petle"
		if (i != currentVertex && i != 0 && (maskCode & (1 << i))) {
			//std::cout << " -> " << i;
			tempResult = newDPHelper(maskCode & (~(1 << i)), i, &tempOrder, matrix);
			if (tempResult + toMatrix[i][currentVertex] < result) {
				result = tempResult + toMatrix[i][currentVertex];
				resultOrder = tempOrder;
				resultOrder.push_back(currentVertex);
			}

			//result = std::min(result, newDPHelper(maskCode & (~(1 << i)), i, matrix)
				//+ matrix->mat[i][currentVertex]);
			//std::cout << result << " " << maskCode << " lub " << (int)(maskCode & (~(1 << i))) << " dla " << i << " currentV: " << currentVertex << "\n";
		}			
	}
	// jak nie ma mniejszego problemu, daj wynik
	
	*vertexOrder = resultOrder;
	// sprawdzenie dodatkowe czy zapisaæ do cache
	std::bitset<32> countBit (maskCode);
	if (countBit.count() > matrixSize - 2) return result;

	cachedPathsNew[currentVertex - 1].insert({ maskCode, Cache(resultOrder, result) });
	/*
	cachedPathsNew[currentVertex - 1][maskCode].path = resultOrder;
	cachedPathsNew[currentVertex - 1][maskCode].pathLength = result;
	*/
	
	return result;
}

int Algorithms::newDPHelperV(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix) {
	// sprawdzenie maski
	// jesli bit 0 i bit previousVertex jest zaznaczony, to odwiedzilismy wszystko inne
	if (maskCode == 1) {
		//std::cout << "Ostatnie 1 + koniec " << maskCode << " " << ((1 << currentVertex) | 1) << "\n";
		vertexOrder->push_back(0);
		vertexOrder->push_back(currentVertex);
		return matrix->mat[0][currentVertex];
	}

	// sprawdzenie cache
	// jesli bylo poprzednio obliczone, wez gotowy wynik

	if (cachedPathsV[currentVertex - 1][maskCode].pathLength != 0) {
		//std::cout << "Cache hit " << maskCode << " " << cachedPathsNew[currentVertex][maskCode].pathLength << "\n";
		*vertexOrder = cachedPathsV[currentVertex - 1][maskCode].path;
		return cachedPathsV[currentVertex - 1][maskCode].pathLength;
	}
	/*
	std::unordered_map<int, Cache>::iterator cacheHit = cachedPathsNew[currentVertex - 1].find(maskCode);
	if (cacheHit != cachedPathsNew[currentVertex - 1].end()) {
		*vertexOrder = cacheHit->second.path;
		//std::cout << "Cache hit:" << maskCode << "\n";
		return cacheHit->second.pathLength;
	}*/

	// obliczanie "reczne"
	// jesli brak cache, przejdz do mniejszych problemów
	int result = INT_MAX, tempResult;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>& toMatrix = (matrix->mat);
	std::vector<short> resultOrder, tempOrder;

	for (int i = 0; i < matrixSize; i++) {
		// nie odnosimy siê do tego samego vertexu lub
		// nie odnosimy siê do startu 
		// plus maskCode wykrywa "petle"
		if (i != currentVertex && i != 0 && (maskCode & (1 << i))) {
			//std::cout << " -> " << i;
			tempResult = newDPHelperV(maskCode & (~(1 << i)), i, &tempOrder, matrix);
			if (tempResult + toMatrix[i][currentVertex] < result) {
				result = tempResult + toMatrix[i][currentVertex];
				resultOrder = tempOrder;
				resultOrder.push_back(currentVertex);
			}

			//result = std::min(result, newDPHelper(maskCode & (~(1 << i)), i, matrix)
				//+ matrix->mat[i][currentVertex]);
			//std::cout << result << " " << maskCode << " lub " << (int)(maskCode & (~(1 << i))) << " dla " << i << " currentV: " << currentVertex << "\n";
		}
	}
	// jak nie ma mniejszego problemu, daj wynik

	*vertexOrder = resultOrder;
	// sprawdzenie dodatkowe czy zapisaæ do cache
	std::bitset<32> countBit(maskCode);
	if (countBit.count() > matrixSize - 2) return result;

	Cache tempCache;
	tempCache.path = resultOrder;
	tempCache.pathLength = result;
	// cachedPathsNew[currentVertex - 1].insert({ maskCode, tempCache });
	
	cachedPathsV[currentVertex - 1][maskCode].path = resultOrder;
	cachedPathsV[currentVertex - 1][maskCode].pathLength = result;

	return result;
}