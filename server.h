#ifndef SERVER
#define SERVER

#include <string>
#include <vector>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <sys/time.h>
#include <string.h>
#include <map>
#include "json_reader.h"
#include "ftp_system.h"
#include "user.h"

#define USERS_KEY "users"
#define COMMAND_CHANNEL_PORT_KEY "commandChannelPort"
#define DATA_CHANNEL_PORT_KEY "dataChannelPort"
#define FILES_KEY "files"
#define USERNAME_KEY "user"
#define PASSWORD_KEY "password"
#define SIZE_KEY "size"
#define ADMIN_KEY "admin"

using namespace std;

class Server{
    private:
        map<int, int> clients_command_data;
        vector<string> files;
        int command_channel_port;
        int data_channel_port;
        vector<int> clients;
        int command_socket;
        int data_socket;
        Ftp_System system;
        vector<User*> get_config_users(string config);
        void set_ports(string config);
        void set_files(string config);
        void bind_sockets();
        void make_sockets_listen();
        void create_sockets();
        void set_sockets(int& max_socket_descriptor, fd_set& readfds);
        void accept_connections();
        void handle_clients_requests(fd_set& readfds);
    public:
        Server(string config_file_path);
        void run();
        void print_server_info();
};

#endif