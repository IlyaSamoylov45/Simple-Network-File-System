all: serverSNFS clientSNFS

serverSNFS:
	g++ -o serverSNFS -std=c++11 -lpthread -lrt -lstdc++ -lboost_system -lboost_filesystem serverSNFS.cpp

clientSNFS:
	g++ -o clientSNFS -std=c++11 -lpthread -lrt -lstdc++ -lboost_system -lboost_filesystem clientSNFS.cpp

clean:
	rm -f clientSNFS serverSNFS

