// Copyright Mattias Larsson SKöld 2020

#pragma once

#include "files/filesystem.h"
#include <asio/ip/tcp.hpp>

//! Header for http request
struct HttpHeader {
    std::string protocol;
    filesystem::path location;

    HttpHeader(std::string data) {
        std::istringstream ss(data);

        {
            std::string line;
            std::getline(ss, line);

            //            fmt::print("first line: {}\n", line);

            std::istringstream liness(line);

            std::string location;

            liness >> protocol;
            liness >> location;

            if (location.find("..") != std::string::npos) {
                location = ""; // Should not be possible to exit root folder
            }

            if (location.front() == '/') {
                location.insert(location.begin(), '.');
            }

            this->location = location;
        }

        //        for (std::string line; std::getline(ss, line);) {
        //            fmt::print("settings: {}\n", line);
        //        }
    }
};
