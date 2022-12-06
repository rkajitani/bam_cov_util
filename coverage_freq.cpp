#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctype.h>

using namespace std;

long get_fasta(ifstream &ifs, string &name, string &seq);

int main(int argc, char **argv)
{
	if (argc != 4 && argc != 2) {
		cerr << "usage: " << argv[0] << " coverage.txt [ref.fa min_len]\n";
		exit(1);
	}

	ifstream ifs;
	string seq;
	string line;
	int pos;
	int cov;
	vector<int> freq;

	if (argc == 4) {
		string name;
		unordered_map<string, string> ref;
		int min_len = atoi(argv[3]);

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

		while (!ifs.eof()) {
			getline(ifs, line);
			if (line.empty())
				continue;

			if (line[0] == '>') {
				name = line.substr(1);
				pos = 0;
			}
			else {
				if (ref[name].size() >= min_len && ref[name][pos] != 'N'){
					cov = atoi(line.c_str());
					if (cov + 1 > freq.size())
						freq.resize(cov + 1, 0);
					++freq[cov];
				}
				++pos;
			}
		}
		ifs.close();
	}
	else {
		ifs.open(argv[1]);
		if (!ifs) {
			cerr << "cannot open\n";
			return 1;
		}

		while (!ifs.eof()) {
			getline(ifs, line);
			if (line.empty())
				continue;

			if (line[0] == '>') {
				pos = 0;
			}
			else {
				cov = atoi(line.c_str());
				if (cov + 1 > freq.size())
					freq.resize(cov + 1, 0);
				++freq[cov];
				++pos;
			}
		}
	}
	
	for (int i = 0; i < freq.size(); ++i)
		cout << i << '\t' << freq[i] << '\n';

	return 0;
}

long get_fasta(ifstream &ifs, string &name, string &seq)
{
	string line;

	seq.clear();
	while (getline(ifs, line)) {
		 if (line.size() > 0 && line[0] == '>') {
		 	name = line.substr(1);
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
