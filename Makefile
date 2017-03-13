CC            = mpic++
CXXFLAGS      = -std=c++0x

all: 
	$(CC) $(CXXFLAGS) main.cpp matrix.cpp mympi.cpp functions.cpp -o main

