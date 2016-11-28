objects = main.o random_device.o \
          basic_set_interface.o capacity.o construction.o \
	  iterators.o set_lookup.o set_modifiers.o

interface : $(objects)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $^ -o $@

$(objects) : skiplist.h

check : interface
	./interface

clean:
	rm interface $(objects)
