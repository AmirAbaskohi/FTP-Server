all: ftp.out

ftp.out: main.o json_reader.o
	g++  main.o json_reader.o -o ftp.out

main.o: main.cpp json_reader.h
	g++ -c main.cpp -o main.o

json_reader.o: json_reader.cpp json_reader.h
	g++ -c json_reader.cpp -o json_reader.o

clean:
	rm *.out
	rm *.o