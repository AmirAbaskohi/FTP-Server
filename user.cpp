#include "user.h"

User::User(string _user_name, string _password, int _size, bool _is_admin)
{
    user_name = _user_name;
    password = _password;
    size = _size;
    is_admin = _is_admin;
}

string User::get_user_name()
{
    return user_name;
}

string User::get_password()
{
    return password;
}

int User::get_size()
{
    return size;
}

bool User::get_is_admin()
{
    return is_admin;
}

void User::descrease_user_size(int decration_size)
{
    size -= decration_size;
}