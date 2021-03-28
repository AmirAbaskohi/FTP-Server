#ifndef FTP_SYSTEM
#define FTP_SYSTEM

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include "user.h"

#define USER_COMMAND "user"
#define PASS_COMMAND "pass"
#define QUIT_COMMAND "quit"
#define LS_COMMAND "ls"

#define BAD_SEQUENCE_OF_COMMANDS "‫‪503:‬‬ ‫‪Bad‬‬ ‫‪sequence‬‬ ‫‪of‬‬ ‫‪commands.‬‬\n"
#define INVALID_USERNAME_OR_PASSWORD "‫‪430:‬‬ ‫‪Invalid‬‬ ‫‪username‬‬ ‫‪or‬‬ ‫‪password‬‬\n"
#define USERNAME_ACCEPTED "‫‪331:‬‬ ‫‪User‬‬ ‫‪name‬‬ ‫‪okay,‬‬ ‫‪need‬‬ ‫‪password.‬\n‬"
#define PASSWORD_ACCEPTED "‫‪230:‬‬ ‫‪User‬‬ ‫‪logged‬‬ ‫‪in,‬‬ ‫‪proceed.‬‬ ‫‪Logged‬‬ ‫‪out‬‬ ‫‪if‬‬ ‫‪appropriate.‬‬\n"
#define SUCCESSFUL_QUIT "‫‪221:‬‬ ‫‪Successful‬‬ ‫‪Quit.‬‬\n"
#define SYNTAX_ERROR "‫‪501:‬‬ ‫‪Syntax‬‬ ‫‪error‬‬ ‫‪in‬‬ ‫‪parameters‬‬ ‫‪or‬‬ ‫‪arguments.‬‬\n"
#define NEED_FOR_ACCOUNT "‫‪332:‬‬ ‫‪Need‬‬ ‫‪account‬‬ ‫‪for‬‬ ‫‪login.‬‬\n"
#define INTERNAL_SERVER_ERROR "500: Error\n"
#define LIST_TRANSFER_DONE "226: List transfer done.\n"

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
        map<int, ftp_user*> online_users;
        vector<User*> all_users;
        vector<string> split_by_space(string cmd);
        string handle_user(vector<string> args, int client_sd);
        string handle_password(vector<string> args, int client_sd);
        string handle_quit(int client_sd);
        string handle_ls(int client_sd);
        User* find_user(string user_name);
        ftp_user* create_online_user(User* user);
    public:
        Ftp_System(vector<User*> _all_users);
        Ftp_System() = default;
        vector<User*> get_all_users();
        void remove_online_user(int client_sd);
        string handle_command(char command[], int client_sd);
        bool has_user_data(int client_sd);
        string get_user_data(int client_sd);
};

#endif