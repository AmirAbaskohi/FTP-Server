#ifndef FTP_SYSTEM
#define FTP_SYSTEM

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
#include "user.h"

#define USER_COMMAND "user"

#define BAD_SEQUENCE_OF_COMMANDS "‫‪503:‬‬ ‫‪Bad‬‬ ‫‪sequence‬‬ ‫‪of‬‬ ‫‪commands.‬‬\n"
#define INVALID_USERNAME_OR_PASSWORD "‫‪430:‬‬ ‫‪Invalid‬‬ ‫‪username‬‬ ‫‪or‬‬ ‫‪password‬‬\n"
#define USERNAME_ACCEPTED "‫‪331:‬‬ ‫‪User‬‬ ‫‪name‬‬ ‫‪okay,‬‬ ‫‪need‬‬ ‫‪password.‬\n‬"

using namespace std;

struct ftp_user
{
    User* user_info;
    bool is_authorized;
};


class Ftp_System{
    private:
        map<int, ftp_user*> online_users;
        vector<User*> all_users;
        vector<string> split_by_space(string cmd);
        string handle_user(vector<string> args, int client_sd);
        User* find_user(string user_name);
        ftp_user* create_online_user(User* user);
    public:
        Ftp_System(vector<User*> _all_users);
        Ftp_System() = default;
        vector<User*> get_all_users();
        string handle_command(char command[], int client_sd);
};

#endif