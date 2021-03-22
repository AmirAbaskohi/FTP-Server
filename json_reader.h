#ifndef JSON_READER
#define JSON_READER

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Json_Reader{
    public: 
        string get_json(string path);

        string find_value(string json, string key);

        vector<string> split_array(string array);
};

#endif