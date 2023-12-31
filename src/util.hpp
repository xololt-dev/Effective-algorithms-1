#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <unordered_map>

class Matrix {
public:
	int size = 0;
	// wypełniane wierszami
	std::vector<std::vector<int>> mat;

	void loadFromFile(std::string fileName);
	void oldLoadFromFile(std::string fileName);
	void generate(int size);
	void display();
};

class Node {
public:
	int vertexNumber;
	Node* parent = nullptr;
	Matrix childMatrix;
	std::vector<Node*> childrenNodes;
	void getChildrenNodes();
	void makeInfinity(int first, int second);
	int reduceRows();
	int reduceColumns();
	int reduceMatrix();
	std::vector<short> getPath();
	int cost;
	Node(int vertex, Node* parentNode, Matrix* matrix, int parentCost);
	~Node();
};

struct Cache {
	std::vector<short> path;
	int pathLength = 0;
	Cache() : path({0}), pathLength(0) {

	}

	Cache(std::vector<short> pathIn, int pathLengthIn) : path(pathIn), pathLength(pathLengthIn) {

	}
};
class Algorithms {
private:
	int pathLength;
	std::vector<short> vertexOrder;
	std::chrono::duration<double> executionTime;

	std::vector<std::unordered_map<int, Cache>> cachedPathsNew;

	int bruteHelperFunction(std::vector<short>* orderQueue, Matrix* matrix);
	static void bruteHelperMultithread(std::vector<short>* orderQueue, int* pathLength, std::vector<short> permutation, int permutationNumber, Matrix* matrix);
	int dynamicHelperFunction(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix);

public:
	void bruteForce(Matrix* matrix, int multithread = 1);
	void branchAndBound(Matrix* matrix);
	void dynamicProgramming(Matrix* matrix);

	void displayResults();
	void benchmark(Matrix* matrix);
};

void clear();