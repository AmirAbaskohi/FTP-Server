#ifndef USER
#define USER

#include <string>

using namespace std;

class User{
    private:
        string user_name;
        string password;
        int size;
        bool is_admin;
    public:
        User(string _user_name, string _password, int _size, bool _is_admin);
        string get_user_name();
        string get_password();
        int get_size();
        bool get_is_admin();
};

#endif