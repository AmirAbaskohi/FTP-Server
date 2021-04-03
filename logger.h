#ifndef LOGGER
#define LOGGER

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>

#define LOG_FILE_NAME "log.txt"

using namespace std;

void write_log(string message);

string return_current_time_and_date();

#endif