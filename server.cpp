#include "server.h"

Server::Server(string config_file_path)
{
    Json_Reader json_reader;
    string config = json_reader.get_json(config_file_path);
    system = Ftp_System(get_config_users(config));
    set_ports(config);
    set_files(config);
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

void Server::set_files(string config)
{
    Json_Reader json_reader;
    vector<string> file_names = json_reader.split_array(json_reader.find_value(config, FILES_KEY));

    for(int i = 0 ; i < file_names.size() ; i++)
        files.push_back(file_names[i]);
}

void Server::print_server_info()
{
    vector<User*> users = system.get_all_users();

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

void Server::create_sockets()
{
    struct sockaddr_in command_address, data_address;
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

    if (setsockopt(command_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        cout << "Setsockopt command socket error!" << endl;  
        exit(EXIT_FAILURE);  
    } 

    if (setsockopt(data_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        cout << "Setsockopt data socket error!" << endl;  
        exit(EXIT_FAILURE);  
    } 


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

void Server::run()
{
    create_sockets();

    fd_set readfds;
    char buffer[1025];
    int max_socket_descriptor, activity, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while(true)  
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
     
        activity = select( max_socket_descriptor + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
            cout << "Select error!" << endl;  
             

        if (FD_ISSET(command_socket, &readfds))  
        {  
            int new_socket = accept(command_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);

            if (new_socket < 0)  
            {  
                cout << "Accept error for command socket!" << endl;  
                exit(EXIT_FAILURE);  
            }  
             
            cout << "New connection for command socket, socket fd is " << new_socket
                 << ", ip is : " << inet_ntoa(address.sin_addr) <<  ", port : "
                 << ntohs(address.sin_port) << endl;  
           
            if( send(new_socket, "hello motherfucker\n", strlen("hello motherfucker\n"), 0) != strlen("hello motherfucker\n") )  
                cout << "send failed" << endl;  
            
            bool is_set = false;
            for (int i = 0; i < clients.size(); i++)  
            {  
                if ( clients[i] == 0 )  
                {  
                    clients[i] = new_socket;
                    is_set = true;
                    break;
                }  
            }  
            if (!is_set)
                clients.push_back(new_socket);
        } 

        for (int i = 0; i < clients.size(); i++)  
        {  
            int sd = clients[i];  
             
            if (FD_ISSET(sd , &readfds))  
            {  
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    cout << "Guest disconnected , ip " << inet_ntoa(address.sin_addr)
                         << " , port " << ntohs(address.sin_port) << endl;
                    
                    system.remover_online_user(sd);
                    close(sd);  
                    clients[i] = 0;  
                }  
                else 
                {   
                    buffer[valread-2] = '\0';
                    string response = system.handle_command(buffer, sd);
                    send(sd ,response.c_str() ,strlen(response.c_str()) , 0);
                }  
            }  
        }  
    }
}