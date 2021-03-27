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
        while(i < json.size() && json[i] != QOUTATION) i++;
        first_index = ++i;
        while(i < json.size() && json[i] != QOUTATION) i++;
        second_index = i;
        i += 2;
        if (json.substr(first_index, second_index - first_index) == key){
            if (json[i] == LBRACK){
                first_index = i++;
                while(i < json.size() && json[i] != RBRACK) i++;
                second_index = i++;
                result = json.substr(first_index, second_index - first_index + 1);
            }
            else if(json[i] == QOUTATION){
                first_index = i++;
                while(i < json.size() && json[i] != QOUTATION) i++;
                second_index = i++;
                result = json.substr(first_index+1, second_index - first_index - 1);
            }
            else{
                first_index = i;
                while(i < json.size() && json[i] != COMMA && json[i] != RBRACE ) i++;
                second_index = i;
                result = json.substr(first_index, second_index - first_index);
            }
            return result;
        }
        if (json[i] == LBRACK){
            while(i < json.size() && json[i] != RBRACK) i++;
            while(i < json.size() && json[i] != COMMA) i++;
        }
        else{
            while(i < json.size() && json[i] != COMMA) i++;
        }
    }
    return "";
}

vector<string> Json_Reader::split_array(string array)
{
    int i = 0;
    int first_index, second_index;
    vector<string> result;
    if (array[i] != LBRACK) return result;
    i++;
    while(i < array.size())
    {
        while (i < array.size() && array[i] != LBRACE && array[i] != QOUTATION) i++;
        if(i < array.size() && array[i] == LBRACE)
        {
            first_index = i;
            while (i < array.size() && array[i] != RBRACE) i++;
            second_index = ++i;
            result.push_back(array.substr(first_index, second_index - first_index));
            while (i < array.size() && array[i] != COMMA && array[i] != RBRACK) i++;
            i++;
        }
        else if (i < array.size() && array[i] == QOUTATION)
        {
            first_index = ++i;
            while(i < array.size() && array[i] != QOUTATION) i++;
            second_index = i;
            result.push_back(array.substr(first_index, second_index - first_index));
            while (i < array.size() && array[i] != COMMA && array[i] != RBRACK) i++;
            i++;
        }
    }
    return result;
}