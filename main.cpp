#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <string.h>
#include <filesystem> 
#include "server.h"

#define CONFIG_FILE_PATH "./config.json"

using namespace std;

int main()
{
    Server server = Server(CONFIG_FILE_PATH);
    
    server.run();

    // cout << rename("/home/arash/Desktop/test.txt", "/home/arash/Desktop/aaaaaaaaaaaaaTest.txt");

    return 0; 
}