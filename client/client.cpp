#include "client.h"

Client::Client(string config_file_path){
    Json_Reader json_reader;
    string config = json_reader.get_json(config_file_path);
    set_ports(config);
}

void Client::set_ports(string config)
{
    Json_Reader json_reader;

    command_channel_port = stoi(json_reader.find_value(config, COMMAND_CHANNEL_PORT_KEY));
    data_channel_port = stoi(json_reader.find_value(config, DATA_CHANNEL_PORT_KEY));
}

vector<string> Client::split(string str, char divider)
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

int Client::connect_on_port(int port){
    int sock, opt = 1;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "\n Socket creation error \n";
        return -1;
    } 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        cout << "Setsockopt command socket error!" << endl;  
        exit(EXIT_FAILURE);  
    } 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Connection Failed" << endl;
        return -1;
    }
    return sock;
}

void Client::run(){
    struct stat st = {0};

    if (stat(FTP_DOWNLOAD_DIR_NAME, &st) == -1)
        mkdir(FTP_DOWNLOAD_DIR_NAME, 0700);

    int data_socket = connect_on_port(data_channel_port);
    int command_socket = connect_on_port(command_channel_port);
    if (data_socket <= 0 || command_socket <= 0) return;

    char response[PACKET_SIZE] = {0};
    string request;
    string data;

    string user_name = "";
    bool is_logged_in = false;

    int valread = read(command_socket , response, PACKET_SIZE);
    cout << response << endl; 
    while(1){

        cout << BOLDGREEN << "FTP" << RESET;
        if (user_name != "" && is_logged_in)
            cout << BOLDCYAN << "[" << user_name << "]" << RESET;
        else if(user_name != "" && !is_logged_in)
            cout << BOLDRED << "[" << user_name << "]" << RESET;
        cout << BOLDGREEN << "> " << RESET;
        getline(cin, request);

        vector<string> request_params = split(request);
        bool is_download_req = request_params.size() > 0 && request_params[0] == DOWNLOAD_COMMAND;
        bool is_user_req = request_params.size() > 0 && request_params[0] == USER_COMMAND;
        bool is_pass_req = request_params.size() > 0 && request_params[0] == PASS_COMMAND;
        bool is_quit_req = request_params.size() > 0 && request_params[0] == QUIT_COMMAND;

        send(command_socket , request.c_str() , strlen(request.c_str()) , 0 );

        memset(response, 0, strlen(response));
        valread = read(command_socket, response, PACKET_SIZE);

        if (strcmp(response, DATA_SENDING_MESSAGE) == 0)
        {
            data = "";

            memset(response, 0, strlen(response));
            valread = read(command_socket, response, PACKET_SIZE);
            int packet_size = atoi(response);

            float progress = 0.0;
            float step = 1.0 / (float)packet_size;
            int barWidth = 70;
            for(int i = 0; i < packet_size; i++)
            {
                memset(response, 0, strlen(response));
                valread = read(data_socket, response, PACKET_SIZE);
                string tmp = response;
                data += tmp;
                progress += step;
                if (i == packet_size - 1 && progress != 1)
                    progress = 1.0;
                if (is_download_req)
                {
                    cout << "[";
                    int pos = barWidth * progress;
                    for (int i = 0; i < barWidth; ++i) {
                        if (i < pos) cout << "=";
                        else if (i == pos) cout << ">";
                        else cout << " ";
                    }
                    cout << "] " << int(progress * 100.0) << " %\r";
                    cout.flush();
                }
            }
            if (is_download_req)
            {
                cout << endl;
                ofstream MyFile(string(FTP_DOWNLOAD_DIR_NAME) + "/" + request_params[1]);
                MyFile << data;
                MyFile.close();
            }
            else
                cout << data << endl;
        }

        memset(response, 0, strlen(response));
        valread = read(command_socket , response, PACKET_SIZE);
        cout << response << endl;

        if (is_user_req)
        {
            vector<string> response_splitted = split(response, ':');
            user_name = stoi(response_splitted[0]) == SUCCESSFUL_USER ? request_params[1] : user_name;
        }
        else if (is_pass_req)
        {
            vector<string> response_splitted = split(response, ':');
            is_logged_in = stoi(response_splitted[0]) == SUCCESSFUL_PASS ? true : false;
        }
        else if (is_quit_req)
        {
            vector<string> response_splitted = split(response, ':');
            if (stoi(response_splitted[0]) == SUCCESSFUL_QUIT)
            {
                user_name = "";
                is_logged_in = false;
            }
        }
    }
}

