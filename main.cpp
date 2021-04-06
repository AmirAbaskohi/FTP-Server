#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <string.h>
#include <filesystem> 
#include "server.h"

#define CONFIG_FILE_PATH "./config.json"

using namespace std;

int main(int argc, char* argv[])
{
    Server server = Server(argv[1]);
    
    server.run();
    
    return 0; 
}