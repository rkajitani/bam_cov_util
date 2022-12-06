#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <cstring>

#define SAMTOOLS "samtools"

using namespace std;

FILE *getline_fp(stringstream &ss, FILE *fp);
int cigar_aln_len(string &cigar);
void increment(vector<int> &vec, int l_pos, int r_pos);
void bam_read_header(unordered_map<string, vector<int> > &coverage, char *bam_name);
void bam_pileup_reads(unordered_map<string, vector<int> > &coverage, int outer_len, char *bam_name);
template<class F> void bam_pileup_pairs(unordered_map<string, vector<int> > &coverage, int outer_len, int min_ins, int max_ins, char *bam_name, F get_ins);
void print_coverage(unordered_map<string, vector<int> > &coverage);

class PE_get_ins
{
public:
	pair<int, int> operator()(pair<int, int> &strand, pair<int, int> l_pos, pair<int, int> r_pos)
	{
		return ((strand.first > 0) ? make_pair(l_pos.first, r_pos.second) : make_pair(l_pos.second, r_pos.first));
	}
};

class MP_get_ins
{
public:
	pair<int, int> operator()(pair<int, int> &strand, pair<int, int> l_pos, pair<int, int> r_pos)
	{
		return ((strand.first < 0) ? make_pair(l_pos.first, r_pos.second) : make_pair(l_pos.second, r_pos.first));
	}
};


int main(int argc, char **argv)
{
	if (argc < 4 || (strcmp(argv[1], "single") != 0 && strcmp(argv[1], "pair") != 0)) {
		cerr << "usage: " << argv[0] << " command(pair or single) arg1 arg2 ...\n";
		cerr << "       " << "single outer_len mapped1.bam [mapped2.bam ...]\n";
		cerr << "       " << "pair outer_len min_ins max_ins MP_flag mapped1.bam [mapped2.bam ...]\n";
		exit(1);
	}

	int outer_len = atoi(argv[2]);
	unordered_map<string, vector<int> > coverage;

	if (strcmp(argv[1], "single") == 0) {
		bam_read_header(coverage, argv[3]);
		for (int i = 3; i < argc; ++i)
			bam_pileup_reads(coverage, outer_len, argv[i]);
	}
	else {
		int min_ins = atoi(argv[3]);
		int max_ins = atoi(argv[4]);
		int MP_flag = atoi(argv[5]);
		bam_read_header(coverage, argv[6]);
		if (MP_flag > 0) {
			MP_get_ins get_ins_func;
			for (int i = 6; i < argc; ++i)
				bam_pileup_pairs(coverage, outer_len, min_ins, max_ins, argv[i], get_ins_func);
		}
		else {
			PE_get_ins get_ins_func;
			for (int i = 6; i < argc; ++i)
				bam_pileup_pairs(coverage, outer_len, min_ins, max_ins, argv[i], get_ins_func);
		}
	}

	print_coverage(coverage);

	return 0;
}

void bam_read_header(unordered_map<string, vector<int> > &coverage, char *bam_name)
{
	coverage.clear();

	string cmd(SAMTOOLS " view -H ");
	FILE *fp;

	cmd += bam_name;
	if ((fp = popen(cmd.c_str(), "r")) == NULL) {
		fputs("cannot open!\n", stderr);
		exit(1);
	}

	stringstream buf;

	while (getline_fp(buf, fp) != NULL) {
		string s;
		buf >> s;
		if (s == "@SQ") {
			string SN, LN;
			buf >> SN >> LN;

			int len = atoi(LN.substr(3).c_str());
			coverage[SN.substr(3)].resize(len, 0);
		}
	}

	pclose(fp);
}


void bam_pileup_reads(unordered_map<string, vector<int> > &coverage, int outer_len, char *bam_name)
{
	string cmd(SAMTOOLS " view ");
	FILE *fp;

	cmd += bam_name;
	if ((fp = popen(cmd.c_str(), "r")) == NULL) {
		fputs("cannot open!\n", stderr);
		exit(1);
	}

	stringstream buf;
	string s;
	string ref_name;
	string cigar;
	int flag;
	int l_pos;
	int r_pos;

	while (getline_fp(buf, fp) != NULL) {
		buf >> s >> flag >> ref_name >> l_pos >> s >> cigar;
		l_pos -= 1;
		r_pos = l_pos + cigar_aln_len(cigar);
		if (!(flag & 0x4) && r_pos - l_pos > 2*outer_len)
			increment(coverage[ref_name], l_pos + outer_len, r_pos - outer_len);
	}

	pclose(fp);
}

template<class F> void bam_pileup_pairs(unordered_map<string, vector<int> > &coverage, int outer_len, int min_ins, int max_ins, char *bam_name, F get_ins)
{
	string cmd(SAMTOOLS " view ");
	FILE* fp;

	cmd += bam_name;
	if ((fp = popen(cmd.c_str(), "r")) == NULL) {
		fputs("cannot open!\n", stderr);
		exit(1);
	}

	string s;
	string cigar;
	pair<stringstream, stringstream> buf;
	pair<string, string> ref_name;
	pair<string, string> seq;
	pair<int, int> flag;
	pair<int, int> strand;
	pair<int, int> l_pos;
	pair<int, int> r_pos;
	pair<int, int> ins_pos;
	long n_input_pair = 0;
	long n_mapped_pair = 0;

	while (getline_fp(buf.first, fp) != NULL) {
		getline_fp(buf.second, fp);
		++n_input_pair;

		buf.first >> s >> flag.first >> ref_name.first >> l_pos.first >> s >> cigar;
		l_pos.first -= 1;
		r_pos.first = l_pos.first + cigar_aln_len(cigar);
		strand.first = (flag.first & 0x10) ? -1 : 1;
		if (flag.first & 0x4)
			continue;

		buf.second >> s >> flag.second >> ref_name.second >> l_pos.second >> s >> cigar;
		l_pos.second -= 1;
		r_pos.second = l_pos.second + cigar_aln_len(cigar);
		strand.second = (flag.second & 0x10) ? -1 : 1;
		if (flag.second & 0x4)
			continue;
		
		if (strand.first * strand.second > 0 || ref_name.first !=  ref_name.second)
			continue;

		ins_pos = get_ins(strand, l_pos, r_pos);
		if (ins_pos.second - ins_pos.first >= min_ins && ins_pos.second - ins_pos.first <= max_ins) {
			increment(coverage[ref_name.first], ins_pos.first + outer_len, ins_pos.second - outer_len);
			++n_mapped_pair;
		}
	}

	std::cerr << "#num_input_pair\t" << n_input_pair << std::endl;
	std::cerr << "#num_mapped_pair\t" << n_mapped_pair << std::endl;
	std::cerr << "%mapped_rate\t" << (double)n_mapped_pair / n_input_pair * 100 << std::endl;

	pclose(fp);
}

int cigar_aln_len(string &cigar)
{
	int aln_len = 0;
	int seg_len = 0;

	for (auto itr = cigar.begin(); itr != cigar.end(); ++itr) {
		if (isdigit(*itr)) {
			seg_len = 10 * seg_len + (*itr - 48);
		}
		else {
			if (*itr != 'I')
				aln_len += seg_len;
			seg_len = 0;
		}
	}

	return aln_len;
}
			

void increment(vector<int> &vec, int l_pos, int r_pos)
{
	vector<int>::iterator end =  r_pos > vec.size() ? vec.end() : vec.begin() + r_pos;
	for (auto itr = (vec.begin() + l_pos); itr!= end; ++itr)
		++(*itr);
}

void print_coverage(unordered_map<string, vector<int> > &coverage)
{
	vector<string> buf;

	for (auto seq_itr = coverage.begin(); seq_itr != coverage.end(); ++seq_itr)
		buf.push_back(seq_itr->first);
	
	sort(buf.begin(), buf.end());

	for (auto seq_itr = buf.begin(); seq_itr != buf.end(); ++seq_itr) {
		cout << '>' << *seq_itr << '\n';
		for (auto pos_itr = coverage[*seq_itr].begin(); pos_itr != coverage[*seq_itr].end(); ++pos_itr)
			cout << *pos_itr << "\n";
	}

/*
	for (auto seq_itr = coverage.begin(); seq_itr != coverage.end(); ++seq_itr) {
		cout << '>' << seq_itr->first << '\n';
		for (auto pos_itr = seq_itr->second.begin(); pos_itr != seq_itr->second.end(); ++pos_itr)
			cout << *pos_itr << "\n";
	}
*/
}

FILE *getline_fp(stringstream &ss, FILE *fp)
{
	ss.str("");
	ss.clear();

	char c = getc(fp);
	while (c != '\n' && c != EOF) {
		ss << c;
		c = getc(fp);
	}
	
	if (c == EOF)
		return NULL;
	else
		return fp;
}

//fields of sam line :
// 1	QNAME	Query template/pair NAME
// 2	FLAG	bitwise FLAG
// 3	RNAME	Reference sequence NAME
// 4	POS	1-based leftmost POSition/coordinate of clipped sequence
// 5	MAPQ	MAPping Quality (Phred-scaled)
// 6	CIAGR	extended CIGAR string
// 7	MRNM	Mate Reference sequence NaMe (‘=’ if same as RNAME)
// 8	MPOS	1-based Mate POSistion
// 9	TLEN	inferred Template LENgth (insert size)
// 10	SEQ	query SEQuence on the same strand as the reference
// 11	QUAL	query QUALity (ASCII-33 gives the Phred base quality)
// 12+	OPT	variable OPTional fields in the format TAG:VTYPE:VALUE

//FLAG field (2) of sam line :
// 0x0001	p	the read is paired in sequencing
// 0x0002	P	the read is mapped in a proper pair
// 0x0004	u	the query sequence itself is unmapped
// 0x0008	U	the mate is unmapped
// 0x0010	r	strand of the query (1 for reverse)
// 0x0020	R	strand of the mate
// 0x0040	1	the read is the first read in a pair
// 0x0080	2	the read is the second read in a pair
// 0x0100	s	the alignment is not primary
// 0x0200	f	the read fails platform/vendor quality checks
// 0x0400	d	the read is either a PCR or an optical duplicate

