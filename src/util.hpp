#include <fstream>
#include <vector>
#include <chrono>

class Matrix {
public:
	int size = 0;
	// wype³niane wierszami
	std::vector<std::vector<int>> mat;

	void loadFromFile(std::string fileName);
	void generate(int size);
	void display();
};

class Algorithms {
private:
	int pathLength;
	std::vector<int> vertexOrder;
	std::chrono::duration<double> executionTime;

public:
	void bruteForce(Matrix* matrix);
	void branchAndBound(std::vector<std::vector<int>> matrix);
	void dynamicProgramming(std::vector<std::vector<int>> matrix);

	int bruteHelperFunction(std::vector<int>* orderQueue, Matrix* matrix);
	static void bruteHelperMultithread(std::vector<int>* orderQueue, int* pathLength, std::vector<std::vector<int>>* permutations, Matrix* matrix);
};

void clear();