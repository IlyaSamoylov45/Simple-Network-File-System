#define FUSE_USE_VERSION 26
#include "clientSNFS.h"



//open todo
extern "C" int fuse_fs_open(struct fuse_fs *fs, const char *path, struct fuse_file_info *fi)
{
	return 0;
}


//write TODO	
extern "C" int fuse_write(const char *pathStr, const char *data, size_t size, off_t offset, struct fuse_file_info *)
{
	return 0;
}

//create TODO
extern "C" int fuse_create(const char* pathStr, mode_t mode, struct fuse_file_info *fileInfo)
{
	return 0;
}

//OPENDIR TODO
extern "C" int fuse_opendir(const char *, struct fuse_file_info *)
{
	return 0;
}

//open TODO
extern "C" int fuse_open(const char *pathStr, struct fuse_file_info *)
{
	return 0;
}


// flush TODO
extern "C" int fuse_flush(const char *pathStr, struct fuse_file_info *)
{
	return 0;
}


//truncate TODO
extern "C" int fuse_truncate(const char *pathStr, off_t length)
{
	return 0;
}



// readir TODO
extern "C" int fuse_readdir(const char* pathStr, void* buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	return 0;
}

extern "C" int fuse_release(const char *pathStr, struct fuse_file_info *)
{
	return 0;
}

// mkdir
// TODO
extern "C" int fuse_mkdir(const char* pathStr, mode_t mode)
{
	return 0;
}

// getattr
// TODO
extern "C" int fuse_getattr(const char* pathStr, struct stat* info)
{
	return 0;

}

//assign functions in main
static struct fuse_operations fuse_oper = {
		0,
};

int main(int argc, char* argv[]){
	if(argc != 7){
		cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
		exit(EXIT_FAILURE);	
	}
	check_values(argv[1], "-serverport");
	check_port(argv[2], "port#");
	int serverPort = stoi(argv[2]);
	check_values(argv[3], "-serveraddress");	
	const char* server = argv[4];
	cout << server << endl;
	check_values(argv[5], "-mount");
	//get path of mountable file
	path curr_path = current_path(); //gets current path
	path p = argv[6];
	path client_directory = curr_path / p;
	check_directory(client_directory);
	
	fuse_oper.open = fuse_open;
	fuse_oper.create = fuse_create;
	fuse_oper.release = fuse_release;
	fuse_oper.getattr = fuse_getattr;
	fuse_oper.readdir = fuse_readdir;
	fuse_oper.truncate = fuse_truncate;
	fuse_oper.flush = fuse_flush;
	fuse_oper.opendir = fuse_opendir;
	fuse_oper.write = fuse_write;
	fuse_oper.mkdir = fuse_mkdir;
	fuse_oper.getattr = fuse_getattr;



	struct sockaddr_in serverAddress;
	struct hostent *hostp;
	int clientSocket;
	int rc;
	socklen_t addr_size;
	char buffer[1024];
	char client_msg[5000];
	
	if((clientSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0){
  		cout << "Client socket error" << endl;
		exit(EXIT_FAILURE);
	}
	else{
		cout << "Client socket successful." << endl;
	}
	
	// Configure settings of the server address struct
	serverAddress.sin_family = AF_INET; // Address = Internet
	serverAddress.sin_port = htons(serverPort); // Set port Number
	
	//Set IP to serverIP
	serverAddress.sin_addr.s_addr = inet_addr(server);
	
	if((serverAddress.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE)	{
		hostp = gethostbyname(server);
        if(hostp == (struct hostent *)NULL){
            printf("HOST NOT FOUND --> ");
            /* h_errno is usually defined */
            /* in netdb.h */
            printf("h_errno = %d\n",h_errno);
            printf("---This is a client program---\n");
            printf("Command usage: %s <server name or IP>\n", argv[0]);
            close(clientSocket);
            exit(-1);
        }
		memcpy(&serverAddress.sin_addr, hostp->h_addr, sizeof(serverAddress.sin_addr)); 
	}
	
	//Set bits of the padding field to 0 
	memset(serverAddress.sin_zero, '\0', sizeof(serverAddress.sin_zero));
	
	//Connect the socket to the server using the address
   addr_size = sizeof(serverAddress);
	
	if((rc = connect(clientSocket, (struct sockaddr *)&serverAddress, addr_size)) < 0)
    {
        cout << "Client-connect() error" << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    else
        cout << "Client-connect() established" << endl;
        
	strcpy(client_msg,"TEST");
	//Send msg to server
	if( send(clientSocket , client_msg , strlen(client_msg) , 0) < 0){
		cout << "Client Send failed\n" << endl;
   }
   
   //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, sizeof(buffer), 0) < 0){
       cout << "Receive failed\n" << endl;
    }
    
    //Print the received message
    cout << "Data received: " << buffer << endl;
    close(clientSocket);
    
}

//checks to see if two values are the same
void check_values(string value, string correct){
  if(value.compare(correct) != 0){
    cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
    cout << "Error was in the argument " << correct << endl;
    cout << "You wrote: " << value << "\n" << endl;
    exit(EXIT_FAILURE);
  }
}

//checks to see if the port is a valid port number and whether the input is an integer
void check_port(string number, string correct){
	try {
    int test = stoi(number);
    if(test < 1024 || test > 65535){
      cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
      cout << "Error was in the argument " << correct << endl;
      cout << "Not a nonnegative digit greater than or equal to 0 and less than 65535! You wrote: " << number << endl;
      cout << "Try something like 12345" << endl;
      exit(EXIT_FAILURE);
    }
    else{
    	cout << "\nValid port number" << endl;
    }
  }
  catch(std::exception const & e){
    cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
    cout << "Error was in the argument " << correct << endl;
    cout << "Not a nonnegative digit greater than or equal to 0 and less than 65535! You wrote: " << number << endl;
    cout << "Try something like 12345" << endl;
    exit(EXIT_FAILURE);
  }
}

//check to see whether the directory exists, if it doesnt then create it,
void check_directory(path directory){
	path p = directory;
	//cout << p;
	if(exists(p)){                   //checks to see whether path p exists
		if (is_regular_file(p)){      //checks to see whether this is a file
			cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
			cout << "Error was in the argument directory" << endl;
			cout << p << " is a file not a directory" << endl;
			exit(EXIT_FAILURE);
		}       
		else if (is_directory(p)){    // checks if p is directory?
      	cout << p << " is a valid directory" << endl;
    	}
    	else{
    		cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
			cout << "Error was in the argument directory" << endl;
			cout << p << " is neither a file nor a directory" << endl;
			exit(EXIT_FAILURE);
    	}
	}
	
	//the given path does not exist so we will create
	else{
		cout << "Given directory does not exist would you like to create it? Please Type Y for yes, N for no" << endl;
		string val;
		cin >> val;
		while(val.compare("Y") != 0 && val.compare("N") != 0){
			cout << "Please Type Y for yes, N for no" << endl;		
			cin >> val;
		}
		if(val.compare("Y") == 0){
			cout << "Creating Directory" << endl;
			try{	
				create_directories(p);
				cout << "Created Directory" << endl;
			}
			catch(std::exception const & e){
				cout << "Failure creating Directory" << endl;
				exit(EXIT_FAILURE);			
			}	
		}
		else{
			cout << "Typed N exiting program, Directory will not be created." << endl;	
			exit(EXIT_FAILURE);	
		}
	}
	
}
