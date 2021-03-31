#ifndef CLIENT
#define CLIENT

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
#include <sstream>
#include <sys/stat.h>
#include "json_reader.h"

#define COMMAND_CHANNEL_PORT_KEY "commandChannelPort"
#define DATA_CHANNEL_PORT_KEY "dataChannelPort"

#define PACKET_SIZE 1024
#define DATA_SENDING_MESSAGE "Datas are sending"
#define NO_DATA "No data"
#define DOWNLOAD_COMMAND "retr"
#define FTP_DOWNLOAD_DIR_NAME "./FTP DOWNLOADS"

using namespace std;

class Client{
    private:
        int command_channel_port;
        int data_channel_port;
        void set_ports(string config);
        int connect_on_port(int port);
        vector<string> split(string str, char divider = ' ');
    public:
        Client(string config_file_path);
        void run();
};

#endif