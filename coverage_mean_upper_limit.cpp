#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctype.h>

using namespace std;

long get_fasta(ifstream &ifs, string &name, string &seq);

int main(int argc, char **argv)
{
	if (argc != 5) {
		cerr << "usage: " << argv[0] << " coverage.txt ref.fa min_len max_depth\n";
		exit(1);
	}

	ifstream ifs;
	string seq;
	string line;
	int pos;
	unordered_map<string, long> num;
	unordered_map<string, long> sum;
	long num_all = 0;
	long sum_all = 0;
	vector<string> seq_name;

	string name;
	unordered_map<string, string> ref;
	int min_len = atoi(argv[3]);
	int max_depth = atoi(argv[4]);

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
			seq_name.push_back(name);
			pos = 0;
			num[name] = 0;
			sum[name] = 0;
		}
		else {
			if (ref[name].size() >= min_len && stoi(line) <= max_depth && ref[name][pos] != 'N'){
				++num[name];
				sum[name] += stoi(line);

				++num_all;
				sum_all +=  stoi(line);
			}
			++pos;
		}
	}
	ifs.close();

	cout << "#ALL" << '\t' << static_cast<double>(sum_all) / num_all  << '\n';
	
	for (auto it = seq_name.begin(); it != seq_name.end(); ++it) {
		if (num[*it] > 0) 
			cout << *it << '\t' << static_cast<double>(sum[*it]) / num [*it]  << '\n';
	}

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
