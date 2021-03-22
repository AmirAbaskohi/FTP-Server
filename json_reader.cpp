#include "json_reader.h"


string Json_Reader::get_json(string path){
    ifstream file;
    file.open(path, ios::in);
    string line;
    string context = "";
    while (getline(file, line))
    {
        for(int i = 0; i < line.size(); i++)
        {
            if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n'){
                context += line[i];
            }
        }
    }
    return context;
}

string Json_Reader::find_value(string json, string key)
{
    string result;
    int first_index, second_index;
    int i = 0,j;
    while(i < json.size() ){
        while(i < json.size() && json[i] != '\"') i++;
        first_index = ++i;
        while(i < json.size() && json[i] != '\"') i++;
        second_index = i;
        i += 2;
        if (json.substr(first_index, second_index - first_index) == key){
            if (json[i] == '['){
                first_index = i;
                while(i < json.size() && json[i] != ']') i++;
                second_index = i;
                result = json.substr(first_index, second_index - first_index + 1);
            }
            else if(json[i] == '\"'){
                first_index = i;
                while(i < json.size() && json[i] != '\"') i++;
                second_index = i;
                result = json.substr(first_index+1, second_index - first_index - 1);
            }
            else{
                first_index = i;
                while(i < json.size() && json[i] != ',' && json[i] != '}' ) i++;
                second_index = i;
                result = json.substr(first_index, second_index - first_index);
            }
            return result;
        }
        if (json[i] == '['){
            while(i < json.size() && json[i] != ']') i++;
            while(i < json.size() && json[i] != ',') i++;
        }
        else{
            while(i < json.size() && json[i] != ',') i++;
        }
    }
    return "";
}

vector<string> Json_Reader::split_array(string array)
{
    int i = 0;
    int first_index, second_index;
    vector<string> result;
    if (array[i] != '[') return result;
    i++;
    while(i < array.size())
    {
        while (i < array.size() && array[i] != '{' && array[i] != '\"') i++;
        if(i < array.size() && array[i] == '{')
        {
            first_index = i;
            while (i < array.size() && array[i] != '}') i++;
            second_index = ++i;
            result.push_back(array.substr(first_index, second_index - first_index));
            while (i < array.size() && array[i] != ',' && array[i] != ']') i++;
            i++;
        }
        else if (i < array.size() && array[i] == '\"')
        {
            first_index = ++i;
            while(i < array.size() && array[i] != '\"') i++;
            second_index = i;
            result.push_back(array.substr(first_index, second_index - first_index));
            while (i < array.size() && array[i] != ',' && array[i] != ']') i++;
            i++;
        }
    }
    return result;
}