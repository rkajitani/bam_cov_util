#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>

using namespace std;

int main(int argc, char **argv)
{
	if (argc == 1) {
		cerr << "usage: " << argv[0] << " coverage1.txt [...]\n";
		exit(1);
	}

	ifstream ifs[argc];

	for (int i = 1; i < argc; ++i) {
		ifs[i].open(argv[i]);
		if (!ifs) {
			cerr << "cannot open\n";
			return 1;
		}
	}

	string line;
	int cov;

	while (!ifs[1].eof()) {
		cov = 0;
		getline(ifs[1], line);
		if (line[0] == '>') {
			for (int i = 2; i < argc; ++i)
				getline(ifs[i], line);
			cout << line << '\n';
		}
		else {
			cov = atoi(line.c_str());
			for (int i = 2; i < argc; ++i) {
				getline(ifs[i], line);
				cov += atoi(line.c_str());
			}
			cout << cov << '\n';
		}
	}

	for (int i = 1; i < argc; ++i)
		ifs[i].close();

	return 0;
}
