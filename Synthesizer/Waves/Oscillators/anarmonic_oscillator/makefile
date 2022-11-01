LIBS:=`root-config --libs`
INCS:=`root-config --cflags`
prog: prog.o vettore.o vettore_lineare.o equazioni.o armonico.o generatore.o gauss.o anarmonico.o
	g++ prog.o vettore.o vettore_lineare.o equazioni.o armonico.o generatore.o gauss.o anarmonico.o -o prog ${INCS} ${LIBS}
prog.o: prog.cpp vettore.h funzionevettBase.h vettore_lineare.h equazioni.h armonico.h generatore.h gauss.h funzioneBase.h anarmonico.h
	g++ -c prog.cpp -o prog.o ${INCS}
vettore.o: vettore.cpp vettore.h
	g++ -c vettore.cpp -o vettore.o
vettore_lineare.o: vettore_lineare.cpp vettore_lineare.h vettore.h
	g++ -c vettore_lineare.cpp -o vettore_lineare.o
equazioni.o: equazioni.cpp equazioni.h funzionevettBase.h vettore.h vettore_lineare.h
	g++ -c equazioni.cpp -o equazioni.o
armonico.o: armonico.cpp armonico.h funzionevettBase.h vettore.h vettore_lineare.h equazioni.h
	g++ -c armonico.cpp -o armonico.o
generatore.o: generatore.cpp generatore.h gauss.h funzioneBase.h
	g++ -c generatore.cpp -o generatore.o
gauss.o: gauss.cpp gauss.h funzioneBase.h
	g++ -c gauss.cpp -o gauss.o
anarmonico.o: anarmonico.cpp anarmonico.h funzionevettBase.h vettore.h vettore_lineare.h equazioni.h
	g++ -c anarmonico.cpp -o anarmonico.o
