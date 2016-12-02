CXXFLAGS += -DSKIPLIST_CORRECTNESS
objects = correctness_checking_rnd.o regressions.o main.o random_device.o exhaustive.o

correctness : $(objects)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $^ -o $@

$(objects) : skiplist.h

check : correctness
	./correctness

clean:
	rm correctness $(objects)
