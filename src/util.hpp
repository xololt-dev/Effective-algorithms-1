#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <unordered_map>

class Matrix {
public:
	int size = 0;
	// wype³niane wierszami
	std::vector<std::vector<int>> mat;

	void loadFromFile(std::string fileName);
	void oldLoadFromFile(std::string fileName);
	void generate(int size);
	void display();
};

struct Cache {
	// int vertexCode = 0;
	std::vector<short> path;
	int pathLength = 0;
	Cache() : path({0}), pathLength(0) {

	}

	Cache(std::vector<short> pathIn, int pathLengthIn) : path(pathIn), pathLength(pathLengthIn) {

	}
};
/*
template <>
class std::hash<Cache>
{
public:
	size_t operator()(const Cache& cache) const
	{
		return cache.vertexCode; //static_cast<size_t>(cache.vertexCode); // << cache.path.back() << cache.path.size(); //cache.path.size() ^ cache.path.front() ^ cache.path.back();//^ cache.pathLength;
	}
};
*/
class Algorithms {
private:
	int pathLength;
	std::vector<short> vertexOrder;
	std::chrono::duration<double> executionTime;

	// std::unordered_set<Cache> cachedPaths;
	// std::vector<std::vector<Cache>>cachedPathsV;
	std::vector<std::vector<Cache>> cachedPathsV;
	std::vector<std::unordered_map<int, Cache>> cachedPathsNew;

	void displayResults();

	int bruteHelperFunction(std::vector<short>* orderQueue, Matrix* matrix);
	static void bruteHelperMultithread(std::vector<short>* orderQueue, int* pathLength, std::vector<short> permutation, int permutationNumber, Matrix* matrix);
	int dpHelp(int vertexCode, std::vector<short>* orderQueue, int previousVertex, Matrix* matrix);
	int newDPHelper(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix);
	int newDPHelperV(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix);

public:
	void bruteForce(Matrix* matrix, int multithread = 1);
	void branchAndBound(std::vector<std::vector<int>> matrix);
	// void dynamicProgramming(Matrix* matrix);
	void dP(Matrix* matrix);
	void newDP(Matrix* matrix);
	void newDPV(Matrix* matrix);
};

void clear();