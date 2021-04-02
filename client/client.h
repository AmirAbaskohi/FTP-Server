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

#define PACKET_SIZE 2048
#define DATA_SENDING_MESSAGE "Datas are sending"
#define NO_DATA "No data"
#define DOWNLOAD_COMMAND "retr"
#define USER_COMMAND "user"
#define QUIT_COMMAND "quit"
#define PASS_COMMAND "pass"
#define FTP_DOWNLOAD_DIR_NAME "./FTP DOWNLOADS"

#define SUCCESSFUL_USER 331
#define SUCCESSFUL_PASS 230
#define SUCCESSFUL_QUIT 221

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace std;

class Client{
    private:
        int command_channel_port;
        int data_channel_port;
        int command_socket;
        int data_socket;
        string user_name;
        bool is_logged_in;
        void set_ports(string config);
        int connect_on_port(int port);
        vector<string> split(string str, char divider = ' ');
        int create_download_directory();
        void send_and_receive_data(string request, char buffer[], vector<string> request_params);
        void print_terminal_command();
        void update_user_name_and_is_logged_in(short req_type, char response[], vector<string> request_params);
    public:
        Client(string config_file_path);
        void run();
};

#endif