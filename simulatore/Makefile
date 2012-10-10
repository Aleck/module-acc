DEPS = simulatore.h
OBJ = simulatore.o
LDFLAGS = -L/opt/openrisc/lib -lsim
CFLAGS = -I/opt/openrisc/include

%.o: %.cpp $(DEPS)
	@ $(CXX) $(CFLAGS) $(CPPFLAGS) -c $<
	@ echo "CC $<"

simulatore: $(OBJ)
	@ $(CXX) -o simulatore $< $(LDFLAGS)
	@ echo "LD simulatore"

clean: 
	@ rm -f *.o
	@ rm -f simulatore
	@ echo "CLEAN simulatore"