#include <fstream>
#include <vector>

class Matrix {
public:
	int size = 0;
	// wypełniane wierszami
	std::vector<std::vector<int>> mat;

	void loadFromFile(std::string fileName);
	void generate(int size);
	void display();
};

void clear();