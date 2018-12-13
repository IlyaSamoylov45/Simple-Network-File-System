#include "serverSNFS.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
path server_directory;

int main(int argc, char* argv[]){
	if(argc != 5){
		cout << "Usage is ./serverSNFS -port port# -mount directory" << endl;
		exit(EXIT_FAILURE);	
	}

	check_values(argv[1], "-port");
	check_port(argv[2], "port#");
	int serverPort = stoi(argv[2]);
	check_values(argv[3], "-mount");
	
	//get path of mountable file
	path curr_path = current_path(); //gets current path
	path p = argv[4];
	server_directory = curr_path / p;
	check_directory(server_directory);
	
	//Create variables 
	int serverSocket;
	int newSocket;
	int rc;
	struct sockaddr_in serverAddress;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;	
	
	//Create socket. 
  	if((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0){
  		cout << "Server socket error" << endl;
		exit(EXIT_FAILURE);
	}
	else{
		cout << "Server socket successful." << endl;
	}
	
	// Configure settings of the server address struct
	serverAddress.sin_family = AF_INET; // Address = Internet
	serverAddress.sin_port = htons(serverPort); // Set port Number
	
	//Set IP to localhost 127.0.0.1 is the local host
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Set bits of the padding field to 0 
	memset(serverAddress.sin_zero, '\0', sizeof(serverAddress.sin_zero));
	
	//Bind to the socket
	if ((rc = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress))) < 0){
		cout << "Server bind error" << endl;
		exit(EXIT_FAILURE);	
	}
	else{
		cout << "Server bind success" << endl;
	}
	
	//Listen on the socket, max of 20 connections to be queued
	if ((rc = listen(serverSocket, 20)) < 0) {
        cout << "Server-listen() error" << endl;
        close(serverSocket);
        exit(EXIT_FAILURE);	
	}
	else{
		cout << "Server-Ready for client connection..." << endl;
	}
	
	//Make threads and accept calls to create new sockets for incoming connections
	pthread_t tid[50];
	int i = 0;
	while(1){
		addr_size = sizeof(serverStorage);
		if((newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size)) < 0){
			cout << "Server-accept() error" << endl;
			close(serverSocket);
        	exit(EXIT_FAILURE);	
		}
		else{
			cout << "Server-accept() worked" << endl;
		}
		if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0){
				cout << "Failed to create new thread" << endl;
		}
		if(i >= 20){
			i = 0;
			while(i < 20){
				pthread_join(tid[i++],NULL);
			}
			i = 0;
		}
	
	}
}


void * socketThread(void *arg){
	char server_msg[150];
	char client_msg[150];
	int newSocket = *((int *)arg);
	
	while(1){
		memset(client_msg, 0, sizeof(client_msg));
		memset(server_msg, 0, sizeof(server_msg));
  		if(recv(newSocket , client_msg , sizeof(client_msg) , 0) <= 0){
  			cout << "Receive from client failed\n" << endl;
			break;
  		}
  		if(strncmp(client_msg, "readdir", 7) == 0){
  			cout << "Returning all filenames from the given directory" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			
  			if(client_msg[8] != '/' || client_msg[9] == '\0' || client_msg[9] == ' '){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be readdir /test");
  				
			}
			else{
				string readdir;
				int i = 8;
				while(client_msg[i] != '\0'){
					readdir += client_msg[i];
					i++;			
				}
				string getNames = getDirectories(readdir);
				const char * fileNames = getNames.c_str();
				strcpy(server_msg, fileNames);
			}
			
  		}
  		else if (strncmp(client_msg, "read", 4) == 0) {
  			cout << "Reading bytes from file" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  		}
  		else if(strncmp(client_msg, "write", 5) == 0){
  			cout << "Writing data to the file" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  		}
  		else if(strncmp(client_msg, "opendir", 7) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Opening Directory");
  		}
  		else if(strncmp(client_msg, "open", 4) == 0){
  			cout << "Opening the file with given directory" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  		}
  		else if(strncmp(client_msg, "close", 5) == 0){
  			cout << "Closing file with given pathname" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  		}
  		
  		
  		else if(strncmp(client_msg, "releasedir", 10) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Releasing Directory");
  		}
  		
  		else if(strncmp(client_msg, "create", 6) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Recieved Create");
  		}
  		
  		else if(strncmp(client_msg, "flush", 5) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Recieved flush");
  		}
  		
  		else if(strncmp(client_msg, "release", 7) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Recieved release");
  		}
 				
  		else if(strncmp(client_msg, "mkdir", 5) == 0){
  			cout << "Making directory with given pathname" << endl;
			cout << "Recieved from Client: " << client_msg << endl;
			if(client_msg[6] != '/' || client_msg[7] == '\0' || client_msg[7] == ' '){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be mkdir /test");
  				
			}
			else{
				string mkdirName;
				int i = 6;
				while(client_msg[i] != '\0'){
					mkdirName += client_msg[i];
					i++;			
				}
				int val = make_directory(mkdirName);
				if(val == -1){
					strcpy(server_msg,"Failure creating directory: ");
  					strcat(server_msg, "-1");
				}
				else{
					strcpy(server_msg,"Success creating directory: ");
  					strcat(server_msg, "1");
				}
			}
  		}
		
		else if(strncmp(client_msg, "truncate", 8) == 0){
  			cout << "Truncating file" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  		}
  		else if(strncmp(client_msg, "getattr", 7) == 0){
  			cout << "Getting attributes" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  		}
  		else{
			cout << "Incorrect input" << endl; 
			strcpy(server_msg,"Incorrect input recieved by server: ");
  			strcat(server_msg, client_msg);
  		}
  		// Send message to the client socket 
  		//pthread_mutex_lock(&lock);
  		//strcpy(server_msg,"Message ");
  		//strcat(server_msg, client_msg);
  		//strcat(server_msg, " recieved by server");
  	
  		//pthread_mutex_unlock(&lock);
  		if(send(newSocket,server_msg,sizeof(server_msg),0) <= 0){
  			cout << "Send to client failed\n" << endl;
			break;
  		}
  		//if(strcmp(client_msg,"Quit") == 0){
		//	break;  		
  		//}
  	}
  	cout << "Exit socketThread" << endl;
  	close(newSocket);
}



//checks to see if two values are the same
void check_values(string value, string correct){
  if(value.compare(correct) != 0){
    cout << "\nUsage is ./serverSNFS -port port# -mount directory" << endl;
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
      cout << "\nUsage is ./serverSNFS -port port# -mount directory" << endl;
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
    cout << "\nUsage is ./serverSNFS -port port# -mount directory" << endl;
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
			cout << "\nUsage is ./serverSNFS -port port# -mount directory" << endl;
			cout << "Error was in the argument directory" << endl;
			cout << p << " is a file not a directory" << endl;
			exit(EXIT_FAILURE);
		}       
		else if (is_directory(p)){    // checks if p is directory?
      	cout << p << " is a valid directory" << endl;
    	}
    	else{
    		cout << "\nUsage is ./serverSNFS -port port# -mount directory" << endl;
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

//mkdir function
int make_directory(string mkdirName){
	path newDir = server_directory / mkdirName;
	
	cout << "Given path: " << newDir << endl;
	if(exists(newDir)){                   //checks to see whether path p exists
		if (is_regular_file(newDir)){      //checks to see whether this is a file
			cout << "This is already a file\n" << endl;
			return -1;
		}       
		else if (is_directory(newDir)){    // checks if p is directory?
      	cout << "Already a directory\n" << endl;
      	return -1;
    	}
    	else{
    		cout << "Neither file nor directory but exists\n" << endl;
			return -1;
    	}
	}
	try{	
		create_directories(newDir);
		cout << "Created Directory\n" << endl;
	}catch(std::exception const & e){
		cout << "Failure creating Directory\n" << endl;
		return -1;			
	}	
	return 1;
}

string getDirectories(string readdirName){
	path p = server_directory / readdirName;
	string newPath = p.string();
	
	string contains;
	DIR *d;
	const char * c = newPath.c_str();
	
	struct dirent *dir;
	d = opendir(c);
	if (d) {
    while ((dir = readdir(d)) != NULL) {
      contains = contains + dir->d_name + " ";
    }
    closedir(d);
  	}
  else{
  	contains = "Directory doesn't exist";
  }
	return contains;
}









