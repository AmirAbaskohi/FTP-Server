all: ftp.out

# linking
ftp.out: main.o server.o ftp_system.o json_reader.o user.o logger.o
	g++  main.o server.o ftp_system.o json_reader.o user.o logger.o -o ftp.out

# main
main.o: main.cpp server.h
	g++ -c main.cpp -o main.o

# server
server.o: server.cpp server.h ftp_system.h json_reader.h user.h logger.h
	g++ -c server.cpp -o server.o

# ftp_system
ftp_system.o: ftp_system.cpp ftp_system.h user.h logger.h
	g++ -c ftp_system.cpp -o ftp_system.o

# json_reader
json_reader.o: json_reader.cpp json_reader.h
	g++ -c json_reader.cpp -o json_reader.o

# user
user.o: user.cpp user.h
	g++ -c user.cpp -o user.o

# logger
logger.o: logger.cpp logger.h
	g++ -c logger.cpp logger.h

clean:
	rm -r -f *.out
	rm -r -f *.o