#include "ftp_system.h"

Ftp_System::Ftp_System(vector<User*> _all_users, vector<string> _admin_files)
{
    all_users = _all_users;
    admin_files = _admin_files;

    char cwd[1024];
    getcwd(cwd, 1024);
    string tmp(cwd);
    default_directory = tmp;
}

vector<string> Ftp_System::split(string str, char divider = ' ')
{
    stringstream ss(str);
    string word;
    vector< string> result;

    while(getline(ss, word, divider))
    {
        if(word != "")
            result.push_back(word);
    }
    return result;
}

int Ftp_System::delete_directory(string path)
{
    DIR *dir;
	struct dirent *ent;

	dir = opendir (path.c_str());

	if (dir != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (strcmp(ent -> d_name, ".") == 0 || strcmp(ent -> d_name, "..") == 0)
                continue;

            int result;

            string name(ent->d_name);
            string addr = path + "/" + name;

            if (ent->d_type != DT_DIR)
                result = remove(addr.c_str());
        	else  
                result = delete_directory(addr);

            if (result != 0)
                return result;
        }
        closedir (dir);
	}

    remove(path.c_str());

    return 0;
}

bool Ftp_System::does_file_exist(string file_name, string directory)
{
    DIR *dir;
	struct dirent *ent;

	dir = opendir (directory.c_str());

	if (dir != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_type != DT_REG)
                continue; 
        	if (strcmp(ent->d_name, file_name.c_str()) == 0)  
            {   
                closedir(dir);
                return true;
            }
        }
        closedir (dir);
	}

    return false;
}

bool Ftp_System::is_file_for_admin(string file_name)
{
    for (int i = 0 ; i < admin_files.size() ; i++)
        if (file_name == admin_files[i])
            return true;
    return false;
}

int Ftp_System::get_file_size(string file_name, string directory)
{
    ifstream in_file(directory+"/"+file_name, ios::binary);
    in_file.seekg(0, ios::end);
    int file_size = in_file.tellg();
    in_file.close();
    return file_size;
}

string Ftp_System::join(vector<string> vstr, char joiner)
{
    string result = "";
    for(int i = 0; i < vstr.size(); i++)
    {
        result += vstr[i];
        if (i < vstr.size()-1)
            result += joiner;
    }
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
    result->data = "";
    result->has_data = false;
    result->current_dir = default_directory;
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
        it->second->has_data = true;
        return LIST_TRANSFER_DONE;
	}
    else
        return INTERNAL_SERVER_ERROR;
}

string Ftp_System::handle_pwd(int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;
    
    return "257: " + it->second->current_dir + "\n";
}

string Ftp_System::handle_mkd(string path, int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;

    vector<string> vpath = split(path, '/');
    vector<string> tmp;

    string new_directory;

    if(path[0] == '/')
        new_directory = "";
    else
        new_directory = it->second->current_dir;

    for(int i = 0; i < vpath.size(); i++)
    {
        if (vpath[i] == ".") continue;
        else if (vpath[i] == "..")
        {
            tmp = split(new_directory, '/');
            if(tmp.size() > 0)
                tmp.pop_back();
            new_directory = "/" + join(tmp, '/');
        }
        else
        {
            new_directory += (new_directory.back() == '/' ? "" : "/") + vpath[i];
        }
    }

    DIR *dir = opendir(new_directory.c_str());
    if (dir != NULL)
        return INTERNAL_SERVER_ERROR;

    if ((mkdir(path.c_str(),0777) == 0))
        return "257: " + path + " created.\n";

    return INTERNAL_SERVER_ERROR;
}

string Ftp_System::handle_dele(string type, string path, int client_sd)
{
    string selected_directory;
    map<int,ftp_user*>::iterator it;
    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;

    if (type == DIRECTORY_FLAG)
    {
        vector<string> vpath = split(path, '/');
        vector<string> tmp;

        if(path[0] == '/')
            selected_directory = "";
        else
            selected_directory = it->second->current_dir;

        for(int i = 0; i < vpath.size(); i++)
        {
            if (vpath[i] == ".") continue;
            else if (vpath[i] == "..")
            {
                tmp = split(selected_directory, '/');
                if(tmp.size() > 0)
                    tmp.pop_back();
                selected_directory = "/" + join(tmp, '/');
            }
            else
            {
                selected_directory += (selected_directory.back() == '/' ? "" : "/") + vpath[i];
                DIR *dir = opendir(selected_directory.c_str());
                if (dir == NULL)
                    return INTERNAL_SERVER_ERROR;
                closedir(dir);
            }
        }
        if ((delete_directory(selected_directory)) == 0)
            return "250: " + path + " deleted.\n";
    }
    else if (type == FILE_FLAG)
    {
        if (is_file_for_admin(path) && !it->second->user_info->get_is_admin())
            return FILE_UNAVAILABLE;

        if (!does_file_exist(path, it->second->current_dir))
            return INTERNAL_SERVER_ERROR;
        
        string file_path = it->second->current_dir + "/" + path ;
        if ((remove(file_path.c_str()) == 0))
            return "250: " + path + " deleted.\n";
    }

    return INTERNAL_SERVER_ERROR;
}

string Ftp_System::handle_cwd(string path, int client_sd)
{
    map<int,ftp_user*>::iterator it;
    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;
    if (path == "")
    {
        it->second->current_dir = default_directory;
        return SUCCESSFUL_CHANGE;
    }

    vector<string> vpath = split(path, '/');
    vector<string> tmp;
    string new_directory;
    if (vpath.size() == 0)
    {
        it->second->current_dir = "/";
        return SUCCESSFUL_CHANGE;
    }

    if(path[0] == '/')
        new_directory = "";
    else
        new_directory = it->second->current_dir;
    
    for(int i = 0; i < vpath.size(); i++)
    {
        if (vpath[i] == ".") continue;
        else if (vpath[i] == ".."){
            tmp = split(new_directory, '/');
            if(tmp.size() > 0)
            {
                tmp.pop_back();
            }
            new_directory = "/" + join(tmp, '/');
        }
        else{
            new_directory += (new_directory.back() == '/' ? "" : "/") + vpath[i];
            DIR *dir = opendir(new_directory.c_str());
            if (dir == NULL)
                return INTERNAL_SERVER_ERROR;
            closedir(dir);
        }
    }
    it->second->current_dir = new_directory;
    
    return SUCCESSFUL_CHANGE;
}

string Ftp_System::handle_rename(string old_name, string new_name, int client_sd)
{
    map<int,ftp_user*>::iterator it;
    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;

    string old_address = it->second->current_dir + "/" + old_name ;
    string new_address = it->second->current_dir + "/" + new_name ;
    if (rename(old_address.c_str(), new_address.c_str()) == 0)
        return SUCCESSFUL_CHANGE;
    else
        return INTERNAL_SERVER_ERROR;
}

string Ftp_System::handle_download(string file_name, int client_sd)
{
    map<int,ftp_user*>::iterator it;
    it = online_users.find(client_sd);

    if (it == online_users.end() || !it->second->is_authorized)
        return NEED_FOR_ACCOUNT;

    if (is_file_for_admin(file_name) && !it->second->user_info->get_is_admin())
        return FILE_UNAVAILABLE;

    if (does_file_exist(file_name, it->second->current_dir))
    {
        int size = get_file_size(file_name, it->second->current_dir);
        if (size > it->second->user_info->get_size())
            return CANT_OPEN_DATA_CONNECTION;

        it->second->user_info->descrease_user_size(size);
        ifstream ifs(it->second->current_dir + "/" + file_name);
        string content((istreambuf_iterator<char>(ifs)),
                        (istreambuf_iterator<char>()));
        ifs.close();

        it->second->data = content;
        it->second->has_data = true;
        return SUCCESSFUL_DOWNLOAD;
    }

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

    vector<string> splitted_cmd = split(cmd);

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
    else if (splitted_cmd[0] == PWD_COMMAND && splitted_cmd.size() == 1)
        return handle_pwd(client_sd);
    else if (splitted_cmd[0] == MKD_COMMAND && splitted_cmd.size() == 2)
        return handle_mkd(splitted_cmd[1], client_sd);
    else if (splitted_cmd[0] == CWD_COMMAND)
    {
        if(splitted_cmd.size() == 1)
            return handle_cwd("", client_sd);
        else if (splitted_cmd.size() == 2)
            return handle_cwd(splitted_cmd[1], client_sd);
    }
    else if (splitted_cmd[0] == RENAME_COMMAND && splitted_cmd.size() == 3)
        return handle_rename(splitted_cmd[1], splitted_cmd[2], client_sd);
    else if (splitted_cmd[0] == DELETE_COMMAND && splitted_cmd.size() == 3 && (splitted_cmd[1] == FILE_FLAG || splitted_cmd[1] == DIRECTORY_FLAG))
        return handle_dele(splitted_cmd[1], splitted_cmd[2], client_sd);
    else if (splitted_cmd[0] == DOWNLOAD_COMMAND && splitted_cmd.size() == 2)
        return handle_download(splitted_cmd[1], client_sd);

    return SYNTAX_ERROR;
}

void Ftp_System::remove_online_user(int client_sd)
{
    map<int,ftp_user*>::iterator it;

    it = online_users.find(client_sd);

    if (it != online_users.end())
        online_users.erase(client_sd);
}

vector<User*> Ftp_System::get_all_users()
{
    return all_users;
}

vector<string> Ftp_System::get_admin_files()
{
    return admin_files;
}
