
CXXFLAGS= -I../include -std=c++17 -lstdc++fs -g -pthread

simple_server_example: simple_server_example.cpp
	${CXX} simple_server_example.cpp -o simple_server_example ${CXXFLAGS}

simple_html_example: simple_html_example.cpp
	${CXX} simple_html_example.cpp -o simple_html_example ${CXXFLAGS}

all: simple_server_example simple_html_example
