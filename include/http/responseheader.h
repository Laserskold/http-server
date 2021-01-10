#pragma once

#include "fmt/ostream.h"
#include <asio/ip/tcp.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace http {

class ResponseHeader {
public:
    struct Attribute {
        std::string name;
        std::string value;
    };

    std::vector<Attribute> attributes;

    std::string httpVersion = "HTTP/1.1";
    std::string status = "200 OK";

    void write(asio::ip::tcp::socket &socket) {
        std::ostringstream ss;

        ss << *this;

        auto s = ss.str();

        socket.write_some(s);
    }

    friend std::ostream &operator<<(std::ostream &stream,
                                    const ResponseHeader &header) {

        fmt::print(stream, "{} {}\n", header.httpVersion, header.status);

        for (const auto &attr : header.attributes) {
            fmt::print(stream, "{}: {}\n", attr.name, attr.value);
        }

        stream << "\n";

        return stream;
    }

    auto &contentType(std::string value) {
        attributes.emplace_back("Content-Type", value);
        return *this;
    }
};

} // namespace http
