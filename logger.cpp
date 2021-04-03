#include "logger.h"

void write_log(string message)
{
    ofstream ofs;
    ofs.open (LOG_FILE_NAME, ofstream::out | ofstream::app);

    ofs << return_current_time_and_date() + " :: " + message + "\n";

    ofs.close();
}

string return_current_time_and_date()
{
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}