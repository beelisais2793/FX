
# File da compilare
MAIN = runge_kutta
INCLUDE = 

# Librerie aggiuntive che prevedo di inserire
MODULES = 

# Compilatore c++
CXX = g++
# Opzioni
CXXFLAGS = -Wall -O2 -std=c++11 -Wextra -Wunused-parameter -march=native -lm

# % = $(MAIN); la funzione parte solo 
# se esistono $(MAIN).cpp e 'Makefile'
$(MAIN): %: %.cpp %.cc %.h Makefile
	$(CXX) $< -o $@ $(CXXFLAGS)
	
# pulisce la directory
clean:
	@ -rm -rf *.d *.o *.tmp $(MAIN)
.PHONY: clean
