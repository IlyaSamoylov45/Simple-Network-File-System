#include "clientDummy.h"

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
	
	
	struct sockaddr_in serverAddress;
	struct hostent *hostp;
	int clientSocket;
	int rc;
	socklen_t addr_size;
	
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
    cout << "Client-connect() established" << endl;
   
   char server_msg[5000];
	char client_msg[5000];
	char temp[5000];
	//string options = "\nProgram will run until you type Quit, \nsystem calls available: \ncreate \nopen \nflush \nrelease \ntruncate \ngetattr \nread \nwrite \nopendir \nreaddir \nreleasedir \nmkdir\n";     
	while(1){
		//cout << options << endl;
		cin.getline(client_msg, sizeof(client_msg));
    
		//Send msg to server
		if( send(clientSocket , client_msg , sizeof(client_msg) , 0) <= 0){
			cout << "Client Send failed\n" << endl;
   	}
   
   	//Read the message from the server into the buffer
    	if(recv(clientSocket, server_msg, sizeof(server_msg), 0) <= 0){
       	cout << "Receive failed\n" << endl;
    	}	
    
    	//Print the received message
    	cout << server_msg << endl;
    	
    	//If quit break
    	if(strcmp(client_msg,"Quit") == 0){
			break;    	
    	}
    }
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
