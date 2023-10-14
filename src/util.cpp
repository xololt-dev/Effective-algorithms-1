#include "util.hpp"
#include <iostream>
#include <string>

void Matrix::loadFromFile(std::string fileName) {
	std::fstream file;
	file.open(fileName, std::ios::in);

	if (file.good()) {
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
		std::vector<std::vector<int>>::iterator matIter;
		
		/*
		* Nie wiem czy ten reserve jest dobrze zrobiony - i tak nie jest to kluczowe
		for (matIter = mat.begin(); matIter != mat.end(); matIter++) {
			(*matIter).reserve(dimension);
		}
		*/

		std::cout << dimension << std::endl << std::endl;

		do {
			file >> stringTemp;
		} while (stringTemp != "EDGE_WEIGHT_SECTION");
		
		// Dotarcie do wartoœci zapisanych w matrycy, podgl¹d wartoœci zapisanych na przek¹tnych (?)
		// std::cout << stringTemp << std::endl;
		file >> stringTemp;
		cross = std::stoi(stringTemp);
		
		matIter = mat.begin();

		while (true) {
			file >> stringTemp;
			if (stringTemp == "EOF") break;
			
			valueInMatrix = std::stoi(stringTemp);

			// czasami przek¹tne s¹ mieszane miêdzy wartoœci¹ cross i 0
			if (valueInMatrix == cross || valueInMatrix == 0) {
				matIter = mat.begin();
				// std::cout << "\n";
			}
			else {
				// pierwszy wiersz wymaga specjalnego traktowania - musimy zrobiæ push_back vectorów
				if (mat.size() < size) {
					std::vector<int> tempVec;
					tempVec.push_back(valueInMatrix);
					mat.push_back(tempVec);
					matIter = mat.begin();
				}
				// reszta normalny pushback
				else {
					(*matIter).push_back(valueInMatrix);
					matIter++;
				}
				/*
				int spacesToAdd = 2 - (int)log10((double)valueInMatrix);
				std::cout << stringTemp << " ";

				while (spacesToAdd > 0) {
					std::cout << " ";
					spacesToAdd--;
				}*/
			}
		}

		file.close();
	}
	else std::cout << "Plik nie zostal otworzony!\n";
}

void Matrix::display() {
	std::vector<std::vector<int>>::iterator matIter;
	std::vector<int>::iterator matIterInner;

	for (int i = 0; i < mat.size() - 1; i++) {
		for (matIter = mat.begin(); matIter != mat.end(); matIter++) {
			matIterInner = (*matIter).begin();
			std::advance(matIterInner, i);

			int spacesToAdd = 2 - (int)log10((double)(*matIterInner));
			std::cout << (*matIterInner) << " ";

			while (spacesToAdd > 0) {
				std::cout << " ";
				spacesToAdd--;
			}
		}
		std::cout << "\n";
	}	
}