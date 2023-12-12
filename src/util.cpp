#include "util.hpp"
#include <iostream>
#include <string>

void Matrix::loadFromFile(std::string fileName) {
	std::fstream file;
	file.open(fileName, std::ios::in);

	if (file.good()) {
		// jeœli istnieje poprzednia matryca, czyœcimy
		if (mat.size()) mat.clear();

		int dimension = 0, cross = 0, valueInMatrix = 0;
		std::string stringTemp;
		
		file >> dimension;

		this->size = dimension;
		this->mat.reserve(dimension);
		std::vector<std::vector<int>>::iterator matIter = mat.begin();

		for (int i = 0; i < dimension; i++) {
			for (int j = 0; j < dimension; j++) {
				file >> valueInMatrix;

				// pierwszy wiersz wymaga specjalnego traktowania - musimy zrobiæ push_back vectorów
				if (mat.size() < size) {
					std::vector<int> tempVec;
					tempVec.push_back(!mat.size() ? 0 : valueInMatrix);
					mat.push_back(tempVec);

					matIter = mat.begin();
				}
				// reszta normalny pushback
				else {
					if (i == j) (*matIter).push_back(0);
					else (*matIter).push_back(valueInMatrix);

					matIter++;
					if (matIter == mat.end()) matIter = mat.begin();
				}
			}
		}

		file.close();
	}
	else std::cout << "Plik nie zostal otworzony!\n";
}

void Matrix::oldLoadFromFile(std::string fileName) {
	std::fstream file;
	file.open(fileName, std::ios::in);

	if (file.good()) {
		// jeœli istnieje poprzednia matryca, czyœcimy
		if (mat.size()) mat.clear();

		int dimension = 0, cross = 0, valueInMatrix = 0;
		std::string stringTemp;

		do {
			file >> stringTemp;
		} while (stringTemp != "DIMENSION:");

		// Znalezienie wielkoœci matrycy
		std::cout << stringTemp << std::endl;
		file >> dimension;

		this->size = dimension;		
		this->mat.reserve(dimension);
		std::vector<std::vector<int>>::iterator matIter = mat.begin();

		std::cout << dimension << std::endl << std::endl;

		do {
			file >> stringTemp;
		} while (stringTemp != "EDGE_WEIGHT_SECTION");
		
		// Dotarcie do wartoœci zapisanych w matrycy, podgl¹d wartoœci zapisanych na przek¹tnych (?)
		file >> stringTemp;
		cross = std::stoi(stringTemp);

		while (stringTemp != "EOF") {
			valueInMatrix = std::stoi(stringTemp);

			// pierwszy wiersz wymaga specjalnego traktowania - musimy zrobiæ push_back vectorów
			if (mat.size() < size) {
				std::vector<int> tempVec;
				tempVec.push_back(!mat.size() ? 0 : valueInMatrix);
				mat.push_back(tempVec);

				matIter = mat.begin();
			}
			// reszta normalny pushback
			else {
				if (valueInMatrix == cross || valueInMatrix == 0) (*matIter).push_back(0);
				else (*matIter).push_back(valueInMatrix);

				matIter++;
				if (matIter == mat.end()) matIter = mat.begin();
			}

			file >> stringTemp;
		}
		file.close();
	}
	else std::cout << "Plik nie zostal otworzony!\n";
}

void Matrix::generate(int size) {
	if (mat.size()) mat.clear();
	srand(time(NULL));
	mat.reserve(size);
	this->size = size;

	for (int i = 0; i < size; i++) {
		std::vector<int> temp;
		temp.reserve(size);
		for (int j = 0; j < size; j++) {
			if (i == j) temp.push_back(0);
			else temp.push_back(1 + rand() % 500);
		}
		mat.push_back(temp);
	}
}

void Matrix::display() {
	std::vector<std::vector<int>>::iterator matIter;
	std::vector<int>::iterator matIterInner;

	for (int i = 0; i < mat.size(); i++) {
		for (matIter = mat.begin(); matIter != mat.end(); matIter++) {
			matIterInner = (*matIter).begin();
			std::advance(matIterInner, i);

			int spacesToAdd = 2;
			if (*matIterInner) {
				spacesToAdd -= (int)log10((double)(*matIterInner));
			}

			std::cout << (*matIterInner) << " ";
			while (spacesToAdd > 0) {
				std::cout << " ";
				spacesToAdd--;
			}
		}
		std::cout << "\n";
	}	
	std::cout << "\n";
}

void clear() {
#if defined _WIN32
	system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
	system("clear");
#endif
}