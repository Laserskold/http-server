#pragma once

#include "files/filesystem.h"
#include "fmt/core.h"
#include "http/responseheader.h"
#include <asio/ip/tcp.hpp>
#include <fstream>

namespace http {

void sendFileNotFound(asio::ip::tcp::socket &socket, filesystem::path path) {
    ResponseHeader{}.contentType("text/html; charset=\"UTF-8\"").write(socket);

    socket.write_some(asio::buffer(fmt::format("file not found")));
}

//! Send file with the right header
void sendFile(asio::ip::tcp::socket &socket,
              filesystem::path path,
              std::string contentType) {
    auto file = std::ifstream{path, std::ios::binary};

    if (file.is_open()) {
        ResponseHeader{}.contentType(contentType).write(socket);

        auto data = std::array<char, 100000>{};

        while (auto count = file.readsome(data.data(), data.size())) {
            socket.write_some(asio::buffer(data.data(), count));
        }
    }
    else {
        sendFileNotFound(socket, path);
    }
}

} // namespace http
