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
    result->current_dir = "./";
    result->data = "";
    result->has_data = false;
    return result;
}

bool Ftp_System::has_user_data(int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it != online_users.end())
        return it->second->has_data;

    return false;
}

string Ftp_System::get_user_data(int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it != online_users.end())
        return it->second->data;

    return "";
}

string Ftp_System::handle_user(vector<string> args, int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it != online_users.end() && it->second->is_authorized)
        return BAD_SEQUENCE_OF_COMMANDS;

    if (it != online_users.end())
        remove_online_user(client_sd);

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

string Ftp_System::handle_quit(int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it == online_users.end())
        return NEED_FOR_ACCOUNT;

    remove_online_user(client_sd);
    return SUCCESSFUL_QUIT;
}

string Ftp_System::handle_ls(int client_sd)
{
    DIR *dir;
	struct dirent *ent;
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;

    it->second->has_data = true;
    it->second->data = "";

    dir = opendir (it->second->current_dir.c_str());

	if (dir != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (strcmp(ent -> d_name, ".") != 0 && strcmp(ent -> d_name, "..") != 0)
                it->second->data += string(ent->d_name) + "\n";
        }
        closedir (dir);
        return LIST_TRANSFER_DONE;
	}
    else
        return INTERNAL_SERVER_ERROR;
}

string Ftp_System::handle_command(char command[], int client_sd)
{
    map<int,ftp_user*>::iterator it;
    it = online_users.find(client_sd);
    if (it != online_users.end())
    {
        it->second->has_data = false;
        it->second->data = "";
    }

    string cmd(command);

    vector<string> splitted_cmd = split_by_space(cmd);

    if (splitted_cmd.size() == 0)
        return SYNTAX_ERROR;

    if (splitted_cmd[0] == USER_COMMAND && splitted_cmd.size() == 2)
        return handle_user(splitted_cmd, client_sd);
    else if (splitted_cmd[0] == PASS_COMMAND && splitted_cmd.size() == 2)
        return handle_password(splitted_cmd, client_sd);
    else if (splitted_cmd[0] == QUIT_COMMAND && splitted_cmd.size() == 1)
        return handle_quit(client_sd);
    else if (splitted_cmd[0] == LS_COMMAND && splitted_cmd.size() == 1)
        return handle_ls(client_sd);

    return SYNTAX_ERROR;
}

vector<User*> Ftp_System::get_all_users()
{
    return all_users;
}

void Ftp_System::remove_online_user(int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it != online_users.end())
        online_users.erase(client_sd);
}