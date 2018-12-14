#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <iterator>
#include <algorithm>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <sys/socket.h>
#include <dirent.h> 
#include <fstream>
#include <sstream>
#include <sys/types.h>

using namespace std;
using namespace boost::filesystem;

void * socketThread(void *);
void check_values(string, string);
void check_port(string, string);
void check_directory(path);
int make_directory(string);
string getDirectories(string);
int createFile(string);
bool is_digits(string);