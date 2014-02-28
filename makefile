sltest: main.cpp skiplist.h 
	g++ -o sltest -std=c++0x -Wall skiplisttest.cpp

clean:
	rm -f *~ *.o sltest
