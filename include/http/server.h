// Copyright Mattias Larsson SKöld 2020
#pragma once

#include "../common/filesystem.h"
#include "http/requestheader.h"
#include <asio.hpp>
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>
#include <functional>
#include <iostream>

namespace http {

class Server {
public:
    using socket = asio::ip::tcp::socket;

    //! Run this to start server
    void start() {
        try {
            startAccept();

            _service.run();
        }
        catch (std::exception &e) {
            if (_error) {
                *_error << e.what() << "\n";
            }
        }
    }

    Server(unsigned short port = 8080)
        : _acceptor(_service,
                    asio::ip::tcp::endpoint{asio::ip::tcp::v4(), port}) {}

    using FilterT = std::function<bool(const RequestHeader &)>;
    using ActionT = std::function<void(socket &socket, const RequestHeader &)>;

    //! Example
    //! server.addAction(
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

    void log(std::ostream &log, std::ostream &error) {
        _log = &log;
        _error = &error;
    }

    //! Disable log output
    void resetLog() {
        _log = nullptr;
        _error = nullptr;
    }

private:
    void handleRequest(socket &socket,
                       asio::error_code ec,
                       const std::string_view data);

    void handleAccept(std::shared_ptr<socket> socket);

    void startAccept();

    std::vector<std::pair<FilterT, ActionT>> _filters;

    ActionT _defaultAction;
    asio::io_service _service;
    asio::ip::tcp::acceptor _acceptor = asio::ip::tcp::acceptor{_service};
    std::ostream *_log = nullptr;
    std::ostream *_error = &std::cerr;
};

// Internal functions ---------------------------------

void Server::handleRequest(Server::socket &socket,
                           asio::error_code ec,
                           const std::string_view data) {
    if (ec) {
        return;
    }

    auto header = RequestHeader{std::string{data.data(), data.size()}};

    if (_log) {
        *_log << "trying to access " << header.location << std::endl;
    }

    bool found = false;

    for (const auto &filter : _filters) {
        if (filter.first(header)) {
            filter.second(socket, header);
            found = true;
            if (_log) {
                *_log << "request finished" << std::endl;
            }
            break;
        }
    }

    if (!found) {
        if (_log) {
            *_log << "doing default action\n";
        }
        if (_defaultAction) {
            _defaultAction(socket, header);
        }
        else {
            constexpr auto requestHeader = std::string_view{
                "HTTP/1.1 404 OK\r\n"
                "Content-Type: text/html; charset=\"UTF-8\"\r\n"
                "Connection: Keep-Alive\r\n"
                "\r\n"};

            socket.write_some(
                asio::buffer(requestHeader.begin(), requestHeader.size()));
        }
    }
}

void Server::handleAccept(std::shared_ptr<Server::socket> socket) {
    if (_log) {
        *_log << "request started" << std::endl;
    }
    auto data = std::make_shared<std::array<char, 1024 * 8>>();

    socket->async_read_some(
        asio::buffer(data->begin(), data->size()),
        [this, socket, data](asio::error_code ec, size_t bytes) {
            handleRequest(*socket, ec, std::string_view{data->data(), bytes});
        });

    startAccept();
}

void Server::startAccept() {
    auto socket = std::make_shared<asio::ip::tcp::socket>(_service);
    _acceptor.async_accept(*socket, [socket, this](asio::error_code ec) {
        if (ec) {
            if (_error) {
                *_error << "accept error\n";
            }
        }
        handleAccept(socket);
    });
}

} // namespace http
