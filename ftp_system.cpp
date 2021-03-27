#include "ftp_system.h"

Ftp_System::Ftp_System(vector<User*> _all_users)
{
    all_users = _all_users;
}

vector<string> Ftp_System::split_by_space(string cmd)
{
    vector<string> result;

    istringstream ss(cmd);
  
    string word;

    while (ss >> word) 
        result.push_back(word);

    return result;
}

User* Ftp_System::find_user(string user_name)
{
    for (int i = 0 ; i < all_users.size() ; i++)
    {
        if (all_users[i] -> get_user_name() == user_name)
            return all_users[i];
    }
    return NULL;
}

ftp_user* Ftp_System::create_online_user(User* user)
{
    ftp_user* result = new ftp_user();
    result->user_info = user;
    result->is_authorized = false;
    return result;
}

string Ftp_System::handle_user(vector<string> args, int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it != online_users.end())
        return BAD_SEQUENCE_OF_COMMANDS;

    User* selected_user = find_user(args[1]);
    if (selected_user == NULL)
        return INVALID_USERNAME_OR_PASSWORD;

    online_users.insert(pair<int, ftp_user*>(client_sd, create_online_user(selected_user)));
    return USERNAME_ACCEPTED;
}

string Ftp_System::handle_password(vector<string> args, int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it == online_users.end() || it->second->is_authorized == true)
        return BAD_SEQUENCE_OF_COMMANDS;

    User* selected_user = it->second->user_info;
    if (selected_user->get_password() != args[1])
        return INVALID_USERNAME_OR_PASSWORD;

    it->second->is_authorized = true;
    return PASSWORD_ACCEPTED;
}

string Ftp_System::handle_command(char command[], int client_sd)
{
    string cmd(command);

    vector<string> splitted_cmd = split_by_space(cmd);

    if (splitted_cmd[0] == USER_COMMAND)
        return handle_user(splitted_cmd, client_sd);
    else if (splitted_cmd[0] == PASS_COMMAND)
        return handle_password(splitted_cmd, client_sd);
    
    return "";
}

vector<User*> Ftp_System::get_all_users()
{
    return all_users;
}