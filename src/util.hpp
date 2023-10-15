#include <fstream>
#include <vector>

class Matrix {
private:
	int size = 0;
	// wype³niane wierszami
	std::vector<std::vector<int>> mat;
	// std::unique_ptr<std::unique_ptr<int[]>[]> matS;

public:
	void loadFromFile(std::string fileName);
	void generate(int size);
	void display();
};

void clear();