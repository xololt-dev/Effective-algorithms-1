#include <vector>
#include <chrono>
#include "util.hpp"

class Algorithms {
private:
	int pathLength;
	std::vector<int> vertexOrder;
	std::chrono::duration<double> executionTime;
	
public:
	void bruteForce(Matrix* matrix);
	void branchAndBound(std::vector<std::vector<int>> matrix);
	void dynamicProgramming(std::vector<std::vector<int>> matrix);
};