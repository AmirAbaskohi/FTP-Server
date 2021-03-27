all: client.out

client.out: main.o client.o json_reader.o
	g++  	main.o client.o json_reader.o -o client.out

# main
main.o: main.cpp client.h
	g++ -c main.cpp -o main.o

# client
client.o: client.cpp client.h json_reader.h
	g++ -c client.cpp -o client.o

# json_reader
json_reader.o: json_reader.cpp json_reader.h
	g++ -c json_reader.cpp -o json_reader.o

clean:
	rm -r -f *.out
	rm -r -f *.o
