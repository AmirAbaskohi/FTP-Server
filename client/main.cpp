#include "client.h"

#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;


int main(int argc, char const *argv[])
{
    Client client("../config.json");
    
    client.run();
    
    return 0;
}