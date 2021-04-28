# FTP Server

This is a simple implementation of FTP Server in C++ language using socket programming.

## How to run?
First of all to compile and run this code you need to use `Unix` operating system(like linux or MacOS) because it is using `Unix` libraries.

To compile server run below command in root directory:
```
make
```
And to compile client run below command in client directory:
```
make
```

Now in their directory run `ftp.out` and 'client.out'. You should run server before running client.

## Commands

Before introducing commands, remember that there is a `config.json` file which contains users and ports. You can change tham if you want.

These are the valid commands:

* `user <username>` command to start logging
* `pass <password>` to enter password for entered username
* `pwd` to see current directory
* `cwd <path>` to change directory
* `mkd <directory pasth>` to make new directory
* `dele -f <file name>` to delete a file
* `dele -d <directroy path>` to delete a directory
* `ls` to see files in current directory
* `rename <last name> <new name>` to change file name in current directory
* `retr <filename>` to download a file
* `help` for getting commands help
* `quit` to sign out

Remeber that the files that you download will be stored in client directoy in `FTP Download` folder.

*Made By Amirhossein Abaskohi and Arash rasouli*
