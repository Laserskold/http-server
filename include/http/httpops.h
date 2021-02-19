#pragma once

#include "server.h"
#include <fstream>
#include <string_view>

namespace http {

void sendFile(asio::ip::tcp::socket &socket, filesystem::path path) {
    auto fullPath = filesystem::absolute(filesystem::current_path()) / path;

    auto file = std::ifstream{fullPath, std::ios::binary};
    if (file.is_open()) {
        auto data = std::array<char, 100000>{};

        while (auto count = file.readsome(data.data(), data.size())) {
            socket.write_some(asio::buffer(data.data(), count));
        }
    }
    else {
        std::cerr << "could not load " << path << "\n";
        socket.write_some(
            asio::buffer("could not load " + path.string() + "\n"));
    }
}

void sendHtml(asio::ip::tcp::socket &socket, filesystem::path path) {
    constexpr auto requestHeader =
        std::string_view{"HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html; charset=\"UTF-8\"\r\n"
                         "Connection: Keep-Alive\r\n"
                         "\r\n"};

    socket.write_some(
        asio::buffer(requestHeader.begin(), requestHeader.size()));

    sendFile(socket, path);
}

} // namespace http
