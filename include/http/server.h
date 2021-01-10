// Copyright Mattias Larsson SKöld 2020
#pragma once

#include "files/filesystem.h"
#include "fmt/core.h"
#include "http/requestheader.h"
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>
#include <iostream>

namespace http {

class Server {
public:
    using socket = asio::ip::tcp::socket;

    void start(unsigned short port = 8080) {
        using asio::ip::tcp;

        try {
            asio::io_service service;

            auto acceptor =
                tcp::acceptor{service, tcp::endpoint{tcp::v4(), port}};

            while (true) {
                if (true) {
                    auto socket = tcp::socket{service};
                    acceptor.accept(socket);

                    std::cout << "request started" << std::endl;

                    socket.wait(socket.wait_read);

                    std::vector<char> buffer(socket.available());

                    socket.read_some(asio::buffer(buffer));

                    auto header = RequestHeader{
                        std::string{buffer.data(), buffer.size()}};

                    std::cout << "trying to access " << header.location
                              << std::endl;

                    bool found = false;

                    for (const auto &filter : _filters) {
                        if (filter.first(header)) {
                            filter.second(socket, header);
                            found = true;
                            std::cout << "request finished" << std::endl;
                            break;
                        }
                    }

                    if (!found) {
                        std::cout << "doing default action\n";
                        _defaultAction(socket, header);
                    }
                }
            }
        }
        catch (std::exception &e) {
            std::cerr << e.what() << "\n";
        }
    }

    using FilterT = std::function<bool(const RequestHeader &)>;
    using ActionT = std::function<void(socket &socket, const RequestHeader &)>;

    //! Example
    //! server.addFilter(
    //!     [](const RequestHeader &header) { return header.location ==
    //!     "./"; },
    //!     [](Server::socket &socket, const RequestHeader &header) {
    //!          doStuff(socket, header);
    //!     });
    void addAction(FilterT filter, ActionT action) {
        _filters.emplace_back(filter, action);
    }

    void defaultAction(ActionT action) {
        _defaultAction = action;
    }

private:
    std::vector<std::pair<FilterT, ActionT>> _filters;

    ActionT _defaultAction;
};

} // namespace http
