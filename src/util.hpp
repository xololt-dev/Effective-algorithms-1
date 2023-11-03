#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_set>

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
	int vertexCode = 0;
	std::vector<int> path;
	int pathLength = 0;
};

template <>
class std::hash<Cache>
{
public:
	size_t operator()(const Cache& cache) const
	{
		return cache.vertexCode; //static_cast<size_t>(cache.vertexCode); // << cache.path.back() << cache.path.size(); //cache.path.size() ^ cache.path.front() ^ cache.path.back();//^ cache.pathLength;
	}
};

class Algorithms {
private:
	int pathLength;
	std::vector<int> vertexOrder;
	std::chrono::duration<double> executionTime;

	std::unordered_set<Cache> cachedPaths;
	std::vector<std::vector<Cache>>cachedPathsV;

	void displayResults();

	int bruteHelperFunction(std::vector<int>* orderQueue, Matrix* matrix);
	static void bruteHelperMultithread(std::vector<int>* orderQueue, int* pathLength, std::vector<int> permutation, int permutationNumber, Matrix* matrix);
	int dpHelp(int vertexCode, std::vector<int>* orderQueue, int previousVertex, Matrix* matrix);

	void addToCurrentIterationCache(std::vector<std::vector<Cache>>* cache, Cache newEntry, int matrixSize, int snippetLength, int currentVertex);
	void updateCacheVector(std::vector<Cache>& cache, int snippetLength, int currentVertex);
	Cache findCachedResult(std::vector<std::vector<Cache>>* cachedPaths, std::vector<int>* permutationVector);

public:
	void bruteForce(Matrix* matrix, int multithread = 1);
	void branchAndBound(std::vector<std::vector<int>> matrix);
	void dynamicProgramming(Matrix* matrix);
	void dP(Matrix* matrix);
};

void clear();