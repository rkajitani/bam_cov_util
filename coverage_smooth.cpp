#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctype.h>
#include <cmath>

using namespace std;

long get_fasta(ifstream &ifs, string &name, string &seq);
void print_smoothed_coverage(const vector<long> &cov, const string ref, const long wsize, const long ssize);

int main(int argc, char **argv)
{
	if (argc != 5) {
		cerr << "usage: " << argv[0] << " coverage.txt ref.fa window_size step_size\n";
		exit(1);
	}

	ifstream ifs;
	string seq;
	string line;
	long pos;
	vector<long> cov;

	string name;
	unordered_map<string, string> ref;
	long wsize = atoi(argv[3]);
	long ssize = atoi(argv[4]);

	ifs.open(argv[2]);
	if (!ifs) {
		cerr << "cannot open\n";
		return 1;
	}
	while (get_fasta(ifs, name, seq))
		ref[name] = seq;
	ifs.close();


	ifs.open(argv[1]);
	if (!ifs) {
		cerr << "cannot open\n";
		return 1;
	}

	name.clear();
	while (!ifs.eof()) {
		getline(ifs, line);
		if (line.empty())
			continue;

		if (line[0] == '>') {
			if (!name.empty()) {
				cout << '>' << name << '\n';
				print_smoothed_coverage(cov, ref[name], wsize, ssize);
			}
			name = line.substr(1);
			cov.clear();
		}
		else {
			cov.push_back(stoi(line));
		}
	}
	if (!name.empty()) {
		cout << '>' << name << '\n';
		print_smoothed_coverage(cov, ref[name], wsize, ssize);
	}
	ifs.close();

	return 0;
}

long get_fasta(ifstream &ifs, string &name, string &seq)
{
	string line;

	seq.clear();
	while (getline(ifs, line)) {
		 if (line.size() > 0 && line[0] == '>') {
			stringstream buf(line.substr(1));
			buf >> name;
		 	break;
		}
	}
	if (line.size() == 0)
		return 0;

	while (getline(ifs, line)) {
		if (line.size() > 0 && line[0] == '>')
			break;
		seq += line;
	}
	if (line.size() > 0 && line[0] == '>') {
		ifs.seekg(-((long)line.size() + 1), ios_base::cur);
	}

	return 1;
}

void print_smoothed_coverage(const vector<long> &cov, const string ref, const long wsize, const long ssize)
{
	if (cov.size() < wsize)
		return;
	
	for (unsigned long lpos = 0; lpos <= cov.size() - wsize; lpos += ssize) {
		long sum = 0;
		long num = 0;
		for (unsigned long i = 0; i < wsize; ++i) {
			if (ref[lpos + i] != 'N') {
				++num;
				sum += cov[lpos + i];
			}
		}
		if (num >= 0.5 * wsize)
			printf("%d\t%f\n", lrint(static_cast<double>(wsize) / 2 + lpos), static_cast<double>(sum) / num);
	}
}
