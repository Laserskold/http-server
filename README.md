Http server
===============
A simple http server in c++

Requirements
-----------------

sudo apt install libasio-dev

Example usage ```c++

#include "http/server.h"
#include "http/standardresponses.h"

int main(int, char *[]) {
    using namespace http;
    auto server = Server{8080};

    // Define action that is selected depending on a rule
    server.addAction(
        [](const RequestHeader &header) {
            // If this returns true this action will be selected
            return header.location.extension() == ".html";
        },
        [](Server::socket &socket, const RequestHeader &header) {
            // Send a file from the applications working directory
            sendFile(socket, header.location, "text/html");
        });

    // Define what to do when other actions does not match
    server.defaultAction([](Server::socket &socket,
                            const RequestHeader &header) {
        constexpr auto requestHeader =
            std::string_view{"HTTP/1.1 404 OK\r\n"
                             "Content-Type: text/html; charset=\"UTF-8\"\r\n"
                             "Connection: Keep-Alive\r\n"
                             "\r\n"
                             "Not supported file format :/"};

        socket.write_some(
            asio::buffer(requestHeader.begin(), requestHeader.size()));
    });

    std::cout << "running server on port 8080\n"
                 "try opening browser at\n"
                 "http://localhost:8080/index.html"
              << std::endl;

    // Serve and handle requests
    server.start();

    return 0;
}


```
