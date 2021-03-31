#include "server.h"

Server::Server(string config_file_path)
{
    Json_Reader json_reader;
    string config = json_reader.get_json(config_file_path);
    system = Ftp_System(get_config_users(config), get_config_files(config));
    set_ports(config);
}

vector<string> Server::split_to_packets(string data, int packet_size)
{
    vector<string> packets;
    for(int i=0; i < data.size(); i += packet_size){
        packets.push_back(data.substr(i, packet_size));
    }
    return packets;
}

vector<User*> Server::get_config_users(string config)
{
    vector<User*> result;

    Json_Reader json_reader;
    vector<string> users_json_objects = json_reader.split_array(json_reader.find_value(config, USERS_KEY));

    for(int i = 0 ; i < users_json_objects.size() ; i++)
    {
        string user_name = json_reader.find_value(users_json_objects[i], USERNAME_KEY);
        string password = json_reader.find_value(users_json_objects[i], PASSWORD_KEY);
        string admin = json_reader.find_value(users_json_objects[i], ADMIN_KEY);
        string size = json_reader.find_value(users_json_objects[i], SIZE_KEY);

        result.push_back(new User(user_name, password, stoi(size), admin == "true" ? true : false));
    }

    return result;
}

void Server::set_ports(string config)
{
    Json_Reader json_reader;

    command_channel_port = stoi(json_reader.find_value(config, COMMAND_CHANNEL_PORT_KEY));
    data_channel_port = stoi(json_reader.find_value(config, DATA_CHANNEL_PORT_KEY));
}

vector<string> Server::get_config_files(string config)
{
    vector<string> result;

    Json_Reader json_reader;
    vector<string> file_names = json_reader.split_array(json_reader.find_value(config, FILES_KEY));

    for(int i = 0 ; i < file_names.size() ; i++)
        result.push_back(file_names[i]);

    return result;
}

void Server::print_server_info()
{
    vector<User*> users = system.get_all_users();
    vector<string> files = system.get_admin_files();

    cout << "Command Port: " << command_channel_port << endl;
    cout << "Data Port: " << data_channel_port << endl;

    cout << "Users:" << endl;
    cout << "Username\tPassword\tSize\t\tIsAdmin" << endl;
    cout << "----------------------------------------------------------------------\n";
    for(int i = 0 ; i < users.size() ; i++)
        cout << users[i] -> get_user_name() << "\t\t" << users[i] -> get_password() << "\t  \t" 
             << users[i] -> get_size() << "\t\t" << users[i] -> get_is_admin() << endl; 

    cout << "Admin Files:" << endl;
    cout << "----------------------------------------------------------------------\n";
    for(int i = 0 ; i < files.size() ; i++)
        cout << files[i] << endl; 
}

void Server::bind_sockets()
{
    struct sockaddr_in command_address, data_address;

    command_address.sin_family = AF_INET;
    command_address.sin_addr.s_addr = INADDR_ANY;
    command_address.sin_port = htons(command_channel_port);
    if (bind(command_socket, (struct sockaddr*)&command_address, sizeof(command_address)) < 0)
    {
        cout << "Binding command socket failed!" << endl;
        exit(EXIT_FAILURE);
    }

    data_address.sin_family = AF_INET;
    data_address.sin_addr.s_addr = INADDR_ANY;
    data_address.sin_port = htons(data_channel_port);
    if (bind(data_socket, (struct sockaddr*)&data_address, sizeof(data_address)) < 0)
    {
        cout << "Binding command socket failed!" << endl;
        exit(EXIT_FAILURE);
    }
}

void Server::make_sockets_listen()
{
    if (listen(command_socket, 3) < 0)  
    {  
        cout << "Listening command socket failed!" << endl;  
        exit(EXIT_FAILURE);  
    }  

    if (listen(data_socket, 3) < 0)  
    {  
        cout << "Listening data socket failed!" << endl;
        exit(EXIT_FAILURE);  
    }  
}

void Server::create_sockets()
{
    int opt = 1;

    command_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (command_socket == 0)
    {
        cout << "Command socket creation failed!" << endl;
        exit(EXIT_FAILURE);
    }

    data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket == 0)
    {
        cout << "Data socket creation failed!" << endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(command_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        cout << "Setsockopt command socket error!" << endl;  
        exit(EXIT_FAILURE);  
    } 

    if (setsockopt(data_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        cout << "Setsockopt data socket error!" << endl;  
        exit(EXIT_FAILURE);  
    } 

    bind_sockets();

    make_sockets_listen();
    
}

void Server::set_sockets(int& max_socket_descriptor, fd_set& readfds)
{
    FD_ZERO(&readfds);  
      
    FD_SET(command_socket, &readfds);
    FD_SET(data_socket, &readfds);

    max_socket_descriptor = command_socket > data_socket ? command_socket : data_socket;  
            
    for ( int i = 0 ; i < clients.size() ; i++)  
    {  
        int sd = clients[i];  

        if (sd > 0)  
            FD_SET( sd , &readfds);  
                
        if (sd > max_socket_descriptor)  
            max_socket_descriptor = sd;  
    }  
}

void Server::accept_connections()
{
    struct sockaddr_in address_command, address_data;
    int addrlen_data = sizeof(address_data);
    int addrlen_command = sizeof(address_command);

    int new_socket_data = accept(data_socket, (struct sockaddr *)&address_data, (socklen_t*)&addrlen_data);
    if (new_socket_data < 0)  
    {  
        cout << "Accept error for data socket!" << endl;  
        exit(EXIT_FAILURE);  
    }
    int new_socket_command = accept(command_socket, (struct sockaddr *)&address_command, (socklen_t*)&addrlen_command);
    if (new_socket_command < 0)  
    {  
        cout << "Accept error for command socket!" << endl;  
        exit(EXIT_FAILURE);  
    }

    cout << "New connection for command socket, socket fd is " << new_socket_command
            << ", ip is : " << inet_ntoa(address_command.sin_addr) <<  ", port : "
            << ntohs(address_command.sin_port) << endl; 

    cout << "New connection for data socket, socket fd is " << new_socket_data
            << ", ip is : " << inet_ntoa(address_data.sin_addr) <<  ", port : "
            << ntohs(address_data.sin_port) << endl; 

    if (send(new_socket_command, "Welcome To FTP\n", strlen("Welcome To FTP\n"), 0) != strlen("Welcome To FTP\n") )  
        cout << "send failed" << endl; 

    bool is_set = false;
    for (int i = 0; i < clients.size(); i++)  
    {  
        if ( clients[i] == 0 )  
        {  
            clients[i] = new_socket_command;
            is_set = true;
            break;
        }  
    }  
    if (!is_set)
        clients.push_back(new_socket_command);

    clients_command_data.insert(pair<int, int>(new_socket_command, new_socket_data));
}

void Server::handle_clients_requests(fd_set& readfds)
{
    char buffer[1025];
    int valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    for (int i = 0; i < clients.size(); i++)  
    {  
        int sd = clients[i];  
            
        if (FD_ISSET(sd , &readfds))  
        {  
            if ((valread = read(sd , buffer, 1024)) == 0)  
            {  
                getpeername(sd , (struct sockaddr*)&address , \
                    (socklen_t*)&addrlen);  

                cout << "Guest disconnected , ip " << inet_ntoa(address.sin_addr)
                        << " , port " << ntohs(address.sin_port) << endl;
                
                system.remove_online_user(sd);
                close(sd);
                close(clients_command_data[sd]);
                clients[i] = 0;  
            }  
            else 
            {   
                buffer[valread] = '\0';
                string response = system.handle_command(buffer, sd);
                if (system.has_user_data(sd))
                {
                    vector<string> packets = split_to_packets(system.get_user_data(sd), PACKET_SIZE);
                    send(sd ,DATA_SENDING_MESSAGE ,strlen(DATA_SENDING_MESSAGE) , 0);
                    usleep(100);
                    string packet_size = to_string(packets.size());
                    send(sd ,packet_size.c_str() ,strlen(packet_size.c_str()) , 0);
                    usleep(100);
                    for(int i = 0; i < packets.size(); i++)
                    {
                        send(clients_command_data[sd], packets[i].c_str(), strlen(packets[i].c_str()),0);
                        usleep(100);
                    }
                }
                else
                {
                    send(sd ,NO_DATA ,strlen(NO_DATA) , 0);
                    usleep(100);
                }
                send(sd ,response.c_str() ,strlen(response.c_str()) , 0);
                usleep(100);
            }  
        }  
    }
}

void Server::run()
{
    create_sockets();

    fd_set readfds;
    int max_socket_descriptor, activity;

    while(true)  
    {  
        set_sockets(max_socket_descriptor, readfds);

        activity = select( max_socket_descriptor + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
            cout << "Select error!" << endl;  
             
        if (FD_ISSET(command_socket, &readfds) && FD_ISSET(data_socket, &readfds))
            accept_connections();

        handle_clients_requests(readfds);
    }
}