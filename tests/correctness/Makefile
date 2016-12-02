CXXFLAGS += -DSKIPLIST_CORRECTNESS
#-DSKIPLIST_TRACE
objects = correctness_checking_rnd.o regressions.o main.o random_device.o exhaustive.o find_test.o

correctness : $(objects)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $^ -o $@

$(objects) : skiplist.h

check : correctness
	./correctness

clean:
	rm correctness $(objects)
