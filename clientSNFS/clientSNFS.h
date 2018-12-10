#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <sys/socket.h>
#include <fuse.h>

using namespace std;
using namespace boost::filesystem;

void * clientThread(void *);
void check_values(string, string);
void check_port(string, string);
void check_directory(path);