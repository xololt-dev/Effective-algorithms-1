#include "util.hpp"

#include <iostream>
#include <conio.h>
#include <string>

Matrix matrix;
Algorithms algo;

void algorithmMenu() {
	char option;
	do {
		std::cout << "\n==== URUCHOM ALGORYTM ===\n";
		std::cout << "1.Brute force (ST)\n";
		std::cout << "2.Brute force (MT)\n";
		std::cout << "3.Podzial i ograniczenia\n";
		std::cout << "4.Programowanie dynamiczne\n";
		std::cout << "5.Benchmark\n";
		std::cout << "0.Powrot\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			algo.bruteForce((Matrix*) &matrix, 0);
			algo.displayResults();
			break;

		case '2':
			algo.bruteForce((Matrix*) &matrix);
			algo.displayResults();
			break;

		case '3':
			// menu_list();
			break;

		case '4':
			algo.dynamicProgramming((Matrix*) &matrix);
			algo.displayResults();
			break;

		case '5':
			algo.benchmark((Matrix*)&matrix);
			break;
		}
	} while (option != '0');
}

int main() {
	char option;
	std::string fileName;
	int value;

	do {
		std::cout << "\n==== MENU GLOWNE ===\n";
		std::cout << "1.Wczytaj z pliku\n";
		std::cout << "2.Wygeneruj losowo dane\n";
		std::cout << "3.Wyswietl dane\n";
		std::cout << "4.Uruchom algorytm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			// matrix.display();
			clear();
			break;

		case '2':
			std::cout << "Podaj ilosc wierzcholkow:";
			std::cin >> value;
			matrix.generate(value);
			clear();
			break;

		case '3':
			clear();
			matrix.display();
			break;

		case '4':
			algorithmMenu();
			break;
		}
	} while (option != '0');

	return 0;
}