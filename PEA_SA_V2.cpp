#include"FileLoader.h"
#include <chrono>
#include <random>
#include <windows.h>
#include <iomanip>

using namespace std;

class Algorytm{
public: 

	long long int read_QPC()
	{
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);
		return((long long int)count.QuadPart);
	}
	
	//Funkcja zwraca liczbę wylosowaną z podanego zakresu
	int RNG(int zakresDolny, int zakresGorny) {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dist(zakresDolny, zakresGorny);
		return dist(gen);
	}
	
	//Funkcja sumuje koszta poszczególnych przejść między miastami w podanej jako argument ścieżce 
	int liczDroge(vector<vector<int>> macierz, vector<int> sciezka) {
		int droga = 0;
		for (int i = 0; i < sciezka.size() - 1; i++) {
			droga = droga + macierz[sciezka[i]][sciezka[i + 1]];
		}
		return droga;
	}

	//Funkcja wypisuje w konsoli wynik działania algorytmu
	void wypisz(vector<int> rozwiazanie, vector<vector<int>> macierz, int i, long long int elapsed, long long int frequency, int policzona) {
		for (int j = 0; j < rozwiazanie.size(); j++) {
			cout << rozwiazanie[j] << ", ";
		}

		cout << endl;
		cout << "Droga: " << policzona;

		cout << endl;
		cout << "Licznik petli: " << i;

		cout << endl;
		cout << "Czas: " << ((float)elapsed / frequency);
		cout << endl;
	}

	//Główna funkcja, służąca do rozwiązania problemu komiwojażera za pomocą metody symulowanego wyżarzania
	//Temperatura początkowa i końcowa, a także czas stopu, cooling rate oraz ilość iteracji wewnętrznej pętli są przekazywane do funkcji 
	int SA(vector<vector<int>> macierz, float czasStopu, double T, double Tmin, double coolingRate, int wewnetrzneIteracje) {
		
		long long int frequency, start, elapsed = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		start = read_QPC();
		srand(unsigned(time(0)));

		vector<int> rozwiazanie;
		vector<int> noweRozwiazanie;
		vector<int> najlepszeRozwiazanie;
		int i = 0, droga, nowaDroga, najlepszaDroga;

		//Wyznaczam rozwiązanie początkowe poprzez wpisanie do vektora wszystkich miast, a następnie ustawieniu ich w losowej kolejności
		for (int j = 0; j < macierz.size(); j++) {
			rozwiazanie.push_back(j);
		}
		rozwiazanie.push_back(0);
		random_shuffle(rozwiazanie.begin() + 1, rozwiazanie.end() - 1);

		droga = liczDroge(macierz, rozwiazanie);

		//Algorytm działa, dopóki nie osiągnie temperatury minimalnej, bądź nie przekroczy ustalonego czasu stopu.
		while (T > Tmin && ((float)elapsed / frequency) < czasStopu) {
			
			//Dla każdej nowej temperatury, resetuję licznik pętli wewnętrznej
			int licznikWewnetrzny = 0;
			

			while (licznikWewnetrzny < wewnetrzneIteracje) {
				
				//Zmieniam rozwiązanie na inne, znajdujące się w sąsiedztwie: Losuję dwa indeksy i zamieniam miejscami liczby o tych indeksach.
				noweRozwiazanie = rozwiazanie;

				int x, y;

				do {
					x = RNG(1, rozwiazanie.size() - 2);
					y = RNG(1, rozwiazanie.size() - 2);
				} while (x == y);
				
				swap(noweRozwiazanie[x], noweRozwiazanie[y]);

				//Obliczam i porównuję koszta nowego i poprzedniego rozwiązania.
				
				nowaDroga = liczDroge(macierz, noweRozwiazanie);
				
				//Jeśli zmiana obniża koszt, akceptuję nową permutację.
				if (nowaDroga < droga) {
					rozwiazanie = noweRozwiazanie;
					najlepszeRozwiazanie = noweRozwiazanie;
					najlepszaDroga = nowaDroga;
					droga = nowaDroga;
				}
				//W przeciwnym razie, akceptuję zmianę permutacji z prawdopodobieństwem zależnym od różnicy kosztów.
				else {
					double losowa = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					double zmiana = exp(-((nowaDroga - droga) / T));
					if (losowa < zmiana) {
						rozwiazanie = noweRozwiazanie;
						droga = nowaDroga;
					}
				}
				licznikWewnetrzny++;
			}
			//Zmniejszam temperaturę.
			T = T * coolingRate;
			//Zwiększam licznik pętli.
			i = i + 1;
			//Mierzę czas.
			elapsed = read_QPC() - start;
		}

		wypisz(najlepszeRozwiazanie, macierz, i, elapsed, frequency, najlepszaDroga);

		return najlepszaDroga;
	}


};


//Klasa Dostęp jest odpowiedzialna za menu oraz testy.
class Dostep {
public:
	float czasStopu = 60;
	double T = 10000, Tmin = 0.001, coolingRate = 0.999, wewnetrznaPetla = 50;

	char wejscie;
	bool petla = true;

	void menu(FileLoader wczyt, Algorytm algorytm) {


		string sciezka = "";
		do {
			cout << endl;
			cout << "0. Wczytaj dane z pliku" << endl;
			cout << "1. Wprowadz parametry" << endl;
			cout << "2. Test algorytmu (bazowo dobrane parametry: Czas stopu = 60s, Temperatura poczatkowa = 10000, Temperatura minimalna = 0.001, Cooling rate = 0.999, Petla wewnetrzna = 50 )" << endl;
			cout << "3. Test algorytmu (parametry uzytkownika: Czas stopu = " << czasStopu << "s, Temperatura poczatkowa = " << T << ", Temperatura minimalna = " << Tmin << ", Cooling rate = " << coolingRate << ", Petla wewnetrzna = " << wewnetrznaPetla << " ) " << endl;
			//cout << "3. Rozpocznij testy czasowe" << endl;
			cout << "q. Wyjdz" << endl;

			cin >> wejscie;

			switch (wejscie) {


			case '0':
				cout << "Podaj sciezke dostepu do pliku (np. Q:\\\\graf.txt)" << endl;
				cin >> sciezka;

				if (!wczyt.file_read_graph(sciezka))
					break;

				for (int i = 0; i < wczyt.macierz.size(); i++) {
					for (int j = 0; j < wczyt.macierz[i].size(); j++) {
						cout << wczyt.macierz[i][j] << " ";
					}
					cout << endl;
				}
				cout << endl;
				break;

			case '3':

				algorytm.SA(wczyt.macierz, czasStopu, T, Tmin, coolingRate, wewnetrznaPetla);
				break;
			case '2':

				algorytm.SA(wczyt.macierz, 60, 10000, 0.001, 0.999, 50);
				break;


			case '4':
				//init(algorytm);
				break;

			case '1':

				cout << endl;

				cout << "Podaj czas stopu (w sekundach)" << endl;
				cin >> czasStopu;
				cout << "Podaj Temperature poczatkowa" << endl;
				cin >> T;
				cout << "Podaj Temperature koncowa" << endl;
				cin >> Tmin;
				cout << "Podaj cooling rate" << endl;
				cin >> coolingRate;
				cout << "Podaj powtorzenia wewnetrznej petli" << endl;
				cin >> wewnetrznaPetla;
				break;

			case 'q':
				petla = false;
				break;

			default:
				break;
			}
		} while (petla);

	}

	void test(string nazwa, int powtorzenia, Algorytm algorytm) {

		vector<int> wyniki;
		long long int frequency, start, elapsed, zmierzonyCzas;
		FileLoader wczyt;
		fstream wynik;
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);


		wynik.open("wynik.txt", ios::app);

		wczyt.file_read_graph(nazwa);

		start = algorytm.read_QPC();

		for (int i = 0; i < powtorzenia; i++) {
			wyniki.push_back(algorytm.SA(wczyt.macierz, 10, 1000, 0.001, 0.99, 20));
		}
		elapsed = algorytm.read_QPC() - start;
		float suma = 0;
		for (int i = 0; i < powtorzenia; i++) {
			suma += wyniki[i];
		}

		wynik << "wielkość problemu: " << nazwa << "; czas:" << fixed << setprecision(3) << (float)elapsed / frequency / powtorzenia << "[s], " << "średnia długość drogi: " << suma / (float)wyniki.size() << endl;
		for (int i = 0; i < powtorzenia; i++) {
			wynik << wyniki[i] << endl;
		}
	}

	void init(Algorytm algorytm) {

		int powtorzenia;
		string nazwa;
		fstream config;

		config.open("start.ini", ios::in);

		if (config) {
			while (!config.eof()) {
				config >> nazwa;
				config >> powtorzenia;
				test(nazwa, powtorzenia, algorytm);
			}
		}
	}
};

int main()
{
	FileLoader wczyt;
	Algorytm algorytm;
	Dostep menGlowne;
	menGlowne.menu(wczyt, algorytm);

	return 0;
}