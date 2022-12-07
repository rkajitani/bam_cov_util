#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>

using namespace std;

int get_median(const vector<int>::iterator begin, const vector<int>::iterator end);


int main(int argc, char **argv)
{
	if (argc != 5) {
		cerr << "usage: " << argv[0] << " coverage.txt cov_th cov_median_th edge_len\n";
		exit(1);
	}

	ifstream ifs(argv[1]);
	if (!ifs) {
		cerr << "cannot open\n";
		return 1;
	}

	const int cov_th = atol(argv[2]);
	const int cov_med_th = atol(argv[3]);
	const long edge_len = atol(argv[4]);

	string line;
	string name;
	int pos;
	vector<int> cov;

	while (!ifs.eof()) {
		getline(ifs, line);
		if (line.empty()) {
			continue;
		}

		if (line[0] == '>') {
			if (cov.size() > 2 * edge_len) {
				int median = get_median(cov.begin() + edge_len, cov.end() - edge_len);
				if (median >= cov_med_th) {
					for (long i = edge_len; i < cov.size() - edge_len; ++i) {
						if (cov[i] <= cov_th) {
							cout << name << '\t' << i << '\n';
						}
					}
				}
			}

			cov.clear();
			name = line.substr(1);
		}
		else {
			cov.push_back(stoi(line));
		}
	}
	if (cov.size() > 2 * edge_len) {
		int median = get_median(cov.begin() + edge_len, cov.end() - edge_len);
		if (median >= cov_med_th) {
			for (long i = edge_len; i < cov.size() - edge_len; ++i) {
				if (cov[i] <= cov_th) {
					cout << name << '\t' << i << '\n';
				}
			}
		}
	}

	ifs.close();
	
	return 0;
}


int get_median(const vector<int>::iterator begin, const vector<int>::iterator end)
{
	vector<int> vec(begin, end);
    partial_sort(vec.begin(), vec.begin() + vec.size() / 2 + 1, vec.end());
    return vec[vec.size() / 2];
}
