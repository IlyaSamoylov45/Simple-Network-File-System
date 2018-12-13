all: clean serverSNFS

serverSNFS:
	g++ -o serverSNFS -std=c++11 -lpthread -lrt -lstdc++ -lboost_system -lboost_filesystem serverSNFS.cpp

clean:
	rm -f serverSNFS
