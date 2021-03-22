#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "json_reader.h"

using namespace std;

int main(){
    Json_Reader a;
    cout << a.get_json("./config.json")<<endl;
    return 0;
}