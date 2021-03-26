#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <string.h>
#include "server.h"

#define CONFIG_FILE_PATH "./config.json"

using namespace std;

int main()
{
    Server server = Server(CONFIG_FILE_PATH);
    
    server.run();

    return 0; 
}