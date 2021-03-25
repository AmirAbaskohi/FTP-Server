all: ftp.out

# linking
ftp.out: main.o server.o json_reader.o user.o
	g++  main.o server.o json_reader.o user.o -o ftp.out

# main
main.o: main.cpp server.h
	g++ -c main.cpp -o main.o

# server
server.o: server.cpp server.h json_reader.h user.h
	g++ -c server.cpp -o server.o

# json_reader
json_reader.o: json_reader.cpp json_reader.h
	g++ -c json_reader.cpp -o json_reader.o

# user
user.o: user.cpp user.h
	g++ -c user.cpp -o user.o

clean:
	rm -r -f *.out
	rm -r -f *.o