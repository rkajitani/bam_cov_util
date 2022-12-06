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
	vector<int> freq;
	long len;
	long n_cov;

	while (!ifs.eof()) {
		getline(ifs, line);
		if (line.empty())
			continue;

		if (line[0] == '>') {
			if (!name.empty())
				cout << name << '\t' << len << '\t' << n_cov << '\n';

			name = line.substr(1);
			len = 0;
			n_cov = 0;
		}
		else{
			++len;
			if (atoi(line.c_str()) >= min_cov)
				++n_cov;
		}
	}
	if (!name.empty())
		cout << name << '\t' << len << '\t' << n_cov << '\n';

	ifs.close();
	
	return 0;
}
