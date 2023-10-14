#include "util.hpp"

#include <iostream>
#include <conio.h>
#include <string>

Matrix matrix;

void displayMenu(std::string info) {
	std::cout << "\n==== MENU GLOWNE ===\n";
	std::cout << "1.Wczytaj z pliku\n";
	std::cout << "2.Wygeneruj losowo dane\n";
	std::cout << "3.Wyswietl dane\n";
	std::cout << "4.Uruchom algorytm\n";
	std::cout << "0.Powrot do menu\n";
	std::cout << "Podaj opcje:";
}

void algorithmMenu() {
	char option;
	do {
		
		std::cout << "1.Brute force\n";
		std::cout << "2.Podzial i ograniczenia\n";
		std::cout << "3.Programowanie dynamiczne\n";
		std::cout << "0.Powrót\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			// menu_table();
			break;

		case '2':
			// menu_list();
			break;

		case '3':
			// menu_heap();
			break;
		}
	} while (option != '0');
}

void utilMenu() {
	char opt;
	std::string fileName;
	int index, value;

	do {
		displayMenu("--- LISTA ---");
		opt = _getche();
		std::cout << std::endl;
		switch (opt) {
		case '1': //wczytytwanie z pliku
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			break;

		case '2': //generacja danych
			std::cout << " Podaj ilosc wierzcholkow:";
			std::cin >> value;

			break;

		case '3': //wyœwietlanie danych
			// myList.display();
			break;
		}
	} while (opt != '0');
}

int main() {
	char option;
	std::string fileName;

	do {
		std::cout << "\n==== MENU GLOWNE ===\n";
		std::cout << "1.Wczytaj z pliku\n";
		std::cout << "2.Wygeneruj losowo dane\n";
		std::cout << "3.Wyswietl dane\n";
		std::cout << "4.Uruchom algorytm\n";
		std::cout << "0.Powrot do menu\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			matrix.display();
			break;

		case '2':
			// menu_list();
			break;

		case '3':
			// menu_heap();
			matrix.display();
			break;
		}
	} while (option != '0');

	return 0;
}