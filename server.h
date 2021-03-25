#ifndef SERVER
#define SERVER

#include <string>
#include <vector>
#include <iostream>
#include "json_reader.h"
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
        vector<User*> users;
        vector<string> files;
        int command_channel_port;
        int data_channel_port;
        void set_users(string config);
        void set_ports(string config);
        void set_files(string config);
    public:
        Server(string config_file_path);
        int get_command_channel_port();
        int get_data_channel_port();
        void print_server_info();
};

#endif