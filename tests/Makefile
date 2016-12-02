CXX = clang++
CXXINCLUDE = -I$(realpath ./../) -I$(realpath ./../src) -I$(realpath .)
CXXFLAGS = -std=c++14 -Wall -O3 -g $(CXXINCLUDE)
VPATH = $(realpath ./../src)

export CXX
export CXXFLAGS
export VPATH

suites = correctness performance interface
# see subdir discussion after https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all clean $(suites)

all: $(suites)

$(suites):
	$(MAKE) -j 4 -C $@

check: correctness interface
	$(MAKE) -j 4 check -C correctness
	$(MAKE) -j 4 check -C interface

clean:
	$(MAKE) clean -C correctness
	$(MAKE) clean -C interface
	$(MAKE) clean -C performance
