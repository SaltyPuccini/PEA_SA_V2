#include "FileLoader.h"

bool FileLoader::file_read_line(ifstream& file, int tab[], int size) {
	string s;
	getline(file, s);

	if (file.fail() || s.empty())
		return (false);

	istringstream in_ss(s);

	for (int i = 0; i < size; i++) {
		in_ss >> tab[i];

		if (in_ss.fail()) {
			return (false);
		}
	}
	return (true);
}

bool FileLoader::file_read_graph(string file_name) {
	ifstream file;
	int tab[3];

	file.open(file_name.c_str());

	if (file.is_open()) {
		if (file_read_line(file, tab, 1)) {
			rozmiar = tab[0];
			int* tablica = new int[rozmiar];
			vector<vector<int>> temp(rozmiar, vector<int>(rozmiar, 0));
			for (int i = 0; i < rozmiar; i++) {
				if (file_read_line(file, tablica, rozmiar)) {
					for (int j = 0; j < rozmiar; j++) {
						temp[i][j] = tablica[j];
					}
				}
				else {
					cout << "File error - READ EDGE" << endl;
					return false;
					break;
				}
			}
			macierz = temp;
			file.close();
			return true;

		}
		else {
			cout << "File error - READ INFO" << endl;
			file.close();
			return false;
		}
	}
	else {
		cout << "File error - OPEN" << endl;
		return false;
	}
}