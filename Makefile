CXX = g++
CXXFLAGS = -std=c++0x -O3 -funroll-loops -finline-limit-50000

PRG = bam_pileup merge_coverage coverage_freq coverage_breakpoint coverage_mean coverage_mean_upper_limit coverage_stat coverage_smooth coverage_count_covered
SRC = bam_pileup.cpp merge_coverage.cpp coverage_freq.cpp coverage_breakpoint.cpp coverage_mean.cpp coverage_mean_upper_limit.cpp coverage_stat.cpp coverage_smooth.cpp coverage_count_covered.cpp

all: $(PRG)

$(PRG): $(SRC)
	$(CXX) -o $@ $(CXXFLAGS) $@.cpp

clean:
	rm -f $(PRG)
