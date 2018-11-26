#include "serverSNFS.h"

char buffer[1024];
char client_msg[5000];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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
	path server_directory = curr_path / p;
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
	
	int newSocket = *((int *)arg);
  	recv(newSocket , client_msg , 2000 , 0);
  // Send message to the client socket 
  	pthread_mutex_lock(&lock);
  	char *message = (char*)malloc(sizeof(client_msg)+20);
  	strcpy(message,"Hello Client : ");
  	strcat(message,client_msg);
  	strcat(message,"\n");
  	strcpy(buffer,message);
  	free(message);
  	pthread_mutex_unlock(&lock);
  	sleep(1);
  	send(newSocket,buffer,13,0);
  	printf("Exit socketThread \n");
  	close(newSocket);
  	pthread_exit(NULL);
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














