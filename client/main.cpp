#include "client.h"

#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;


int main(int argc, char* argv[])
{
    Client client(argv[1]);
    
    client.run();
    
    return 0;
}