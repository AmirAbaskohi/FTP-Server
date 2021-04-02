#ifndef FTP_SYSTEM
#define FTP_SYSTEM

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include "user.h"
#include <filesystem>

#define USER_COMMAND "user"
#define PASS_COMMAND "pass"
#define QUIT_COMMAND "quit"
#define LS_COMMAND "ls"
#define PWD_COMMAND "pwd"
#define MKD_COMMAND "mkd"
#define DELETE_COMMAND "dele"
#define CWD_COMMAND "cwd"
#define RENAME_COMMAND "rename"
#define DOWNLOAD_COMMAND "retr"
#define DELETE_COMMAND "dele"
#define HELP_COMMAND "help"
#define FILE_FLAG "-f"
#define DIRECTORY_FLAG "-d"

#define BAD_SEQUENCE_OF_COMMANDS "‫‪503:‬‬ ‫‪Bad‬‬ ‫‪sequence‬‬ ‫‪of‬‬ ‫‪commands.‬‬\n"
#define INVALID_USERNAME_OR_PASSWORD "‫‪430:‬‬ ‫‪Invalid‬‬ ‫‪username‬‬ ‫‪or‬‬ ‫‪password‬‬\n"
#define USERNAME_ACCEPTED "‫‪331:‬‬ ‫‪User‬‬ ‫‪name‬‬ ‫‪okay,‬‬ ‫‪need‬‬ ‫‪password.‬\n‬"
#define PASSWORD_ACCEPTED "‫‪230:‬‬ ‫‪User‬‬ ‫‪logged‬‬ ‫‪in,‬‬ ‫‪proceed.‬‬ ‫‪Logged‬‬ ‫‪out‬‬ ‫‪if‬‬ ‫‪appropriate.‬‬\n"
#define SUCCESSFUL_QUIT "‫‪221:‬‬ ‫‪Successful‬‬ ‫‪Quit.‬‬\n"
#define SYNTAX_ERROR "‫‪501:‬‬ ‫‪Syntax‬‬ ‫‪error‬‬ ‫‪in‬‬ ‫‪parameters‬‬ ‫‪or‬‬ ‫‪arguments.‬‬\n"
#define NEED_FOR_ACCOUNT "‫‪332:‬‬ ‫‪Need‬‬ ‫‪account‬‬ ‫‪for‬‬ ‫‪login.‬‬\n"
#define INTERNAL_SERVER_ERROR "500: Error\n"
#define LIST_TRANSFER_DONE "226: List transfer done.\n"
#define SUCCESSFUL_CHANGE "250: Successful change.\n"
#define CANT_OPEN_DATA_CONNECTION "425: Can\'t open data connection.\n"
#define SUCCESSFUL_DOWNLOAD "226: Successful download.\n"
#define FILE_UNAVAILABLE "550: File unavailable.\n"

using namespace std;

struct ftp_user
{
    User* user_info;
    bool is_authorized;
    string current_dir;
    string data;
    bool has_data;
};


class Ftp_System{
    private:
        string default_directory;
        map<int, ftp_user*> online_users;
        vector<User*> all_users;
        vector<string> admin_files;
        vector<string> split(string str, char divider);
        string join(vector<string> vstr, char joiner);
        string handle_user(vector<string> args, int client_sd);
        string handle_password(vector<string> args, int client_sd);
        string handle_quit(int client_sd);
        string handle_ls(int client_sd);
        string handle_pwd(int client_sd);
        string handle_mkd(string path, int client_sd);
        string handle_dele(string type, string path, int client_sd);
        string handle_cwd(string path, int client_sd);
        string handle_rename(string old_name, string new_name, int client_sd);
        string handle_download(string file_name, int client_sd);
        string handle_help(int client_sd);
        bool does_file_exist(string file_name, string directory);
        bool is_file_for_admin(string file_name);
        long get_file_size(string file_name, string directory);
        int delete_directory(string path);
        User* find_user(string user_name);
        ftp_user* create_online_user(User* user);
    public:
        Ftp_System(vector<User*> _all_users, vector<string> _admin_files);
        Ftp_System() = default;
        vector<User*> get_all_users();
        vector<string> get_admin_files();
        void remove_online_user(int client_sd);
        string handle_command(char command[], int client_sd);
        bool has_user_data(int client_sd);
        string get_user_data(int client_sd);
};

#endif