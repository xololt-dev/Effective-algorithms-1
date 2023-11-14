#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <bitset>
#include <iomanip>
#include <queue>

void Algorithms::bruteForce(Matrix* matrix, int multithread) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	
	if (multithread) {
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
			vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
				&*ordersIterator,
				&*pathLengthsIterator,
				permutationVector,
				permutationVector.front(),
				matrix)
			);

			ordersIterator++;
			pathLengthsIterator++;

			std::swap(permutationVector[0], permutationVector[i + 1]);
		}
		vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
			&*ordersIterator,
			&*pathLengthsIterator,
			permutationVector,
			permutationVector.front(),
			matrix)
		);

		for (auto a : vectorOfThreadsInFlight) a->join();

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

		for (auto a : vectorOfThreadsInFlight) {
			delete a;
		}
	}
	else {
		this->pathLength = bruteHelperFunction(&this->vertexOrder, matrix);
		this->executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now);
	}
}

void Algorithms::dynamicProgramming(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	const int matrixSize = matrix->size;
	int result = INT_MAX, tempResult = 0;
	// rezerwacja wymaganej iloœci pamiêci
	std::unordered_map<int, Cache> insideTempMap;
	cachedPathsNew.resize(matrixSize - 1, insideTempMap);
	for (auto& a : cachedPathsNew)
		a.reserve(pow(2, matrixSize - 2));
	std::vector<short> vertexOrder, tempOrder;

	// odwiedz kazdy wierzcholek oprócz startu (0)
	for (int i = 1; i < matrixSize; i++) {
		tempResult = dynamicHelperFunction((1 << matrixSize) - 1 - (int)pow(2, i), i, &tempOrder, matrix);
		
		// jeœli rezultat znaleziony przez helper function jest mniejszy od obecnego najmniejszego, ustaw tempResult jako obecne rozwi¹zanie
		if (tempResult + matrix->mat[i][0] < result) {
			vertexOrder = tempOrder;
			result = tempResult + matrix->mat[i][0];
		}
	}

	// kolejnoœæ otrzymywana z helper function jest w odwrotnej kolejnoœci
	std::reverse(vertexOrder.begin(), vertexOrder.end());
	vertexOrder.pop_back();
	this->pathLength = result;
	this->vertexOrder = vertexOrder;

	this->executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now);

	cachedPathsNew.clear();	
}

void Algorithms::benchmark(Matrix* matrix) {
	std::fstream file;
	file.open("benchmarkResults2.txt", std::ios::out);

	if (file.good()) {
		file << std::fixed << std::setprecision(19);
		
		for (int i = 4; i < 15; i++) {
			std::cout << i << "\n--------------------\n";
			for (int j = 0; j < 100; j++) {
				matrix->generate(i);
				file << i;
				dynamicProgramming(matrix);
				file << ";" << executionTime.count();
				bruteForce(matrix, 1);
				file << ";" << executionTime.count();
				bruteForce(matrix, 0);
				file << ";" << executionTime.count() << "\n";
			}
		}
		
		std::cout << 15 << "\n--------------------\n";
		for (int j = 0; j < 100; j++) {			
			matrix->generate(15);
			file << 15;
			dynamicProgramming(matrix);
			file << ";" << executionTime.count();
			bruteForce(matrix, 1);
			file << ";" << executionTime.count() << "\n";
		}
		for (int i = 16; i < 25; i++) {
			std::cout << i << "\n--------------------\n";
			for (int j = 0; j < 100; j++) {
				matrix->generate(i);
				file << i;
				dynamicProgramming(matrix);
				file << ";" << executionTime.count() << "\n";
			}
		}

		file.close();
	}
	else std::cout << "Plik nie zostal otworzony!\n";
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
	} while (std::next_permutation(permutation.begin(), permutation.end()) && (permutation.front() == permutationNumber));

	// zamiast return
	*pathLength = shortestPath;
}

int Algorithms::dynamicHelperFunction(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix) {
	// sprawdzenie maski
	// jesli bit 0 i bit previousVertex jest zaznaczony, to odwiedzilismy wszystko inne
	if (maskCode == 1) {
		vertexOrder->push_back(0);
		vertexOrder->push_back(currentVertex);
		return matrix->mat[0][currentVertex];
	}

	// sprawdzenie cache
	// jesli bylo poprzednio obliczone, wez gotowy wynik
	std::unordered_map<int, Cache>::iterator cacheHit = cachedPathsNew[currentVertex - 1].find(maskCode);
	if (cacheHit != cachedPathsNew[currentVertex - 1].end()) {
		*vertexOrder = cacheHit->second.path;
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

			tempResult = dynamicHelperFunction(maskCode & (~(1 << i)), i, &tempOrder, matrix);
			if (tempResult + toMatrix[i][currentVertex] < result) {
				result = tempResult + toMatrix[i][currentVertex];
				resultOrder = tempOrder;
				resultOrder.push_back(currentVertex);
			}
		}			
	}
	// jak nie ma mniejszego problemu, daj wynik
	
	*vertexOrder = resultOrder;
	// sprawdzenie dodatkowe czy zapisaæ do cache, 
	// jeœli za d³ugi ci¹g (nie do wykorzystania), nie wpisujemy do Cache
	std::bitset<32> countBit (maskCode);
	if (countBit.count() > matrixSize - 2) return result;

	cachedPathsNew[currentVertex - 1].insert({ maskCode, Cache(resultOrder, result) });

	return result;
}

Node::Node(int vertex, Node* parentNode, Matrix* matrix, int parentCost) {
	childMatrix = *matrix;
	vertexNumber = vertex;
	parent = parentNode;
	cost = parentCost;
}

void Node::getChildrenNodes() {
	int verNumber = vertexNumber;
	for (int k = 0; k < childMatrix.size; ++k) {
		std::vector<int>* row = &childMatrix.mat[k];
		if ((*row)[verNumber] != -1 && k != verNumber) { //Dodawanie potomkÃ³w
			Node* newChildNode = new Node(k, this, &childMatrix, cost + childMatrix.mat[k][verNumber]);
			childrenNodes.push_back(newChildNode);
		}
	}
}

int Node::reduceColumns() {
	int columnsReductionCost = 0;
	int minValue;
	for (int i = 0; i < childMatrix.size; ++i) {
		minValue = INT_MAX;
		std::vector<int>* column = &childMatrix.mat[i];
		for (int k = 0; k < column->size(); ++k) {
			if ((*column)[k] < minValue && i != k && (*column)[k] > -1) {
				minValue = (*column)[k];
			}
		}
		if (minValue != INT_MAX) {
			columnsReductionCost += minValue;
			for (int j = 0; j < column->size(); ++j) {
				if ((*column)[j] > 0)  (*column)[j] -= minValue;
			}
		}
	}
	//childMatrix.display();
	//std::cout << "Redukcja kolumn: " << columnsReductionCost << "\n";
	return columnsReductionCost;
}

int Node::reduceRows() {
	int rowsReductionCost = 0;
	int minValue;
	for (int i = 0; i < childMatrix.size; ++i) {
		minValue = INT_MAX;
		for (int k = 0; k < childMatrix.size; ++k) {
			std::vector<int>* row = &childMatrix.mat[k];
			if ((*row)[i] < minValue && i != k && (*row)[i]>-1) minValue = (*row)[i];
		}
		if (minValue != INT_MAX) {
			rowsReductionCost += minValue;
			for (int k = 0; k < childMatrix.size; ++k) {
				std::vector<int>* row = &childMatrix.mat[k];
				if ((*row)[i] > 0) (*row)[i] -= minValue;
			}
		}
	}
	//childMatrix.display();
	//std::cout << "Redukcja rzedow: " << rowsReductionCost << "\n";
	return rowsReductionCost;
}

void Node::makeInfinity(int first, int second) {
	if (parent == nullptr) return;
	auto* row = &childMatrix.mat[second];
	for (int i = 0; i < row->size(); ++i) {
		(*row)[i] = -1;
	}
	for (int i = 0; i < row->size(); ++i) {
		childMatrix.mat[i][first] = -1;
	}
	Node* pointer = this;
	while (pointer->parent != nullptr)
	{
		childMatrix.mat[pointer->parent->vertexNumber][second] = -1;
		pointer = pointer->parent;
	}
};

int Node::reduceMatrix() {
	int reductionCost = reduceRows() + reduceColumns();
	cost += reductionCost;
	return reductionCost;
}

std::vector<int> Node::getPath()
{
	Node* pointer = this;
	std::vector<int> returnVector;
	while (pointer->parent != nullptr)
	{
		returnVector.push_back(pointer->vertexNumber);
		pointer = pointer->parent;
	}
	returnVector.push_back(pointer->vertexNumber);
	return returnVector;
}

void Algorithms::branchAndBound(Matrix* matrix) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	auto compare = [](Node* x, Node* y) {return x->cost > y->cost; };
	int finalcost = INT_MAX;
	std::vector<int> finalPath;
	std::priority_queue<Node*, std::vector<Node*>, decltype(compare)>queue(compare);
	Node* root = new Node(0, nullptr, matrix, 0);
	root->reduceMatrix();
	queue.push(root);
	while (!queue.empty()) {
		
		//std::cout << "==============\n";
		if (queue.top()->cost > finalcost) {
			break;
		}

		auto node = queue.top();
		/*
		std::cout << "Node cost: " << node->cost << "\n";
		std::cout << "Node matrix: \n";
		node->childMatrix.display();

		std::cout << "===========\n";
		*/
		queue.pop();
		node->getChildrenNodes();
		if (node->childrenNodes.size() != 0)
		{
			for (int i = 0; i < node->childrenNodes.size(); ++i) {
				node->childrenNodes[i]->makeInfinity(node->vertexNumber, node->childrenNodes[i]->vertexNumber);
				node->childrenNodes[i]->reduceMatrix();
				queue.push(node->childrenNodes[i]);
			}
		}
		else {
			finalcost = node->cost;
			finalPath = node->getPath();
		}
	}
	this->executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now);
	
	std::move(finalPath.begin(), finalPath.end(), this->vertexOrder);
	this->pathLength = finalcost;
	std::cout << "FINAL PATH:" << std::endl;
	for (int i = finalPath.size() - 1; i > -1; --i) {
		std::cout << " " << finalPath[i] << " " << std::endl;
	}
	std::cout << "FINAL PATH COST: " << finalcost << std::endl;
}