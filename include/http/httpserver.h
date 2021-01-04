// Copyright Mattias Larsson SKöld 2020
#pragma once

#include "files/filesystem.h"
#include "fmt/core.h"
#include "http/httpheader.h"
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>
#include <iostream>

class HttpServer {
public:
    void start() {
        using asio::ip::tcp;

        try {
            asio::io_service service;

            tcp::acceptor acceptor{service, tcp::endpoint{tcp::v4(), 9042}};

            while (true) {
                if (true) {
                    auto socket = tcp::socket{service};
                    acceptor.accept(socket);

                    std::cout << "request started" << std::endl;

                    socket.wait(socket.wait_read);

                    std::vector<char> buffer(socket.available());

                    socket.read_some(asio::buffer(buffer));

                    auto header =
                        HttpHeader{std::string{buffer.data(), buffer.size()}};

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

    using FilterT = std::function<bool(const HttpHeader &)>;
    using ActionT =
        std::function<void(asio::ip::tcp::socket &socket, const HttpHeader &)>;

    void addFilter(FilterT filter, ActionT action) {
        _filters.emplace_back(filter, action);
    }

    void defaultAction(ActionT action) {
        _defaultAction = action;
    }

private:
    std::vector<std::pair<FilterT, ActionT>> _filters;

    ActionT _defaultAction;
};
