#include <iostream>
#include <cstring>
#include <windows.h>
#include <random>
#include <ctime>
#include <stdlib.h>

using namespace std;

//Structy, funkcje, stale i inne
enum class tura { gracz1, gracz2 };
tura aktualna = tura::gracz1;
const char alf[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };	//Do podpisania planszy
const int kompas[][2] = { {0,-1}, {-1,0}, {0,1},{1,0} }; //Do kierunkow (sciezki(...), zbudujstatki(...))
void czytaj(int plansza[10][10]);	//Czyta wskazana plansze
void menu(); //Tworzy i obsluguje menu
void pvp();	//Obsluguje tryb gracz vs gracz
void pve();	//Obsluguje tryb gracz vs AI

struct statek {
	int rodzaj; //Ilosc pol
	int hp;	//Ilosc niezniszczonych pol
	int pola[4][2];	//Wspolrzedne pol
};
struct gracz {
	int statki[10][10]{};	//Plansza z wlasnymi statkami
	int atak[10][10]{};	//Plansza z statkami przeciwnika
	statek flota[10]{};	//Trzyma statki
	bool dostepne[4]{};	//Do wyboru sciezek - dziala w sciezki(...) i zbudujstatek(...)
	gracz* przeciwnik = nullptr; //Wskaznik do przeciwnika
	statek* cel = nullptr; //Cel
	tura mojatura = tura::gracz1;
	bool sciezki(int x, int y, int statek);	//Sprawdza jak mozna zbudowac statek
	void zaminuj(int numer_statku);	//Blokuje pola sasiadujace ze statkiem
	void czysc(int co);	//Czysci plansze statki z informacji dodatkowych (-1, 2)
	void zbudujstatek(int x, int y, int numer_statku, int rodzaj);	//Obsluguje budowe statku
	void strzelaj(int x, int y); //Obsluguje strzelanie
	bool zyje(); //Sprawdza czy zyje
	
};
struct komputer : gracz {
	bool goni = false;		// |
	int slad[4][2]{};		// |
	int trafionych{};		// - obsluguja strzelanie komputera
	int roznica[2]{};		// |
	bool zawrot = false;	// |
	void zbudujflote();	//Buduje wszystkie statki komputera
	void pomysl();	//Decyduje co robic
	void pierwszy();	//Wykonuje strzal gdy nie prowadzi poscigu
	void poscig();	//Dobija raz trafiony statek
};
struct czlowiek : gracz {
	void rozdaj();	//Obsluguje gracza do rozdania statkow
	void pytajostrzal(); //Pyta o koordynaty do strzalu
};

czlowiek gracz1, gracz2;
komputer komp;
const gracz* comp = &komp;	//Do sprawdzania czy gracz jest komputerem

int main() {
	gracz1.mojatura = tura::gracz1;
	gracz2.mojatura = tura::gracz2;
	komp.mojatura = tura::gracz2;
	menu();
	return 0;
}

void menu() {
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	char wybor{};

	SetConsoleTextAttribute(hConsole, 3);
	cout << R"( _______ _________ _______ _________ _       _________
(  ____ \\__   __/(  ___  )\__   __/| \    /\\__   __/
| (    \/   ) (   | (   ) |   ) (   |  \  / /   ) (   
| (_____    | |   | (___) |   | |   |  (_/ /    | |   
(_____  )   | |   |  ___  |   | |   |   _ (     | |   
      ) |   | |   | (   ) |   | |   |  ( \ \    | |   
/\____) |   | |   | )   ( |   | |   |  /  \ \___) (___
\_______)   )_(   |/     \|   )_(   |_/    \/\_______/
                                                      )" << "\n";
	cout.width(10);
	SetConsoleTextAttribute(hConsole, 4);
	cout << " " << "1. Gracz przeciwko graczowi\n";
	cout.width(10);
	SetConsoleTextAttribute(hConsole, 2);
	cout << " " << "2. Gracz przeciwko komputerowi\n";
	cout.width(10);
	SetConsoleTextAttribute(hConsole, 15);
	cout << " " << "3. Instrukcja\n";
	cout.width(10);
	SetConsoleTextAttribute(hConsole, 4);
	cout << " " << "4. Wyjdz\n";
	SetConsoleTextAttribute(hConsole, 15);
	cin >> wybor;
	switch (wybor) {
	case '1':
		system("cls");
		gracz1.przeciwnik = &gracz2;
		gracz2.przeciwnik = &gracz1;
		pvp();
		break;
	case '2':
		system("cls");
		gracz1.przeciwnik = &komp;
		komp.przeciwnik = &gracz1;
		pve();
		break;
	case '3':
		system("cls");
		//instrukcja
		break;
	case '4':
		exit(0);
		break;
	default:
		system("cls");
		cout << "\n\n";
		cout.width(10);
		cout << " " << "Nieznany wybor\n\n\n";
		system("pause");
		system("cls");
		menu();
		break;
	}
}

void czytaj(int plansza[10][10]) {
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cout.width(3);
	cout << " ";
	for (int i = 0; i < 10; i++) {
		cout.width(2);
		SetConsoleTextAttribute(hConsole, 15);
		cout << alf[i] << " ";
	}
	cout << "\n";
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 11; j++) {
			if (j == 0) {
				SetConsoleTextAttribute(hConsole, 15);
				cout.width(2);
				cout << i + 1;
			}
			else {
				cout.width(3);
				switch (plansza[i][j - 1]) {
				case 1:
					SetConsoleTextAttribute(hConsole, 6);
					cout << 1;
					break;
				case -1:
					SetConsoleTextAttribute(hConsole, 4);
					cout << -1;
					break;
				case 2:
					SetConsoleTextAttribute(hConsole, 2);
					cout << 2;
					break;
				case 0:
					SetConsoleTextAttribute(hConsole, 3);
					cout << 0;
					break;
				case 3:
					SetConsoleTextAttribute(hConsole, 15);
					cout << 3;
					break;
				}
			}
		}
		cout << "\n";
		SetConsoleTextAttribute(hConsole, 15);
	}
};

bool gracz::sciezki(int x, int y, int statek) {
	int zmiana[2]{};
	for (int i = 0; i < 4; i++) {
		dostepne[i] = true;
	}
	//Sprawdza kazdy kierunek
	for (int kierunek = 0; kierunek < 4; kierunek++) {
		zmiana[0] = kompas[kierunek][0];
		zmiana[1] = kompas[kierunek][1];
		for (int i = 1; i < flota[statek].rodzaj; i++) {
			if (y + i * zmiana[0] > 9 || y + i * zmiana[0] < 0) {
				dostepne[kierunek] = false;
				break;
			}
			if (x + i * zmiana[1] > 9 || x + i * zmiana[1] < 0) {
				dostepne[kierunek] = false;
				break;
			}
			if (statki[y + i * zmiana[0]][x + i * zmiana[1]] != 0) {
				dostepne[kierunek] = false;
				break;
			}
		}
		//Zaznacza dostepne kierunki jako dostepne (2)
		if (dostepne[kierunek] && this != comp) { //Drugi warunek sprawdza czy funkcji uzywa komputer
			for (int j = 1; j < flota[statek].rodzaj; j++) {
				statki[y + j * zmiana[0]][x + j * zmiana[1]] = 2;
			}
		}
	}
	//Odpowiada czy w ogole mozliwe jest budowanie w tym miejscu
	if (dostepne[0] || dostepne[1] || dostepne[2] || dostepne[3]) {
		return true;
	}
	return false;
}

void gracz::czysc(int co) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (statki[i][j] == co) {
				statki[i][j] = 0;
			}
		}
	}
}

void gracz::zaminuj(int numer_statku) {
	int x{}, y{};
	for (int i = 0; i < flota[numer_statku].rodzaj; i++) {
		y = flota[numer_statku].pola[i][0];
		x = flota[numer_statku].pola[i][1];
		for (int k = -1; k < 2; k++) {
			for (int l = -1; l < 2; l++) {
				if (statki[y + k][x + l] == 0) {
					if (y + k < 10 && y + k >= 0) {
						if (x + l < 10 && x + l >= 0) {
							statki[y + k][x + l] = -1;
						}
					}
				}
			}
		}
	}
}

void gracz::zbudujstatek(int x, int y, int numer_statku, int rodzaj) {
	char wybor{};
	int kierunek[2]{};
	string zdania[] = { "L - lewo\n", "G - gora\n", "P - prawo\n", "D - dol\n" };
	char tlumaczwybor[] = { 'L', 'G', 'P', 'D' };

	//Podaje mozliwosci tylko dla dostepnych kierunkow
	cout << "Wybierz kierunek\n";
	for (int i = 0; i < 4; i++) {
		if (dostepne[i]) {
			cout << zdania[i];
		}
	}
	//Pyta do skutku
	while (true) {
		cin >> wybor;
		for (int i = 0; i < 4; i++) {
			if ((wybor == tlumaczwybor[i] || wybor - 32 == tlumaczwybor[i]) && dostepne[i]) {
				kierunek[0] = kompas[i][0];
				kierunek[1] = kompas[i][1];
				goto wyjscie;
			}
		}
		cout << "Bledny kierunek\n";
	}
wyjscie:
	//Buduje reszte statku
	for (int i = 1; i < rodzaj; i++) {
		statki[y + i * kierunek[0]][x + i * kierunek[1]] = 1;
		flota[numer_statku].pola[i][0] = y + i * kierunek[0];
		flota[numer_statku].pola[i][1] = x + i * kierunek[1];
	}
}

void czlowiek::rozdaj() {
	string wspolrzedne;
	int x{}, y{}, numer_statku{};

	//Przechodzi po rodzajach statku i odpowiednich dla nich ilosciach
	for (int rodzaj_statku = 4; rodzaj_statku >= 1; rodzaj_statku--) {
		for (int ilosc = 5 - rodzaj_statku; ilosc >= 1; ilosc--) {
			flota[numer_statku].rodzaj = rodzaj_statku;
			flota[numer_statku].hp = rodzaj_statku;
			czytaj(statki);
			cout << rodzaj_statku << ":" << 5 - ilosc << " " << "Podaj pole na statek (np.: A4)\n";
			cin >> wspolrzedne;
			system("cls");

			//Sprawdza pierwsza wspolrzedna
			if (wspolrzedne[0] > 90) {
				wspolrzedne[0] -= 32;
			}
			if (wspolrzedne[0] >= 65 && wspolrzedne[0] <= 74) {
				x = wspolrzedne[0] - 65;
			}
			else {
				cout << "Bledna pierwsza wspolrzedna\n";
				ilosc++;
				continue;
			}

			//Sprawdza druga wspolrzedna
			if (wspolrzedne[1] <= 57 && wspolrzedne[1] >= 49 && (wspolrzedne[2] == 0 || wspolrzedne[2] == 48)) {
				if (wspolrzedne[1] == '1' && wspolrzedne[2] == 48) {
					y = 9;
				}
				else {
					y = 9 - (58 - wspolrzedne[1]);
				}
			}
			else {
				cout << "Bledna druga wspolrzedna\n";
				ilosc++;
				continue;
			}
			//Sprawdza czy pole jest wolne
			if (statki[y][x] != 0) {
				cout << "Podane wspolrzedne sa juz zablokowane\n";
				ilosc++;
				continue;
			}

			//Sprawdza sciezki
			if (!sciezki(x, y, numer_statku)) {
				czysc(2);
				cout << "Stad nie mozna poprowadzic statku\n";
				ilosc++;
				continue;
			}
			//Zaczyna budowe
			statki[y][x] = 1;
			flota[numer_statku].pola[0][0] = y;
			flota[numer_statku].pola[0][1] = x;
			czytaj(statki);
			if (rodzaj_statku != 1) {
				zbudujstatek(x, y, numer_statku, rodzaj_statku);
				czysc(2);
			}
			zaminuj(numer_statku);
			system("cls");
			numer_statku++;
		}
	}
	czysc(-1);
}

void komputer::zbudujflote() {
	int x{}, y{}, numer_statku{}, kierunek{};
	int zmiana[2]{};
	mt19937 generator(time(nullptr));
	uniform_int_distribution<int> planszowa(0, 9);
	uniform_int_distribution<int> kierunkowa(0, 3);
	
	for (int rodzaj_statku = 4; rodzaj_statku >= 1; rodzaj_statku--) {
		for (int ilosc = 5 - rodzaj_statku; ilosc >= 1; ilosc--) {
			flota[numer_statku].rodzaj = rodzaj_statku;
			flota[numer_statku].hp = rodzaj_statku;
			while (true) {
				x = planszowa(generator);
				y = planszowa(generator);
				if (statki[y][x] == 0 && sciezki(x, y, numer_statku)) {
					while (true) {
						kierunek = kierunkowa(generator);
						if (dostepne[kierunek]) {
							zmiana[0] = kompas[kierunek][0];
							zmiana[1] = kompas[kierunek][1];
							break;
						}
					}
					for (int i = 0; i < rodzaj_statku; i++) {
						statki[y + i * zmiana[0]][x + i * zmiana[1]] = 1;
						flota[numer_statku].pola[i][0] = y + i * zmiana[0];
						flota[numer_statku].pola[i][1] = x + i * zmiana[1];
					}
					zaminuj(numer_statku);
					numer_statku++;
					break;
				}
			}
		}
	}
	czysc(-1);
}

void czlowiek::pytajostrzal() {
	string wspolrzedne;
	int x{}, y{};
	bool poprawnekoordy = false;	
	while (!poprawnekoordy) {
		cout << "Wybierz pole do ataku\n";
		cin >> wspolrzedne;
		poprawnekoordy = true;
		//Sprawdza pierwsza wspolrzedna
		if (wspolrzedne[0] > 90) {
			wspolrzedne[0] -= 32;
		}
		if (wspolrzedne[0] >= 65 && wspolrzedne[0] <= 74) {
			x = wspolrzedne[0] - 65;
		}
		else {
			cout << "Bledna pierwsza wspolrzedna\n";
			poprawnekoordy = false;
			continue;
		}
		//Sprawdza druga wspolrzedna
		if (wspolrzedne[1] <= 57 && wspolrzedne[1] >= 49 && (wspolrzedne[2] == 0 || wspolrzedne[2] == 48)) {
			if (wspolrzedne[1] == '1' && wspolrzedne[2] == 48) {
				y = 9;
			}
			else {
				y = 9 - (58 - wspolrzedne[1]);
			} 
		}
		else {
			cout << "Bledna druga wspolrzedna\n";
			poprawnekoordy = false;
			continue;
		}
		cout << y << " : " << x << "\n";
		if (atak[y][x] == -1) {
			cout << "Pole zostalo juz ostrzelane\n";
			poprawnekoordy = false;
			continue;
		}
		//Konczy petle
		if (poprawnekoordy) {
			break;
		}
	}
	strzelaj(x, y);
}

void gracz::strzelaj(int x, int y) {
	switch (przeciwnik->statki[y][x]) {
	case 1:
		atak[y][x] = -1;
		przeciwnik->statki[y][x] = -1;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				if (przeciwnik->flota[i].pola[j][0] == y && przeciwnik->flota[i].pola[j][1] == x) {
					cel = &przeciwnik->flota[i];
					przeciwnik->flota[i].hp--;
					if (przeciwnik->flota[i].hp <= 0) {
						system("cls");
						czytaj(statki);
						czytaj(atak);
						cout << "ZATOPIONY!\n";
						system("pause");
					}
					goto wj;
				}
			}
		}
	wj:
		break;
	case 0:
		atak[y][x] = 3;
		przeciwnik->statki[y][x] = 3;
		system("cls");
		czytaj(statki);
		czytaj(atak);
		aktualna = przeciwnik->mojatura;
		system("pause");
		break;
	}
}

void komputer::pomysl() {
	if (goni) {
		if (cel->hp <= 0) {
			cel = nullptr;
			trafionych = 0;
			goni = false;
			zawrot = false;
			return;
		}
		else {
			poscig();
		}
	}
	else {
		pierwszy();
	}
}

void komputer::poscig() {
	mt19937 generator(time(nullptr));
	uniform_int_distribution<int> kierunkowa(0, 3);
	int los{};
	int y = slad[trafionych][0];
	int x = slad[trafionych][1];

	//Goni reszte statku (juz po nietrafieniu podczas poscigu)
	if (zawrot) {
		for (int i = 0; i < 4; i++) {
			if (przeciwnik->statki[cel->pola[i][0]][cel->pola[i][1]] == 1) {
				przeciwnik->statki[y][x] = -1;
				atak[y][x] = -1;
				cel->hp--;
				return;
			}
		}
	}
	//Obiera kierunek w ktorym bedzie szukal reszty statku
	if (trafionych == 1) {
		do {
			los = kierunkowa(generator);
			roznica[1] = kompas[los][1];
			roznica[0] = kompas[los][0];
		} while (y + roznica[0] > 9 || y + roznica[0] < 0 || x + roznica[1] > 9 || x + roznica[1] < 0
			|| atak[y + roznica[0]][x + roznica[1]] != 0);
	}
	//Strzela w wylosowanym kierunku
	y += roznica[0];
	x += roznica[1];
	if (przeciwnik->statki[y][x] == 1) {
		//Trafienie
		trafionych++;
		slad[trafionych][0] = y;
		slad[trafionych][1] = x;
		przeciwnik->statki[y][x] = -1;
		atak[y][x] = -1;
		cel->hp--;
	}
	else {
		//Nietrafienie
		przeciwnik->statki[y][x] = 3;
		atak[y][x] = 3;
		aktualna = przeciwnik->mojatura;
		if (trafionych != 1) {
			zawrot = true;
		}
	}
}

void komputer::pierwszy() {
	mt19937 generator(time(nullptr));
	uniform_int_distribution<int> planszowa(0, 9);
	int y{}, x{};

	do {
		y = planszowa(generator);
		x = planszowa(generator);
	} while (atak[y][x] != 0);
	switch (przeciwnik->statki[y][x]) {
	case 1:
		//Trafienie
		przeciwnik->statki[y][x] = -1;
		atak[y][x] = -1;
		//Znajduje postrzelony statek i obiera sobie za cel
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				if (przeciwnik->flota[i].pola[j][0] == y && przeciwnik->flota[i].pola[j][1] == x) {
					cel = &przeciwnik->flota[i];
					cel->hp--;
					goni = true;
					trafionych = 1;
					slad[trafionych][0] = y;
					slad[trafionych][1] = x;
					return;
				}
			}
		}
		break;
	case 0:
		przeciwnik->statki[y][x] = 3;
		atak[y][x] = 3;
		aktualna = przeciwnik->mojatura;
		return;
		break;
	}
}

bool gracz::zyje() {
	for (int i = 0; i < 10; i++) {
		if (flota[i].hp > 0) {
			return true;
		}
	}
	return false;
}

void pvp() {
	gracz1.rozdaj();
	gracz2.rozdaj();
	while (gracz1.zyje() && gracz2.zyje()) {
		switch (aktualna) {
		case tura::gracz1:
			czytaj(gracz1.statki);
			czytaj(gracz1.atak);
			gracz1.pytajostrzal();
			system("cls");
			break;
		case tura::gracz2:
			czytaj(gracz2.statki);
			czytaj(gracz2.atak);
			gracz2.pytajostrzal();
			system("cls");
			break;
		}
	}
	if (!gracz1.zyje()) {
		cout << "\nWYGARAL GRACZ 2\n";
	}
	else {
		cout << "\nWYGRAL GRACZ 1\n";
	}
	system("pause");
	system("cls");
	main();
}

void pve() {
	gracz1.rozdaj();
	komp.zbudujflote();
	while (gracz1.zyje() && komp.zyje()) {
		switch (aktualna) {
			case tura::gracz1:
				cout.width(10);
				czytaj(gracz1.statki);
				czytaj(gracz1.atak);
				gracz1.pytajostrzal();
				system("cls");
				//czytaj(komp.atak);
				break;
			case tura::gracz2:
				komp.pomysl();
				break;
		}
	}
	if (!gracz1.zyje()) {
		cout << "\nGAMEOVER\n";
	}
	else {
		cout << "\nWYGRALES\n";
	}
}