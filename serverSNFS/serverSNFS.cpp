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
	char server_msg[5000];
	char client_msg[5000];
	int newSocket = *((int *)arg);
	fstream inFile;
	string openfilename = "";
	while(1){
		memset(client_msg, 0, sizeof(client_msg));
		memset(server_msg, 0, sizeof(server_msg));
  		if(recv(newSocket , client_msg , sizeof(client_msg) , 0) <= 0){
  			cout << "Receive from client failed\n" << endl;
			break;
  		}
  		if(strncmp(client_msg, "readdir ", 8) == 0){
  			cout << "Returning all filenames from the given directory" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			
  			if(client_msg[8] != '/'){
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
  		
  		else if (strncmp(client_msg, "read ", 5) == 0) {
  			cout << "Reading bytes from file" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			
  			if(client_msg[5] != '/'){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be read /test.txt");	
			}
			string readFile;
			string size;
			string offset;
			int i = 5;
			while(client_msg[i] != ' '){
					readFile += client_msg[i];
					i++;			
			}
			i += 1;
			while(client_msg[i] != ' ' && client_msg[i] != '\0'){
					size += client_msg[i];
					i++;			
			}
			//cout << size << endl;
			i += 1;
			while(client_msg[i] != ' ' && client_msg[i] != '\0'){
					offset += client_msg[i];
					i++;			
			}
			if(!is_digits(size) || !is_digits(offset) || size.empty() || offset.empty()){
				cout << "Incorrect format" << endl;
				strcpy(server_msg,"Incorrect format recieved by server digit error: ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be read /test.txt 100 10");
			}
			else if(readFile.substr( readFile.length() - 4 ) != ".txt"){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '.txt': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be read /test.txt 100 10");	
			}
			else if(!inFile.is_open()){
				cout << "No File is open" << endl; 
				strcpy(server_msg,"No File is open: ");
  				strcat(server_msg, readFile.c_str());		
			}
			else if(readFile.compare(openfilename) != 0){
				cout << "Current file is not open! " << endl; 
				cout << openfilename << endl;
				strcpy(server_msg,"Written file is not open, current file open: ");
  				strcat(server_msg, openfilename.c_str());	
			}
			else{
				int total = stoi(size);
				int start = stoi(offset);
				cout << "Starting at: " << offset << ", total is: " << total << endl;
				if(total > (sizeof(server_msg) - 25)){
					strcpy(server_msg, "Size is larger than the buffer of the message");
				}			
				else{
					inFile.seekg(start, ios::beg);
					inFile.read(server_msg, total);
					cout <<"Server msg: " <<  server_msg <<endl;
				}
			}
							
			
		}
			
  		else if(strncmp(client_msg, "write ", 6) == 0){
  			cout << "Writing data to the file" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			
  			if(client_msg[6] != '/'){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be write /test.txt 100 example input");	
			}
			string writeFile;
			string writeInfo;
			string offset;
			int i = 6;
			while(client_msg[i] != ' '){
					writeFile += client_msg[i];
					i++;			
			}
			i += 1;
			while(client_msg[i] != ' ' && client_msg[i] != '\0'){
					offset += client_msg[i];
					i++;			
			}
			//cout << size << endl;
			i += 1;
			while(client_msg[i] != '\0'){
					writeInfo += client_msg[i];
					i++;			
			}
			if(!is_digits(offset)){
				cout << "Incorrect format" << endl;
				strcpy(server_msg,"Incorrect format recieved by server digit error: ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be write /test.txt 100 text to write in");
			}
			else if(writeFile.substr( writeFile.length() - 4 ) != ".txt"){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '.txt': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be write /test.txt 100 text to write in");	
			}
			else if(!inFile.is_open()){
				cout << "No File is open" << endl; 
				strcpy(server_msg,"No File is open: ");
  				strcat(server_msg, writeFile.c_str());		
			}
			else if(writeFile.compare(openfilename) != 0){
				cout << "Current file is not open! " << endl; 
				cout << openfilename << endl;
				strcpy(server_msg,"Writing to file is not open, current file open: ");
  				strcat(server_msg, openfilename.c_str());	
			}
			else{
				int start = stoi(offset);
				cout << "Starting at: " << offset << ", write data is: \n" << writeInfo << endl;
				int length = writeInfo.length();
				char writeArray[length+1];	
				
				int fileLength;
				
				int begin = inFile.tellg();
				inFile.seekg(0, ios::end); 
				int end = inFile.tellg();
				fileLength = end - begin;
				cout << "Start is: " <<  start << " Filelength is: " << fileLength << endl;
				
				if(start > fileLength){
					strcat(server_msg, "Offset too large");
				}else{
					strcpy(writeArray, writeInfo.c_str());			
					inFile.seekp(start, ios::beg);
					inFile.write(writeArray, length);
					strcat(server_msg, "\nWrote to file");
					inFile.close();
					path temp = server_directory / openfilename;
					string fileLocation = temp.string();
					inFile.open(fileLocation);
				}
			}
							
  		}
  		else if(strncmp(client_msg, "opendir ", 8) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Opening Directory");
  		}
  		
  		
  		else if(strncmp(client_msg, "open ", 5) == 0){
  			cout << "Opening the file with given directory" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			if(client_msg[5] != '/'){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be open /test.txt");	
			}
			else{
				string openFile;
				int i = 5;
  				while(client_msg[i] != '\0'){
					openFile += client_msg[i];
					i++;			
				}
				if(openFile.substr( openFile.length() - 4 ) != ".txt"){
					cout << "Incorrect format" << endl; 
					strcpy(server_msg,"Incorrect format recieved by server no '.txt': ");
  					strcat(server_msg, client_msg);
  					strcat(server_msg, "\nExample should be open /test.txt");	
				}
				else{
					path temp = server_directory / openFile;
					string fileLocation = temp.string();
					cout << fileLocation << endl;
					if (inFile.is_open()) {
						cout << "File is already open: " << openfilename << endl;
						strcpy(server_msg,"Cannot open file, a file is already open: ");
						
						//char char_array[150] =
						cout << openfilename << endl;
  						strcat(server_msg, openfilename.c_str());	
  						
					}
					else{
						inFile.open(fileLocation);
						if(!inFile){
							cout << "Cannot open file, file does not exist. Creating new file" << endl;
							strcpy(server_msg,"Cannot open file, file does not exist. Creating new file: ");
  							strcat(server_msg, openFile.c_str());	
  							inFile.open(fileLocation,  fstream::in | fstream::out | fstream::trunc);
							openfilename = openFile;					
						}
						else{
							cout << "Successfully opened file" << endl;
							strcpy(server_msg,"Successfully opened file: ");
  							strcat(server_msg, openFile.c_str());	
  							openfilename = openFile;	
						}
					}
				}
			}
  		}
  		else if(strncmp(client_msg, "close ", 6) == 0){
  			cout << "Closing file with given pathname" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			if(client_msg[6] != '/'){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be close /test.txt");	
			}
			else{
				string openFile;
				int i = 6;
  				while(client_msg[i] != '\0'){
					openFile += client_msg[i];
					i++;			
				}
				if(openFile.substr( openFile.length() - 4 ) != ".txt"){
					cout << "Incorrect format" << endl; 
					strcpy(server_msg,"Incorrect format recieved by server no '.txt': ");
  					strcat(server_msg, client_msg);
  					strcat(server_msg, "\nExample should be close /test.txt");	
				}
				else{
					if(!inFile.is_open()){
						cout << "No File is open" << endl; 
						strcpy(server_msg,"No File is open, tried to close: ");
  						strcat(server_msg, openFile.c_str());
  						
					}
					else{
						string closeFileName = openFile;
						if(closeFileName.compare(openfilename) == 0){
							cout << "Closing file" << endl; 
							cout << openfilename << endl;
							strcpy(server_msg,"Closed: ");
  							strcat(server_msg, openfilename.c_str());
  							inFile.close();
  							openfilename = "";
						}
						else{
							cout << "File :" << endl; 
							cout << openfilename << " not currently open" <<endl;
							strcpy(server_msg,"No File named: ");
  							strcat(server_msg, closeFileName.c_str());
							strcat(server_msg, " is open. Current open file is: ");
  							strcat(server_msg, openfilename.c_str());
						}					
					} 
				}
			}
  			
  		}
  		
  		
  		else if(strncmp(client_msg, "releasedir ", 11) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Releasing Directory");
  		}
  		
  		else if(strncmp(client_msg, "create ", 7) == 0){
  			cout << "Create the file with given name" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			if(client_msg[7] != '/'){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be create /test.txt");	
			}
			else{
				string newFile;
				int i = 7;
  				while(client_msg[i] != '\0'){
					newFile += client_msg[i];
					i++;			
				}
				if(newFile.substr( newFile.length() - 4 ) != ".txt"){
					cout << "Incorrect format" << endl; 
					strcpy(server_msg,"Incorrect format recieved by server no '.txt': ");
  					strcat(server_msg, client_msg);
  					strcat(server_msg, "\nExample should be create /test.txt");	
				}
				else if(inFile.is_open()){
					cout << "A File is open" << endl; 
					strcpy(server_msg,"Please close file before you create: ");
  					strcat(server_msg, openfilename.c_str());
  						
				}
				else{
					int val = createFile(newFile);
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
  		}
  		
  		else if(strncmp(client_msg, "flush ", 6) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Recieved flush");
  		}
  		
  		else if(strncmp(client_msg, "release ", 8) == 0){
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"Recieved release");
  		}
 				
  		else if(strncmp(client_msg, "mkdir ", 6) == 0){
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
		
		else if(strncmp(client_msg, "truncate ", 9) == 0){
  			cout << "Truncating file" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			if(client_msg[9] != '/'){
				cout << "Incorrect format" << endl; 
				strcpy(server_msg,"Incorrect format recieved by server no '/': ");
  				strcat(server_msg, client_msg);
  				strcat(server_msg, "\nExample should be truncate /test.txt 10");	
			}
			else{
				string trunFile;
				string offsetTrunc;
				int i = 9;
  				while(client_msg[i] != ' '){
					trunFile += client_msg[i];
					i++;			
				}
				i += 1;
				while(client_msg[i] != '\0'){
					offsetTrunc += client_msg[i];
					i++;			
				}
				if(!is_digits(offsetTrunc) || offsetTrunc.empty()){
					cout << "Incorrect format" << endl;
					strcpy(server_msg,"Incorrect format recieved by server digit error: ");
  					strcat(server_msg, client_msg);
  					strcat(server_msg, "\nExample should be truncate /test.txt 10");
				}
				else if(trunFile.substr( trunFile.length() - 4 ) != ".txt"){
					cout << "Incorrect format" << endl; 
					strcpy(server_msg,"Incorrect format recieved by server no '.txt': ");
  					strcat(server_msg, client_msg);
  					strcat(server_msg, "\nExample should be truncate /test.txt 10");	
				}
				else if(!inFile.is_open()){
					cout << "No File is open" << endl; 
					strcpy(server_msg,"No File is open: ");
  					strcat(server_msg, trunFile.c_str());		
				}
				else if(trunFile.compare(openfilename) != 0){
					cout << "Current file is not open! " << endl; 
					cout << openfilename << endl;
					strcpy(server_msg,"Written file is not open, current file open: ");
  					strcat(server_msg, openfilename.c_str());	
				}
				else{
					int end = stoi(offsetTrunc);
					path p = server_directory / trunFile;
					int truncVal = truncate((p.string()).c_str(), end);
					cout << truncVal << endl;
					if(truncVal == 0){
						strcpy(server_msg,"Successfully truncated file");
					}
					else{
						strcpy(server_msg,"Truncating failure");
					}
				}
  			}
  		}
  		else if(strncmp(client_msg, "getattr ", 8) == 0){
  			cout << "Getting attributes" << endl;
  			cout << "Recieved from Client: " << client_msg << endl;
  			strcpy(server_msg,"getattr success! ");
  			//strcat(server_msg, client_msg);
  		}
  		else{
			cout << "Incorrect input" << endl; 
			strcpy(server_msg,"Incorrect input recieved by server: ");
  			strcat(server_msg, client_msg);
  		}
  		// Send message to the client socket 
  		if(send(newSocket,server_msg,sizeof(server_msg),0) <= 0){
  			cout << "Send to client failed\n" << endl;
			break;
  		}
  		
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


int createFile(string createFile){
	path newFile = server_directory / createFile;
	ifstream tempFile;
	cout << "Given path: " << newFile << endl;
	if(exists(newFile)){                   //checks to see whether path p exists
		if (is_regular_file(newFile)){      //checks to see whether this is a file
			cout << "This is already a file\n" << endl;
			return -1;
		}       
		else if (is_directory(newFile)){    // checks if p is directory?
      	cout << "Already a directory\n" << endl;
      	return -1;
    	}
    	else{
    		cout << "Neither file nor directory but exists\n" << endl;
			return -1;
    	}
	}
	
	string newpath = newFile.string();
	tempFile.open(newpath);
	if(!tempFile){
		tempFile.open(newpath,  fstream::in | fstream::out | fstream::trunc);
		tempFile.close();
		return 1;
	}
	return -1;
}

bool is_digits(string str){
	if(str.empty()){
		return false;	
	}
   return str.find_first_not_of("0123456789") == std::string::npos;
}




