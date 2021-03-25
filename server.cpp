#include "server.h"

Server::Server(string config_file_path)
{
    Json_Reader json_reader;
    string config = json_reader.get_json(config_file_path);
    set_users(config);
    set_ports(config);
    set_files(config);
}

void Server::set_users(string config)
{
    Json_Reader json_reader;
    vector<string> users_json_objects = json_reader.split_array(json_reader.find_value(config, USERS_KEY));

    for(int i = 0 ; i < users_json_objects.size() ; i++)
    {
        string user_name = json_reader.find_value(users_json_objects[i], USERNAME_KEY);
        string password = json_reader.find_value(users_json_objects[i], PASSWORD_KEY);
        string admin = json_reader.find_value(users_json_objects[i], ADMIN_KEY);
        string size = json_reader.find_value(users_json_objects[i], SIZE_KEY);

        user_name = user_name.substr(0, user_name.find("\""));
        password = password.substr(0, password.find("\""));
        admin = admin.substr(0, admin.find("\""));
        size = size.substr(0, size.find("\""));

        users.push_back(new User(user_name, password, stoi(size), admin == "true" ? true : false));
    }
}

void Server::set_ports(string config)
{
    Json_Reader json_reader;

    command_channel_port = stoi(json_reader.find_value(config, COMMAND_CHANNEL_PORT_KEY));
    data_channel_port = stoi(json_reader.find_value(config, DATA_CHANNEL_PORT_KEY));
}

void Server::set_files(string config)
{
    Json_Reader json_reader;
    vector<string> file_names = json_reader.split_array(json_reader.find_value(config, FILES_KEY));

    for(int i = 0 ; i < file_names.size() ; i++)
        files.push_back(file_names[i]);
}

void Server::print_server_info()
{
    cout << "Command Port: " << command_channel_port << endl;
    cout << "Data Port: " << data_channel_port << endl;

    cout << "Users:" << endl;
    cout << "Username\tPassword\tSize\t\tIsAdmin" << endl;
    cout << "----------------------------------------------------------------------\n";
    for(int i = 0 ; i < users.size() ; i++)
        cout << users[i] -> get_user_name() << "\t\t" << users[i] -> get_password() << "\t  \t" 
             << users[i] -> get_size() << "\t\t" << users[i] -> get_is_admin() << endl; 

    cout << "Files:" << endl;
    cout << "----------------------------------------------------------------------\n";
    for(int i = 0 ; i < files.size() ; i++)
        cout << files[i] << endl; 
}

int Server::get_command_channel_port()
{
    return command_channel_port;
}

int Server::get_data_channel_port()
{
    return data_channel_port;
}