all: clean clientSNFS

clientSNFS:
	g++ -o clientSNFS -std=c++11 -lpthread -lrt -lstdc++ -lboost_system -lfuse -lboost_filesystem -D_FILE_OFFSET_BITS=64 clientSNFS.cpp

clean:
	rm -f clientSNFS
