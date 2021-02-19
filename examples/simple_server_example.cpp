//! A simple http server that publishes files in the current directory
//!
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
            // or send error code if does not exist
            sendFile(socket, header.location, "text/html");
        });

    // Define what to do when other actions does not match
    server.defaultAction(
        [](Server::socket &socket, const RequestHeader &header) {
            auto responseHeader = ResponseHeader{};
            responseHeader.status = "404 Not supported file format :/";
            responseHeader.write(socket);
        });

    std::cout << "running server on port 8080\n"
                 "try opening browser at\n"
                 "http://localhost:8080/index.html"
              << std::endl;

    // Serve and handle requests
    server.start();

    return 0;
}
