
APPLICATION=noiseband

OBJS=noiseband.o Stk.o FileWrite.o Generator.o SineWave.o

FLAGS+=-O3 -DNDEBUG
# comment this out for big endian machines (such as PowerPC)
FLAGS+=-D__LITTLE_ENDIAN__

all: $(APPLICATION)

$(APPLICATION): $(OBJS)
	$(CXX) -o $@ $(OBJS)

%.o : %.cpp
	$(CXX) -c $(FLAGS) $<
# NB doesn't take .h files into account but should

clean:
	rm -f $(OBJS)

clobber: clean
	rm -f $(APPLICATION)

