DEPS = simulatore.h
OBJ = simulatore.o
LDFLAGS = -L/opt/openrisc/lib
CFLAGS = -I/opt/openrisc/include

%.o: %.cpp $(DEPS)
	@ $(CXX) $(CFLAGS) $(LDFLAGS) -lsim $(CPPFLAGS) -c $<
	@ echo "CC $<"

simulatore: $(OBJ)
	@ $(CXX) -o simulatore $<
	@ echo "LD simulatore"

clean: 
	@ rm -f *.o
	@ rm -f simulatore
	@ echo "CLEAN simulatore"