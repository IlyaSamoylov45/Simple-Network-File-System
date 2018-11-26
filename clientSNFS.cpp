#include "clientSNFS.h"

int main(int argc, char* argv[]){
	if(argc != 7){
		cout << "Usage is ./clientSNFS -serverport port# -serveraddress address# -mount directory" << endl;
		exit(EXIT_FAILURE);	
	}
	check_values(argv[1], "-serverport");
	check_port(argv[2], "port#");
	int serverPort = stoi(argv[2]);
	check_values(argv[3], "-serveraddress");	
	string server = argv[4];
	check_values(argv[5], "-mount");
	//get path of mountable file
	path curr_path = current_path(); //gets current path
	path p = argv[6];
	path client_directory = curr_path / p;
	check_directory(client_directory);
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
