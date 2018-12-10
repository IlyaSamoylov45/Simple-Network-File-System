all: clientSNFS

clientSNFS:
	g++ -o clientSNFS -std=c++11 -lpthread -lrt -lstdc++ -lboost_system -lboost_filesystem clientSNFS.cpp

clean:
	rm -f clientSNFS