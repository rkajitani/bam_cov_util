#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 3) {
		cerr << "usage: " << argv[0] << " coverage.txt min_coverage\n";
		exit(1);
	}

	ifstream ifs(argv[1]);
	if (!ifs) {
		cerr << "cannot open\n";
		return 1;
	}

	const int min_cov = atoi(argv[2]);

	string line;
	string name;
	int pos;
	vector<int> freq;

	while (!ifs.eof()) {
		getline(ifs, line);
		if (line.empty())
			continue;

		if (line[0] == '>') {
			name = line.substr(1);
			pos = 0;
		}
		else{
			if (atoi(line.c_str()) < min_cov)
				cout << name << '\t' << pos << '\n';
			++pos;
		}
	}

	ifs.close();
	
	return 0;
}
